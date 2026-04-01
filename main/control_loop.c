#include <stdio.h>
#include <stdint.h>

#include "mqtt_driver.h"
#include "aht20_driver.h"
#include "as7341_driver.h"
#include "led_controller.h"

static float current_duty = 4096.0f;
void execute_main_loop(void *param){
    printf("Starting loop\n");
    // ── Initial sensor readings ───────────────────────────────────────────────
    char payload[64];

    float temperature, humidity;
    aht20_read(&temperature, &humidity);
    snprintf(payload, sizeof(payload), "%.2f\t%.2f", temperature, humidity);
    mqtt_publish(payload);

    float par = read_par();
    snprintf(payload, sizeof(payload), "%.4f", par);
    mqtt_publish(payload);

    // ── LED proportional control loop ─────────────────────────────────────────

    led_set_duty((uint16_t)current_duty);

    current_duty = LED_DUTY_MAX / 2.0f; /* reset search point each cycle */
    float error;
    int8_t converged = par_control_loop(current_duty, &error);
    snprintf(payload, sizeof(payload), "%.4f\t%d", error, converged);
    mqtt_publish(payload);

}
