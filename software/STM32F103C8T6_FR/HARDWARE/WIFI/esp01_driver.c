#include "esp01_driver.h"

/**
 * @brief ESP8266��ʼ��
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
        printf("ESP8266��ʼ����ʼ...\r\n");
    }
    /* ����ģʽ */
    if (!ESP8266_SendCommand(dev, "AT+CWMODE=1", "OK", 5000))
    {
        if (debug)
        {
            printf("����ģʽʧ��\r\n");
        }
        return false;
    }

    /* ���ö�����ģʽ */
//    if (!ESP8266_SendCommand(dev, "CIPMUX=0", "OK", 3000)) {
//        if (debug) {
//            printf("���õ�����ģʽʧ��\r\n");
//        }
//        return false;
//    }
//    
    dev->state = ESP8266_STATE_READY;
    return true;
}

/**
 * @brief ����ESP8266
 */
bool ESP8266_Reset(ESP8266_Device_t *dev)
{
    return ESP8266_SendCommand(dev, "RST", "ready", 5000);
}

/**
 * @brief ����WiFi
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
            printf("WiFi���ӳɹ�: %s\r\n", ssid);
        }
        return true;
    }
    if (dev->debug)
    {
        printf("WiFi����ʧ��: %s\r\n", ssid);
    }
    return false;
}
/**
 * @brief ����MQTT CONNECT����
 */
