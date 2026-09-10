#include "stdbool.h"
#include "esp_err.h"

void init_NVS(void);

esp_err_t app_wifi_init(void);
esp_err_t app_wifi_start(void);

void fetch(void);