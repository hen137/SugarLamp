#include <stdio.h>

#include "esp_log.h"

#include "esp_tls.h"
#include "esp_http_client.h"

#include "esp_dexcom.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char *DEXCOM_TAG = "dexcom";

// Executes "Login" request to Dexcom Share API and returns session ID
static char *_get_session_id(const char *account_id, const char *password, const region_t *region)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};

    esp_http_client_config_t http_config = {
        .url = _get_base_url(region),
        .path = DEXCOM_LOGIN_ID_ENDPOINT,
        // .event_handler = _http_event_handler,
        // .timeout_ms = 5000,
        .user_data = local_response_buffer, // Pass address of local buffer to get response
        // .disable_auto_redirect = true,
        // .transport_type = HTTP_TRANSPORT_OVER_SSL,
        // .crt_bundle_attach = esp_crt_bundle_attach
    };
    ESP_LOGI(DEXCOM_TAG, "HTTP request with url =>");
    esp_http_client_handle_t client = esp_http_client_init(&http_config);

    char *post_data = malloc(256);
    sprintf(post_data, "{\"accountId\":\"%s\",\"password\":\"%s\",\"applicationId\":\"%s\"}", account_id, password, _get_application_id(region));
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(DEXCOM_TAG, "HTTP POST Status = %d, content_length = %" PRId64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(DEXCOM_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    ESP_LOG_BUFFER_HEX(DEXCOM_TAG, local_response_buffer, strlen(local_response_buffer));

    esp_http_client_cleanup(client);

    return strdup(local_response_buffer);
}

static const char *_get_application_id(const region_t *region)
{
    switch (*region)
    {
    case US:
        return DEXCOM_APPLICATION_ID_US; // Example application ID for US
    case OUS:
        return DEXCOM_APPLICATION_ID_OUS; // Example application ID for OUS
    case JP:
        return DEXCOM_APPLICATION_ID_JP; // Example application ID for JP
    default:
        ESP_LOGE(DEXCOM_TAG, "Invalid region specified");
        return NULL;
    }
}

static const char *_get_base_url(const region_t *region)
{
    switch (*region)
    {
    case US:
        return DEXCOM_BASE_URL;
    case OUS:
        return DEXCOM_BASE_URL_OUS;
    case JP:
        return DEXCOM_BASE_URL_JP;
    default:
        ESP_LOGE(DEXCOM_TAG, "Invalid region specified");
        return NULL;
    }
}

dexcom_handle_t init_dexcom(const dexcom_config_t *config)
{
    ESP_LOGI(DEXCOM_TAG, "Initializing Dexcom session for user: %s", config->username);

    return (dexcom_handle_t){
        .session_id = _get_session_id(config->account_id, config->password, config->region),
        .application_id = _get_application_id(config->region),
        .base_url = _get_base_url(config->region),
    };
}

dexcom_glucose_reading_t get_latest_glucose_reading(const dexcom_handle_t *handle)
{
    ESP_LOGI(DEXCOM_TAG, "Fetching latest glucose reading for session: %s", handle->session_id);
    // Implementation for fetching the latest glucose reading

    // validate session

    // new session if expired/empty

    return (dexcom_glucose_reading_t){
        .glucose_value = 0.0f,
        .units = MGDL,
        .timestamp = NULL,
    };
}