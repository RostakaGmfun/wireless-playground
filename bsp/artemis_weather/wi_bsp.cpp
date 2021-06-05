#include "bsp/wi_bsp.hpp"
#include "port/port_i2c.hpp"
#include "port/port_uart.hpp"
#include "port/port_gpio.hpp"
#include "drivers/sht3x.hpp"
#include "drivers/apds9960.hpp"

#include "am_mcu_apollo.h"

#include "FreeRTOS.h"
#include "semphr.h"

extern "C" void am_ctimer_isr(void)
{
    uint32_t ui32Status;

    //
    // Check the timer interrupt status.
    //
    ui32Status = am_hal_ctimer_int_status_get(false);
    am_hal_ctimer_int_clear(ui32Status);

    //
    // Run handlers for the various possible timer events.
    //
    am_hal_ctimer_int_service(ui32Status);
}

namespace wi
{

static driver_sht3x *sht3x_;
static driver_apds9960 *apds9960_;
static port_i2c *i2c_sensor_bus_;
static port_uart *console_uart_;
static port_gpio *gpio_;

static void *adc_handle_;
static SemaphoreHandle_t adc_semaphore_;

static constexpr struct {
    int id;
    int func;
    port_gpio_dir_t dir;
    port_gpio_pull_t pull;
} pin_config[] = {
    {
        .id = 5,
        .func = AM_HAL_PIN_5_GPIO,
    },
    {
        .id = 39,
        .func = AM_HAL_PIN_39_M4SCL,
    },
    {
        .id = 40,
        .func = AM_HAL_PIN_40_M4SDAWIR3,
    },
    {
        .id = 48,
        .func = AM_HAL_PIN_48_UART0TX,
    },
    {
        .id = 49,
        .func = AM_HAL_PIN_49_UART0RX,
    },
    {
        .id = 16,
        .func = AM_HAL_PIN_16_ADCSE0,
    },
};

static bool init_adc()
{
    am_hal_adc_config_t ADCConfig;

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_initialize(0, &adc_handle_))
    {
        return false;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_power_control(adc_handle_, AM_HAL_SYSCTRL_WAKE, false))
    {
        return false;
    }

    ADCConfig.eClock             = AM_HAL_ADC_CLKSEL_HFRC;
    ADCConfig.ePolarity          = AM_HAL_ADC_TRIGPOL_RISING;
    ADCConfig.eTrigger           = AM_HAL_ADC_TRIGSEL_SOFTWARE;
    ADCConfig.eReference         = AM_HAL_ADC_REFSEL_INT_2P0;
    ADCConfig.eClockMode         = AM_HAL_ADC_CLKMODE_LOW_LATENCY;
    ADCConfig.ePowerMode         = AM_HAL_ADC_LPMODE0;
    ADCConfig.eRepeat            = AM_HAL_ADC_SINGLE_SCAN;
    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure(adc_handle_, &ADCConfig))
    {
        return false;
    }

    adc_semaphore_ = xSemaphoreCreateBinary();

    am_hal_adc_interrupt_enable(adc_handle_, AM_HAL_ADC_INT_CNVCMP);
    NVIC_SetPriority(ADC_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(ADC_IRQn);

    return true;
}

static float adc_to_temp(uint32_t adc_sample, float ref_voltage)
{
    const float fADCTempVolts = (float)adc_sample * ref_voltage / (1024.0f);
    float fVT[3];
    fVT[0] = fADCTempVolts;
    fVT[1] = 0.0f;
    fVT[2] = -123.456;
    am_hal_adc_control(adc_handle_, AM_HAL_ADC_REQ_TEMP_CELSIUS_GET, fVT);
    return fVT[1];
}

static float adc_to_vsolar(uint32_t adc_sample, float ref_voltage)
{
    return ((float)adc_sample * ref_voltage / 16384.0f) * 6.1146;
}

