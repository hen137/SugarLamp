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

    const char DEXCOM_APPLICATION_ID_US[] = "d89443d2-327c-4a6f-89e5-496bbb0317db";
    const char DEXCOM_APPLICATION_ID_OUS[] = "d89443d2-327c-4a6f-89e5-496bbb0317db";
    const char DEXCOM_APPLICATION_ID_JP[] = "d8665ade-9673-4e27-9ff6-92db4ce13d13";

    const char DEXCOM_BASE_URL[] = "https://share2.dexcom.com/ShareWebServices/Services";
    const char DEXCOM_BASE_URL_OUS[] = "https://shareous1.dexcom.com/ShareWebServices/Services/";
    const char DEXCOM_BASE_URL_JP[] = "https://share.dexcom.jp/ShareWebServices/Services/";

    const char DEXCOM_LOGIN_ID_ENDPOINT[] = "General/LoginPublisherAccountById";
    const char DEXCOM_AUTHENTICATE_ENDPOINT[] = "General/AuthenticatePublisherAccount";
    const char DEXCOM_GLUCOSE_READINGS_ENDPOINT[] = "Publisher/ReadPublisherLatestGlucoseValues";

    /**
     * @brief Enumerates the trend directions for Dexcom glucose readings
     * 
     */
    const enum DEXCOM_TREND_DIRECTIONS {
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
    const enum TREND_DESCRIPTIONS {
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

    const int DEXCOM_MAX_MINUTES = 1440; // 24 hours
    const int DEXCOM_MAX_READINGS = 288;
    const float DEXCOM_MGDL_TO_MMOLL = 0.0555;

    /**
     * @brief Enumerates the units for glucose values
     * 
     */
    typedef enum
    {
        MGDL, /*!< Milligrams per deciliter */
        MMOLL, /*!< Millimoles per liter */
    } glucose_units_t;

    /**
     * @brief Enumerates the regions for Dexcom services
     * 
     */
    typedef enum
    {
        US, /*!< United States */
        OUS, /*!< Outside United States */
        JP, /*!< Japan */
    } region_t;

    /**
     * @brief Dexcom Settings Configuration Structure
     * 
     */
    typedef struct
    {
        const char *username; /*!< Username for Dexcom account */
        const char *account_id; /*!< Account ID for Dexcom account */
        const char *password; /*!< Password for Dexcom account */
        const region_t *region; /*< Region for Dexcom services */
    } dexcom_config_t;

    /**
     * @brief Dexcom Handle
     * 
     */
    typedef struct
    {
        const char *session_id; /*!< Session ID for Dexcom services */
        const char *application_id; /*!< Application ID for Dexcom services */
        const char *base_url; /*!< Base URL for Dexcom services */
    } dexcom_handle_t;

    /**
     * @brief Dexcom Glucose Reading Structure
     * 
     */
    typedef struct
    {
        float glucose_value; /*!< Glucose value */
        glucose_units_t units; /*!< Units for glucose value */
        int trend_direction; /*!< Trend direction */
        const char *trend_description; /*!< Description of the trend */
        const char *timestamp; /*!< Timestamp for the glucose reading */
    } dexcom_glucose_reading_t;

    /**
     * @brief Gets the session ID for a Dexcom account
     * 
     * @param account_id 
     * @param password 
     * @param region 
     * @return char* 
     */
    static char *_get_session_id(const char *account_id, const char *password, const region_t *region);

    /**
     * @brief Gets the application ID for a Dexcom region
     * 
     * @param region 
     * @return const char* 
     */
    static const char *_get_application_id(const region_t *region);

    /**
     * @brief Gets the base URL for a Dexcom region
     * 
     * @param region 
     * @return const char* 
     */
    static const char *_get_base_url(const region_t *region);

    /**
     * @brief Initializes a Dexcom handle with the provided configuration
     * 
     * @param config 
     * @return dexcom_handle_t 
     */
    dexcom_handle_t init_dexcom(const dexcom_config_t *config);

    /**
     * @brief Get the latest glucose reading object
     * 
     * @param handle 
     * @return dexcom_glucose_reading_t 
     */
    dexcom_glucose_reading_t get_latest_glucose_reading(const dexcom_handle_t *handle);

#ifdef __cplusplus
}
#endif