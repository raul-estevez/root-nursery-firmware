#include "mqtt_driver.h"
#include "config.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "cJSON.h"
#include "nvs_flash.h"

#include <stdio.h>
// #include "freertos/event_groups.h"

static esp_mqtt_client_handle_t mqtt_client;
static EventGroupHandle_t mqtt_event_group;

nursery_config_t config = {
    .name         = "default",
    .topic        = "nursery/default",
    .sense_period = 10,
    .par_setpoint = 0,
};

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        printf("MQTT connected!\n");
        esp_mqtt_client_subscribe(client, config.topic, 0);
        xEventGroupSetBits(mqtt_event_group, MQTT_CONNECTED_BIT);
        break;
    case MQTT_EVENT_DISCONNECTED:
        printf("MQTT disconnected\n");
        xEventGroupClearBits(mqtt_event_group, MQTT_CONNECTED_BIT);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        printf("Subscribed\n");
        xEventGroupSetBits(mqtt_event_group, MQTT_SUBSCRIBED_BIT);
        break;
    case MQTT_EVENT_DATA:
        printf("Received data\n");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        char *payload = strndup(event->data, event->data_len);
        cJSON *root = cJSON_Parse(payload);

        cJSON *name         = cJSON_GetObjectItem(root, "name");
        cJSON *sense_period = cJSON_GetObjectItem(root, "sense_period");
        cJSON *par_setpoint = cJSON_GetObjectItem(root, "par_setpoint");

        if (cJSON_IsString(name) && name->valuestring != NULL) {
            strncpy(config.name, name->valuestring, sizeof(config.name) - 1);
        }
        if (cJSON_IsNumber(sense_period)) {
            config.sense_period = sense_period->valueint;
        }
        if (cJSON_IsNumber(par_setpoint)) {
            config.par_setpoint = par_setpoint->valueint;
        }

        // Change the topic
        esp_mqtt_client_unsubscribe(client, config.topic);
        snprintf(config.topic, sizeof(config.topic), "nursery/%s", config.name);
        esp_mqtt_client_subscribe(client, config.topic, 0);

        cJSON_Delete(root);
        free(payload);

        // Save current config on the NVS
        nvs_handle_t nvs_hd;
        nvs_open("configuration", NVS_READWRITE, &nvs_hd);
        nvs_set_str(nvs_hd, "name", config.name);
        nvs_set_u16(nvs_hd, "sense_period", config.sense_period);
        nvs_set_u16(nvs_hd, "par_setpoint", config.par_setpoint);
        nvs_commit(nvs_hd);
        nvs_close(nvs_hd);

        xEventGroupSetBits(mqtt_event_group, MQTT_DATA_BIT);
        break;
    default:
        printf("DEFAULT\n");
        break;
    }
}

void mqtt_start(void)
{
    mqtt_event_group = xEventGroupCreate();
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URL,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
    printf("Publishing to: %s\n", config.topic);
}

void mqtt_publish(const char *payload)
{
    esp_mqtt_client_publish(mqtt_client, config.topic, payload, 0, 0, 0);
}

EventGroupHandle_t mqtt_get_event_group(void)
{
    return mqtt_event_group;
}
