#include "berry.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_heap_caps.h"

static const char *TAG = "berry";

#define SCRIPT_MAX_SIZE (128 * 1024)

void berry_runtime_start(void)
{
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        0x41,      // your custom subtype
        "script"
    );

    if (!part) {
        ESP_LOGE(TAG, "Script partition not found");
        return;
    }

    size_t size = part->size;
    if (size > SCRIPT_MAX_SIZE) {
        ESP_LOGE(TAG, "Script too large");
        return;
    }

    uint8_t *buffer = heap_caps_malloc(size + 1, MALLOC_CAP_8BIT);
    if (!buffer) {
        ESP_LOGE(TAG, "No memory");
        return;
    }

    esp_err_t err = esp_partition_read(part, 0, buffer, size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Partition read failed");
        free(buffer);
        return;
    }

    buffer[size] = '\0';

    bvm *vm = be_vm_new();
    if (!vm) {
        ESP_LOGE(TAG, "VM create failed");
        free(buffer);
        return;
    }

    if (be_loadstring(vm, (const char *)buffer) != 0) {
        ESP_LOGE(TAG, "Compile error: %s", be_tostring(vm, -1));
    } else {
        if (be_pcall(vm, 0) != 0) {
            ESP_LOGE(TAG, "Runtime error: %s", be_tostring(vm, -1));
        }
    }

    be_vm_delete(vm);
    free(buffer);
}

