#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "sdkconfig.h"
    

// ── I2C ──────────────────────────────────────────────────────────────────────
#define I2C_PORT            I2C_NUM_0
#define I2C_SDA_IO          CONFIG_I2C_MASTER_SDA
#define I2C_SCL_IO          CONFIG_I2C_MASTER_SCL
#define I2C_FREQ_HZ         100000

// ── WiFi ─────────────────────────────────────────────────────────────────────
#define WIFI_SSID           CONFIG_WIFI_SSID
#define WIFI_PASS           CONFIG_WIFI_PASSWORD
#define WIFI_MAX_RETRY      CONFIG_MAX_RETRY
#define WIFI_CONNECT_TIMEOUT_MS  10000

// ── MQTT ─────────────────────────────────────────────────────────────────────
#define MQTT_BROKER_URL     CONFIG_BROKER_URL
#define MQTT_TOPIC          CONFIG_MQTT_TOPIC_DATA
#define MQTT_CONNECT_TIMEOUT_MS  10000


// ── Event group bits ─────────────────────────────────────────────────────────
#define WIFI_CONNECTED_BIT  (1 << 0)
#define WIFI_FAIL_BIT       (1 << 1)
#define MQTT_CONNECTED_BIT  (1 << 0)
#define MQTT_DATA_BIT       (1 << 1)
#define MQTT_SUBSCRIBED_BIT (1 << 2)

typedef struct {
    char name[16];
    char topic[32];         // nursery/<name>
    uint16_t  sense_period; // Seconds
    uint16_t  par_setpoint; // umol/m2/s
} nursery_config_t;

extern nursery_config_t config;

void get_config(void);

