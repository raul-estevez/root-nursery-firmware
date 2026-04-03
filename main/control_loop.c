#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "mqtt_driver.h"
#include "aht20_driver.h"
#include "as7341_driver.h"
#include "led_controller.h"
#include "control_loop.h"
#include "config.h"
#include "cJSON.h"

static float current_duty = 4096.0f;

int8_t par_control_loop(float* error_ret)
{
    float error = 0.0f;

    for (uint16_t i = 0; i < PAR_MAX_ITER; i++) {
        float par = read_par();
        error = config.par_setpoint - par;

        if (fabsf(error) <= PAR_TOL) {
            *error_ret = error;
            return 1;
        }

        float adjustment = error * PAR_P_GAIN;
        if      (adjustment >  PAR_MAX_CHANGE) adjustment =  PAR_MAX_CHANGE;
        else if (adjustment < -PAR_MAX_CHANGE) adjustment = -PAR_MAX_CHANGE;

        current_duty += adjustment;
        if (current_duty < LED_DUTY_MIN) current_duty = LED_DUTY_MIN;
        if (current_duty > LED_DUTY_MAX) current_duty = LED_DUTY_MAX;

        led_set_duty((uint16_t)current_duty);
        // printf("%.4f\t %d\n",error, (uint16_t)current_duty);
    }

    *error_ret = error;
    return -1;
}

void execute_main_loop(void *params){
    (void) params;
    // printf("Starting loop\n");
    // ── Initial sensor readings ───────────────────────────────────────────────

    float temperature, humidity;
    aht20_read(&temperature, &humidity);
    // snprintf(payload, sizeof(payload), "%.2f\t%.2f", temperature, humidity);
    // mqtt_publish(payload);
    
    as7341_channels_spectral_data_t spectrum_raw = read_raw_spectrum();
    float par = compute_par(spectrum_raw);

    // float par = read_par();
    // snprintf(payload, sizeof(payload), "%.4f", par);
    // mqtt_publish(payload);

    // led_set_duty((uint16_t)current_duty);

    // current_duty = LED_DUTY_MAX / 2.0f; /* reset search point each cycle */
    float error;
    int8_t converged = par_control_loop(&error);

    // snprintf(payload, sizeof(payload), "%.4f\t%d", error, converged);

    cJSON *root = cJSON_CreateObject();

    cJSON_AddNumberToObject(root, "temperature", temperature);
    cJSON_AddNumberToObject(root, "humidity", humidity);
    cJSON_AddNumberToObject(root, "power", current_duty);
    cJSON_AddNumberToObject(root, "par", par);

    cJSON *spectrum_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(spectrum_json, "f1", spectrum_raw.f1);
    cJSON_AddNumberToObject(spectrum_json, "f2", spectrum_raw.f2);
    cJSON_AddNumberToObject(spectrum_json, "f3", spectrum_raw.f3);
    cJSON_AddNumberToObject(spectrum_json, "f4", spectrum_raw.f4);
    cJSON_AddNumberToObject(spectrum_json, "f5", spectrum_raw.f5);
    cJSON_AddNumberToObject(spectrum_json, "f6", spectrum_raw.f6);
    cJSON_AddNumberToObject(spectrum_json, "f7", spectrum_raw.f7);
    cJSON_AddNumberToObject(spectrum_json, "f8", spectrum_raw.f8);
    cJSON_AddNumberToObject(spectrum_json, "clear", spectrum_raw.clear);
    cJSON_AddNumberToObject(spectrum_json, "nir", spectrum_raw.nir);

    cJSON_AddItemToObject(root, "spectrum", spectrum_json);

    char *payload = cJSON_PrintUnformatted(root);
    mqtt_publish(payload);

    cJSON_free(payload);
    cJSON_Delete(root);  // also frees spectrum_json
}
