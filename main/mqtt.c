    #include "mqtt.h"
    #include "DHT.h"
    #include "led_lib.h"

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t global_client;
//static mqtt_data_pt_t mqtt_data_pt = NULL;
uint32_t MQTT_CONNECTED = 0;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNECTED = 1;

        msg_id = esp_mqtt_client_subscribe(client, "esp_sub", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNECTED = 0;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //DHT data:
        int ret = readDHT();
        if (ret == DHT_OK) {
            float temperature = getTemperature();
            float humidity = getHumidity();
            ESP_LOGI(TAG, "Temperature: %.1f, Humidity: %.1f", temperature, humidity);

            char *json_str = convert_model_sensor_to_json(temperature, humidity);
            ESP_LOGI(TAG,"%s",json_str);
            if (json_str != NULL) {
                msg_id = esp_mqtt_client_publish(client, "/sensor/data", json_str, 0, 0, 0);
                ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
                free(json_str);
            } else {
                ESP_LOGE(TAG, "Failed to create JSON string");
            }
        } else {
            errorHandler(ret);
        }

        //LED DATA:
        if (getState() == 1 || getState() == 0) {
        ESP_LOGI(TAG, "=== SIGNAL LED ===");
        int signal_led = getState();
        ESP_LOGI(TAG, "Status: %d", signal_led);

        char *json_state = convert_model_signaldiv_to_json(signal_led);
        ESP_LOGI(TAG, "%s", json_state);
        if (json_state != NULL) {
            msg_id = esp_mqtt_client_publish(client, "led_status/status", json_state, 0, 0, 0);
            ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);
            free(json_state);
        } else {
            ESP_LOGE(TAG, "Failed to create JSON string");
        }
    }

        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        //event->data[event->data_len] = '\0';
        //mqtt_data_pt(event->data, event->data_len);// thực hiện hàm get_data_call_back
                                                    // mqtt_data_callback_: gán địa chỉ = get_data_call_back
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_mqtt_client_handle_t get_mqtt_client_handle(void)
{
    return global_client;
}

//DHT convert:
char *convert_model_sensor_to_json(float temperature, float humidity)
{
    // create a new cJSON object
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        printf("Error: Failed to create cJSON object\n");
        return NULL;
    }
    // modify the JSON data
    char temp_str[10];
    char hum_str[10];
    snprintf(temp_str, sizeof(temp_str), "%.1f", temperature);
    snprintf(hum_str, sizeof(hum_str), "%.1f", humidity);

    cJSON_AddStringToObject(json, "temp", temp_str);
    cJSON_AddStringToObject(json, "hum", hum_str);

    // convert the cJSON object to a JSON string
    char *json_str = cJSON_PrintUnformatted(json);

    // free the JSON string and cJSON object
    cJSON_Delete(json);

    return json_str;
}

//LED signal convert:
char *convert_model_signaldiv_to_json(int signal)
{
    // create a new cJSON object
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        printf("Error: Failed to create cJSON object");
        return NULL;
    }
    // modify the JSON data
    char ledstate_str[10];
    if(signal == 1) snprintf(ledstate_str, sizeof(ledstate_str), "Bật");
    else snprintf(ledstate_str, sizeof(ledstate_str), "Tắt");

    cJSON_AddStringToObject(json, "state", ledstate_str);
    // convert the cJSON object to a JSON string
    char *json_state = cJSON_PrintUnformatted(json);
    // free the JSON object (not the string)
    cJSON_Delete(json);

    return json_state;
}
//#################################


/*void mqtt_data_publish_update()
{

    esp_mqtt_client_handle_t client = get_mqtt_client_handle();
    if (client != NULL)
    {
        char json_string[20]; // Đảm bảo bộ đệm đủ lớn
        sprintf(json_string, "{\"update\": true}"); // Sử dụng sprintf để định d
        // Gửi dữ liệu lên broker MQTT với chủ đề là "data"
        int msg_id = esp_mqtt_client_publish(client, "esp_pub", json_string, 0, 0, 0);
        if (msg_id < 0)
        {
            ESP_LOGE(TAG, "Failed to publish data to MQTT broker");
        }
        else
        {
            ESP_LOGI(TAG, "Published data to MQTT broker, msg_id=%d", msg_id);
        }
    }
    else
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
    }
}*/

void mqtt_app_start()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = EXAMPLE_ESP_MQQT_BORKER_URI,
        .broker.address.port = EXAMPLE_ESP_MQQT_BORKER_PORT,
        //.broker.address.transport = EXAMPLE_ESP_MQQT_BORKER_TRANSPORT,
        .credentials.username = EXAMPLE_ESP_MQQT_CREDENTIALS_USERNAME,
    };

    global_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(global_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(global_client);
}
// callback:
/*void mqtt_data_pt_set_callback(mqtt_data_pt_t mqtt_func_ptr)
{
    mqtt_data_pt = mqtt_func_ptr;
}*/
