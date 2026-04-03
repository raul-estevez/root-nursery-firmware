/*
 * as7341_driver.c
 *
 * Thin wrapper around the AS7341 8-channel spectral sensor.
 *
 * Raw ADC counts from the eight visible-light channels (f1-f8) are converted
 * to an estimated PAR value (umol/m2/s) using a linear regression model whose
 * coefficients were fitted offline against a reference PAR meter doi:10.3390/electronics14112225.
 *
 * The new ESP-IDF I2C master bus API (driver/i2c_master.h) is used here,
 * unlike the AHT20 driver which uses the legacy i2c_bus component.
 */

#include "as7341_driver.h"
#include "config.h"

#include "driver/i2c_master.h"

// TODO: Add the capability to use other gains.

/*
 * Linear model: PAR = c0 + c1*f1 + c2*f2 + ... + c8*f8
 * Index 0 is the intercept; indices 1-8 correspond to channels f1-f8.
 * Coefficients were derived from a least-squares fit against a calibrated
 * reference sensor.
 */
const float par_coef[9] = {
    -1.83008,  /* intercept */
    -0.10893,  /* f1 */
    -0.19323,  /* f2 */
     0.149401, /* f3 */
     0.234282, /* f4 */
     0.019283, /* f5 */
    -0.1623,   /* f6 */
     0.133297, /* f7 */
     0.087622  /* f8 */
};


/*
 * Initialise the AS7341, take one spectral measurement, then clean up.
 * A fresh I2C bus and device handle are created and destroyed on every call
 * to avoid holding the bus between sensing periods.
 */
as7341_channels_spectral_data_t read_raw_spectrum(void)
{

    i2c_master_bus_config_t bus_cfg = {
        .clk_source      = I2C_CLK_SRC_DEFAULT,
        .i2c_port        = I2C_PORT,
        .scl_io_num      = I2C_SCL_IO,
        .sda_io_num      = I2C_SDA_IO,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus;
    i2c_new_master_bus(&bus_cfg, &bus);

    as7341_config_t dev_cfg = {
        .i2c_address    = I2C_AS7341_DEV_ADDR,
        .i2c_clock_speed = I2C_AS7341_DEV_CLK_SPD,
        .atime          = 35,
        .astep          = 999,
        .spectral_gain  = AS7341_SPECTRAL_GAIN_1X,
    };
    as7341_handle_t dev;
    as7341_init(bus, &dev_cfg, &dev);

    as7341_channels_spectral_data_t adc_data;
    as7341_get_spectral_measurements(dev, &adc_data);

    as7341_delete(dev);
    i2c_del_master_bus(bus);
    return adc_data;
}

/* Apply the linear PAR model to a set of raw ADC channel counts. */
float compute_par(as7341_channels_spectral_data_t spectrum)
{
    return par_coef[0]
         + par_coef[1] * spectrum.f1
         + par_coef[2] * spectrum.f2
         + par_coef[3] * spectrum.f3
         + par_coef[4] * spectrum.f4
         + par_coef[5] * spectrum.f5
         + par_coef[6] * spectrum.f6
         + par_coef[7] * spectrum.f7
         + par_coef[8] * spectrum.f8;
}

/* Convenience wrapper: read the sensor and return PAR in a single call. */
float read_par(void)
{
    return compute_par(read_raw_spectrum());
}
