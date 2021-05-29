#include "wsf_types.h"
#include "wsf_buf.h"
#include "wsf_timer.h"
#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "dm_api.h"
#include "app_api.h"
#include "att_api.h"
#include "svc_core.h"
#include "svc_dis.h"
#include "l2c_api.h"
#include "smp_api.h"
#include "gatt/gatt_api.h"
#include "hci_drv.h"
#include "hci_drv.h"
#include "FreeRTOS.h"
#include "timers.h"
extern "C" {
#include "hci_drv_apollo3.h"
}
#include "apollo3.h"
#include <cstring>

#include "wi.hpp"
#include "ble_sensor_svc.hpp"

extern "C" void am_ble_isr(void)
{
    HciDrvIntService();
}

namespace wi
{

static void sensorSetSystemId(void)
{
  uint8_t *bdaddr = HciGetBdAddr();
  uint8_t  sys_id[8];

  /* Formatted according to GATT specification for System ID characteristic (0x2A23). */
  sys_id[0] = bdaddr[0];
  sys_id[1] = bdaddr[1];
  sys_id[2] = bdaddr[2];
  sys_id[3] = 0xFE;
  sys_id[4] = 0xFF;
  sys_id[5] = bdaddr[3];
  sys_id[6] = bdaddr[4];
  sys_id[7] = bdaddr[5];
  WI_LOG_INFO("Addr: %02x:%02x:%02x:%02x:%02x:%02x", bdaddr[0], bdaddr[1], bdaddr[2],
          bdaddr[3], bdaddr[4], bdaddr[5]);
  AttsSetAttr(DIS_SID_HDL, sizeof(sys_id), sys_id);
}

static uint8_t sensorAdvDataDisc[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */
static uint8_t sensorAdvDataConn[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */

static const uint8_t sensorAdvDataFlags[] =
{
  DM_FLAG_LE_LIMITED_DISC | DM_FLAG_LE_BREDR_NOT_SUP
};

/* scan data */
static uint8_t sensorScanDataDisc[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */
static uint8_t sensorScanDataConn[HCI_ADV_DATA_LEN]; /* value is set in sensorStart() */
static const char sensorScanDataLocalName[] =
{
  'S', 'e', 'n', 's', 'o', 'r'
};

/*! configurable parameters for slave */
static const appAdvCfg_t sensorAdvCfg =
{
  {30000, 0, 0},         /*! Advertising durations in ms */
  {200,                    0, 0},         /*! Advertising intervals in 0.625 ms units */
};

/*! configurable parameters for slave */
static appSlaveCfg_t sensorSlaveCfg =
{
  1                                       /*! Maximum connections */
};

/*! configurable parameters for security */
static const appSecCfg_t sensorSecCfg =
{
  DM_AUTH_BOND_FLAG,                      /*! Authentication and bonding flags */
  0,                                      /*! Initiator key distribution flags */
  DM_KEY_DIST_LTK,                        /*! Responder key distribution flags */
  FALSE,                                  /*! TRUE if Out-of-band pairing data is present */
  FALSE                                   /*! TRUE to initiate security upon connection */
};

/*! configurable parameters for connection parameter update */
static const appUpdateCfg_t sensorUpdateCfg =
{
  0,                                      /*! Connection idle period in ms before attempting
                                             connection parameter update; set to zero to disable */
  640,                                    /*! Minimum connection interval in 1.25ms units */
  800,                                    /*! Maximum connection interval in 1.25ms units */
  0,                                      /*! Connection latency */
  600,                                    /*! Supervision timeout in 10ms units */
  5                                       /*! Number of update attempts before giving up */
};


static void ble_stack_configure()
{
  /* set advertising response data for discoverable mode */
  memset(sensorAdvDataDisc, 0, sizeof(sensorAdvDataDisc));
  AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, 0, (uint8_t *) sensorAdvDataDisc);
  AppAdvSetAdValue(APP_ADV_DATA_DISCOVERABLE, DM_ADV_TYPE_FLAGS, sizeof(sensorAdvDataFlags),
                   (uint8_t *) sensorAdvDataFlags);

  /* set scan response data for discoverable mode */
  memset(sensorScanDataDisc, 0, sizeof(sensorScanDataDisc));
  AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, 0, (uint8_t *) sensorScanDataDisc);
  AppAdvSetAdValue(APP_SCAN_DATA_DISCOVERABLE, DM_ADV_TYPE_LOCAL_NAME, sizeof(sensorScanDataLocalName), (uint8_t *) sensorScanDataLocalName);

  /* set advertising response data for connectable mode */
  memset(sensorAdvDataConn, 0, sizeof(sensorAdvDataConn));
  AppAdvSetData(APP_ADV_DATA_CONNECTABLE, 0, (uint8_t *) sensorAdvDataConn);
  AppAdvSetAdValue(APP_ADV_DATA_CONNECTABLE, DM_ADV_TYPE_FLAGS, sizeof(sensorAdvDataFlags),
                   (uint8_t *) sensorAdvDataFlags);

  /* set scan response data for connectable mode */
  memset(sensorScanDataConn, 0, sizeof(sensorScanDataConn));
  AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, 0, (uint8_t *) sensorScanDataConn);
  AppAdvSetAdValue(APP_SCAN_DATA_CONNECTABLE, DM_ADV_TYPE_LOCAL_NAME, sizeof(sensorScanDataLocalName), (uint8_t *) sensorScanDataLocalName);

  /* set system ID according to BDADDR */
  sensorSetSystemId();

  /* start advertising */
  AppSetAdvType(DM_ADV_CONN_UNDIRECT);
  AppAdvStart(APP_MODE_AUTO_INIT);
}

#define WSF_BUF_POOLS 4

static uint32_t g_pui32BufMem[
        (WSF_BUF_POOLS*16 + 16*8 + 32*4 + 64*6 + 280*8) / sizeof(uint32_t)];

// Default pool descriptor.
static wsfBufPoolDesc_t g_psPoolDescriptors[WSF_BUF_POOLS] =
{
    {  16,  8 },
    {  32,  4 },
    {  64,  6 },
    { 280,  8 }
};

static void ble_stack_dm_cb(dmEvt_t *pDmEvt)
{
    /* process advertising and connection-related messages */
    AppSlaveProcDmMsg(pDmEvt);

    /* process security-related messages */
    AppSlaveSecProcDmMsg(pDmEvt);

    switch (pDmEvt->hdr.event) {
    case DM_RESET_CMPL_IND:
        AttsCalculateDbHash();
        ble_stack_configure();
        break;
    default:
        break;
    }
}

void ble_stack_init()
{
    NVIC_SetPriority(BLE_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);
    WsfOsInit();
    WsfTimerInit();

    //
    // Initialize a buffer pool for WSF dynamic memory needs.
    //
    auto wsfBufMemLen = WsfBufInit(sizeof(g_pui32BufMem), (uint8_t *)g_pui32BufMem, WSF_BUF_POOLS,
               g_psPoolDescriptors);

    //
    // Initialize the WSF security service.
    //
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    //
    // Set up callback functions for the various layers of the ExactLE stack.
    //
    auto handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmAdvInit();
    DmConnInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
    L2cInit();
    L2cSlaveInit();

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();
    AttcInit();

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(251);

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    pAppAdvCfg    = (appAdvCfg_t *)&sensorAdvCfg;
    pAppSlaveCfg  = (appSlaveCfg_t *)&sensorSlaveCfg;
    pAppSecCfg    = (appSecCfg_t *)&sensorSecCfg;
    pAppUpdateCfg = (appUpdateCfg_t *)&sensorUpdateCfg;

    /* Initialize application framework */
    AppSlaveInit();
    AppServerInit();

    //handlerId = WsfOsSetNextHandler(ThroughputHandler);
    //ThroughputHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(HciDrvHandler);
    HciDrvHandlerInit(handlerId);

    //ButtonHandlerId = WsfOsSetNextHandler(button_handler);
}

void ble_stack_start()
{
    /* Register for stack callbacks */
    DmRegister(ble_stack_dm_cb);
    DmConnRegister(DM_CLIENT_ID_APP, ble_stack_dm_cb);
    AttRegister([] (attEvt_t *pEvt) { });
    AttConnRegister(AppServerConnCback);

    /* Initialize attribute server database */
    SvcCoreGattCbackRegister(GattReadCback, GattWriteCback);
    SvcCoreAddGroup();
    SvcDisAddGroup();

    ble_sensor_svc_init(0);

    /* set up CCCD table and callback */
    //AttsCccRegister(APPS_MAIN_NUM_CCC_IDX, (attsCccSet_t *) MainSensor_CccSet, sensorCccCback);

    /* Reset the device */
    DmDevReset();
}

void ble_stack_process_events()
{
    wsfOsDispatcher();
}

} // namespace wi
