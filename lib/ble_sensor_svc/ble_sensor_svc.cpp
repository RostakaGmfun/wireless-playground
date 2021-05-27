#include "ble_sensor_svc.hpp"
#include "wsf_types.h"
#include "att_api.h"
#include "util/bstream.h"

#define TEMP_HANDLE_START  0x60                          /*!< \brief Start handle. */
#define TEMP_HANDLE_END   (TEMP_HANDLE_END_PLUS_ONE - 1) /*!< \brief End handle. */

#define TEMP_UUID_SVC           0x3010
#define TEMP_UUID_CHR_DATA      0x3011

namespace wi
{

enum
{
  TEMP_HANDLE_SVC = TEMP_HANDLE_START, /*!< \brief Service declaration. */

  TEMP_HANDLE_DATA_CHR,                /*!< \brief Data characteristic declaration. */
  TEMP_HANDLE_DATA,                    /*!< \brief Data characteristic value. */
  TEMP_HANDLE_DATA_CLIENT_CHR_CONFIG,  /*!< \brief Data characteristic CCCD. */
  TEMP_HANDLE_DATA_CHR_USR_DESCR,      /*!< \brief Data characteristic user description. */

  TEMP_HANDLE_END_PLUS_ONE             /*!< \brief Maximum handle. */
};


/* Temperature service declaration. */
static const uint8_t  tempValSvc[] = {UINT16_TO_BYTES(TEMP_UUID_SVC)};
static const uint16_t tempLenSvc   = sizeof(tempValSvc);

/* Temperature data characteristic. */
static const uint8_t  tempValDataChr[] = {ATT_PROP_READ | ATT_PROP_NOTIFY,
                                          UINT16_TO_BYTES(TEMP_HANDLE_DATA),
                                          UINT16_TO_BYTES(TEMP_UUID_CHR_DATA)};
static const uint16_t tempLenDataChr   = sizeof(tempValDataChr);

/* Temperature data. */
static const uint8_t  tempUuidData[] = {UINT16_TO_BYTES(TEMP_UUID_CHR_DATA)};
static       uint8_t  tempValData[]  = {0x00, 0x00};
static const uint16_t tempLenData    = sizeof(tempValData);

/* Temperature data client characteristic configuration. */
static       uint8_t  tempValDataClientChrConfig[] = {0x00, 0x00};
static const uint16_t tempLenDataClientChrConfig   = sizeof(tempValDataClientChrConfig);

/* Temperature data characteristic user description. */
static const uint8_t  tempValDataChrUsrDescr[] = "Temp Data";
static const uint16_t tempLenDataChrUsrDescr   = sizeof(tempValDataChrUsrDescr) - 1u;

/* Attribute list for temp group. */
static const attsAttr_t tempList[] =
{
  /* Service declaration. */
  {
    attPrimSvcUuid,
    (uint8_t *) tempValSvc,
    (uint16_t *) &tempLenSvc,
    sizeof(tempValSvc),
    0,
    ATTS_PERMIT_READ
  },
  /* Characteristic declaration. */
  {
    attChUuid,
    (uint8_t *) tempValDataChr,
    (uint16_t *) &tempLenDataChr,
    sizeof(tempValDataChr),
    0,
    ATTS_PERMIT_READ
  },
  /* Characteristic value. */
  {
    tempUuidData,
    (uint8_t *) tempValData,
    (uint16_t *) &tempLenData,
    sizeof(tempValData),
    0,
    ATTS_PERMIT_READ
  },
  /* Client characteristic configuration. */
  {
    attCliChCfgUuid,
    (uint8_t *) tempValDataClientChrConfig,
    (uint16_t *) &tempLenDataClientChrConfig,
    sizeof(tempValDataClientChrConfig),
    ATTS_SET_CCC,
    ATTS_PERMIT_READ | ATTS_PERMIT_WRITE
  },
  /* Characteristic user description. */
  {
    attChUserDescUuid,
    (uint8_t *) tempValDataChrUsrDescr,
    (uint16_t *) &tempLenDataChrUsrDescr,
    sizeof(tempValDataChrUsrDescr) - 1,
    0,
    ATTS_PERMIT_READ
  },
};

class ble_sensor_svc: public ble_sensor_listener
{
public:
    ble_sensor_svc(uint32_t handler_id): handler_id_(handler_id)
    {
        AttsAddGroup(&tempGroup_);
    }

    ~ble_sensor_svc()
    {
        AttsRemoveGroup(TEMP_HANDLE_START);
    }

    void update_temperature(int temp_celsium) override
    {
        uint8_t tempData[2] = {UINT16_TO_BYTES((uint16_t)temp_celsium)};
        AttsSetAttr(TEMP_HANDLE_DATA, sizeof(tempData), tempData);
    }

private:
    wsfHandlerId_t handler_id_;

    attsGroup_t tempGroup_ =
    {
        NULL,
        (attsAttr_t *) tempList,
        NULL,
        NULL,
        TEMP_HANDLE_START,
        TEMP_HANDLE_END
    };
};

ble_sensor_listener *ble_sensor_svc_init(uint32_t handler_id)
{
    return new ble_sensor_svc(handler_id);
}

} // namespace wi
