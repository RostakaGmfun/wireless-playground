#include "FreeRTOS.h"
#include "task.h"
#include "wi.hpp"

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
    WI_LOG_ERROR_NOLOCK("Malloc failed");
    while (1);
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    WI_LOG_ERROR_NOLOCK("Stack overflow");
    while (1);
}

extern "C" void vAssertFailedHook(const char *file, int line)
{
    WI_LOG_ERROR_NOLOCK("Assert failed %s %d", file, line);
    while (1);
}

extern "C" void vPrintCrashDump(uint32_t *pulFaultStackAddress)
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr; /* Link register. */
    uint32_t pc; /* Program counter. */
    uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    WI_LOG_ERROR_NOLOCK("Crash: r0 %08x r1 %08x r2 %08x r3 %08x r12 %08x lr %08x pc %08x pssr %08x",
            r0, r1, r2, r3, r12, lr, pc, psr);
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