uint16_t Build_MQTT_Connect_Packet(char *buffer, const char *client_id,
    const char *username, const char *password)
{
    uint8_t *ptr = (uint8_t *)buffer;

    /* === �̶�ͷ Fixed Header === */
    // CONNECT�������� (0x10)
    *ptr++ = 0x10;

    // ʣ�೤��λ�� (�Ժ����)
    uint8_t *remaining_len_ptr = ptr;
    ptr++;

    /* === �ɱ�ͷ Variable Header === */
    // Э�������� (MSB + LSB)
    *ptr++ = 0x00;
    *ptr++ = 0x04;

    // Э���� "MQTT"
    *ptr++ = 'M';
    *ptr++ = 'Q';
    *ptr++ = 'T';
    *ptr++ = 'T';

    // Э�鼶�� MQTT 3.1.1
    *ptr++ = 0x04;

    // ���ӱ�־ Connect Flags
    uint8_t connect_flags = 0x02; // Clean Session = 1

    // ������û�������
    if (username != NULL && strlen(username) > 0)
    {
        connect_flags |= 0x80; // User Name Flag = 1
    }
    if (password != NULL && strlen(password) > 0)
    {
        connect_flags |= 0x40; // Password Flag = 1
    }

    *ptr++ = connect_flags;

    // ��������ʱ�� Keep Alive (60��)
    *ptr++ = 0x00;
    *ptr++ = 0x3C;

    /* === ��Ч�غ� Payload === */
    // �ͻ���ID Client Identifier
    uint16_t client_id_len = strlen(client_id);
    *ptr++ = (client_id_len >> 8) & 0xFF; // ���ȸ�λ
    *ptr++ = client_id_len & 0xFF;        // ���ȵ�λ
    memcpy(ptr, client_id, client_id_len);
    ptr += client_id_len;

    // �û��� User Name (OneNET��ʽ: ��ƷID$�豸����)
    if (connect_flags & 0x80)
    {
        uint16_t username_len = strlen(username);
        *ptr++ = (username_len >> 8) & 0xFF;
        *ptr++ = username_len & 0xFF;
        memcpy(ptr, username, username_len);
        ptr += username_len;
    }

    // ���� Password (OneNET��ʽ: ��Ȩ��Ϣ��token)
    if (connect_flags & 0x40)
    {
        uint16_t password_len = strlen(password);
        *ptr++ = (password_len >> 8) & 0xFF;
        *ptr++ = password_len & 0xFF;
        memcpy(ptr, password, password_len);
        ptr += password_len;
    }

    /* === ���㲢����ʣ�೤�� === */
    uint16_t remaining_len = (ptr - (uint8_t *)buffer) - 2; // ��ȥ�̶�ͷ��2�ֽ�
    *remaining_len_ptr = remaining_len;

    return ptr - (uint8_t *)buffer;
}
uint16_t Build_MQTT_PUB_Packet(char *buffer, const char *client_id, const char *message)
{
    uint8_t *ptr = (uint8_t *)buffer;

    /* === �̶�ͷ Fixed Header === */
    // CONNECT�������� (0x10)
    *ptr++ = 0x30;

    // ʣ�೤��λ�� (�Ժ����)
    uint8_t *remaining_len_ptr = ptr;
    ptr++;

    /* === ��Ч�غ� Payload === */
    // �ͻ���ID Client Identifier
    uint16_t client_id_len = strlen(client_id);
    *ptr++ = (client_id_len >> 8) & 0xFF; // ���ȸ�λ
    *ptr++ = client_id_len & 0xFF;        // ���ȵ�λ
    memcpy(ptr, client_id, client_id_len);
    ptr += client_id_len;
    /* === ��Ч�غ� Payload === */
    // �ͻ���ID Client Identifier
    uint16_t message_len = strlen(message);
    // *ptr++ = (message_len >> 8) & 0xFF; // ���ȸ�λ
    // *ptr++ = message_len & 0xFF;        // ���ȵ�λ
    memcpy(ptr, message, message_len);
    ptr += message_len;


    /* === ���㲢����ʣ�೤�� === */
    uint16_t remaining_len = (ptr - (uint8_t *)buffer) - 2; // ��ȥ�̶�ͷ��2�ֽ�
    *remaining_len_ptr = remaining_len;

    return ptr - (uint8_t *)buffer;
}
/**
 * @brief ����MQTT������
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
    //         printf("����: ��������WiFi\r\n");
    //     }
    //     return false;
    // }

    /* ����TCP���� */
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", MQTT_SERVER, MQTT_PORT);
    if (!ESP8266_SendCommand(dev, cmd, "CONNECT", 5000))
    {
        if (dev->debug)
        {
            printf("TCP����ʧ��\r\n");
        }
        return false;
    }
    HAL_Delay(2000);

    /* ����MQTT CONNECT���� */
    // OneNET MQTT���Ӳ���
    char client_id[64];
    char username[64];
    char password[64];

    // �����ͻ���ID (�豸����)
    snprintf(client_id, sizeof(client_id), "%s", MQTT_CLIENT_ID);

    // �����û��� (��ƷID$�豸����)
    snprintf(username, sizeof(username), "%s", MQTT_NAME);

    // ���� (��Ȩ��Ϣ��token)
    snprintf(password, sizeof(password), "%s", MQTT_PASSWORD);

    // ����MQTT CONNECT��
    uint16_t packet_len = Build_MQTT_Connect_Packet(mqtt_connect, client_id, username, password);

    if (dev->debug)
    {
        printf("MQTT CONNECT������: %d\r\n", packet_len);
        printf("������(HEX): ");
        for (int i = 0; i < packet_len; i++)
        {
            printf("%02X ", (uint8_t)mqtt_connect[i]);
        }
        printf("\r\n");
    }

    /* ����MQTT���� */
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", packet_len);
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        if (dev->debug)
        {
            printf("���ͳ�������ʧ��\r\n");
        }
        return false;
    }
    HAL_Delay(500);

    /* ����MQTT�������� */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_connect, packet_len, 5000);
    HAL_Delay(2000);

    /* �ȴ�������Ӧ CONNACK (0x20) */
    // CONNACK��Ӧ��ʽ: 0x20 0x02 0x00 0x00 (�ɹ�)
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
                uint16_t len = g_usart3_rx_len;  /* �õ��˴ν��յ������ݳ��� */
                if (len > 0)
                {
                    memcpy(response, g_usart3_rx_buf, len);
                    uint16_t mqtt_len = Parse_IPD_Response(response, mqtt_data);
                    printf("�յ���Ӧ: ");
                    for (int i = 0; i < mqtt_len; i++)
                    {
                        printf("%02X ", (uint8_t)mqtt_data[i]);
                    }
                    printf("\r\n");
                }
                usart3_restart_receive();                      /* �������״̬ */
            }

            // ���CONNACK��Ӧ (0x20 0x02 0x00 0x00)
            if ((uint8_t)mqtt_data[0] == 0x20 &&
                (uint8_t)mqtt_data[1] == 0x02 &&
                (uint8_t)mqtt_data[3] == 0x00) // ����ȷ�ϱ�־
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
            printf("MQTT���ӳɹ�\r\n");
        }
        return true;
    }

    if (dev->debug)
    {
        printf("MQTT����ʧ�ܣ�δ�յ�CONNACK\r\n");
    }
    return false;
}
/**
 * @brief ����IPD���ݲ���ȡMQTT��Ӧ
 * @param response ԭʼ��Ӧ����
 * @param mqtt_data �����MQTT���ݻ�����
 * @return MQTT���ݳ��ȣ�0��ʾû����Ч����
 */
