#include "device_manager.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "DeviceManager";

DeviceManager::DeviceManager() 
   :wifi(this)
{
    queue = xQueueCreate(10, sizeof(Event));
}

void DeviceManager::start() {
    xTaskCreate(
        &DeviceManager::taskEntry,
        "device_mgr",
        4096,
        this,
        5,
        nullptr
    );
}

void DeviceManager::postEvent(EventType type) {
    Event e{type};
    xQueueSend(queue, &e, portMAX_DELAY);
}

void DeviceManager::taskEntry(void* param) {
    auto* instance = static_cast<DeviceManager*>(param);
    instance->run();
}

void DeviceManager::run() {
    while (true) {
        Event e;
        if (xQueueReceive(queue, &e, portMAX_DELAY)) {
            handleEvent(e);
        }
    }
}

void DeviceManager::handleEvent(const Event& e) {
    switch (e.type) {

        case EventType::Boot:
            ESP_LOGI(TAG, "Boot begins. Calling wifi.");
            wifi.start();
            break;

        case EventType::WifiConnecting:
            break;

        case EventType::WifiConnected:
            break;

        case EventType::WifiDisconnected:
            break;

        case EventType::WifiBadCredentials:
            break;

        case EventType::WifiProvisioningStarted:
            break;

        case EventType::WifiProvisioningCompleted:
            break;
    }
}