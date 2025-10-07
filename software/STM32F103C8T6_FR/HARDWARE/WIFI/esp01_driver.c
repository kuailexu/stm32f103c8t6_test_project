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
    if (!ESP8266_SendCommand(dev, "AT+CWMODE=1", "OK", 5000))
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
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
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
 * @brief 构建MQTT CONNECT报文
 */
uint16_t Build_MQTT_Connect_Packet(char *buffer, const char *client_id,
    const char *username, const char *password)
{
    uint8_t *ptr = (uint8_t *)buffer;

    /* === 固定头 Fixed Header === */
    // CONNECT报文类型 (0x10)
    *ptr++ = 0x10;

    // 剩余长度位置 (稍后回填)
    uint8_t *remaining_len_ptr = ptr;
    ptr++;

    /* === 可变头 Variable Header === */
    // 协议名长度 (MSB + LSB)
    *ptr++ = 0x00;
    *ptr++ = 0x04;

    // 协议名 "MQTT"
    *ptr++ = 'M';
    *ptr++ = 'Q';
    *ptr++ = 'T';
    *ptr++ = 'T';

    // 协议级别 MQTT 3.1.1
    *ptr++ = 0x04;

    // 连接标志 Connect Flags
    uint8_t connect_flags = 0x02; // Clean Session = 1

    // 如果有用户名密码
    if (username != NULL && strlen(username) > 0)
    {
        connect_flags |= 0x80; // User Name Flag = 1
    }
    if (password != NULL && strlen(password) > 0)
    {
        connect_flags |= 0x40; // Password Flag = 1
    }

    *ptr++ = connect_flags;

    // 保持连接时间 Keep Alive (60秒)
    *ptr++ = 0x00;
    *ptr++ = 0x3C;

    /* === 有效载荷 Payload === */
    // 客户端ID Client Identifier
    uint16_t client_id_len = strlen(client_id);
    *ptr++ = (client_id_len >> 8) & 0xFF; // 长度高位
    *ptr++ = client_id_len & 0xFF;        // 长度低位
    memcpy(ptr, client_id, client_id_len);
    ptr += client_id_len;

    // 用户名 User Name (OneNET格式: 产品ID$设备名称)
    if (connect_flags & 0x80)
    {
        uint16_t username_len = strlen(username);
        *ptr++ = (username_len >> 8) & 0xFF;
        *ptr++ = username_len & 0xFF;
        memcpy(ptr, username, username_len);
        ptr += username_len;
    }

    // 密码 Password (OneNET格式: 鉴权信息或token)
    if (connect_flags & 0x40)
    {
        uint16_t password_len = strlen(password);
        *ptr++ = (password_len >> 8) & 0xFF;
        *ptr++ = password_len & 0xFF;
        memcpy(ptr, password, password_len);
        ptr += password_len;
    }

    /* === 计算并回填剩余长度 === */
    uint16_t remaining_len = (ptr - (uint8_t *)buffer) - 2; // 减去固定头的2字节
    *remaining_len_ptr = remaining_len;

    return ptr - (uint8_t *)buffer;
}
uint16_t Build_MQTT_PUB_Packet(char *buffer, const char *client_id, const char *message)
{
    uint8_t *ptr = (uint8_t *)buffer;

    /* === 固定头 Fixed Header === */
    // CONNECT报文类型 (0x10)
    *ptr++ = 0x30;

    // 剩余长度位置 (稍后回填)
    uint8_t *remaining_len_ptr = ptr;
    ptr++;

    /* === 有效载荷 Payload === */
    // 客户端ID Client Identifier
    uint16_t client_id_len = strlen(client_id);
    *ptr++ = (client_id_len >> 8) & 0xFF; // 长度高位
    *ptr++ = client_id_len & 0xFF;        // 长度低位
    memcpy(ptr, client_id, client_id_len);
    ptr += client_id_len;
    /* === 有效载荷 Payload === */
    // 客户端ID Client Identifier
    uint16_t message_len = strlen(message);
    // *ptr++ = (message_len >> 8) & 0xFF; // 长度高位
    // *ptr++ = message_len & 0xFF;        // 长度低位
    memcpy(ptr, message, message_len);
    ptr += message_len;


    /* === 计算并回填剩余长度 === */
    uint16_t remaining_len = (ptr - (uint8_t *)buffer) - 2; // 减去固定头的2字节
    *remaining_len_ptr = remaining_len;

    return ptr - (uint8_t *)buffer;
}
/**
 * @brief 连接MQTT服务器
 */
