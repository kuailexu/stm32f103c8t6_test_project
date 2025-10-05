#include "gtim.h"
#include "led.h"
#include "freertos_demo.h"

TIM_HandleTypeDef g_timx_handle; /* ��ʱ��x��� */


/**
 * @brief       ͨ�ö�ʱ��TIMX��ʱ�жϳ�ʼ������
 * @note
 *              ͨ�ö�ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ36M, ���Զ�ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();                                 /* ʹ��TIMxʱ�� */

    g_timx_handle.Instance = GTIM_TIMX_INT;                     /* ͨ�ö�ʱ��x */
    g_timx_handle.Init.Prescaler = psc;                         /* Ԥ��Ƶϵ�� */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* ��������ģʽ */
    g_timx_handle.Init.Period = arr;                            /* �Զ�װ��ֵ */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 5, 0);             /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                     /* ����ITMx�ж� */

    HAL_TIM_Base_Start_IT(&g_timx_handle);                      /* ʹ�ܶ�ʱ��x�Ͷ�ʱ��x�����ж� */
}

/**
 * @brief       ��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void GTIM_TIMX_INT_IRQHandler(void)
{
    /* ���´���û��ʹ�ö�ʱ��HAL�⹲�ô���������������ֱ��ͨ���ж��жϱ�־λ�ķ�ʽ */
    if(__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);  /* �����ʱ������жϱ�־λ */
			// ��������֪ͨ������������
		// if (KEYTask_Handler != NULL){
        //     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        //     vTaskNotifyGiveFromISR(KEYTask_Handler, &xHigherPriorityTaskWoken);
        //     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        // }
        
    }
}

TIM_HandleTypeDef g_timx_pwm_chy_handle;     /* ��ʱ��x��� */

/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y PWM��� ��ʼ��������ʹ��PWMģʽ1��
 * @note
 *              ͨ�ö�ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ36M, ���Զ�ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_pwm_chy  = {0};                          /* ��ʱ��PWM������� */

    g_timx_pwm_chy_handle.Instance = GTIM_TIMX_PWM;                     /* ��ʱ��x */
    g_timx_pwm_chy_handle.Init.Prescaler = psc;                         /* ��ʱ����Ƶ */
    g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* ��������ģʽ */
    g_timx_pwm_chy_handle.Init.Period = arr;                            /* �Զ���װ��ֵ */
    HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle);                           /* ��ʼ��PWM */

    timx_oc_pwm_chy.OCMode = TIM_OCMODE_PWM1;                           /* ģʽѡ��PWM1 */
    timx_oc_pwm_chy.Pulse = arr / 2;                                    /* ���ñȽ�ֵ,��ֵ����ȷ��ռ�ձ� */
                                                                        /* Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50% */
    timx_oc_pwm_chy.OCPolarity = TIM_OCPOLARITY_LOW;                    /* ����Ƚϼ���Ϊ�� */
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, GTIM_TIMX_PWM_CHY); /* ����TIMxͨ��y */
    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY);       /* ������ӦPWMͨ�� */
}

/**
 * @brief       ��ʱ���ײ�������ʱ��ʹ�ܣ���������
                �˺����ᱻHAL_TIM_PWM_Init()����
 * @param       htim:��ʱ�����
 * @retval      ��
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_PWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();               /* ����ͨ��y��CPIOʱ�� */
        GTIM_TIMX_PWM_CHY_CLK_ENABLE();

        gpio_init_struct.Pin = GTIM_TIMX_PWM_CHY_GPIO_PIN; /* ͨ��y��CPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;           /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;               /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;     /* ���� */
        HAL_GPIO_Init(GTIM_TIMX_PWM_CHY_GPIO_PORT, &gpio_init_struct);
    //    GTIM_TIMX_PWM_CHY_GPIO_REMAP();                    /* IO��REMAP����, �Ƿ��Ҫ�鿴ͷ�ļ����õ�˵�� */
    }
}


TIM_HandleTypeDef g_timx_cap_chy_handle;      /* ��ʱ��x��� */

