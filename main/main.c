#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_dexcom.h"

#include "main.h"

static const char *MAIN_TAG = "app_main";

void init_NVS()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* NVS partition was truncated
         * and needs to be erased */
        ret = nvs_flash_erase();

        /* Retry nvs_flash_init */
        ret |= nvs_flash_init();
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Failed to init NVS");
        return;
    }
}

void app_main()
{
    init_NVS();
    app_wifi_init();

    app_wifi_start();

    dexcom_config_t config = {
        .username = "your_username",
        .account_id = "your_account_id",
        .password = "your_password",
        .region = US,
    };
    dexcom_handle_t handle = init_dexcom(&config);

    while (1)
    {
        dummy_fetch();
        // dexcom_glucose_reading_t reading = get_latest_glucose_reading(&handle, 10, 1);
        // ESP_LOGI(MAIN_TAG, "Latest glucose reading: %.2f %s, Trend: %d, Description: %d, Timestamp: %s",
        //          reading.glucose_value,
        //          reading.units == MGDL ? "mg/dL" : "mmol/L",
        //          reading.trend_direction,
        //          reading.trend_description,
        //          reading.timestamp);

        // get battery level

        // update LEDs

        // sleep for X seconds/minutes
    }
}
