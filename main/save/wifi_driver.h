#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/**
 * @brief Initialise Wi-Fi in station mode and begin connecting.
 *        Returns after starting the Wi-Fi stack; use wifi_get_event_group()
 *        to wait for WIFI_CONNECTED_BIT or WIFI_FAIL_BIT.
 */
void wifi_init_sta(void);

/**
 * @brief Return the FreeRTOS event group used by WiFi.
 *        Bits are defined in config.h.
 */
EventGroupHandle_t wifi_get_event_group(void);
