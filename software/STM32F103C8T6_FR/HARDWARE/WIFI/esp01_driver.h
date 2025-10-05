#ifndef ESP01_DRIVER_H
#define ESP01_DRIVER_H

#include "common.h"
#include "sys.h"
#include "usart.h"

/* OneNETƽ̨���� */
#define ONENET_SERVER        "183.230.40.39"     /* OneNET��������ַ */
#define ONENET_PORT          6002                /* MQTT TCP�˿� */
#define ONENET_DEVICE_ID     "your_device_id"    /* �豸ID */
#define ONENET_PRODUCT_ID    "your_product_id"   /* ��ƷID */
#define ONENET_ACCESS_KEY    "your_access_key"   /* �豸��Կ */

/* MQTT���ⶨ�� */
#define TOPIC_DEVICE_PROPERTY "$sys/product_id/device_id/thing/property/post"  /* �����ϱ� */
#define TOPIC_DEVICE_EVENT    "$sys/product_id/device_id/thing/event/post"     /* �¼��ϱ� */
#define TOPIC_COMMAND         "$sys/product_id/device_id/thing/command/+"      /* ������� */

/* ���������� */
#define DATASTREAM_TEMP       "temperature"
#define DATASTREAM_HUMID      "humidity"
#define DATASTREAM_STATUS     "device_status"

// ����ģʽ����
typedef enum
{
    ESP01_MODE_STA = 1,      // Stationģʽ
    ESP01_MODE_AP = 2,       // APģʽ
    ESP01_MODE_STA_AP = 3    // ���ģʽ
} ESP01_Mode_t;

/* ESP8266״̬���� */
typedef enum
{
    ESP8266_STATE_IDLE = 0,
    ESP8266_STATE_READY,
    ESP8266_STATE_WIFI_CONNECTED,
    ESP8266_STATE_MQTT_CONNECTED,
    ESP8266_STATE_ERROR
} ESP8266_State_t;

/* ESP8266�豸�ṹ�� */
typedef struct
{
    UART_HandleTypeDef *huart;
    ESP8266_State_t state;
    bool debug;
    char response_buffer[512];
    uint32_t last_heartbeat;
    uint16_t rx_index;
} ESP8266_Device_t;
/* �������� */
bool ESP8266_Init(ESP8266_Device_t *dev, UART_HandleTypeDef *huart, bool debug);
bool ESP8266_Reset(ESP8266_Device_t *dev);
bool ESP8266_ConnectWiFi(ESP8266_Device_t *dev, const char *ssid, const char *password);
bool ESP8266_ConnectMQTT(ESP8266_Device_t *dev);
bool ESP8266_PublishData(ESP8266_Device_t *dev, const char *datastream, float value);
bool ESP8266_PublishMultiData(ESP8266_Device_t *dev, float temp, float humid, int status);
bool ESP8266_WaitForResponse(ESP8266_Device_t *dev, const char *expected, uint32_t timeout);
bool ESP8266_SendCommand(ESP8266_Device_t *dev, const char *cmd, const char *expected, uint32_t timeout);
void ESP8266_ProcessResponse(ESP8266_Device_t *dev, uint8_t *data, uint16_t len);

#endif /* ESP01_DRIVER_H */
