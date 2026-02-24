#include "berry.h"
#include "be_vm.h"
#include "be_constobj.h"

#include "esp_log.h"
#include "esp_partition.h"
#include "esp_heap_caps.h"

static const char *TAG = "berry";

#define SCRIPT_MAX_SIZE (128 * 1024)
#define SCRIPT_SUBTYPE  0x41

typedef struct {
    uint32_t length;
} script_header_t;

/* External module registration */
extern void register_system_module(bvm *vm);

/* Global VM */
static bvm *g_vm = NULL;


/* ------------------------------------------------------------
   Load script from partition
------------------------------------------------------------ */
static char *load_script(size_t *out_len)
{
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        SCRIPT_SUBTYPE,
        "script"
    );

    if (!part) {
        ESP_LOGE(TAG, "Script partition not found");
        return NULL;
    }

    script_header_t header;

    if (esp_partition_read(part, 0, &header, sizeof(header)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read header");
        return NULL;
    }

    if (header.length == 0 || header.length > SCRIPT_MAX_SIZE) {
        ESP_LOGE(TAG, "Invalid script length");
        return NULL;
    }

    char *buffer = heap_caps_malloc(header.length + 1, MALLOC_CAP_8BIT);
    if (!buffer) {
        ESP_LOGE(TAG, "No memory for script");
        return NULL;
    }

    if (esp_partition_read(part, sizeof(header), buffer, header.length) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read script body");
        free(buffer);
        return NULL;
    }

    buffer[header.length] = '\0';
    *out_len = header.length;

    return buffer;
}


/* ------------------------------------------------------------
   Runtime start
------------------------------------------------------------ */
void berry_runtime_start(void)
{
    size_t script_len = 0;
    char *script = load_script(&script_len);

    if (!script) {
        ESP_LOGW(TAG, "No script loaded");
        return;
    }

    g_vm = be_vm_new();
    if (!g_vm) {
        ESP_LOGE(TAG, "VM creation failed");
        free(script);
        return;
    }



    /* Register your custom modules */
    register_system_module(g_vm);

    /* Compile script */
    if (be_loadstring(g_vm, script) != BE_OK) {
        ESP_LOGE(TAG, "Compile error: %s", be_tostring(g_vm, -1));
        be_pop(g_vm, 1);
        be_vm_delete(g_vm);
        g_vm = NULL;
        free(script);
        return;
    }

    /* Execute script */
    if (be_pcall(g_vm, 0) != BE_OK) {
        ESP_LOGE(TAG, "Runtime error: %s", be_tostring(g_vm, -1));
        be_pop(g_vm, 1);
        be_vm_delete(g_vm);
        g_vm = NULL;
        free(script);
        return;
    }

    free(script);

    ESP_LOGI(TAG, "Berry script executed successfully");
}


/* ------------------------------------------------------------
   Optional: Stop runtime
------------------------------------------------------------ */
void berry_runtime_stop(void)
{
    if (g_vm) {
        be_vm_delete(g_vm);
        g_vm = NULL;
        ESP_LOGI(TAG, "Berry VM stopped");
    }
}


/* ------------------------------------------------------------
   Optional: Call Berry function later from C
------------------------------------------------------------ */
void berry_call(const char *func_name)
{
    if (!g_vm) {
        ESP_LOGW(TAG, "VM not running");
        return;
    }

    be_getglobal(g_vm, func_name);

    if (!be_isfunction(g_vm, -1)) {
        ESP_LOGW(TAG, "Function not found: %s", func_name);
        be_pop(g_vm, 1);
        return;
    }

    if (be_pcall(g_vm, 0) != BE_OK) {
        ESP_LOGE(TAG, "Error calling %s: %s",
                 func_name, be_tostring(g_vm, -1));
        be_pop(g_vm, 1);
    }
}