uint16_t Parse_IPD_Response(const char *response, char *mqtt_data)
{
    // ����IPDǰ׺
    char *ipd_start = strstr(response, "+IPD,");
    if (ipd_start == NULL)
    {
        return 0; // û��IPD����
    }

    // ����"+IPD," (5���ַ�)
    ipd_start += 5;

    // �������ݳ���
    uint16_t data_len = 0;
    while (*ipd_start >= '0' && *ipd_start <= '9')
    {
        data_len = data_len * 10 + (*ipd_start - '0');
        ipd_start++;
    }

    // �ҵ�ð�ŷָ���
    if (*ipd_start != ':')
    {
        return 0; // ��ʽ����
    }

    // ����ð��
    ipd_start++;

    // ����MQTT����
    if (data_len > 0)
    {
        memcpy(mqtt_data, ipd_start, data_len);
        return data_len;
    }

    return 0;
}
/**
 * @brief ��������������
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
            printf("����: MQTTδ����\r\n");
        }
        return false;
    }
    char message[256];
    /* ����MQTT�������� */
    snprintf(client_id, sizeof(client_id), "%s", MQTT_CLIENT_ID);
    // snprintf(message, sizeof(message), "hello");
    snprintf(message, sizeof(message),
        "{\"id\":123,\"version\":\"1.0\",\"params\":{\"%s\":{\"value\":%.2f}}}",
        datastream, value);
    uint16_t packet_len = Build_MQTT_PUB_Packet(mqtt_pub_data, client_id, message);
    if (dev->debug)
    {
        printf("MQTT PUB������: %d\r\n", packet_len);
        printf("������(HEX): ");
        for (int i = 0; i < packet_len; i++)
        {
            printf("%02X ", (uint8_t)mqtt_pub_data[i]);
        }
        printf("\r\n");
    }
    /* �������ݳ��� */
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", packet_len);
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        return false;
    }

    /* �������� */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_pub_data, packet_len, 5000);
    HAL_Delay(2000);
    /* �ȴ�������Ӧ CONNACK (0x40) */
    // CONNACK��Ӧ��ʽ: 0x40 0x02 0x00 0x01 (�ɹ�)
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
                uint16_t len = g_usart3_rx_len;  /* �õ��˴ν��յ������ݳ��� */
                if (len > 0)
                {
                    memcpy(response, g_usart3_rx_buf, len);
                    uint16_t mqtt_len = Parse_IPD_Response(response, mqtt_data);
                    printf("�յ���Ӧ: ");

                    for (int i = 0; i < mqtt_len; i++)
                    {
                        printf("%02X ", (uint8_t)mqtt_data[i]);
                    }
                    printf("\r\n");
                }
                usart3_restart_receive();                      /* �������״̬ */
                connack_received = true;
                break;
            }

            // ���CONNACK��Ӧ (0x40 0x02 0x00 0x00)
            // if ((uint8_t)mqtt_data[0] == 0x40 &&
            //     (uint8_t)mqtt_data[1] == 0x02 &&
            //     (uint8_t)mqtt_data[3] == 0x00) // ����ȷ�ϱ�־
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
            printf("���ݷ����ɹ�: %s=%.2f\r\n", datastream, value);
        }
        return true;
    }
    else
    {
        if (dev->debug)
        {
            printf("���ݷ���ʧ�ܣ�δ�յ���Ӧ\r\n");
        }
        return false;
    }

    // if (dev->debug)
    // {
    //     printf("���ݷ���ʧ��\r\n");
    // }
    // return false;
}

