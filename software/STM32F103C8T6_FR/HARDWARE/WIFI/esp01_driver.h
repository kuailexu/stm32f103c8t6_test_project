#ifndef ESP01_DRIVER_H
#define ESP01_DRIVER_H

#include "common.h"
#include "sys.h"
#include "usart.h"

/* OneNET平台配置 */
#define ONENET_SERVER        "183.230.40.39"     /* OneNET服务器地址 */
#define ONENET_PORT          6002                /* MQTT TCP端口 */
#define ONENET_DEVICE_ID     "your_device_id"    /* 设备ID */
#define ONENET_PRODUCT_ID    "your_product_id"   /* 产品ID */
#define ONENET_ACCESS_KEY    "your_access_key"   /* 设备密钥 */

/* MQTT主题定义 */
#define TOPIC_DEVICE_PROPERTY "$sys/product_id/device_id/thing/property/post"  /* 属性上报 */
#define TOPIC_DEVICE_EVENT    "$sys/product_id/device_id/thing/event/post"     /* 事件上报 */
#define TOPIC_COMMAND         "$sys/product_id/device_id/thing/command/+"      /* 命令接收 */

/* 数据流名称 */
#define DATASTREAM_TEMP       "temperature"
#define DATASTREAM_HUMID      "humidity"
#define DATASTREAM_STATUS     "device_status"

// 工作模式定义
typedef enum
{
    ESP01_MODE_STA = 1,      // Station模式
    ESP01_MODE_AP = 2,       // AP模式
    ESP01_MODE_STA_AP = 3    // 混合模式
} ESP01_Mode_t;

/* ESP8266状态定义 */
typedef enum
{
    ESP8266_STATE_IDLE = 0,
    ESP8266_STATE_READY,
    ESP8266_STATE_WIFI_CONNECTED,
    ESP8266_STATE_MQTT_CONNECTED,
    ESP8266_STATE_ERROR
} ESP8266_State_t;

/* ESP8266设备结构体 */
typedef struct
{
    UART_HandleTypeDef *huart;
    ESP8266_State_t state;
    bool debug;
    char response_buffer[512];
    uint32_t last_heartbeat;
    uint16_t rx_index;
} ESP8266_Device_t;
/* 函数声明 */
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
