#pragma once

#include <stdint.h>

/* GPIO pin and PWM frequency for the grow-light LED. */
#define LED_GPIO            (0)
#define LED_FREQ_HZ         (100)

#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   LEDC_TIMER_13_BIT   /* 13-bit: range 0-8191 */
#define LEDC_CLK_SRC    LEDC_AUTO_CLK


/* 13-bit LEDC resolution: valid duty range is 0 to 8191. */
#define LED_DUTY_MAX        (8191)
#define LED_DUTY_MIN        (0)

/* Proportional controller tuning parameters. Feel free to tune for you setup */
#define PAR_TOL             (1.0f)    /* Convergence tolerance in umol/m2/s */
#define PAR_P_GAIN          (40.0f)   /* Proportional gain (duty counts per umol/m2/s error) */
#define PAR_MAX_CHANGE      (1000.0f) /* Maximum duty-cycle step per iteration */
#define PAR_MAX_ITER        (15)      /* Maximum iterations per control call */

/**
 * @brief Initialise the LEDC peripheral for LED PWM output.
 *
 * Must be called once before led_set_duty() or par_control_loop().
 */
void led_init(void);

/**
 * @brief Set the LED PWM duty cycle.
 *
 * @param duty  Value in [LED_DUTY_MIN, LED_DUTY_MAX].
 *              Internally inverted so that a higher value means more light.
 */
void led_set_duty(uint16_t duty);
