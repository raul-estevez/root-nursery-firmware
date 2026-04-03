#pragma once

#include <stdint.h>

/**
 * @brief One sensing and reporting cycle, intended as an esp_timer callback.
 *
 * Runs the PAR controller, reads temperature/humidity, then publishes a JSON
 * payload containing all sensor readings and the current LED duty cycle to
 * the configured MQTT topic.
 *
 * @param param  Unused. Pass NULL when calling directly.
 */
void execute_main_loop(void *param);

/**
 * @brief Run the proportional PAR controller.
 *
 * Iterates up to PAR_MAX_ITER times, adjusting the LED duty cycle until
 * the measured PAR is within PAR_TOL of the configured setpoint, or the
 * iteration budget is exhausted.
 *
 * @param[out] error_ret  PAR error (setpoint - measured) after the last
 *                        iteration, regardless of whether convergence was
 *                        reached.
 * @return  1 if converged within tolerance, -1 if iterations ran out.
 */
int8_t par_control_loop(float* error_ret);
