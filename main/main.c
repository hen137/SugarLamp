#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

    xTaskCreate(dexcom_fetch, "fetch_task", 8192, NULL, 5, NULL);
}