static int adc_measure(am_hal_adc_slot_chan_e channel, am_hal_adc_slot_prec_e resolution, am_hal_adc_meas_avg_e avg, uint32_t *p_sample)
{

    // Temperature
    am_hal_adc_slot_config_t      ADCSlotConfig;
    ADCSlotConfig.eMeasToAvg      = avg;
    ADCSlotConfig.ePrecisionMode  = resolution;
    ADCSlotConfig.eChannel        = channel;
    ADCSlotConfig.bWindowCompare  = false;
    ADCSlotConfig.bEnabled        = true;

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_configure_slot(adc_handle_, 0, &ADCSlotConfig))
    {
        return false;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_enable(adc_handle_))
    {
        return false;
    }

    xSemaphoreTake(adc_semaphore_, 0);
    auto ret = am_hal_adc_sw_trigger(adc_handle_);
    if (AM_HAL_STATUS_SUCCESS != ret) {
        am_hal_adc_disable(adc_handle_);
        return 1;
    }

    if (!xSemaphoreTake(adc_semaphore_, pdMS_TO_TICKS(100))) {
        am_hal_adc_disable(adc_handle_);
        return 1;
    }

    am_hal_adc_sample_t sample;
    uint32_t num_samples = 1;
    am_hal_adc_samples_read(adc_handle_, true, nullptr, &num_samples, &sample);

    *p_sample = sample.ui32Sample;

    am_hal_adc_disable(adc_handle_);
}

extern "C" void am_adc_isr(void)
{
    uint32_t ui32IntMask;

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_interrupt_status(adc_handle_, &ui32IntMask, false))
    {
        return;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_adc_interrupt_clear(adc_handle_, ui32IntMask))
    {
        return;
    }

    if (ui32IntMask & AM_HAL_ADC_INT_CNVCMP)
    {
        BaseType_t wakeup_needed = 0;
        xSemaphoreGiveFromISR(adc_semaphore_, &wakeup_needed);
        portYIELD_FROM_ISR(wakeup_needed);
    }
}

void bsp_init()
{
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    am_hal_pwrctrl_low_power_init();
    am_hal_rtc_osc_disable();
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_STOP, 0);

    gpio_ = new port_gpio(0);
    for (const auto &pin : pin_config) {
        gpio_->configure(pin.id, pin.dir, pin.pull, pin.func);
    }

    i2c_sensor_bus_ = new port_i2c(4);
    console_uart_ = new port_uart(0, 115200);
    sht3x_ = new driver_sht3x(0x45, *i2c_sensor_bus_);
    apds9960_ = new driver_apds9960(0x39, *i2c_sensor_bus_);

    init_adc();

    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);

    am_hal_interrupt_master_enable();
}

driver_sht3x &bsp_sht3x_get()
{
    return *sht3x_;
}

driver_apds9960 &bsp_apds9960_get()
{
    return *apds9960_;
}

port_uart &bsp_console_uart_get()
{
    return *console_uart_;
}

port_gpio &bsp_gpio_get()
{
    return *gpio_;
}

void bsp_measure_vbatt(int *p_vbatt_mv)
{

}

void bsp_measure_vsolar(int *p_vsolar_mv)
{
    uint32_t sample;
    if (0 == adc_measure(AM_HAL_ADC_SLOT_CHSEL_SE0, AM_HAL_ADC_SLOT_14BIT,
                    AM_HAL_ADC_SLOT_AVG_128, &sample)) {
        *p_vsolar_mv = adc_to_vsolar((sample & 0xFFC0) >> 6, 2.0f);
    } else {
        *p_vsolar_mv = 0;
    }
}

void bsp_measure_chip_temp(int *p_chip_temp)
{
    uint32_t sample;
    if (0 == adc_measure(AM_HAL_ADC_SLOT_CHSEL_TEMP, AM_HAL_ADC_SLOT_10BIT,
                    AM_HAL_ADC_SLOT_AVG_1, &sample)) {
        *p_chip_temp = adc_to_temp((sample & 0xFFC0) >> 6, 2.0f);
    } else {
        *p_chip_temp = 0;
    }
}

bool bsp_probe_i2c(uint8_t addr)
{
    i2c_sensor_bus_->configure(100000);
    uint8_t data = 0;
    return i2c_sensor_bus_->write(addr, &data, sizeof(data), 1000);
}

} // namespace wi
