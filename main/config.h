#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "sdkconfig.h"
    

/* I2C bus shared by both the AHT20 and AS7341 drivers. */
#define I2C_PORT            I2C_NUM_0
#define I2C_SDA_IO          CONFIG_I2C_MASTER_SDA
#define I2C_SCL_IO          CONFIG_I2C_MASTER_SCL
#define I2C_FREQ_HZ         100000

/* WiFi station credentials and connection retry limit, sourced from Kconfig. */
#define WIFI_SSID           CONFIG_WIFI_SSID
#define WIFI_PASS           CONFIG_WIFI_PASSWORD
#define WIFI_MAX_RETRY      CONFIG_MAX_RETRY
#define WIFI_CONNECT_TIMEOUT_MS  10000

/* MQTT broker and default publish topic, sourced from Kconfig. */
#define MQTT_BROKER_URL     CONFIG_BROKER_URL
#define MQTT_TOPIC          CONFIG_MQTT_TOPIC_DATA
#define MQTT_CONNECT_TIMEOUT_MS  10000


/* FreeRTOS event group bit definitions.
 * WiFi and MQTT each have their own event group; bit positions are reused
 * across the two groups since the handles are distinct. */
#define WIFI_CONNECTED_BIT  (1 << 0)
#define WIFI_FAIL_BIT       (1 << 1)
#define MQTT_CONNECTED_BIT  (1 << 0)
#define MQTT_DATA_BIT       (1 << 1)
#define MQTT_SUBSCRIBED_BIT (1 << 2)

/* Runtime device configuration. Populated by get_config() at startup and
 * updated in place by the MQTT event handler when a new config message
 * arrives. */
typedef struct {
    char name[16];
    char topic[32];         /* nursery/<name> */
    uint16_t  sense_period; /* Seconds between sensor readings */
    uint16_t  par_setpoint; /* Target PAR in umol/m2/s */
} nursery_config_t;

/* Global config instance, defined in mqtt_driver.c. */
extern nursery_config_t config;

/**
 * @brief Load configuration from NVS, or wait for it via MQTT on first boot.
 *
 * Must be called after WiFi and MQTT are connected, and after the MQTT client
 * has subscribed to the device topic so it can receive the initial config.
 */
void get_config(void);
