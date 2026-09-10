#include <string.h>
#include <stdio.h>

#ifndef ESP_DEXCOM_H
#define ESP_DEXCOM_H

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

// const enum DEXCOM_TREND_DIRECTIONS {
//     None, // unconfirmed
//     DoubleUp,
//     SingleUp,
//     FortyFiveUp,
//     Flat,
//     FortyFiveDown,
//     SingleDown,
//     DoubleDown,
//     NotComputable,  // unconfirmed
//     RateOutOfRange, // unconfirmed
// };

// const enum TREND_DESCRIPTIONS {
//     NoDesc,
//     "rising quickly",
//     "rising",
//     "rising slightly",
//     "steady",
//     "falling slightly",
//     "falling",
//     "falling quickly",
//     "unable to determine trend",
//     "trend unavailable",
// };

const int DEXCOM_MAX_MINUTES = 1440; // 24 hours
const int DEXCOM_MAX_READINGS = 288;
const float DEXCOM_MGDL_TO_MMOLL = 0.0555;

// 

void func(void);

#endif // ESP_DEXCOM_H