/**
 * @brief �����������������
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
            printf("����: MQTTδ����\r\n");
        }
        return false;
    }

    /* ��������������ݵ��MQTT���� */
    snprintf(mqtt_data, sizeof(mqtt_data),
        "{\"id\":123,\"version\":\"1.0\",\"params\":{"
        "\"%s\":{\"value\":%.2f},"
        "\"%s\":{\"value\":%.2f},"
        "\"%s\":{\"value\":%d}}}",
        DATASTREAM_TEMP, temp,
        DATASTREAM_HUMID, humid,
        DATASTREAM_STATUS, status);

    /* �������ݳ��� */
    snprintf(cmd, sizeof(cmd), "CIPSEND=%d", strlen(mqtt_data));
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        return false;
    }

    /* �������� */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_data, strlen(mqtt_data), 5000);

    if (ESP8266_WaitForResponse(dev, "SEND OK", 3000))
    {
        if (dev->debug)
        {
            printf("�����ݷ����ɹ�: �¶�=%.2f, ʪ��=%.2f, ״̬=%d\r\n", temp, humid, status);
        }
        return true;
    }

    if (dev->debug)
    {
        printf("�����ݷ���ʧ��\r\n");
    }
    return false;
}

/**
 * @brief ����AT����ȴ���Ӧ
 */
bool ESP8266_SendCommand(ESP8266_Device_t *dev, const char *cmd, const char *expected, uint32_t timeout)
{
    char full_cmd[128];

    if (dev == NULL || cmd == NULL) return false;

    /* ��ս��ջ����� */
    memset(dev->response_buffer, 0, sizeof(dev->response_buffer));

    /*��ȷ��AT�����ʽ */
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
        printf("����: %s\r\n", full_cmd);
    }

    /* �ȴ���Ӧ */
    return ESP8266_WaitForResponse(dev, expected, timeout);
}

/**
 * @brief �ȴ��ض���Ӧ
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
            /* ����Ƿ��յ�Ԥ����Ӧ */
            if (strstr(dev->response_buffer, expected) != NULL)
            {
                if (dev->debug)
                {
                    printf("�յ�Ԥ����Ӧ: %s\r\n", expected);
                }
                return true;
            }
            /* ����Ƿ���� */
            if (strstr(dev->response_buffer, "ERROR") != NULL)
            {
                if (dev->debug)
                {
                    printf("�յ�������Ӧ\r\n");
                }
                return false;
            }
            /* �������״̬ */
        }
        HAL_Delay(10);

    }

    if (dev->debug)
    {
        printf("�ȴ���Ӧ��ʱ\r\n");
    }
    memset(dev->response_buffer, 0, sizeof(dev->response_buffer));
    return false;
}

/**
 * @brief ����ESP8266����Ӧ����
 */
void ESP8266_ProcessResponse(ESP8266_Device_t *dev, uint8_t *data, uint16_t len)
{
    if (dev == NULL || data == NULL || len == 0) return;

    /* �����յ�������׷�ӵ���Ӧ������ */
    if (strlen(dev->response_buffer) + len < sizeof(dev->response_buffer))
    {
        strncat(dev->response_buffer, (char *)data, len);
    }

    if (dev->debug)
    {
        printf("ESP8266��Ӧ: %.*s\r\n", len, data);
    }
}
