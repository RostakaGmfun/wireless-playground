set(AM_SDK_HAL $ENV{AM_SDK_PATH}/mcu/apollo3/hal)

add_library(am_sdk_hal
  ${AM_SDK_HAL}/am_hal_iom.c
  ${AM_SDK_HAL}/am_hal_uart.c
  ${AM_SDK_HAL}/am_hal_ble_patch_b0.c
  ${AM_SDK_HAL}/am_hal_cmdq.c
  ${AM_SDK_HAL}/am_hal_gpio.c
  ${AM_SDK_HAL}/am_hal_wdt.c
  ${AM_SDK_HAL}/am_hal_pwrctrl.c
  ${AM_SDK_HAL}/am_hal_flash.c
  ${AM_SDK_HAL}/am_hal_cachectrl.c
  ${AM_SDK_HAL}/am_hal_ctimer.c
  ${AM_SDK_HAL}/am_hal_secure_ota.c
  ${AM_SDK_HAL}/am_hal_mcuctrl.c
  ${AM_SDK_HAL}/am_hal_debug.c
  ${AM_SDK_HAL}/am_hal_security.c
  ${AM_SDK_HAL}/am_hal_rtc.c
  ${AM_SDK_HAL}/am_hal_tpiu.c
  ${AM_SDK_HAL}/am_hal_sysctrl.c
  ${AM_SDK_HAL}/am_hal_pdm.c
  ${AM_SDK_HAL}/am_hal_interrupt.c
  ${AM_SDK_HAL}/am_hal_mspi.c
  ${AM_SDK_HAL}/am_hal_queue.c
  ${AM_SDK_HAL}/am_hal_ble.c
  ${AM_SDK_HAL}/am_hal_global.c
  ${AM_SDK_HAL}/am_hal_burst.c
  ${AM_SDK_HAL}/am_hal_scard.c
  ${AM_SDK_HAL}/am_hal_stimer.c
  ${AM_SDK_HAL}/am_hal_adc.c
  ${AM_SDK_HAL}/am_hal_clkgen.c
  ${AM_SDK_HAL}/am_hal_reset.c
  ${AM_SDK_HAL}/am_hal_systick.c
  ${AM_SDK_HAL}/am_hal_ios.c
  ${AM_SDK_HAL}/am_hal_itm.c)

target_include_directories(am_sdk_hal PUBLIC
  ${AM_SDK_HAL}
  ${AM_SDK_HAL}/../regs
  ${AM_SDK_HAL}/../
  $ENV{AM_SDK_PATH}/CMSIS/AmbiqMicro/Include $ENV{AM_SDK_PATH}/CMSIS/ARM/Include)

target_compile_definitions(am_sdk_hal PUBLIC
  PART_apollo3
  AM_PART_APOLLO3
  AM_DEBUG_PRINTF
  AM_PACKAGE_BGA
  AM_FREERTOS
  # Needed?
  gcc)

set(AM_SDK_FREERTOS $ENV{AM_SDK_PATH}/third_party/FreeRTOSv10.1.1/Source)

add_library(am_sdk_freertos
  ${AM_SDK_FREERTOS}/croutine.c
  ${AM_SDK_FREERTOS}/event_groups.c
  ${AM_SDK_FREERTOS}/list.c
  ${AM_SDK_FREERTOS}/queue.c
  ${AM_SDK_FREERTOS}/stream_buffer.c
  ${AM_SDK_FREERTOS}/tasks.c
  ${AM_SDK_FREERTOS}/timers.c
  ${AM_SDK_FREERTOS}/portable/MemMang/heap_4.c
  ${AM_SDK_FREERTOS}/portable/GCC/AMapollo2/port.c)

target_compile_definitions(am_sdk_freertos PUBLIC
  PART_apollo3
  AM_PART_APOLLO3
  AM_DEBUG_PRINTF
  AM_PACKAGE_BGA
  # Needed?
  gcc)

target_include_directories(am_sdk_freertos PUBLIC
  ${AM_SDK_FREERTOS}/include
  ${AM_SDK_FREERTOS}/portable/GCC/AMapollo2)

target_link_libraries(am_sdk_freertos am_sdk_hal)

set(AM_SDK_UTILS $ENV{AM_SDK_PATH}/utils)