bool ESP8266_ConnectMQTT(ESP8266_Device_t *dev)
{
    char cmd[256];
    char mqtt_connect[512];

    if (dev == NULL) return false;
    // if (dev->state != ESP8266_STATE_WIFI_CONNECTED)
    // {
    //     if (dev->debug)
    //     {
    //         printf("错误: 请先连接WiFi\r\n");
    //     }
    //     return false;
    // }

    /* 建立TCP连接 */
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", MQTT_SERVER, MQTT_PORT);
    if (!ESP8266_SendCommand(dev, cmd, "CONNECT", 5000))
    {
        if (dev->debug)
        {
            printf("TCP连接失败\r\n");
        }
        return false;
    }
    HAL_Delay(2000);

    /* 构建MQTT CONNECT报文 */
    // OneNET MQTT连接参数
    char client_id[64];
    char username[64];
    char password[64];

    // 构建客户端ID (设备名称)
    snprintf(client_id, sizeof(client_id), "%s", MQTT_CLIENT_ID);

    // 构建用户名 (产品ID$设备名称)
    snprintf(username, sizeof(username), "%s", MQTT_NAME);

    // 密码 (鉴权信息或token)
    snprintf(password, sizeof(password), "%s", MQTT_PASSWORD);

    // 构建MQTT CONNECT包
    uint16_t packet_len = Build_MQTT_Connect_Packet(mqtt_connect, client_id, username, password);

    if (dev->debug)
    {
        printf("MQTT CONNECT包长度: %d\r\n", packet_len);
        printf("包内容(HEX): ");
        for (int i = 0; i < packet_len; i++)
        {
            printf("%02X ", (uint8_t)mqtt_connect[i]);
        }
        printf("\r\n");
    }

    /* 发送MQTT连接 */
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", packet_len);
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        if (dev->debug)
        {
            printf("发送长度设置失败\r\n");
        }
        return false;
    }
    HAL_Delay(500);

    /* 发送MQTT连接数据 */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_connect, packet_len, 5000);
    HAL_Delay(2000);

    /* 等待连接响应 CONNACK (0x20) */
    // CONNACK响应格式: 0x20 0x02 0x00 0x00 (成功)
    char response[64];
    uint32_t start_time = HAL_GetTick();
    bool connack_received = false;
    char mqtt_data[32];
    while ((HAL_GetTick() - start_time) < 5000)
    {
        if (g_usart3_idle_flag)
        {
            if (dev->debug)
            {
                uint16_t len = g_usart3_rx_len;  /* 得到此次接收到的数据长度 */
                if (len > 0)
                {
                    memcpy(response, g_usart3_rx_buf, len);
                    uint16_t mqtt_len = Parse_IPD_Response(response, mqtt_data);
                    printf("收到响应: ");
                    for (int i = 0; i < mqtt_len; i++)
                    {
                        printf("%02X ", (uint8_t)mqtt_data[i]);
                    }
                    printf("\r\n");
                }
                usart3_restart_receive();                      /* 清除接收状态 */
            }

            // 检查CONNACK响应 (0x20 0x02 0x00 0x00)
            if ((uint8_t)mqtt_data[0] == 0x20 &&
                (uint8_t)mqtt_data[1] == 0x02 &&
                (uint8_t)mqtt_data[3] == 0x00) // 连接确认标志
            {
                connack_received = true;
                break;
            }
        }
    }

    if (connack_received)
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
        printf("MQTT连接失败，未收到CONNACK\r\n");
    }
    return false;
}
/**
 * @brief 解析IPD数据并提取MQTT响应
 * @param response 原始响应数据
 * @param mqtt_data 输出的MQTT数据缓冲区
 * @return MQTT数据长度，0表示没有有效数据
 */
uint16_t Parse_IPD_Response(const char *response, char *mqtt_data)
{
    // 查找IPD前缀
    char *ipd_start = strstr(response, "+IPD,");
    if (ipd_start == NULL)
    {
        return 0; // 没有IPD数据
    }

    // 跳过"+IPD," (5个字符)
    ipd_start += 5;

    // 解析数据长度
    uint16_t data_len = 0;
    while (*ipd_start >= '0' && *ipd_start <= '9')
    {
        data_len = data_len * 10 + (*ipd_start - '0');
        ipd_start++;
    }

    // 找到冒号分隔符
    if (*ipd_start != ':')
    {
        return 0; // 格式错误
    }

    // 跳过冒号
    ipd_start++;

    // 复制MQTT数据
    if (data_len > 0)
    {
        memcpy(mqtt_data, ipd_start, data_len);
        return data_len;
    }

    return 0;
}
/**
 * @brief 发布传感器数据
 */
