#pragma once

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define MQTT_TOPIC_DATA CONFIG_MQTT_TOPIC_DATA

/**
 * @brief Initialise and start the MQTT client.
 *
 * Creates the MQTT event group, connects to the broker defined by
 * MQTT_BROKER_URL, and registers the internal event handler. Sets
 * MQTT_CONNECTED_BIT in the event group once the broker acknowledges the
 * connection.
 */
void mqtt_start(void);

/**
 * @brief Publish a string payload to the current device topic (QoS 0).
 *
 * @param payload  Null-terminated string to publish.
 */
void mqtt_publish(const char *payload);

/**
 * @brief Return the FreeRTOS event group used by the MQTT driver.
 *
 * Callers can wait on MQTT_CONNECTED_BIT, MQTT_SUBSCRIBED_BIT, and
 * MQTT_DATA_BIT (defined in config.h).
 */
EventGroupHandle_t mqtt_get_event_group(void);
