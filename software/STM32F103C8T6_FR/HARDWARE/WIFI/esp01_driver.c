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
    if (!ESP8266_SendCommand(dev, "CWMODE=1", "OK", 3000))
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
    if (debug)
    {
        printf("ESP8266��ʼ���ɹ�\r\n");
    }
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
    snprintf(cmd, sizeof(cmd), "CWJAP=\"%s\",\"%s\"", ssid, password);
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
 * @brief ����MQTT������
 */
bool ESP8266_ConnectMQTT(ESP8266_Device_t *dev)
{
    char cmd[256];
    if (dev == NULL) return false;
    if (dev->state != ESP8266_STATE_WIFI_CONNECTED)
    {
        if (dev->debug)
        {
            printf("����: ��������WiFi\r\n");
        }
        return false;
    }
    /* ����TCP���� */
    snprintf(cmd, sizeof(cmd), "CIPSTART=\"TCP\",\"%s\",%d", ONENET_SERVER, ONENET_PORT);
    if (!ESP8266_SendCommand(dev, cmd, "CONNECT", 10000))
    {
        if (dev->debug)
        {
            printf("TCP����ʧ��\r\n");
        }
        return false;
    }

    /* ׼��MQTT���ӱ��� */
    char mqtt_connect[512];
    /* ������Ҫ����ʵ�ʵ�MQTT CONNECT���� */
    /* �򻯰汾��ʵ��ʹ��ʱ��Ҫ����OneNET��MQTTЭ�鹹�� */

    /* ����MQTT���� */
    snprintf(cmd, sizeof(cmd), "CIPSEND=%d", strlen(mqtt_connect));
    if (!ESP8266_SendCommand(dev, cmd, ">", 3000))
    {
        return false;
    }

    /* ����MQTT�������� */
    HAL_UART_Transmit(dev->huart, (uint8_t *)mqtt_connect, strlen(mqtt_connect), 5000);
    /* �ȴ�������Ӧ */
    if (ESP8266_WaitForResponse(dev, "CONNACK", 5000))
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
        printf("MQTT����ʧ��\r\n");
    }
    return false;
}

/**
 * @brief ��������������
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
            printf("����: MQTTδ����\r\n");
        }
        return false;
    }

    /* ����MQTT�������� */
    snprintf(mqtt_data, sizeof(mqtt_data),
        "{\"id\":123,\"version\":\"1.0\",\"params\":{\"%s\":{\"value\":%.2f}}}",
        datastream, value);

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
            printf("���ݷ����ɹ�: %s=%.2f\r\n", datastream, value);
        }
        return true;
    }

    if (dev->debug)
    {
        printf("���ݷ���ʧ��\r\n");
    }
    return false;
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
//    memset(dev->response_buffer, 0, sizeof(dev->response_buffer));

    /*��ȷ��AT�����ʽ */
    if (strncmp(cmd, "AT", 2) == 0)
    {
        // ����������AT��ͷ��ֱ��ʹ��
        snprintf(full_cmd, sizeof(full_cmd), "%s\r\n", cmd);
    }
    else
    {
        // �������AT+ǰ׺
        snprintf(full_cmd, sizeof(full_cmd), "AT+%s\r\n", cmd);
    }
    HAL_UART_Transmit(dev->huart, (uint8_t *)full_cmd, strlen(full_cmd), 2000);
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

        HAL_Delay(10);
    }

    if (dev->debug)
    {
        printf("�ȴ���Ӧ��ʱ\r\n");
    }
    //		memset(dev->response_buffer, 0, sizeof(dev->response_buffer));
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
