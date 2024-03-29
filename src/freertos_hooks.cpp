#include "FreeRTOS.h"
#include "task.h"
#include "wi.hpp"
#include "apollo3.h"

static void print_stack(uint32_t *ptr)
{
    for (int i = 0; i < 128; i++) {
        WI_LOG_ERROR_NOLOCK("%08x", ptr[i]);
    }
}

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
    vTaskSuspendAll();
    WI_LOG_ERROR_NOLOCK("Assert failed %s %d", file, line);
    uint32_t dummy = 0;
    print_stack(&dummy);
    while (1);
}

extern "C" void vPrintCrashDump(uint32_t *pulFaultStackAddress, int type)
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

    WI_LOG_ERROR_NOLOCK("Crash: %d r0 %08x r1 %08x r2 %08x r3 %08x r12 %08x lr %08x pc %08x pssr %08x",
            type, r0, r1, r2, r3, r12, lr, pc, psr);
    WI_LOG_ERROR_NOLOCK("Crash: %08x %08x %08x %08x %08x",
            SCB->HFSR, SCB->CFSR, SCB->BFAR, SCB->MMFAR, pulFaultStackAddress);
    print_stack(pulFaultStackAddress);
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
