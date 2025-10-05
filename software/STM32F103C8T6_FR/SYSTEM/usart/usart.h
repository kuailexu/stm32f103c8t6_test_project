/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2023-06-05
 * @brief       ���ڳ�ʼ������(һ���Ǵ���1)��֧��printf
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211103
 * ��һ�η���
 * V1.1 20230605
 * ɾ��USART_UX_IRQHandler()�����ĳ�ʱ������޸�HAL_UART_RxCpltCallback()
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"

int fputc(int ch, FILE *f); /* �ض���fputc���� */
/******************************************************************************************/
/* ���� �� ���� ����
 * Ĭ�������USART1��.
 * ע��: ͨ���޸��⼸���궨��,����֧��USART1~UART5����һ������.
 */
#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   GPIO_PIN_9
#define USART_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   GPIO_PIN_10
#define USART_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 ʱ��ʹ�� */

#define USART3_TX_GPIO_PORT                  GPIOB
#define USART3_TX_GPIO_PIN                   GPIO_PIN_10
#define USART3_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define USART3_RX_GPIO_PORT                  GPIOB
#define USART3_RX_GPIO_PIN                   GPIO_PIN_11
#define USART3_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define USART3_UX                            USART3
#define USART3_UX_IRQn                       USART3_IRQn
#define USART3_UX_IRQHandler                 USART3_IRQHandler
#define USART3_UX_CLK_ENABLE()               do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0)  /* USART3 ʱ��ʹ�� */

 /******************************************************************************************/
#define USART_EN_RX                 1           /* ʹ�ܣ�1��/��ֹ��0������1���� */
#define USART3_EN_RX                1           /* ʹ�ܣ�1��/��ֹ��0������3���� */
/* ����1���� */
#define USART1_REC_LEN         200         /* �����������ֽ��� 200 */
#define USART1_RXBUFFERSIZE    1                        /* �����С */
/* ����2���� */
#define USART3_REC_LEN         200     /* �����������ֽ��� 200 */
#define USART3_RXBUFFERSIZE    1       /* �����С */
extern volatile uint8_t g_usart3_idle_flag;
extern volatile uint16_t g_usart3_rx_len;
extern UART_HandleTypeDef g_uart1_handle;       /* HAL UART��� */
extern UART_HandleTypeDef g_uart3_handle;       /* HAL UART��� */

/* ����1����ͻ����� */
extern UART_HandleTypeDef g_uart1_handle;
extern uint8_t g_usart1_rx_buf[USART1_REC_LEN];
extern uint16_t g_usart1_rx_sta;
extern uint8_t g_usart1_rx_buffer[USART1_RXBUFFERSIZE];

/* ����2����ͻ����� */
extern UART_HandleTypeDef g_uart3_handle;
extern uint8_t g_usart3_rx_buf[USART3_REC_LEN];
extern uint16_t g_usart3_rx_sta;
extern uint8_t g_usart3_rx_buffer[USART3_RXBUFFERSIZE];

void usart1_init(uint32_t baudrate);                /* ���ڳ�ʼ������ */
void usart3_init(uint32_t baudrate);
void usart3_restart_receive(void);
#endif