bool ESP8266_PublishData(ESP8266_Device_t *dev, const char *datastream, float value)
{
    char cmd[128];
    char client_id[64];
    char mqtt_pub_data[256];

    if (dev == NULL || datastream == NULL) return false;

    if (dev->state != ESP8266_STATE_MQTT_CONNECTED)
    {
        if (dev->debug)
        {
            printf("错误: MQTT未连接\r\n");
        }
        return false;
    }
    char message[256];
    /* 构建MQTT发布报文 */
    snprintf(client_id, sizeof(client_id), "%s", MQTT_CLIENT_ID);
    // snprintf(message, sizeof(message), "hello");
    snprintf(message, sizeof(message),
        "{\"id\":123,\"version\":\"1.0\",\"params\":{\"%s\":{\"value\":%.2f}}}",
        datastream, value);
    uint16_t packet_len = Build_MQTT_PUB_Packet(mqtt_pub_data, client_id, message);
    if (dev->debug)
    {
        printf("MQTT PUB包长度: %d\r\n", packet_len);
        printf("包内容(HEX): ");
        for (int i = 0; i < packet_len; i++)
        {
            printf("%02X ", (uint8_t)mqtt_pub_data[i]);
        }
        printf("\r\n");
    }
    /* 发送数据长度 */
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", packet_len);
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        return false;
    }

    /* 发送数据 */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_pub_data, packet_len, 5000);
    HAL_Delay(2000);
    /* 等待连接响应 CONNACK (0x40) */
    // CONNACK响应格式: 0x40 0x02 0x00 0x01 (成功)
    char response[64];
    uint32_t start_time = HAL_GetTick();
    bool connack_received = false;
    char mqtt_data[32];
    while ((HAL_GetTick() - start_time) < 5000)
    {
        if (g_usart3_idle_flag)
        {
            if (dev->debug)
            {
                uint16_t len = g_usart3_rx_len;  /* 得到此次接收到的数据长度 */
                if (len > 0)
                {
                    memcpy(response, g_usart3_rx_buf, len);
                    uint16_t mqtt_len = Parse_IPD_Response(response, mqtt_data);
                    printf("收到响应: ");

                    for (int i = 0; i < mqtt_len; i++)
                    {
                        printf("%02X ", (uint8_t)mqtt_data[i]);
                    }
                    printf("\r\n");
                }
                usart3_restart_receive();                      /* 清除接收状态 */
                connack_received = true;
                break;
            }

            // 检查CONNACK响应 (0x40 0x02 0x00 0x00)
            // if ((uint8_t)mqtt_data[0] == 0x40 &&
            //     (uint8_t)mqtt_data[1] == 0x02 &&
            //     (uint8_t)mqtt_data[3] == 0x00) // 连接确认标志
            // {
            //     connack_received = true;
            //     break;
            // }
        }
    }

    if (connack_received)
    {
        if (dev->debug)
        {
            printf("数据发布成功: %s=%.2f\r\n", datastream, value);
        }
        return true;
    }
    else
    {
        if (dev->debug)
        {
            printf("数据发布失败，未收到响应\r\n");
        }
        return false;
    }

    // if (dev->debug)
    // {
    //     printf("数据发布失败\r\n");
    // }
    // return false;
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
    memset(dev->response_buffer, 0, sizeof(dev->response_buffer));

    /*正确的AT命令格式 */
    if (strncmp(cmd, "AT", 2) == 0)
    {
        snprintf(full_cmd, sizeof(full_cmd), "%s\r\n", cmd);
    }
    else
    {
        snprintf(full_cmd, sizeof(full_cmd), "%s\r\n", cmd);
    }
    HAL_UART_Transmit(dev->huart, (uint8_t *)full_cmd, strlen(full_cmd), timeout);
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
        if (g_usart3_idle_flag)
        {
            memcpy(dev->response_buffer, g_usart3_rx_buf, sizeof(dev->response_buffer) - 1);
            usart3_restart_receive();
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
            /* 清除接收状态 */
        }
        HAL_Delay(10);

    }

    if (dev->debug)
    {
        printf("等待响应超时\r\n");
    }
    memset(dev->response_buffer, 0, sizeof(dev->response_buffer));
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
