#pragma once

#include <cstring>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "events.h"       // Your EventType enum
#include "event_sink.h"   // Your interface


class WifiManager {
public:
    explicit WifiManager(IEventSink* sink);

    void init();          // call once at boot
    void start();         // attempt connection
    void provision(const char* ssid, const char* pass);

private:
    bool loadCredentials(char* ssid, size_t ssid_len,
                         char* pass, size_t pass_len);

    void saveCredentials(const char* ssid, const char* pass);
    void connectSTA(const char* ssid, const char* pass);
    void startProvisioningMode();

    void handleWifiEvent(esp_event_base_t base,
                         int32_t id,
                         void* data);

    static void wifiEventHandler(void* arg,
                                 esp_event_base_t base,
                                 int32_t id,
                                 void* data);

private:
    IEventSink* eventSink;
};