void TIM2_Capture_Init(void)
{   
    TIM_IC_InitTypeDef timx_ic_cap_chy = {0};
    // 1. ��ʱ����������
    g_timx_cap_chy_handle.Instance = GTIM_TIMX_CAP;
    g_timx_cap_chy_handle.Init.Prescaler = 71;           // 72MHz
    g_timx_cap_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_cap_chy_handle.Init.Period = 0xFFFF;         // 16λ���
    HAL_TIM_IC_Init(&g_timx_cap_chy_handle);

    // 2. ���벶��ͨ������
    timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_BOTHEDGE; // �����غ��½���
    timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;
    timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1; 
    timx_ic_cap_chy.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&g_timx_cap_chy_handle, &timx_ic_cap_chy, GTIM_TIMX_CAP_CHY);

    __HAL_TIM_ENABLE_IT(&g_timx_cap_chy_handle, TIM_IT_UPDATE); // ʹ�ܸ����ж�
    HAL_TIM_IC_Start_IT(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY); // �������벶��
}
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)                    /*����ͨ������*/
    {
        GTIM_TIMX_CAP_CHY_CLK_ENABLE();                     /* ʹ��TIMxʱ�� */
        HAL_NVIC_SetPriority(GTIM_TIMX_CAP_IRQn, 5, 3);     /* ��ռ1�������ȼ�3 */
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CAP_IRQn);             /* ����ITMx�ж� */
    }
}
uint8_t g_timxchy_cap_sta = 0;    /* ���벶��״̬ */
uint16_t g_timxchy_cap_val = 0;   /* ���벶��ֵ */

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cap_chy_handle);  /* ��ʱ��HAL�⹲�ô����� */
}
uint32_t rise_time = 0, fall_time = 0;  // �ߵ�ƽ���
uint32_t period = 0, high_time = 0;    // �͵�ƽ���
uint32_t new_rise = 0;
uint8_t  capture_count = 0; // ������ɱ�־
float fre = 0, duty_cycle = 0;
/**
 * @brief       ��ʱ�����벶���жϴ���ص�����
 * @param       htim:��ʱ�����ָ��
 * @note        �ú�����HAL_TIM_IRQHandler�лᱻ����
 * @retval      ��
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

    if (htim->Instance == GTIM_TIMX_CAP)
    {
        switch (capture_count) {
            case 0: // ��һ��������
                rise_time = 0;
                capture_count = 1;
                __HAL_TIM_SET_COUNTER(&g_timx_cap_chy_handle, 0);  
                TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);
                TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING);
                break;
                
            case 1: // �½���
                fall_time = HAL_TIM_ReadCapturedValue(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);
                TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);
                TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);
                if (fall_time > rise_time) {
                    high_time = fall_time - rise_time;
                } else {
                    high_time = (0xFFFF - rise_time) + fall_time;
                }
                capture_count = 2;
                break;
            case 2: // �ڶ��������أ��������ڣ�
                new_rise = HAL_TIM_ReadCapturedValue(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);;
                if (new_rise > rise_time) {
                    period = new_rise - rise_time;
                } else {
                    period = (0xFFFF - rise_time) + new_rise;
                }
                
                // ����Ƶ�ʺ�ռ�ձ�
                fre = 1000000.0f / period;  // 1MHzʱ��
                duty_cycle = (high_time * 100.0f) / period;
                
                rise_time = new_rise;
                capture_count = 3;  // ׼����һ�β���
                break;
        }
    }
}

/**
 * @brief       ��ʱ�������жϻص�����
 * @param        htim:��ʱ�����ָ��
 * @note        �˺����ᱻ��ʱ���жϺ�����ͬ���õ�
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_CAP)
    {
//        if ((g_timxchy_cap_sta & 0X80) == 0)            /* ��δ�ɹ����� */
//        {
//            if (g_timxchy_cap_sta & 0X40)               /* �Ѿ����񵽸ߵ�ƽ�� */
//            {
//                if ((g_timxchy_cap_sta & 0X3F) == 0X3F) /* �ߵ�ƽ̫���� */
//                {
//                    TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);                     /* һ��Ҫ�����ԭ�������� */
//                    TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);/* ����TIM5ͨ��1�����ز��� */
//                    g_timxchy_cap_sta |= 0X80;          /* ��ǳɹ�������һ�� */
//                    g_timxchy_cap_val = 0XFFFF;
//                }
//                else      /* �ۼƶ�ʱ��������� */
//                {
//                    g_timxchy_cap_sta++;
//                }
//            }
//        }
    }
}
