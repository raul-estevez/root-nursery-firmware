/*
 * wifi_driver.c
 *
 * WiFi station initialisation and connection management.
 *
 * After wifi_init_sta() returns, the caller should wait on the event group
 * returned by wifi_get_event_group() for either WIFI_CONNECTED_BIT (success)
 * or WIFI_FAIL_BIT (retry limit exceeded).
 */

#include "wifi_driver.h"
#include "config.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

static EventGroupHandle_t wifi_event_group;
static int s_retry_num = 0;

/*
 * Handles WiFi and IP events:
 *   STA_START:        begin the connection attempt.
 *   STA_DISCONNECTED: retry up to WIFI_MAX_RETRY times, then set WIFI_FAIL_BIT.
 *   GOT_IP:           reset the retry counter and set WIFI_CONNECTED_BIT.
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
        } else {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* Configure the WiFi stack in station mode, register event handlers for
 * connection and IP events, and start the connection process. Credentials
 * are taken from WIFI_SSID and WIFI_PASS defined in config.h (sourced from
 * Kconfig). */
void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid     = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

EventGroupHandle_t wifi_get_event_group(void)
{
    return wifi_event_group;
}
