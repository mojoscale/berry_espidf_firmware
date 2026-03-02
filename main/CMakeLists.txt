idf_component_register(
    SRCS
        "berry_espidf_project.cpp"
        "berry_runtime.c"
        "device_manager/device_manager.cpp"
        "device_manager/device_manager.h"
        "device_manager/wifi_manager.h"
        "device_manager/wifi_manager.cpp"
        "device_manager/events.h"
        "device_manager/event_sink.h"
        "berry_modules/mod_system.c"


    INCLUDE_DIRS "." "berry_modules" "device_manager"

    REQUIRES
        berry
        esp_partition
        esp_system
        heap

    PRIV_REQUIRES
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        esp_http_server
)