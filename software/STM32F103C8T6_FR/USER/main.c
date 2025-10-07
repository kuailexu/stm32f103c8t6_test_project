#include "sys.h"  
#include "usart.h"
#include "delay.h"
#include "mem_mgr.h"
#include "freertos_demo.h"
#include "led.h"
#include "QDTFT_demo.h"
#include "adc.h"
#include "key.h"
#include "gtim.h"
#include "esp01_driver.h"
/* 全局变量 */
ESP8266_Device_t esp8266;
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];
/* WiFi配置 */
#define WIFI_SSID        "TP-LINK_WYG"
#define WIFI_PASSWORD    "1111122222"
/* 局部变量 */
RCC_ClkInitTypeDef clk_config;
uint32_t flash_latency;
int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL6);
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
    delay_init(72);
    usart1_init(115200);
    printf("系统启动...\r\n");
    usart3_init(115200);
    printf("串口2初始化完成\r\n");
    if (ESP8266_Init(&esp8266, &g_uart3_handle, true))
    {
        printf("ESP8266初始化成功\r\n");
        HAL_Delay(1000);
        /* 连接WiFi */
        if (ESP8266_ConnectWiFi(&esp8266, WIFI_SSID, WIFI_PASSWORD))
        {
            printf("WiFi连接成功\r\n");
            HAL_Delay(1000);
            //     /* 连接MQTT */
            //     if (ESP8266_ConnectMQTT(&esp8266))
            //     {
            //         printf("MQTT连接成功，准备上传数据...\r\n");
            //     }
        }
    }
    // led_init();
    // Lcd_Init();
    // key_init();
    // mem_init();
    // gtim_timx_int_init(100-1,7200-1);
    // gtim_timx_pwm_chy_init(50 - 1, 72 - 1);
    // adc_dma_init((uint32_t)&g_adc_dma_buf);
    // TIM2_Capture_Init();
    // Redraw_Mainmenu();
    while (1)
    {
        /* 检查串口1是否接收到完整数据 */
        if (g_usart1_rx_sta & 0x8000)
        {
            uint16_t len = g_usart1_rx_sta & 0x3FFF;  /* 得到此次接收到的数据长度 */
            printf("\r\n收到串口1数据: %.*s\r\n", len, g_usart1_rx_buf);
            if (strstr((const char *)g_usart1_rx_buf, "MQTT") != NULL)
            {
                ESP8266_ConnectMQTT(&esp8266);
            }
            else if (strstr((const char *)g_usart1_rx_buf, "PUB") != NULL)
            {
                ESP8266_PublishData(&esp8266, DATASTREAM_TEMP, 25.5);
            }
            else if (strstr((const char *)g_usart1_rx_buf, "SUB") != NULL)
            {
                ESP8266_SubscribeData(&esp8266);
            }
            else if (esp8266.state == ESP8266_STATE_READY)
            {
                ESP8266_SendCommand(&esp8266, (const char *)g_usart1_rx_buf, "OK", 3000);
            }
            memset(g_usart1_rx_buf, 0, sizeof(g_usart1_rx_buf));
            g_usart1_rx_sta = 0;                      /* 清除接收状态 */
        }
        if (g_usart3_idle_flag)
        {
            uint16_t len = g_usart3_rx_len;  /* 得到此次接收到的数据长度 */
            if (len > 0)
            {
                if (strstr((const char *)g_usart3_rx_buf, "CLOSED") != NULL)
                {
                    printf("ESP8266连接已关闭，尝试重新连接MQTT...\r\n");
                    ESP8266_ConnectMQTT(&esp8266);
                }
                printf("\r\n收到串口3数据: %.*s\r\n", len, (const char *)g_usart3_rx_buf);
            }
            usart3_restart_receive();                      /* 清除接收状态 */
        }
        HAL_Delay(10);

        // 		HAL_RCC_GetClockConfig(&clk_config, &flash_latency);
        // 		KeyEvent event = Button_Debounce(Read_KEY0, KEY_ID_0);
        // 		if (event == KEY_EVENT_PRESSED) {
        // 			LED1(0);
        // 			delay_ms(200);
        // 			LED1_TOGGLE();
        // 			freertos_demo();
        // 			break;
        // 		}
        // 		delay_ms(10); 
    }
}
