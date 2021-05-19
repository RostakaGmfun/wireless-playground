#include "FreeRTOS.h"
#include "task.h"

extern "C" uint32_t wi_freertos_sleep(uint32_t idleTime)
{
    return 0;
}

extern "C" void wi_freertos_wakeup(uint32_t idleTime)
{
    return;
}


extern "C" void vApplicationMallocFailedHook(void)
{
    while (1);
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    while (1);
}

void *operator new(size_t size) {
    return pvPortMalloc(size);
}

void *operator new[](size_t size) {
    return pvPortMalloc(size);
}

void operator delete(void *p) {
    vPortFree(p);
}

void operator delete[](void *p) {
    vPortFree(p);
}

void operator delete(void *p, size_t size) {
    (void)size;
    vPortFree(p);
}

void operator delete[](void *p, size_t size) {
    (void)size;
    vPortFree(p);
}
