/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-06-05
 * @brief       串口初始化代码(一般是串口1)，支持printf
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211103
 * 第一次发布
 * V1.1 20230605
 * 删除USART_UX_IRQHandler()函数的超时处理和修改HAL_UART_RxCpltCallback()
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"

int fputc(int ch, FILE *f); /* 重定义fputc函数 */
/******************************************************************************************/
/* 引脚 和 串口 定义
 * 默认是针对USART1的.
 * 注意: 通过修改这几个宏定义,可以支持USART1~UART5任意一个串口.
 */
#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   GPIO_PIN_9
#define USART_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   GPIO_PIN_10
#define USART_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 时钟使能 */

#define USART3_TX_GPIO_PORT                  GPIOB
#define USART3_TX_GPIO_PIN                   GPIO_PIN_10
#define USART3_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define USART3_RX_GPIO_PORT                  GPIOB
#define USART3_RX_GPIO_PIN                   GPIO_PIN_11
#define USART3_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define USART3_UX                            USART3
#define USART3_UX_IRQn                       USART3_IRQn
#define USART3_UX_IRQHandler                 USART3_IRQHandler
#define USART3_UX_CLK_ENABLE()               do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0)  /* USART3 时钟使能 */

 /******************************************************************************************/
#define USART_EN_RX                 1           /* 使能（1）/禁止（0）串口1接收 */
#define USART3_EN_RX                1           /* 使能（1）/禁止（0）串口3接收 */
/* 串口1定义 */
#define USART1_REC_LEN         200         /* 定义最大接收字节数 200 */
#define USART1_RXBUFFERSIZE    1                        /* 缓存大小 */
/* 串口2定义 */
#define USART3_REC_LEN         200     /* 定义最大接收字节数 200 */
#define USART3_RXBUFFERSIZE    1       /* 缓存大小 */
extern volatile uint8_t g_usart3_idle_flag;
extern volatile uint16_t g_usart3_rx_len;
extern UART_HandleTypeDef g_uart1_handle;       /* HAL UART句柄 */
extern UART_HandleTypeDef g_uart3_handle;       /* HAL UART句柄 */

/* 串口1句柄和缓冲区 */
extern UART_HandleTypeDef g_uart1_handle;
extern uint8_t g_usart1_rx_buf[USART1_REC_LEN];
extern uint16_t g_usart1_rx_sta;
extern uint8_t g_usart1_rx_buffer[USART1_RXBUFFERSIZE];

/* 串口2句柄和缓冲区 */
extern UART_HandleTypeDef g_uart3_handle;
extern uint8_t g_usart3_rx_buf[USART3_REC_LEN];
extern uint16_t g_usart3_rx_sta;
extern uint8_t g_usart3_rx_buffer[USART3_RXBUFFERSIZE];

void usart1_init(uint32_t baudrate);                /* 串口初始化函数 */
void usart3_init(uint32_t baudrate);
void usart3_restart_receive(void);
#endif
