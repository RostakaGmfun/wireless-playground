#include "port/port_i2c.hpp"
#include "am_mcu_apollo.h"

namespace wi
{

port_i2c::port_i2c(int id, unsigned int clock_speed_hz): id_(id)
{
    am_hal_iom_config_t config = {
        .eInterfaceMode = AM_HAL_IOM_I2C_MODE,
        .ui32ClockFreq = clock_speed_hz,
    };

    am_hal_iom_initialize(id_, &i2c_handle_);
    am_hal_iom_power_ctrl(i2c_handle_, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_iom_configure(i2c_handle_, &config);
    am_hal_iom_enable(i2c_handle_);
    i2c_mutex_ = xSemaphoreCreateMutex();
    i2c_semaphore_ = xSemaphoreCreateBinary();
}

port_i2c::~port_i2c()
{
    vSemaphoreDelete(i2c_mutex_);
    vSemaphoreDelete(i2c_semaphore_);
    am_hal_iom_uninitialize(i2c_handle_);
}

void port_i2c::configure(unsigned int clock_speed_hz)
{
    am_hal_iom_config_t config = {
        .eInterfaceMode = AM_HAL_IOM_I2C_MODE,
        .ui32ClockFreq = clock_speed_hz,
    };
    am_hal_iom_configure(i2c_handle_, &config);
}

bool port_i2c::write(uint8_t addr, const uint8_t *p_data, size_t size, unsigned int timeout_ms)
{
    am_hal_iom_transfer_t transaction;

    transaction.ui8RepeatCount = 0;
    transaction.ui32PauseCondition = 0;
    transaction.ui32StatusSetClr = 0;
    transaction.ui8Priority = 1;
    transaction.uPeerInfo.ui32I2CDevAddr = addr;
    transaction.bContinue = false;
    transaction.eDirection = AM_HAL_IOM_TX;
    transaction.ui32InstrLen = 0;
    transaction.ui32Instr = 0;
    transaction.ui32NumBytes = size;
    transaction.pui32TxBuffer = (uint32_t *)p_data; // TODO: check if 4-byte alignment is needed

    xSemaphoreTake(i2c_semaphore_, 0); // clear semaphore before new transfer

    if (am_hal_iom_nonblocking_transfer(i2c_handle_, &transaction, [] (void *ctx, uint32_t status) {
                        port_i2c *p_this = static_cast<port_i2c *>(ctx);
                        BaseType_t wakeup_needed = 0;
                        xSemaphoreGiveFromISR(p_this->i2c_semaphore_, &wakeup_needed);
                        portYIELD_FROM_ISR(wakeup_needed);
                    }, (void *)this))
    {
        return false;
    }

    return pdTRUE == xSemaphoreTake(i2c_semaphore_, pdMS_TO_TICKS(timeout_ms));
}

bool port_i2c::read(uint8_t addr, uint8_t *p_data, size_t size, unsigned int timeout_ms)
{
    am_hal_iom_transfer_t transaction;

    transaction.ui8Priority = 1;
    transaction.eDirection = AM_HAL_IOM_RX;
    transaction.ui32InstrLen = 0;
    transaction.ui32Instr = 0;
    transaction.ui32NumBytes = size;
    transaction.pui32RxBuffer = (uint32_t *)p_data;
    transaction.uPeerInfo.ui32I2CDevAddr = addr;
    transaction.ui8RepeatCount = 0;
    transaction.ui32PauseCondition = 0;
    transaction.ui32StatusSetClr = 0;
    transaction.bContinue = false;

    xSemaphoreTake(i2c_semaphore_, 0); // clear semaphore before new transfer

    if (am_hal_iom_nonblocking_transfer(i2c_handle_, &transaction, [] (void *ctx, uint32_t status) {
                        port_i2c *p_this = static_cast<port_i2c *>(ctx);
                        BaseType_t wakeup_needed = 0;
                        xSemaphoreGiveFromISR(p_this->i2c_semaphore_, &wakeup_needed);
                        portYIELD_FROM_ISR(wakeup_needed);
                    }, (void *)this))
    {
        return false;
    }

    return pdTRUE == xSemaphoreTake(i2c_semaphore_, pdMS_TO_TICKS(timeout_ms));
}

bool port_i2c::lock(unsigned int timeout_ms)
{
    return pdTRUE == xSemaphoreTake(i2c_mutex_, pdMS_TO_TICKS(timeout_ms));
}

void port_i2c::unlock()
{
    xSemaphoreGive(i2c_mutex_);
}

void port_i2c::enable()
{
    am_hal_iom_power_ctrl(i2c_handle_, AM_HAL_SYSCTRL_WAKE, true);
}

void port_i2c::disable()
{
    am_hal_iom_power_ctrl(i2c_handle_, AM_HAL_SYSCTRL_DEEPSLEEP, true);
}

} // namespace wi
