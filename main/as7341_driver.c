#include "as7341_driver.h"
#include "config.h"

#include "driver/i2c_master.h"

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

static i2c_master_bus_handle_t create_i2c_bus(void)
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
    return bus;
}

as7341_channels_spectral_data_t spectrum_read_raw(void)
{
    i2c_master_bus_handle_t bus = create_i2c_bus();

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

float read_par(void)
{
    return compute_par(spectrum_read_raw());
}
