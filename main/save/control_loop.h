#include <stdint.h>

void execute_main_loop(void *param);
/**
 * @brief Run the proportional PAR controller.
 *
 * Iterates up to PAR_MAX_ITER times, adjusting the LED duty cycle until
 * the measured PAR is within PAR_TOL of PAR_IDEAL, or iterations are
 * exhausted.
 *
 * @param current_duty  Starting duty cycle.
 * @return              Remaining error after the last iteration.
 */
int8_t par_control_loop(float* error_ret);
