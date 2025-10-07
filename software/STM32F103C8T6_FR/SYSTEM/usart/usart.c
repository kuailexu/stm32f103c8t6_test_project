#include "sys.h"
#include "usart.h"
#include "esp01_driver.h"

/* ���ʹ��os,����������ͷ�ļ�����. */
#if SYSTEM_SUPPORT_OS
//#include "includes.h" /* os ʹ�� */
#endif

/******************************************************************************************/
/* �������´���, ֧��printf����, ������Ҫѡ��use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* ʹ��AC6������ʱ */
__asm(".global __use_no_semihosting\n\t");  /* ������ʹ�ð�����ģʽ */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6����Ҫ����main����Ϊ�޲�����ʽ�����򲿷����̿��ܳ��ְ�����ģʽ */

#else
/* ʹ��AC5������ʱ, Ҫ�����ﶨ��__FILE �� ��ʹ�ð�����ģʽ */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* ��ʹ�ð�����ģʽ��������Ҫ�ض���_ttywrch\_sys_exit\_sys_command_string����,��ͬʱ����AC6��AC5ģʽ */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE �� stdio.h���涨��. */
FILE __stdout;

/* MDK����Ҫ�ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

    USART_UX->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    return ch;
}
#endif
/******************************************************************************************/

#if USART_EN_RX /*���ʹ���˽���*/

/* ����1���ջ�������״̬ */
uint8_t g_usart1_rx_buf[USART1_REC_LEN];
uint16_t g_usart1_rx_sta = 0;
uint8_t g_usart1_rx_buffer[USART1_RXBUFFERSIZE];

/* ����2���ջ�������״̬ */
uint8_t g_usart3_rx_buf[USART3_REC_LEN];
uint16_t g_usart3_rx_sta = 0;
uint8_t g_usart3_rx_buffer[USART3_RXBUFFERSIZE];
/* USART3 IDLE�ж���ر��� */
volatile uint8_t g_usart3_idle_flag = 0;    /* IDLE�жϱ�־ */
volatile uint16_t g_usart3_rx_len = 0;      /* ʵ�ʽ��յ������ݳ��� */
/* ���ھ�� */
UART_HandleTypeDef g_uart1_handle, g_uart3_handle;
/**
 * @brief       ����X��ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @note        ע��: ����������ȷ��ʱ��Դ, ���򴮿ڲ����ʾͻ������쳣.
 *              �����USART��ʱ��Դ��sys_stm32_clock_init()�������Ѿ����ù���.
 * @retval      ��
 */
void usart1_init(uint32_t baudrate)
{
    /*UART ��ʼ������*/
    g_uart1_handle.Instance = USART_UX;                                       /* USART_UX */
    g_uart1_handle.Init.BaudRate = baudrate;                                  /* ������ */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;                           /* һ��ֹͣλ */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;                            /* ����żУ��λ */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* ��Ӳ������ */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;                               /* �շ�ģʽ */
    HAL_UART_Init(&g_uart1_handle);                                           /* HAL_UART_Init()��ʹ��UART1 */

    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_usart1_rx_buffer, USART1_RXBUFFERSIZE);
}
void usart3_init(uint32_t baudrate)
{
    /*UART ��ʼ������*/
    g_uart3_handle.Instance = USART3_UX;                                       /* USART_UX */
    g_uart3_handle.Init.BaudRate = baudrate;                                  /* ������ */
    g_uart3_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart3_handle.Init.StopBits = UART_STOPBITS_1;                           /* һ��ֹͣλ */
    g_uart3_handle.Init.Parity = UART_PARITY_NONE;                            /* ����żУ��λ */
    g_uart3_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* ��Ӳ������ */
    g_uart3_handle.Init.Mode = UART_MODE_TX_RX;                               /* �շ�ģʽ */
    HAL_UART_Init(&g_uart3_handle);                                           /* HAL_UART_Init()��ʹ��UART1 */
    /* ����IDLE�ж� */
    __HAL_UART_ENABLE_IT(&g_uart3_handle, UART_IT_IDLE);
    __HAL_UART_ENABLE_IT(&g_uart3_handle, UART_IT_RXNE);  // ȷ�������ж�Ҳ����
    /* ���ý���״̬ */
    g_usart3_rx_sta = 0;
    g_usart3_idle_flag = 0;
    g_usart3_rx_len = 0;

    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart3_handle, (uint8_t *)g_usart3_rx_buffer, USART3_RXBUFFERSIZE);
}
/**
 * @brief       UART�ײ��ʼ������
 * @param       huart: UART�������ָ��
 * @note        �˺����ᱻHAL_UART_Init()����
 *              ���ʱ��ʹ�ܣ��������ã��ж�����
 * @retval      ��
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;

    if (huart->Instance == USART_UX)                            /* ����Ǵ���1�����д���1 MSP��ʼ�� */
    {
        USART_TX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���TX��ʱ�� */
        USART_RX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���RX��ʱ�� */
        USART_UX_CLK_ENABLE();                                  /* ʹ�ܴ���ʱ�� */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* ���ڷ������ź� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO�ٶ�����Ϊ���� */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* ����RX�� ģʽ���� */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* ����RX�� �������ó�����ģʽ */

#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* ʹ��USART1�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 5, 5);              /* ��2��������ȼ�:��ռ���ȼ�3�������ȼ�3 */
#endif
    }
    if (huart->Instance == USART3_UX)                            /* ����Ǵ���1�����д���1 MSP��ʼ�� */
    {
        USART3_TX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���TX��ʱ�� */
        USART3_RX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���RX��ʱ�� */
        USART3_UX_CLK_ENABLE();                                  /* ʹ�ܴ���ʱ�� */

        gpio_init_struct.Pin = USART3_TX_GPIO_PIN;               /* ���ڷ������ź� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO�ٶ�����Ϊ���� */
        HAL_GPIO_Init(USART3_TX_GPIO_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = USART3_RX_GPIO_PIN;               /* ����RX�� ģʽ���� */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART3_RX_GPIO_PORT, &gpio_init_struct);   /* ����RX�� �������ó�����ģʽ */

#if USART3_EN_RX
        HAL_NVIC_EnableIRQ(USART3_UX_IRQn);                      /* ʹ��USART3�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART3_UX_IRQn, 5, 4);              /* ��2��������ȼ�:��ռ���ȼ�3�������ȼ�3 */
#endif
    }
}

