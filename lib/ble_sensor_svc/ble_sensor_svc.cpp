#include "ble_sensor_svc.hpp"
#include "wsf_types.h"
#include "att_api.h"
#include "util/bstream.h"

#define SENSOR_HANDLE_START  0x60                          /*!< \brief Start handle. */
#define SENSOR_HANDLE_END   (SENSOR_HANDLE_END_PLUS_ONE - 1) /*!< \brief End handle. */

#define SENSOR_UUID_SVC         0x3010
#define TEMP_UUID_CHR_DATA      0x3011
#define HUMIDITY_UUID_CHR_DATA  0x3012
#define ALS_UUID_CHR_DATA       0x3013

namespace wi
{

enum
{
  SENSOR_HANDLE_SVC = SENSOR_HANDLE_START, /*!< \brief Service declaration. */

  SENSOR_HANDLE_TEMP,                    /*!< \brief Temperature value. */
  SENSOR_HANDLE_HUMIDITY,                 /*!< \brief Humidity value. */
  SENSOR_HANDLE_ALS,                     /*!< \brief ALS value. */

  SENSOR_HANDLE_END_PLUS_ONE             /*!< \brief Maximum handle. */
};


/* Sensor service declaration. */
static const uint8_t  sensorValSvc[] = {UINT16_TO_BYTES(SENSOR_UUID_SVC)};
static const uint16_t sensorLenSvc   = sizeof(sensorValSvc);

/* Temperature data characteristic. */
static const uint8_t  tempValDataChr[] = {ATT_PROP_READ,
                                          UINT16_TO_BYTES(SENSOR_HANDLE_TEMP)};
static const uint16_t tempLenDataChr   = sizeof(tempValDataChr);

/* Temperature data. */
static const uint8_t  tempUuidData[] = {UINT16_TO_BYTES(TEMP_UUID_CHR_DATA)};
static       uint8_t  tempValData[]  = {0x00, 0x00};
static const uint16_t tempLenData    = sizeof(tempValData);

/* Humidity data characteristic. */
static const uint8_t  humidityValDataChr[] = {ATT_PROP_READ,
                                          UINT16_TO_BYTES(SENSOR_HANDLE_HUMIDITY)};
static const uint16_t humidityLenDataChr   = sizeof(humidityValDataChr);

/* Humidity data. */
static const uint8_t  humidityUuidData[] = {UINT16_TO_BYTES(HUMIDITY_UUID_CHR_DATA)};
static       uint8_t  humidityValData[]  = {0x00, 0x00};
static const uint16_t humidityLenData    = sizeof(humidityValData);

/* ALS data characteristic. */
static const uint8_t  alsValDataChr[] = {ATT_PROP_READ,
                                          UINT16_TO_BYTES(SENSOR_HANDLE_ALS)};
static const uint16_t alsLenDataChr   = sizeof(alsValDataChr);

/* ALS data. */
static const uint8_t  alsUuidData[] = {UINT16_TO_BYTES(ALS_UUID_CHR_DATA)};
static       uint8_t  alsValData[4 * sizeof(uint16_t)]  = {0x00};
static const uint16_t alsLenData    = sizeof(alsValData);

/* Attribute list for temp group. */
static const attsAttr_t tempList[] =
{
  /* Service declaration. */
  {
    attPrimSvcUuid,
    (uint8_t *) sensorValSvc,
    (uint16_t *) &sensorLenSvc,
    sizeof(sensorValSvc),
    0,
    ATTS_PERMIT_READ
  },
  /* Temperature Characteristic declaration. */
  {
    attChUuid,
    (uint8_t *) tempValDataChr,
    (uint16_t *) &tempLenDataChr,
    sizeof(tempValDataChr),
    0,
    ATTS_PERMIT_READ
  },
  /* Temperature Characteristic value. */
  {
    tempUuidData,
    (uint8_t *) tempValData,
    (uint16_t *) &tempLenData,
    sizeof(tempValData),
    0,
    ATTS_PERMIT_READ
  },
  /* Humidity characteristic declaration. */
  {
    attChUuid,
    (uint8_t *) humidityValDataChr,
    (uint16_t *) &humidityLenDataChr,
    sizeof(humidityValDataChr),
    0,
    ATTS_PERMIT_READ
  },
  /* Humidity characteristic value. */
  {
    humidityUuidData,
    (uint8_t *) humidityValData,
    (uint16_t *) &humidityLenData,
    sizeof(humidityValData),
    0,
    ATTS_PERMIT_READ
  },
  /* ALS Characteristic declaration. */
  {
    attChUuid,
    (uint8_t *) alsValDataChr,
    (uint16_t *) &alsLenDataChr,
    sizeof(alsValDataChr),
    0,
    ATTS_PERMIT_READ
  },
  /* ALS Characteristic value. */
  {
    alsUuidData,
    (uint8_t *) alsValData,
    (uint16_t *) &alsLenData,
    sizeof(alsValData),
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
        AttsRemoveGroup(SENSOR_HANDLE_START);
    }

    void update_temperature(int temp_celsium) override
    {
        uint8_t tempData[2] = {UINT16_TO_BYTES((uint16_t)temp_celsium)};
        AttsSetAttr(SENSOR_HANDLE_TEMP, sizeof(tempData), tempData);
    }

    void update_humidity(int rh) override
    {
        uint8_t humidityData[2] = {UINT16_TO_BYTES((uint16_t)rh)};
        AttsSetAttr(SENSOR_HANDLE_HUMIDITY, sizeof(humidityData), humidityData);
    }

    void update_als(uint16_t r, uint16_t g, uint16_t b, uint16_t c) override
    {
        uint8_t alsData[4 * sizeof(uint16_t)] = {UINT16_TO_BYTES(r),
                                                 UINT16_TO_BYTES(g),
                                                 UINT16_TO_BYTES(b),
                                                 UINT16_TO_BYTES(c)};
        AttsSetAttr(SENSOR_HANDLE_HUMIDITY, sizeof(alsData), alsData);
    }

private:
    wsfHandlerId_t handler_id_;

    attsGroup_t tempGroup_ =
    {
        NULL,
        (attsAttr_t *) tempList,
        NULL,
        NULL,
        SENSOR_HANDLE_START,
        SENSOR_HANDLE_END
    };
};

static ble_sensor_listener *svc_instance_;

void ble_sensor_svc_init(uint32_t handler_id)
{
    svc_instance_ = new ble_sensor_svc(handler_id);
}

ble_sensor_listener *ble_sensor_svc_get()
{
    return svc_instance_;
}

} // namespace wi
