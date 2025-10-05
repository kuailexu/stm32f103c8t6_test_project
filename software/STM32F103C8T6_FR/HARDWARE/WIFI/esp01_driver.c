#include "esp01_driver.h"

/**
 * @brief ESP8266初始化
 */
bool ESP8266_Init(ESP8266_Device_t *dev, UART_HandleTypeDef *huart, bool debug)
{
    if (dev == NULL || huart == NULL) return false;

    dev->huart = huart;
    dev->debug = debug;
    dev->state = ESP8266_STATE_IDLE;
    dev->last_heartbeat = 0;
    memset(dev->response_buffer, 0, sizeof(dev->response_buffer));
    if (debug)
    {
        printf("ESP8266初始化开始...\r\n");
    }
    /* 设置模式 */
    if (!ESP8266_SendCommand(dev, "CWMODE=1", "OK", 3000))
    {
        if (debug)
        {
            printf("设置模式失败\r\n");
        }
        return false;
    }

    /* 设置多连接模式 */
//    if (!ESP8266_SendCommand(dev, "CIPMUX=0", "OK", 3000)) {
//        if (debug) {
//            printf("设置单连接模式失败\r\n");
//        }
//        return false;
//    }
//    
    dev->state = ESP8266_STATE_READY;
    if (debug)
    {
        printf("ESP8266初始化成功\r\n");
    }
    return true;
}

/**
 * @brief 重置ESP8266
 */
bool ESP8266_Reset(ESP8266_Device_t *dev)
{
    return ESP8266_SendCommand(dev, "RST", "ready", 5000);
}

/**
 * @brief 连接WiFi
 */
bool ESP8266_ConnectWiFi(ESP8266_Device_t *dev, const char *ssid, const char *password)
{
    char cmd[128];
    if (dev == NULL || ssid == NULL || password == NULL) return false;
    snprintf(cmd, sizeof(cmd), "CWJAP=\"%s\",\"%s\"", ssid, password);
    if (ESP8266_SendCommand(dev, cmd, "WIFI GOT IP", 15000))
    {
        dev->state = ESP8266_STATE_WIFI_CONNECTED;

        if (dev->debug)
        {
            printf("WiFi连接成功: %s\r\n", ssid);
        }
        return true;
    }
    if (dev->debug)
    {
        printf("WiFi连接失败: %s\r\n", ssid);
    }
    return false;
}

/**
 * @brief 连接MQTT服务器
 */
bool ESP8266_ConnectMQTT(ESP8266_Device_t *dev)
{
    char cmd[256];
    if (dev == NULL) return false;
    if (dev->state != ESP8266_STATE_WIFI_CONNECTED)
    {
        if (dev->debug)
        {
            printf("错误: 请先连接WiFi\r\n");
        }
        return false;
    }
    /* 建立TCP连接 */
    snprintf(cmd, sizeof(cmd), "CIPSTART=\"TCP\",\"%s\",%d", ONENET_SERVER, ONENET_PORT);
    if (!ESP8266_SendCommand(dev, cmd, "CONNECT", 10000))
    {
        if (dev->debug)
        {
            printf("TCP连接失败\r\n");
        }
        return false;
    }

    /* 准备MQTT连接报文 */
    char mqtt_connect[512];
    /* 这里需要构建实际的MQTT CONNECT报文 */
    /* 简化版本，实际使用时需要按照OneNET的MQTT协议构建 */

    /* 发送MQTT连接 */
    snprintf(cmd, sizeof(cmd), "CIPSEND=%d", strlen(mqtt_connect));
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        return false;
    }

    /* 发送MQTT连接数据 */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_connect, strlen(mqtt_connect), 5000);
    /* 等待连接响应 */
    if (ESP8266_WaitForResponse(dev, "CONNACK", 5000))
    {
        dev->state = ESP8266_STATE_MQTT_CONNECTED;
        dev->last_heartbeat = HAL_GetTick();
        if (dev->debug)
        {
            printf("MQTT连接成功\r\n");
        }
        return true;
    }
    if (dev->debug)
    {
        printf("MQTT连接失败\r\n");
    }
    return false;
}

/**
 * @brief 发布传感器数据
 */
bool ESP8266_PublishData(ESP8266_Device_t *dev, const char *datastream, float value)
{
    char cmd[128];
    char mqtt_data[256];

    if (dev == NULL || datastream == NULL) return false;

    if (dev->state != ESP8266_STATE_MQTT_CONNECTED)
    {
        if (dev->debug)
        {
            printf("错误: MQTT未连接\r\n");
        }
        return false;
    }

    /* 构建MQTT发布报文 */
    snprintf(mqtt_data, sizeof(mqtt_data),
        "{\"id\":123,\"version\":\"1.0\",\"params\":{\"%s\":{\"value\":%.2f}}}",
        datastream, value);

    /* 发送数据长度 */
    snprintf(cmd, sizeof(cmd), "CIPSEND=%d", strlen(mqtt_data));
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        return false;
    }

    /* 发送数据 */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_data, strlen(mqtt_data), 5000);

    if (ESP8266_WaitForResponse(dev, "SEND OK", 3000))
    {
        if (dev->debug)
        {
            printf("数据发布成功: %s=%.2f\r\n", datastream, value);
        }
        return true;
    }

    if (dev->debug)
    {
        printf("数据发布失败\r\n");
    }
    return false;
}

