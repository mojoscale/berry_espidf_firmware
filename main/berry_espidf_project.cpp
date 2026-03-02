#include <stdio.h>
#include "device_manager/device_manager.h"

// If berry_runtime_start is implemented in C file,
// we must declare it with C linkage.
//extern "C" void berry_runtime_start(void);

// If you no longer use start_device_manager(), remove it.
// extern "C" void start_device_manager(void);

extern "C" void app_main(void)
{
    static DeviceManager manager;

    manager.start();

    manager.postEvent(EventType::Boot);

    // If you want Berry runtime:
    // berry_runtime_start();
}