add_library(am_sdk_utils
  ${AM_SDK_UTILS}/am_util_delay.c
  ${AM_SDK_UTILS}/am_util_stdio.c
  ${AM_SDK_UTILS}/am_util_id.c
  ${AM_SDK_UTILS}/am_util_debug.c
  ${AM_SDK_UTILS}/am_util_ble.c
  ${AM_SDK_UTILS}/am_util_string.c
  ${AM_SDK_UTILS}/am_util_time.c)

target_compile_definitions(am_sdk_utils PUBLIC
  PART_apollo3
  AM_PART_APOLLO3
  AM_DEBUG_PRINTF
  AM_PACKAGE_BGA
  # Needed?
  gcc)

target_include_directories(am_sdk_utils PUBLIC ${AM_SDK_UTILS})

target_link_libraries(am_sdk_utils am_sdk_hal)

set(AM_SDK_CORDIO $ENV{AM_SDK_PATH}/third_party/cordio)

add_library(am_sdk_cordio
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_assert.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_math.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_os.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_buf.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_msg.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_trace.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_efs.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_queue.c
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos/wsf_timer.c
  ${AM_SDK_CORDIO}/wsf/sources/util/crc32.c
  ${AM_SDK_CORDIO}/wsf/sources/util/bstream.c
  ${AM_SDK_CORDIO}/wsf/sources/util/print.c
  ${AM_SDK_CORDIO}/wsf/sources/util/calc128.c
  ${AM_SDK_CORDIO}/wsf/sources/util/wstr.c
  ${AM_SDK_CORDIO}/wsf/sources/util/bda.c
  ${AM_SDK_CORDIO}/wsf/sources/util/terminal.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/apollo3/hci_vs_apollo3.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/apollo3/hci_drv_apollo3.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_evt.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_cmd_phy.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_cmd_ae.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_tr.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_vs_ae.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_cmd_cte.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_core.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_core_ps.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_cmd.c
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq/hci_cmd_past.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_sec_master.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_slave.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_scan_leg.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_dev_priv.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_master_ae.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_slave_leg.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_adv_ae.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_scan_ae.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_dev.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_sm.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_master.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_sync_ae.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_cte.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_past.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_scan.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_sec_slave.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_sec_lesc.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_slave_ae.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_phy.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_adv_leg.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_priv.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_sec.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_conn_master_leg.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm/dm_adv.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/hci/hci_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/l2c/l2c_slave.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/l2c/l2c_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/l2c/l2c_coc.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/l2c/l2c_master.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/cfg/cfg_stack.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/attc_disc.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/attc_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_ccc.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/attc_read.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_dyn.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_sign.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/attc_proc.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/att_uuid.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/attc_write.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_proc.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_read.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_write.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_ind.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/attc_sign.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/att_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/att/atts_csf.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smp_db.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpr_sc_act.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smp_act.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smp_sc_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpr_sc_sm.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpi_sm.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smp_non.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpr_sm.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smp_sc_act.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpr_act.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpi_sc_sm.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smp_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpi_sc_act.c
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp/smpi_act.c
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common/sec_main.c
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common/sec_aes.c
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common/sec_ecc_hci.c
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common/sec_ccm_hci.c
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common/sec_aes_rev.c
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common/sec_cmac_hci.c
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common/sec_ecc_debug.c)

target_include_directories(am_sdk_cordio PUBLIC
  ${AM_SDK_CORDIO}/ble-host/include
  ${AM_SDK_CORDIO}/ble-profiles/include
  ${AM_SDK_CORDIO}/wsf/include
  ${AM_SDK_CORDIO}/wsf/sources/port/freertos
  ${AM_SDK_CORDIO}/wsf/sources
  ${AM_SDK_CORDIO}/wsf/sources/util
  ${AM_SDK_CORDIO}/ble-host/sources/hci/ambiq
  ${AM_SDK_CORDIO}/ble-host/sources/stack/cfg
  ${AM_SDK_CORDIO}/ble-host/sources/stack/hci
  ${AM_SDK_CORDIO}/ble-host/sources/stack/smp
  ${AM_SDK_CORDIO}/ble-profiles/sources/services
  ${AM_SDK_CORDIO}/ble-host/sources/stack/dm
  ${AM_SDK_CORDIO}/ble-host/sources/sec/common)

target_compile_definitions(am_sdk_cordio PUBLIC
  SEC_ECC_CFG=SEC_ECC_CFG_UECC
  WSF_TRACE_ENABLED
  AM_PART_APOLLO3
  # Needed?
  gcc)

target_link_libraries(am_sdk_cordio am_sdk_freertos am_sdk_utils)
