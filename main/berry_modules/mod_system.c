#include "berry.h"
#include "be_vm.h"
#include "esp_system.h"
#include "esp_log.h"

static int m_heap(bvm *vm)
{
    ESP_LOGI("TEST", "m_heap executed");
    
    be_pushint(vm, esp_get_free_heap_size());
    //return 1;
    be_return(vm);
}



void register_system_module(bvm *vm)
{
    be_regfunc(vm, "heap", m_heap);


}