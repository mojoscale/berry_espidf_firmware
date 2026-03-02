#include "wifi_manager.h"
#include "esp_log.h"

static const char* TAG = "WifiManager";

WifiManager::WifiManager(IEventSink* sink)
    : eventSink(sink)
{
}

void WifiManager::init()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register WiFi + IP events
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &WifiManager::wifiEventHandler,
        this));

    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &WifiManager::wifiEventHandler,
        this));
}

void WifiManager::start()
{
    char ssid[32] = {0};
    char pass[64] = {0};

    if (!loadCredentials(ssid, sizeof(ssid), pass, sizeof(pass))) {
        ESP_LOGI(TAG, "No credentials found. Starting provisioning.");
        startProvisioningMode();
        return;
    }

    connectSTA(ssid, pass);
}

bool WifiManager::loadCredentials(char* ssid, size_t ssid_len,
                                   char* pass, size_t pass_len)
{
    nvs_handle_t handle;

    if (nvs_open("wifi", NVS_READONLY, &handle) != ESP_OK)
        return false;

    size_t s_len = ssid_len;
    size_t p_len = pass_len;

    esp_err_t err1 = nvs_get_str(handle, "ssid", ssid, &s_len);
    esp_err_t err2 = nvs_get_str(handle, "pass", pass, &p_len);

    nvs_close(handle);

    return (err1 == ESP_OK);
}

void WifiManager::saveCredentials(const char* ssid, const char* pass)
{
    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open("wifi", NVS_READWRITE, &handle));

    ESP_ERROR_CHECK(nvs_set_str(handle, "ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(handle, "pass", pass));
    ESP_ERROR_CHECK(nvs_commit(handle));

    nvs_close(handle);
}

void WifiManager::connectSTA(const char* ssid, const char* pass)
{
    eventSink->postEvent(EventType::WifiConnecting);

    wifi_config_t config = {};
    strcpy((char*)config.sta.ssid, ssid);
    strcpy((char*)config.sta.password, pass);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void WifiManager::startProvisioningMode()
{
    eventSink->postEvent(EventType::WifiProvisioningStarted);

    // You will handle serial externally.
    // When credentials arrive, call provision().
}

void WifiManager::provision(const char* ssid, const char* pass)
{
    ESP_LOGI(TAG, "Provisioning credentials received");

    saveCredentials(ssid, pass);

    eventSink->postEvent(EventType::WifiProvisioningCompleted);

    connectSTA(ssid, pass);
}

void WifiManager::wifiEventHandler(void* arg,
                                   esp_event_base_t base,
                                   int32_t id,
                                   void* data)
{
    auto* self = static_cast<WifiManager*>(arg);
    self->handleWifiEvent(base, id, data);
}

void WifiManager::handleWifiEvent(esp_event_base_t base,
                                  int32_t id,
                                  void* data)
{
    if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WiFi Connected");
        eventSink->postEvent(EventType::WifiConnected);
        return;
    }

    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {

        auto* evt = (wifi_event_sta_disconnected_t*)data;

        if (evt->reason == WIFI_REASON_AUTH_FAIL) {
            ESP_LOGI(TAG, "WiFi Auth Failed");
            eventSink->postEvent(EventType::WifiBadCredentials);
        } else {
            ESP_LOGI(TAG, "WiFi Disconnected");
            eventSink->postEvent(EventType::WifiDisconnected);
        }
    }
}