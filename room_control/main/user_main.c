/*
@file user_main.c
@author Guilherme Guimarães
@brief Entry point for the ESP32 application.
@see https://idyl.io
@see https://github.com/tonyp7/esp32-wifi-manager
*/

#include "rc.h"

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

/* @brief tag used for ESP serial console messages */
static const char *TAG = "MQTT_FISHMONITOR";

esp_mqtt_client_handle_t client;


power_t power = {
    .ac1 = 1,
    .ac2 = 1
};

// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// ###########################################     WIFI_CONFIG     #########################################
// #########################################################################################################
// #########################################################################################################
// #########################################################################################################


/**
 * @brief RTOS task that periodically prints the heap memory available.
 * @note Pure debug information, should not be ever started on production code! This is an example on how you can integrate your code with wifi-manager
 */
void monitoring_task(void *pvParameter)
{
	for(;;){
		ESP_LOGI(TAG, "free heap: %d",esp_get_free_heap_size());
		vTaskDelay( pdMS_TO_TICKS(10000) );
	}
}


/**
 * @brief this is an exemple of a callback that you can setup in your own app to get notified of wifi manager event.
 */
void cb_connection_ok(void *pvParameter){
	ip_event_got_ip_t* param = (ip_event_got_ip_t*)pvParameter;

	/* transform IP to human readable string */
	char str_ip[16];
	esp_ip4addr_ntoa(&param->ip_info.ip, str_ip, IP4ADDR_STRLEN_MAX);

	ESP_LOGI(TAG, "I have a connection and my IP is %s!", str_ip);
}

void init_wifi(){
	/* start the wifi manager */
	wifi_manager_start();

	/* register a callback as an example to how you can integrate your code with the wifi manager */
	wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &cb_connection_ok);
}


// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// ###########################################    SENSORS UPDATE   #########################################
// #########################################################################################################
// #########################################################################################################
// #########################################################################################################

void update_relay(){
    gpio_set_level(CONFIG_PIN_RELAY_BAR_1, power.ac1);
    gpio_set_level(CONFIG_PIN_RELAY_BAR_2, power.ac2);
    gpio_set_level(CONFIG_PIN_RELAY_BAR_3, power.ac3);
    gpio_set_level(CONFIG_PIN_RELAY_BAR_4, power.ac4);
    gpio_set_level(CONFIG_PIN_RELAY_BAR_5, power.ac5);
    gpio_set_level(CONFIG_PIN_RELAY_BAR_6, power.ac6);
    gpio_set_level(CONFIG_PIN_RELAY_BAR_7, power.ac7);
    gpio_set_level(CONFIG_PIN_RELAY_BAR_8, power.ac8);
    ESP_LOGI(TAG, "Updated Power 1:%d", power.ac1);
    ESP_LOGI(TAG, "Updated Power 2:%d", power.ac2);
    ESP_LOGI(TAG, "Updated Power 3:%d", power.ac3);
    ESP_LOGI(TAG, "Updated Power 4:%d", power.ac4);
    ESP_LOGI(TAG, "Updated Power 5:%d", power.ac5);
    ESP_LOGI(TAG, "Updated Power 6:%d", power.ac6);
    ESP_LOGI(TAG, "Updated Power 7:%d", power.ac7);
    ESP_LOGI(TAG, "Updated Power 8:%d", power.ac8);
}


// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// ###########################################     MQTT_CONFIG     #########################################
// #########################################################################################################
// #########################################################################################################
// #########################################################################################################


static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            esp_mqtt_client_subscribe(client, "fgguilherme/control/#", 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            char topic[30];
            sprintf(topic,"%.*s", event->topic_len, event->topic);
            ESP_LOG_BUFFER_CHAR(TAG, event->topic, event->topic_len);
            ESP_LOG_BUFFER_CHAR(TAG, event->data, event->data_len);
            if(strcmp(topic,"fgguilherme/control/power")==0){
                cJSON *root = cJSON_Parse(event->data);
                cJSON *ac1=cJSON_GetObjectItem(root,"ac1");
                cJSON *ac2=cJSON_GetObjectItem(root,"ac2");
                cJSON *ac3=cJSON_GetObjectItem(root,"ac3");
                cJSON *ac4=cJSON_GetObjectItem(root,"ac4");
                cJSON *ac5=cJSON_GetObjectItem(root,"ac5");
                cJSON *ac6=cJSON_GetObjectItem(root,"ac6");
                cJSON *ac7=cJSON_GetObjectItem(root,"ac7");
                cJSON *ac8=cJSON_GetObjectItem(root,"ac8");

                if (ac1) power.ac1 = ac1->valueint;
                if (ac2) power.ac2 = ac2->valueint;   
                if (ac3) power.ac3 = ac3->valueint;
                if (ac4) power.ac4 = ac4->valueint;
                if (ac5) power.ac5 = ac5->valueint;
                if (ac6) power.ac6 = ac6->valueint;
                if (ac7) power.ac7 = ac7->valueint;
                if (ac8) power.ac8 = ac8->valueint;             
            }
            
            update_relay();
            
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) 
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}


// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// ###############################################     MAIN     ############################################
// #########################################################################################################
// #########################################################################################################
// #########################################################################################################

void app_main()
{

    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = ((1ULL<<CONFIG_PIN_RELAY_BAR_1) | 
                            (1ULL<<CONFIG_PIN_RELAY_BAR_2) | 
                            (1ULL<<CONFIG_PIN_RELAY_BAR_3) | 
                            (1ULL<<CONFIG_PIN_RELAY_BAR_4) | 
                            (1ULL<<CONFIG_PIN_RELAY_BAR_5) | 
                            (1ULL<<CONFIG_PIN_RELAY_BAR_6) | 
                            (1ULL<<CONFIG_PIN_RELAY_BAR_7) | 
                            (1ULL<<CONFIG_PIN_RELAY_BAR_8));
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

	init_wifi();

	mqtt_app_start();
	/* your code should go here. Here we simply create a task on core 2 that monitors free heap memory */
	xTaskCreatePinnedToCore(&monitoring_task, "monitoring_task", 2048, NULL, 1, NULL, 1);
}
