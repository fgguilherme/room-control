#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_pthread.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "cJSON.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_event.h"

#include "driver/gpio.h"

#include "wifi_manager.h"

#include "mqtt_client.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

typedef struct {
    bool ac1;
    bool ac2;
    bool ac3;
    bool ac4;
    bool ac5;
    bool ac6;
    bool ac7;
    bool ac8;
} power_t;

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif