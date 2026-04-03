#pragma once

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define MQTT_TOPIC_DATA CONFIG_MQTT_TOPIC_DATA
/**
 * @brief Initialise and start the MQTT client.
 *        Sets MQTT_CONNECTED_BIT in the WiFi event group on connection.
 */
void mqtt_start(void);

/**
 * @brief Publish a string payload to MQTT_TOPIC (defined in config.h).
 *
 * @param payload  Null-terminated string to publish.
 */
void mqtt_publish(const char *payload);

/**
 * @brief Return the FreeRTOS event group used by MQTT.
 *        Bits are defined in config.h.
 */
EventGroupHandle_t mqtt_get_event_group(void);
