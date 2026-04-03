#pragma once

#include "as7341.h"

/**
 * @brief Read raw ADC counts from the AS7341 spectral sensor.
 *
 * Uses the new I2C master bus API on I2C_PORT with the pins defined in
 * config.h. The bus is created and destroyed within the call.
 *
 * @return Struct with per-channel ADC values (f1-f8, clear, nir).
 */
as7341_channels_spectral_data_t read_raw_spectrum(void);

/**
 * @brief Compute PAR (umol/m2/s) from a raw spectral reading.
 *
 * Applies the linear regression model stored in par_coef[].
 *
 * @param spectrum  Raw ADC data previously returned by read_raw_spectrum().
 * @return Estimated PAR value.
 */
float compute_par(as7341_channels_spectral_data_t spectrum);

/**
 * @brief Convenience wrapper: read the sensor and return PAR in one call.
 *
 * @return Estimated PAR value.
 */
float read_par(void);
