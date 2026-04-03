#include "led_controller.h"
#include "as7341_driver.h"

#include "driver/ledc.h"

#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   LEDC_TIMER_13_BIT
#define LEDC_CLK_SRC    LEDC_AUTO_CLK

void led_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num       = LEDC_TIMER,
        .freq_hz         = LED_FREQ_HZ,
        .clk_cfg         = LEDC_CLK_SRC,
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER,
        .gpio_num   = LED_GPIO,
        .duty       = 0,
        .hpoint     = 0,
    };
    ledc_channel_config(&channel);
}

void led_set_duty(uint16_t duty)
{
    /* Invert: hardware is active-low, so higher duty = more light */
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LED_DUTY_MAX - duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

