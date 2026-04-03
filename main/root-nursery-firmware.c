/*
 * root-nursery-firmware.c
 *
 * Application entry point. Brings up the network stack, connects to WiFi and
 * MQTT, loads persistent configuration, initialises the LED controller, and
 * schedules the sensor/control loop on a periodic timer.
 */

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
    /* Initialise NVS, the TCP/IP stack, and the default event loop. These
     * must come first because WiFi and MQTT both depend on them. */
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    /* Start WiFi in station mode and wait until connected or failed. */
    wifi_init_sta();

    EventBits_t bits = xEventGroupWaitBits(wifi_get_event_group(),
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE,
                                           pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS));
    if (bits & WIFI_FAIL_BIT) {
        printf("Failed to connect to WiFi\n");
        return;
    }

    /* Start the MQTT client and wait for the broker connection. */
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

    /* Load device name, sensing period, and PAR setpoint from NVS.
     * If this is the first boot, get_config() blocks until a configuration
     * message arrives over MQTT's topic nursery/default. */
    get_config();

    /* Initialise the LEDC peripheral for PWM control of the grow light. */
    led_init();

    /* Create a periodic timer that fires execute_main_loop() every
     * sense_period seconds. Run one iteration immediately before starting
     * the timer so there is no initial delay. */
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
