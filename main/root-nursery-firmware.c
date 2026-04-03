#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_timer.h"

#include "config.h"
#include "wifi_driver.h"
#include "mqtt_driver.h"
#include "aht20_driver.h"
#include "as7341_driver.h"
#include "led_controller.h"
#include "control_loop.h"

void app_main(void)
{
    // ── System unit ───────────────────────────────────────────────────────────
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    // ── WiFi ──────────────────────────────────────────────────────────────────
    wifi_init_sta();

    EventBits_t bits = xEventGroupWaitBits(wifi_get_event_group(),
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE,
                                           pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS));
    if (bits & WIFI_FAIL_BIT) {
        printf("Failed to connect to WiFi\n");
        return;
    }

    // ── MQTT ──────────────────────────────────────────────────────────────────
    printf("WiFi connected! Starting MQTT...\n");
    mqtt_start();

    EventBits_t mqtt_bits = xEventGroupWaitBits(mqtt_get_event_group(),
                                                MQTT_CONNECTED_BIT,
                                                pdFALSE, pdFALSE,
                                                pdMS_TO_TICKS(MQTT_CONNECT_TIMEOUT_MS));
    if (!(mqtt_bits & MQTT_CONNECTED_BIT)) {
        printf("MQTT connection timeout\n");
        return;
    }

    // Get the config (name, period and par) from the NVS or wait for it to be transmitted via MQTT
    get_config();
    // Initialize the LEDC for the controlling of the LED's
    led_init();

    // Configure and run the timer to periodically sense the parameters
    esp_timer_handle_t timer_hd;
    const esp_timer_create_args_t timer_config = {
        .callback = &execute_main_loop,
        .name = "main_loop_timer"
    };
    esp_timer_create(&timer_config, &timer_hd);
    int64_t timeout_us = config.sense_period * 1000000;

    execute_main_loop(NULL);

    esp_timer_start_periodic(timer_hd, timeout_us);
}
