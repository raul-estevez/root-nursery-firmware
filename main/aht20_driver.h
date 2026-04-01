#pragma once

/**
 * @brief Read temperature and relative humidity from the AHT20 sensor.
 *
 * Uses the legacy I2C bus API (i2c_bus) on I2C_PORT with the pins
 * defined in config.h.
 *
 * @param[out] temperature  Temperature in °C.
 * @param[out] humidity     Relative humidity in %.
 */
void aht20_read(float *temperature, float *humidity);
