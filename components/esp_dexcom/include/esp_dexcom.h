#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Dexcom Share Root Certificate
     *
     * openssl s_client -showcerts -connect share2.dexcom.com:443 </dev/null>
     */
    extern const char dexcom_share_root_cert_pem_start[] asm("_binary_dexcom_share_root_cert_pem_start");
    extern const char dexcom_share_root_cert_pem_end[] asm("_binary_dexcom_share_root_cert_pem_end");

    // Dexcom Share API

    extern const char *DEXCOM_APPLICATION_ID_US;
    extern const char *DEXCOM_APPLICATION_ID_OUS;
    extern const char *DEXCOM_APPLICATION_ID_JP;

    extern const char *DEXCOM_BASE_URL;
    extern const char *DEXCOM_BASE_URL_OUS;
    extern const char *DEXCOM_BASE_URL_JP;

    extern const char *DEXCOM_LOGIN_ID_ENDPOINT;
    extern const char *DEXCOM_AUTHENTICATE_ENDPOINT;
    extern const char *DEXCOM_GLUCOSE_READINGS_ENDPOINT;

    /**
     * @brief Enumerates the trend directions for Dexcom glucose readings
     *
     */
    enum DEXCOM_TREND_DIRECTIONS
    {
        None, // unconfirmed
        DoubleUp,
        SingleUp,
        FortyFiveUp,
        Flat,
        FortyFiveDown,
        SingleDown,
        DoubleDown,
        NotComputable,  // unconfirmed
        RateOutOfRange, // unconfirmed
    };

    /**
     * @brief Enumerates the descriptions for Dexcom glucose trend directions
     *
     */
    enum TREND_DESCRIPTIONS
    {
        NoDesc,
        RisingQuickly,
        Rising,
        RisingSlightly,
        Steady,
        FallingSlightly,
        Falling,
        FallingQuickly,
        UnableToDetermineTrend,
        TrendUnavailable,
    };

    extern const int DEXCOM_MAX_MINUTES;
    extern const int DEXCOM_MAX_READINGS;
    extern const float DEXCOM_MGDL_TO_MMOLL;

    /**
     * @brief Enumerates the units for glucose values
     *
     */
    enum GLUCOSE_UNITS
    {
        MGDL,  /*!< Milligrams per deciliter */
        MMOLL, /*!< Millimoles per liter */
    };

    /**
     * @brief Enumerates the regions for Dexcom services
     *
     */
    enum REGIONS
    {
        US,  /*!< United States */
        OUS, /*!< Outside United States */
        JP,  /*!< Japan */
    };

    /**
     * @brief Dexcom Settings Configuration Structure
     *
     */
    typedef struct
    {
        char *username;      /*!< Username for Dexcom account */
        char *account_id;    /*!< Account ID for Dexcom account */
        char *password;      /*!< Password for Dexcom account */
        enum REGIONS region; /*< Region for Dexcom services */
    } dexcom_config_t;

    /**
     * @brief Dexcom Handle
     *
     */
    typedef struct
    {
        char *session_id;           /*!< Session ID for Dexcom services */
        const char *application_id; /*!< Application ID for Dexcom services */
        const char *base_url;       /*!< Base URL for Dexcom services */
    } dexcom_handle_t;

    /**
     * @brief Dexcom Glucose Reading Structure
     *
     */
    typedef struct
    {
        float glucose_value;                          /*!< Glucose value */
        enum GLUCOSE_UNITS units;                     /*!< Units for glucose value */
        enum DEXCOM_TREND_DIRECTIONS trend_direction; /*!< Trend direction */
        enum TREND_DESCRIPTIONS trend_description;    /*!< Description of the trend */
        const char *timestamp;                        /*!< Timestamp for the glucose reading */
    } dexcom_glucose_reading_t;

    /**
     * @brief Gets the session ID for a Dexcom account
     *
     * @param account_id
     * @param password
     * @param region
     * @return char*
     */
    char *_get_session_id(char *account_id, char *password, enum REGIONS region);

    /**
     * @brief Gets the application ID for a Dexcom region
     *
     * @param region
     * @return const char*
     */
    const char *_get_application_id(enum REGIONS region);

    /**
     * @brief Gets the base URL for a Dexcom region
     *
     * @param region
     * @return const char*
     */
    const char *_get_base_url(enum REGIONS region);

    /**
     * @brief Executes a POST request to the specified URL with the given parameters and post data
     *
     * @param url
     * @param params
     * @param post_data
     * @return char*
     */
    char *_post(char *url, char *params, char *post_data);

    /**
     * @brief Converts glucose values from mg/dL to mmol/L
     *
     * @param mgdl
     * @return float
     */
    float convert_mgdl_to_mmoll(float mgdl);

    /**
     * @brief Converts glucose values from mmol/L to mg/dL
     *
     * @param mmoll
     * @return float
     */
    float convert_mmoll_to_mgdl(float mmoll);

    /**
     * @brief Initializes a Dexcom handle with the provided configuration
     *
     * @param config
     * @return dexcom_handle_t
     */
    dexcom_handle_t init_dexcom(dexcom_config_t *config);

    /**
     * @brief Get the latest glucose reading object
     *
     * @param handle
     * @return dexcom_glucose_reading_t
     */
    dexcom_glucose_reading_t get_latest_glucose_reading(dexcom_handle_t *handle, int minutes, int max_count);

    void dummy_fetch();

#ifdef __cplusplus
}
#endif