/**
 * @brief       �������ݽ��ջص�����
                ���ݴ������������
 * @param       huart:���ھ��
 * @retval      ��
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART_UX)                    /* ����Ǵ���1 */
    {
        if ((g_usart1_rx_sta & 0x8000) == 0)             /* ����δ��� */
        {
            if (g_usart1_rx_sta & 0x4000)                /* ���յ���0x0d�����س����� */
            {
                if (g_usart1_rx_buffer[0] != 0x0a)             /* ���յ��Ĳ���0x0a�������ǻ��м��� */
                {
                    g_usart1_rx_sta = 0;                 /* ���մ���,���¿�ʼ */
                }
                else                                    /* ���յ�����0x0a�������м��� */
                {
                    g_usart1_rx_sta |= 0x8000;           /* ��������� */
                }
            }
            else                                        /* ��û�յ�0X0d�����س����� */
            {
                if (g_usart1_rx_buffer[0] == 0x0d)
                    g_usart1_rx_sta |= 0x4000;
                else
                {
                    g_usart1_rx_buf[g_usart1_rx_sta & 0X3FFF] = g_usart1_rx_buffer[0];
                    g_usart1_rx_sta++;

                    if (g_usart1_rx_sta > (USART1_REC_LEN - 1))
                    {
                        g_usart1_rx_sta = 0;             /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_usart1_rx_buffer, USART1_RXBUFFERSIZE);
    }
    if (huart->Instance == USART3_UX)                    /* ����Ǵ���3 */
    {
        /* �����յ������ݴ��뻺���� */
        if (g_usart3_rx_sta < USART3_REC_LEN)
        {
            g_usart3_rx_buf[g_usart3_rx_sta & 0X3FFF] = g_usart3_rx_buffer[0];
            g_usart3_rx_sta++;
        }
        else
        {
            /* �������������� */
            g_usart3_rx_sta = 0;
        }

        /* ���¿��������ж� */
        HAL_UART_Receive_IT(&g_uart3_handle, (uint8_t *)g_usart3_rx_buffer, USART3_RXBUFFERSIZE);
    }
}

/**
 * @brief       ����1�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART_UX_IRQHandler(void)
{
    HAL_UART_IRQHandler(&g_uart1_handle);   /* ����HAL���жϴ����ú��� */
}
/**
 * @brief       ����3�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART3_UX_IRQHandler(void)
{
    /* �ڵ���HAL����ǰ�ȼ��IDLE��־ */
    if (__HAL_UART_GET_FLAG(&g_uart3_handle, UART_FLAG_IDLE))
    {
        /* ���IDLE��־λ - ����ͬʱ��ȡSR��DR�Ĵ��� */
        __HAL_UART_CLEAR_IDLEFLAG(&g_uart3_handle);

        /* ����IDLE�жϱ�־ */
        g_usart3_idle_flag = 1;
        g_usart3_rx_len = g_usart3_rx_sta;
        g_usart3_rx_sta |= 0x8000;

//        printf("USART3 IDLE�жϴ��������ճ���: %d\r\n", g_usart3_rx_len);

        /* ����ֱ�ӷ��أ�������HAL��������HAL��������ǵı�־ */
        return;
    }
    HAL_UART_IRQHandler(&g_uart3_handle);   /* ����HAL���жϴ����ú��� */
}
/**
 * @brief       ���¿�ʼUSART3����
 * @param       ��
 * @retval      ��
 */
void usart3_restart_receive(void)
{
    g_usart3_rx_sta = 0;
    g_usart3_idle_flag = 0;
    g_usart3_rx_len = 0;
    memset(g_usart3_rx_buf, 0, sizeof(g_usart3_rx_buf));
    HAL_UART_Receive_IT(&g_uart3_handle, (uint8_t *)g_usart3_rx_buffer, USART3_RXBUFFERSIZE);
}

#endif

