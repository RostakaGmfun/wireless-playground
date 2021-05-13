#include <cstdint>
#include "apollo3.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

void am_bsp_itm_printf_enable(void)
{
    am_hal_tpiu_config_t TPIUcfg;

    //
    // Enable the ITM interface and the SWO pin.
    //
    am_hal_itm_enable();
//  am_hal_gpio_pinconfig(AM_BSP_GPIO_ITM_SWO, g_AM_BSP_GPIO_ITM_SWO);

    //
    // Enable the ITM and TPIU
    // Set the BAUD clock for 1M
    //
    TPIUcfg.ui32SetItmBaud = AM_HAL_TPIU_BAUD_1M;
    am_hal_tpiu_enable(&TPIUcfg);
    //am_hal_gpio_pinconfig(AM_BSP_GPIO_ITM_SWO, g_AM_BSP_GPIO_ITM_SWO);

    //
    // Attach the ITM to the STDIO driver.
    //
    am_util_stdio_printf_init(am_hal_itm_print);
} // am_bsp_itm_printf_enable()

//*****************************************************************************
//
// @brief Disable printing over ITM.
//
//*****************************************************************************
void am_bsp_itm_printf_disable(void)
{
    //
    // Disable the ITM/TPIU
    //
    am_hal_itm_disable();

    //
    // Detach the ITM interface from the STDIO driver.
    //
    am_util_stdio_printf_init(0);

    //
    // Disconnect the SWO pin
    //
    //am_hal_gpio_pinconfig(AM_BSP_GPIO_ITM_SWO, g_AM_HAL_GPIO_DISABLE);
} // am_bsp_itm_printf_disable()


//*****************************************************************************
//
//! @brief Prepare the MCU for low power operation.
//!
//! This function enables several power-saving features of the MCU, and
//! disables some of the less-frequently used peripherals. It also sets the
//! system clock to 24 MHz.
//!
//! @return None.
//
//*****************************************************************************
void am_bsp_low_power_init(void)
{
    //
    // Initialize for low power in the power control block
    //
    am_hal_pwrctrl_low_power_init();

    //
    // Disable the RTC.
    //
    am_hal_rtc_osc_disable();

#ifdef AM_BSP_NUM_LEDS
    //
    // Initialize the LEDs.
    // On the apollo3_evb, when the GPIO outputs are disabled (the default at
    // power up), the FET gates are floating and partially illuminating the LEDs.
    //
    uint32_t ux, ui32GPIONumber;
    for (ux = 0; ux < AM_BSP_NUM_LEDS; ux++)
    {
        ui32GPIONumber = am_bsp_psLEDs[ux].ui32GPIONumber;

        //
        // Configure the pin as a push-pull GPIO output
        // (aka AM_DEVICES_LED_POL_DIRECT_DRIVE_M).
        //
        am_hal_gpio_pinconfig(ui32GPIONumber, g_AM_HAL_GPIO_OUTPUT);

        //
        // Turn off the LED.
        //
        am_hal_gpio_state_write(ui32GPIONumber, AM_HAL_GPIO_OUTPUT_TRISTATE_DISABLE);
        am_hal_gpio_state_write(ui32GPIONumber, AM_HAL_GPIO_OUTPUT_CLEAR);
    }
#endif // AM_BSP_NUM_LEDS

} // am_bsp_low_power_init()


int main(void)
{
    am_util_id_t sIdDevice;
    uint32_t ui32StrBuf;
    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for ITM output
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Hello World!\n\n");

    //
    // Print the device info.
    //
    am_util_id_device(&sIdDevice);
    am_util_stdio_printf("Vendor Name: %s\n", sIdDevice.pui8VendorName);
    am_util_stdio_printf("Device type: %s\n", sIdDevice.pui8DeviceName);


    am_util_stdio_printf("Qualified: %s\n",
                         sIdDevice.sMcuCtrlDevice.ui32Qualified ?
                         "Yes" : "No");

    am_util_stdio_printf("Device Info:\n"
                         "\tPart number: 0x%08X\n"
                         "\tChip ID0:    0x%08X\n"
                         "\tChip ID1:    0x%08X\n"
                         "\tRevision:    0x%08X (Rev%c%c)\n",
                         sIdDevice.sMcuCtrlDevice.ui32ChipPN,
                         sIdDevice.sMcuCtrlDevice.ui32ChipID0,
                         sIdDevice.sMcuCtrlDevice.ui32ChipID1,
                         sIdDevice.sMcuCtrlDevice.ui32ChipRev,
                         sIdDevice.ui8ChipRevMaj, sIdDevice.ui8ChipRevMin );

    //
    // If not a multiple of 1024 bytes, append a plus sign to the KB.
    //
    ui32StrBuf = ( sIdDevice.sMcuCtrlDevice.ui32FlashSize % 1024 ) ? '+' : 0;
    am_util_stdio_printf("\tFlash size:  %7d (%d KB%s)\n",
                         sIdDevice.sMcuCtrlDevice.ui32FlashSize,
                         sIdDevice.sMcuCtrlDevice.ui32FlashSize / 1024,
                         &ui32StrBuf);

    ui32StrBuf = ( sIdDevice.sMcuCtrlDevice.ui32SRAMSize % 1024 ) ? '+' : 0;
    am_util_stdio_printf("\tSRAM size:   %7d (%d KB%s)\n\n",
                         sIdDevice.sMcuCtrlDevice.ui32SRAMSize,
                         sIdDevice.sMcuCtrlDevice.ui32SRAMSize / 1024,
                         &ui32StrBuf);
    //
    // Print the compiler version.
    //
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);
#if defined(AM_PART_APOLLO3)  || defined(AM_PART_APOLLO3P)
    am_util_stdio_printf("HAL Compiler:    %s\n", g_ui8HALcompiler);
    am_util_stdio_printf("HAL SDK version: %d.%d.%d\n",
                         g_ui32HALversion.s.Major,
                         g_ui32HALversion.s.Minor,
                         g_ui32HALversion.s.Revision);
    am_util_stdio_printf("HAL compiled with %s-style registers\n",
                         g_ui32HALversion.s.bAMREGS ? "AM_REG" : "CMSIS");

    am_hal_security_info_t secInfo;
    char sINFO[32];
    uint32_t ui32Status;
    ui32Status = am_hal_security_get_info(&secInfo);
    if (ui32Status == AM_HAL_STATUS_SUCCESS)
    {
        if ( secInfo.bInfo0Valid )
        {
            am_util_stdio_sprintf(sINFO, "INFO0 valid, ver 0x%X", secInfo.info0Version);
        }
        else
        {
            am_util_stdio_sprintf(sINFO, "INFO0 invalid");
        }

        am_util_stdio_printf("SBL ver: 0x%x - 0x%x, %s\n",
            secInfo.sblVersion, secInfo.sblVersionAddInfo, sINFO);
    }
    else
    {
        am_util_stdio_printf("am_hal_security_get_info failed 0x%X\n", ui32Status);
    }
#endif // AM_PART_APOLLO3



    //
    // We are done printing.
    // Disable debug printf messages on ITM.
    //
    am_bsp_itm_printf_disable();

    //
    // Loop forever while sleeping.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