/**
 * @brief 发布多个传感器数据
 */
bool ESP8266_PublishMultiData(ESP8266_Device_t *dev, float temp, float humid, int status)
{
    char cmd[128];
    char mqtt_data[512];

    if (dev == NULL) return false;

    if (dev->state != ESP8266_STATE_MQTT_CONNECTED)
    {
        if (dev->debug)
        {
            printf("错误: MQTT未连接\r\n");
        }
        return false;
    }

    /* 构建包含多个数据点的MQTT报文 */
    snprintf(mqtt_data, sizeof(mqtt_data),
        "{\"id\":123,\"version\":\"1.0\",\"params\":{"
        "\"%s\":{\"value\":%.2f},"
        "\"%s\":{\"value\":%.2f},"
        "\"%s\":{\"value\":%d}}}",
        DATASTREAM_TEMP, temp,
        DATASTREAM_HUMID, humid,
        DATASTREAM_STATUS, status);

    /* 发送数据长度 */
    snprintf(cmd, sizeof(cmd), "CIPSEND=%d", strlen(mqtt_data));
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        return false;
    }

    /* 发送数据 */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_data, strlen(mqtt_data), 5000);

    if (ESP8266_WaitForResponse(dev, "SEND OK", 3000))
    {
        if (dev->debug)
        {
            printf("多数据发布成功: 温度=%.2f, 湿度=%.2f, 状态=%d\r\n", temp, humid, status);
        }
        return true;
    }

    if (dev->debug)
    {
        printf("多数据发布失败\r\n");
    }
    return false;
}

/**
 * @brief 发送AT命令并等待响应
 */
bool ESP8266_SendCommand(ESP8266_Device_t *dev, const char *cmd, const char *expected, uint32_t timeout)
{
    char full_cmd[128];

    if (dev == NULL || cmd == NULL) return false;

    /* 清空接收缓冲区 */
//    memset(dev->response_buffer, 0, sizeof(dev->response_buffer));

    /*正确的AT命令格式 */
    if (strncmp(cmd, "AT", 2) == 0)
    {
        // 如果命令本身以AT开头，直接使用
        snprintf(full_cmd, sizeof(full_cmd), "%s\r\n", cmd);
    }
    else
    {
        // 否则添加AT+前缀
        snprintf(full_cmd, sizeof(full_cmd), "AT+%s\r\n", cmd);
    }
    HAL_UART_Transmit(dev->huart, (uint8_t *)full_cmd, strlen(full_cmd), 2000);
    if (dev->debug)
    {
        printf("发送: %s\r\n", full_cmd);
    }

    /* 等待响应 */
    return ESP8266_WaitForResponse(dev, expected, timeout);
}

/**
 * @brief 等待特定响应
 */
bool ESP8266_WaitForResponse(ESP8266_Device_t *dev, const char *expected, uint32_t timeout)
{
    uint32_t start_time = HAL_GetTick();

    while ((HAL_GetTick() - start_time) < timeout)
    {
        /* 检查是否收到预期响应 */
        if (strstr(dev->response_buffer, expected) != NULL)
        {
            if (dev->debug)
            {
                printf("收到预期响应: %s\r\n", expected);
            }
            return true;
        }

        /* 检查是否出错 */
        if (strstr(dev->response_buffer, "ERROR") != NULL)
        {
            if (dev->debug)
            {
                printf("收到错误响应\r\n");
            }
            return false;
        }

        HAL_Delay(10);
    }

    if (dev->debug)
    {
        printf("等待响应超时\r\n");
    }
    //		memset(dev->response_buffer, 0, sizeof(dev->response_buffer));
    return false;
}

/**
 * @brief 处理ESP8266的响应数据
 */
void ESP8266_ProcessResponse(ESP8266_Device_t *dev, uint8_t *data, uint16_t len)
{
    if (dev == NULL || data == NULL || len == 0) return;

    /* 将接收到的数据追加到响应缓冲区 */
    if (strlen(dev->response_buffer) + len < sizeof(dev->response_buffer))
    {
        strncat(dev->response_buffer, (char *)data, len);
    }

    if (dev->debug)
    {
        printf("ESP8266响应: %.*s\r\n", len, data);
    }
}
