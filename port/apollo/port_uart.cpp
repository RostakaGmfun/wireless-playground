#include "port/port_uart.hpp"

#include "am_mcu_apollo.h"

namespace wi
{

port_uart::port_uart(int id, int baudrate): id_(id)
{
    am_hal_uart_initialize(id_, &uart_handle_);
    am_hal_uart_power_control(uart_handle_, AM_HAL_SYSCTRL_WAKE, false);
    const am_hal_uart_config_t uart_config =
    {
        .ui32BaudRate = baudrate,
        .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                        AM_HAL_UART_RX_FIFO_1_2),

        .pui8TxBuffer = tx_buffer_,
        .ui32TxBufferSize = sizeof(tx_buffer_),
        .pui8RxBuffer = nullptr,
        .ui32RxBufferSize = NULL,
    };
    am_hal_uart_configure(uart_handle_, &uart_config);

    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + id_));
    am_hal_interrupt_master_enable();
}

port_uart::~port_uart()
{
    am_hal_uart_deinitialize(uart_handle_);
}

bool port_uart::write(const uint8_t *p_data, size_t size, unsigned int timeout_ms)
{
    const am_hal_uart_transfer_t op =
    {
        .ui32Direction = AM_HAL_UART_WRITE,
        .pui8Data = (uint8_t *) p_data,
        .ui32NumBytes = size,
        .ui32TimeoutMs = timeout_ms,
        .pui32BytesTransferred = nullptr,
    };

    if (AM_HAL_STATUS_SUCCESS != am_hal_uart_transfer(uart_handle_, &op)) {
        return false;
    }
    return true;
}

bool port_uart::read(uint8_t *p_data, size_t size, unsigned int timeout_ms)
{
    return false;
}

bool port_uart::lock(unsigned int timeout_ms)
{
    return true;
}

void port_uart::unlock()
{

}

void port_uart::enable()
{
    am_hal_uart_power_control(uart_handle_, AM_HAL_SYSCTRL_WAKE, true);
}

void port_uart::disable()
{
    am_hal_uart_power_control(uart_handle_, AM_HAL_SYSCTRL_DEEPSLEEP, true);
}

} // namespace wi
