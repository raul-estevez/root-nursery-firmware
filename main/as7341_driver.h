#pragma once

#include "as7341.h"

// PAR calibration coefficients (linear model over the 8 spectral channels)
extern const float par_coef[9];

/**
 * @brief Read raw ADC counts from the AS7341 spectral sensor.
 *
 * Uses the new I2C master bus API on I2C_PORT with the pins defined in
 * config.h.
 *
 * @return Struct with per-channel ADC values (f1–f8, clear, nir).
 */
as7341_channels_spectral_data_t read_raw(void);

/**
 * @brief Compute PAR (µmol/m²/s) from a raw spectral reading.
 *
 * @param spectrum  Raw ADC data previously returned by as7341_read_raw().
 * @return Estimated PAR value.
 */
float compute_par(as7341_channels_spectral_data_t spectrum);

/**
 * @brief Convenience wrapper: read sensor and return PAR in one call.
 *
 * @return Estimated PAR value.
 */
float read_par(void);
