#include <stdio.h>

#include "config.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "mqtt_driver.h"

void get_config(void)
{
    nvs_handle_t nvs_hd;
    nvs_open("configuration", NVS_READWRITE, &nvs_hd);

    size_t required_size = 0;
    esp_err_t err = nvs_get_str(nvs_hd, "name", NULL, &required_size); // First gets the string size
    if (err == ESP_OK){
        // We have a name, load all data and continue normally
        printf("Got name, loading configuration\n");
        // Load from NVS to memory
        nvs_get_str(nvs_hd, "name", config.name, &required_size);
        nvs_get_u16(nvs_hd, "sense_period", &config.sense_period);
        nvs_get_u16(nvs_hd, "par_setpoint", &config.par_setpoint);
        snprintf(config.topic, sizeof(config.topic), "nursery/%s", config.name);
        nvs_close(nvs_hd);
    } else {
        // We don't have a name, first boot. Wait for MQTT data
        
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
