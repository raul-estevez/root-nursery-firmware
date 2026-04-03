/*
 * aht20_driver.c
 *
 * Thin wrapper around the AHT20 temperature and humidity sensor.
 *
 * Uses the legacy ESP-IDF i2c_bus component rather than the newer
 * driver/i2c_master.h API. A bus handle is created at the start of each
 * read and released before returning.
 */

#include "aht20_driver.h"
#include "config.h"

#include "aht20.h"
#include "i2c_bus.h"   /* ESP-IDF legacy i2c_bus component */

/*
 * Create a temporary I2C bus, read one temperature and humidity sample from
 * the AHT20, then tear down the bus. The raw 20-bit values from the sensor
 * are discarded; only the converted float results are returned.
 */
void aht20_read(float *temperature, float *humidity)
{
    const i2c_config_t i2c_bus_conf = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = I2C_SDA_IO,
        .sda_pullup_en    = GPIO_PULLUP_DISABLE,
        .scl_io_num       = I2C_SCL_IO,
        .scl_pullup_en    = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    i2c_bus_handle_t i2c_bus = i2c_bus_create(I2C_PORT, &i2c_bus_conf);

    aht20_i2c_config_t i2c_conf = {
        .bus_inst  = i2c_bus,
        .i2c_addr  = AHT20_ADDRRES_0,
    };

    aht20_dev_handle_t handle;
    aht20_new_sensor(&i2c_conf, &handle);

    /* temperature_raw and humidity_raw are required by the API but not used
     * further; the converted float values are the outputs of interest. */
    uint32_t temperature_raw, humidity_raw;
    aht20_read_temperature_humidity(handle, &temperature_raw, temperature,
                                    &humidity_raw, humidity);

    aht20_del_sensor(handle);
    i2c_bus_delete(&i2c_bus);
}
