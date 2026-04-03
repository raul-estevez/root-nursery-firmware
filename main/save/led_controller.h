#pragma once

#include <stdint.h>

// ── PWM / LEDC parameters ────────────────────────────────────────────────────
#define LED_GPIO            (0)
#define LED_FREQ_HZ         (100)
#define LED_DUTY_MAX        (8192)   /* 13-bit resolution → 2^13 */
#define LED_DUTY_MIN        (0)

// ── Proportional controller parameters ───────────────────────────────────────
#define PAR_IDEAL           (50.0f)  /* target PAR (µmol/m²/s)  */
#define PAR_TOL             (1.0f)   /* convergence tolerance    */
#define PAR_P_GAIN          (40.0f)  /* proportional gain        */
#define PAR_MAX_CHANGE      (1000.0f)/* max duty-cycle step      */
#define PAR_MAX_ITER        (15)     /* iterations per control call */

/**
 * @brief Initialise the LEDC peripheral for LED PWM output.
 *        Must be called once before set_led_duty() or par_control_loop().
 */
void led_init(void);

/**
 * @brief Set the LED PWM duty cycle.
 *
 * @param duty  Value in [LED_DUTY_MIN, LED_DUTY_MAX].
 *              Internally inverted so that higher duty = more light.
 */
void led_set_duty(uint16_t duty);

