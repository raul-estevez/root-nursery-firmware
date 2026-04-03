/*
 * config.c
 *
 * Loads device configuration from non-volatile storage (NVS) on boot.
 *
 * On first boot the NVS partition contains no "name" key. In that case
 * get_config() blocks indefinitely, waiting for a configuration payload to
 * arrive over MQTT. Once received, the values are persisted to NVS so that
 * subsequent boots skip the MQTT wait.
 */

#include <stdio.h>

#include "config.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "mqtt_driver.h"

/* Global config instance, shared with all other modules via config.h.
 * Default values are applied here; they are replaced by NVS or MQTT data
 * during get_config(). */
nursery_config_t config = {
    .name         = "default",
    .topic        = "nursery/default",
    .sense_period = 10,
    .par_setpoint = 0,
};

void get_config(void)
{
    nvs_handle_t nvs_hd;
    nvs_open("configuration", NVS_READWRITE, &nvs_hd);

    /* Probe for the "name" key to detect whether this is the first boot.
     * nvs_get_str with a NULL buffer returns only the required size. */
    size_t required_size = 0;
    esp_err_t err = nvs_get_str(nvs_hd, "name", NULL, &required_size);
    if (err == ESP_OK){
        /* Configuration already exists in NVS; load it directly. */
        printf("Got name, loading configuration\n");
        nvs_get_str(nvs_hd, "name", config.name, &required_size);
        nvs_get_u16(nvs_hd, "sense_period", &config.sense_period);
        nvs_get_u16(nvs_hd, "par_setpoint", &config.par_setpoint);
        snprintf(config.topic, sizeof(config.topic), "nursery/%s", config.name);
        nvs_close(nvs_hd);
    } else {
        /* First boot: block until the MQTT handler populates config from an
         * incoming message, then persist the values to NVS. */
        printf("Waiting for configuration\n");
        xEventGroupWaitBits(mqtt_get_event_group(),
                            MQTT_DATA_BIT | MQTT_SUBSCRIBED_BIT,
                            pdTRUE, pdTRUE,
                            portMAX_DELAY);
        printf("Got data\n");
        nvs_set_str(nvs_hd, "name", config.name);
        nvs_set_u16(nvs_hd, "sense_period", config.sense_period);
        nvs_set_u16(nvs_hd, "par_setpoint", config.par_setpoint);
        nvs_commit(nvs_hd);
        nvs_close(nvs_hd);
        printf("Configuration saved\n");
    }

    printf("Configuration done\n");

    printf("%s\n", config.name);
    printf("%s\n", config.topic);
    printf("%d\n", config.sense_period);
    printf("%d\n", config.par_setpoint);
}
