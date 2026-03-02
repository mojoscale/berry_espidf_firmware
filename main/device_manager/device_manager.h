#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "events.h"
#include "event_sink.h"
#include "wifi_manager.h"

class DeviceManager : public IEventSink {
public:
    DeviceManager();
    void start();
    void postEvent(EventType type) override;

private:
    static void taskEntry(void* param);
    void run();
    void handleEvent(const Event& e);

    QueueHandle_t queue;
    WifiManager wifi;
};