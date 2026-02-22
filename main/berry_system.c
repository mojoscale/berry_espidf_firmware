#include "berry.h"
#include "be_vm.h"
#include "esp_system.h"

static int m_heap(bvm *vm)
{
    be_pushint(vm, esp_get_free_heap_size());
    return 1;
}

void berry_register_system(bvm *vm)
{
    be_newobject(vm);                // create table

    be_pushcfunction(vm, m_heap);
    be_setmember(vm, -2, "heap");

    be_setglobal(vm, "system");      // global system = table
}