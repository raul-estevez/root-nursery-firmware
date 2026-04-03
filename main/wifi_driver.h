#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/**
 * @brief Initialise WiFi in station mode and begin connecting.
 *
 * Returns after starting the WiFi stack. Use wifi_get_event_group() to wait
 * for WIFI_CONNECTED_BIT or WIFI_FAIL_BIT (both defined in config.h).
 */
void wifi_init_sta(void);

/**
 * @brief Return the FreeRTOS event group used by the WiFi driver.
 *
 * Callers can wait on WIFI_CONNECTED_BIT and WIFI_FAIL_BIT (defined in
 * config.h).
 */
EventGroupHandle_t wifi_get_event_group(void);
