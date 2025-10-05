#ifndef __GTIM_H
#define __GTIM_H

#include "sys.h"
#define CAPTURE_BUF_SIZE  128
extern TIM_HandleTypeDef g_timx_cap_chy_handle;
extern uint8_t g_timxchy_cap_sta;    /* ���벶��״̬ */
extern uint16_t g_timxchy_cap_val;   /* ���벶��ֵ */
extern uint32_t rise_time,  fall_time ;  // �ߵ�ƽ���
extern uint32_t period , high_time ;    // �͵�ƽ���
extern uint8_t  capture_count ; // ������ɱ�־
extern float fre , duty_cycle ;
/******************************************************************************************/
/* ͨ�ö�ʱ�� ���� */

/* TIMX �ж϶��� 
 * Ĭ�������TIM2~TIM5.
 * ע��: ͨ���޸���4���궨��,����֧��TIM1~TIM17����һ����ʱ��.
 */
 
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 ʱ��ʹ�� */

/******************************************************************************************/
/* TIMX PWM������� 
 * ���������PWM����LED0(RED)������
 * Ĭ�������TIM2~TIM5
 * ע��: ͨ���޸��⼸���궨��,����֧��TIM1~TIM8����һ����ʱ��,����һ��IO�����PWM
 */
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOA
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          GPIO_PIN_8
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

/* TIMX REMAP����
 * ��Ϊ����LED0����PB5��, ����ͨ������TIM2�Ĳ�����ӳ�书��, ���ܽ�TIM2_CH3�����PB5��
 * ���, ����ʵ��GTIM_TIMX_PWM_CHY_GPIO_REMAP
 * ����Щʹ��Ĭ�����õĶ�ʱ��PWM�����, ����������ӳ��, �ǲ���Ҫ�ú�����!
 */
#define GTIM_TIMX_PWM_CHY_GPIO_REMAP()      do{__HAL_RCC_AFIO_CLK_ENABLE();\
                                                __HAL_AFIO_REMAP_TIM1_DISABLE();\
                                            }while(0)            /* ͨ��REMAP����, �ú������Ǳ����, ������Ҫʵ�� */

#define GTIM_TIMX_PWM                       TIM1 
#define GTIM_TIMX_PWM_CHY                   TIM_CHANNEL_1                               /* ͨ��Y,  1<= Y <=4 */
#define GTIM_TIMX_PWM_CHY_CCRX              TIM1->CCR1                                  /* ͨ��Y������ȽϼĴ��� */
#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM2 ʱ��ʹ�� */

/******************************************************************************************/
/*********************************������ͨ�ö�ʱ�����벶��ʵ����غ궨��*************************************/

 /* TIMX ���벶���� 
 * ��������벶��ʹ�ö�ʱ��TIM5_CH1,����WK_UP����������
 * Ĭ�������TIM2~TIM5. 
 * ע��: ͨ���޸��⼸���궨��,����֧��TIM1~TIM8����һ����ʱ��,����һ��IO�������벶��
 *       �ر�Ҫע��:Ĭ���õ�PA0,���õ�����������!���������IO,��Ӧ����������ʽҲ�ø�!
 */
#define GTIM_TIMX_CAP_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CAP_CHY_GPIO_PIN             GPIO_PIN_1
#define GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define GTIM_TIMX_CAP                          TIM2                       
#define GTIM_TIMX_CAP_IRQn                     TIM2_IRQn
#define GTIM_TIMX_CAP_IRQHandler               TIM2_IRQHandler
#define GTIM_TIMX_CAP_CHY                      TIM_CHANNEL_4                                 /* ͨ��Y,  1<= Y <=4 */
#define GTIM_TIMX_CAP_CHY_CCRX                 TIM2->CCR4                                    /* ͨ��Y������ȽϼĴ��� */
#define GTIM_TIMX_CAP_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)    /* TIM5 ʱ��ʹ�� */


void gtim_timx_int_init(uint16_t arr, uint16_t psc);        /* ͨ�ö�ʱ�� ��ʱ�жϳ�ʼ������ */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* ͨ�ö�ʱ�� PWM��ʼ������ */
void TIM2_Capture_Init(void);                     /* ��ʱ��2���벶���ʼ������ */
#endif

















