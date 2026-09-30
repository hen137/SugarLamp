#include <stdio.h>

#include "esp_log.h"

#include "esp_tls.h"
#include "esp_http_client.h"

#include "cJSON.h"

#include "esp_dexcom.h"

#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char *DEXCOM_TAG = "dexcom";

const char DEXCOM_APPLICATION_ID_US[] = "d89443d2-327c-4a6f-89e5-496bbb0317db";
const char DEXCOM_APPLICATION_ID_OUS[] = "d89443d2-327c-4a6f-89e5-496bbb0317db";
const char DEXCOM_APPLICATION_ID_JP[] = "d8665ade-9673-4e27-9ff6-92db4ce13d13";

const char DEXCOM_BASE_URL[] = "share2.dexcom.com/ShareWebServices/Services/";
const char DEXCOM_BASE_URL_OUS[] = "shareous1.dexcom.com/ShareWebServices/Services/";
const char DEXCOM_BASE_URL_JP[] = "share.dexcom.jp/ShareWebServices/Services/";

const char DEXCOM_LOGIN_ID_ENDPOINT[] = "General/LoginPublisherAccountById";
const char DEXCOM_AUTHENTICATE_ENDPOINT[] = "General/AuthenticatePublisherAccount";
const char DEXCOM_GLUCOSE_READINGS_ENDPOINT[] = "Publisher/ReadPublisherLatestGlucoseValues";

const int DEXCOM_MAX_MINUTES = 1440; // 24 hours
const int DEXCOM_MAX_READINGS = 288;
const float DEXCOM_MGDL_TO_MMOLL = 0.0555;

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_HEADERS_COMPLETE:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_ON_HEADERS_COMPLETE");
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        // Clean the buffer in case of a new request
        if (output_len == 0 && evt->user_data)
        {
            // we are just starting to copy the output data into the use
            memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
        }
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
         *  However, event handler can also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the buffer
            int copy_len = 0;
            if (evt->user_data)
            {
                // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                if (copy_len)
                {
                    memcpy(evt->user_data + output_len, evt->data, copy_len);
                }
            }
            else
            {
                int content_len = esp_http_client_get_content_length(evt->client);
                if (output_buffer == NULL)
                {
                    // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore should be null terminated.
                    output_buffer = (char *)calloc(content_len + 1, sizeof(char));
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(DEXCOM_TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                copy_len = MIN(evt->data_len, (content_len - output_len));
                if (copy_len)
                {
                    memcpy(output_buffer + output_len, evt->data, copy_len);
                }
            }
            output_len += copy_len;
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(DEXCOM_TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(DEXCOM_TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(DEXCOM_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(DEXCOM_TAG, "HTTP_EVENT_REDIRECT");
        esp_http_client_set_header(evt->client, "From", "user@example.com");
        esp_http_client_set_header(evt->client, "Accept", "text/html");
        esp_http_client_set_redirection(evt->client);
        break;
    default:
        break;
    }
    return ESP_OK;
}

// Executes "Login" request to Dexcom Share API and returns session ID
static char *_get_session_id(char *account_id, char *password, enum REGIONS region)
{
    char *url = malloc(256);
    sprintf(url, "https://%s%s", _get_base_url(region), DEXCOM_LOGIN_ID_ENDPOINT);

    char *post_data = malloc(256);
    sprintf(post_data, "{\"accountId\":\"%s\",\"password\":\"%s\",\"applicationId\":\"%s\"}", account_id, password, _get_application_id(region));
    
    // TODO: handle response -> extract session ID
    _post(url, NULL, post_data);

    free(url);
    free(post_data);

    // For demonstration purposes, returning a dummy session ID
    return strdup("dummy_session_id");
}

static const char *_get_application_id(enum REGIONS region)
{
    switch (region)
    {
    case US:
        return DEXCOM_APPLICATION_ID_US;
    case OUS:
        return DEXCOM_APPLICATION_ID_OUS;
    case JP:
        return DEXCOM_APPLICATION_ID_JP;
    default:
        ESP_LOGE(DEXCOM_TAG, "Invalid region specified");
        return NULL;
    }
}

static const char *_get_base_url(enum REGIONS region)
{
    switch (region)
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

static void _post(char *url, char *params, char *post_data)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};

    esp_http_client_config_t config = {
        .url = url,
        .query = params,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
        .cert_pem = dexcom_share_root_cert_pem_start,
    };
    esp_http_client_handle_t http_client = esp_http_client_init(&config);

    esp_http_client_set_method(http_client, HTTP_METHOD_POST);
    esp_http_client_set_header(http_client, "Accept-Encoding", "application/json");
    esp_http_client_set_header(http_client, "Content-Type", "application/json");
    esp_http_client_set_post_field(http_client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(http_client);

    if (err == ESP_OK)
    {
        ESP_LOGI(DEXCOM_TAG, "HTTP POST Status = %d, content_length = %" PRId64,
                 esp_http_client_get_status_code(http_client),
                 esp_http_client_get_content_length(http_client));
    }
    else
    {
        ESP_LOGE(DEXCOM_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    ESP_LOG_BUFFER_CHAR(DEXCOM_TAG, local_response_buffer, strlen(local_response_buffer));

    esp_http_client_cleanup(http_client);
}

float convert_mgdl_to_mmoll(float mgdl)
{
    return mgdl * DEXCOM_MGDL_TO_MMOLL;
}

float convert_mmoll_to_mgdl(float mmoll)
{
    return mmoll / DEXCOM_MGDL_TO_MMOLL;
}

dexcom_handle_t init_dexcom(dexcom_config_t *config)
{
    ESP_LOGI(DEXCOM_TAG, "Initializing Dexcom session for user: %s", config->username);

    return (dexcom_handle_t){
        .session_id = _get_session_id(config->account_id, config->password, config->region),
        .application_id = _get_application_id(config->region),
        .base_url = _get_base_url(config->region),
    };
}

dexcom_glucose_reading_t get_latest_glucose_reading(dexcom_handle_t *handle, int minutes, int max_count)
{
    ESP_LOGI(DEXCOM_TAG, "Fetching latest glucose reading for session: %s", handle->session_id);
    // Implementation for fetching the latest glucose reading

    // validate session

    // new session if expired/empty

    char *url = malloc(256);
    sprintf(url, "https://%s%s", handle->base_url, DEXCOM_GLUCOSE_READINGS_ENDPOINT);

    char *post_data = malloc(256);
    sprintf(post_data, "{\"sessionId\":\"%s\",\"minutes\":\"%d\",\"maxCount\":\"%d\"}", handle->session_id, minutes, max_count);
    
    _post(url, NULL, post_data);

    free(url);
    free(post_data);

    return (dexcom_glucose_reading_t){
        .glucose_value = 0.0f,
        .units = MGDL,
        .trend_direction = None,
        .trend_description = NoDesc,
        .timestamp = NULL,
    };
}

void dummy_fetch()
{
    char *account_id = "your_account_id";
    char *password = "your_password";
    enum REGIONS region = US;

    char *url = malloc(256);
    sprintf(url, "https://%s%s", _get_base_url(region), DEXCOM_LOGIN_ID_ENDPOINT);

    char *post_data = malloc(256);
    sprintf(post_data, "{\"accountId\":\"%s\",\"password\":\"%s\",\"applicationId\":\"%s\"}", account_id, password, _get_application_id(region));
    
    _post(url, NULL, post_data);

    free(url);
    free(post_data);
}