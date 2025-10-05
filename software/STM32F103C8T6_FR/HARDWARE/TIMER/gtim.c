#include "gtim.h"
#include "led.h"
#include "freertos_demo.h"

TIM_HandleTypeDef g_timx_handle; /* 定时器x句柄 */


/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();                                 /* 使能TIMx时钟 */

    g_timx_handle.Instance = GTIM_TIMX_INT;                     /* 通用定时器x */
    g_timx_handle.Init.Prescaler = psc;                         /* 预分频系数 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* 递增计数模式 */
    g_timx_handle.Init.Period = arr;                            /* 自动装载值 */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 5, 0);             /* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                     /* 开启ITMx中断 */

    HAL_TIM_Base_Start_IT(&g_timx_handle);                      /* 使能定时器x和定时器x更新中断 */
}

/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_INT_IRQHandler(void)
{
    /* 以下代码没有使用定时器HAL库共用处理函数来处理，而是直接通过判断中断标志位的方式 */
    if(__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);  /* 清除定时器溢出中断标志位 */
			// 发送任务通知给主控制任务
		// if (KEYTask_Handler != NULL){
        //     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        //     vTaskNotifyGiveFromISR(KEYTask_Handler, &xHigherPriorityTaskWoken);
        //     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        // }
        
    }
}

TIM_HandleTypeDef g_timx_pwm_chy_handle;     /* 定时器x句柄 */

/**
 * @brief       通用定时器TIMX 通道Y PWM输出 初始化函数（使用PWM模式1）
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_pwm_chy  = {0};                          /* 定时器PWM输出配置 */

    g_timx_pwm_chy_handle.Instance = GTIM_TIMX_PWM;                     /* 定时器x */
    g_timx_pwm_chy_handle.Init.Prescaler = psc;                         /* 定时器分频 */
    g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* 递增计数模式 */
    g_timx_pwm_chy_handle.Init.Period = arr;                            /* 自动重装载值 */
    HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle);                           /* 初始化PWM */

    timx_oc_pwm_chy.OCMode = TIM_OCMODE_PWM1;                           /* 模式选择PWM1 */
    timx_oc_pwm_chy.Pulse = arr / 2;                                    /* 设置比较值,此值用来确定占空比 */
                                                                        /* 默认比较值为自动重装载值的一半,即占空比为50% */
    timx_oc_pwm_chy.OCPolarity = TIM_OCPOLARITY_LOW;                    /* 输出比较极性为低 */
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, GTIM_TIMX_PWM_CHY); /* 配置TIMx通道y */
    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY);       /* 开启对应PWM通道 */
}

/**
 * @brief       定时器底层驱动，时钟使能，引脚配置
                此函数会被HAL_TIM_PWM_Init()调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_PWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();               /* 开启通道y的CPIO时钟 */
        GTIM_TIMX_PWM_CHY_CLK_ENABLE();

        gpio_init_struct.Pin = GTIM_TIMX_PWM_CHY_GPIO_PIN; /* 通道y的CPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;           /* 复用推完输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;               /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;     /* 高速 */
        HAL_GPIO_Init(GTIM_TIMX_PWM_CHY_GPIO_PORT, &gpio_init_struct);
    //    GTIM_TIMX_PWM_CHY_GPIO_REMAP();                    /* IO口REMAP设置, 是否必要查看头文件配置的说明 */
    }
}


TIM_HandleTypeDef g_timx_cap_chy_handle;      /* 定时器x句柄 */

void TIM2_Capture_Init(void)
{   
    TIM_IC_InitTypeDef timx_ic_cap_chy = {0};
    // 1. 定时器基础配置
    g_timx_cap_chy_handle.Instance = GTIM_TIMX_CAP;
    g_timx_cap_chy_handle.Init.Prescaler = 71;           // 72MHz
    g_timx_cap_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_cap_chy_handle.Init.Period = 0xFFFF;         // 16位最大
    HAL_TIM_IC_Init(&g_timx_cap_chy_handle);

    // 2. 输入捕获通道配置
    timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_BOTHEDGE; // 上升沿和下降沿
    timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;
    timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1; 
    timx_ic_cap_chy.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&g_timx_cap_chy_handle, &timx_ic_cap_chy, GTIM_TIMX_CAP_CHY);

    __HAL_TIM_ENABLE_IT(&g_timx_cap_chy_handle, TIM_IT_UPDATE); // 使能更新中断
    HAL_TIM_IC_Start_IT(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY); // 启动输入捕获
}
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)                    /*输入通道捕获*/
    {
        GTIM_TIMX_CAP_CHY_CLK_ENABLE();                     /* 使能TIMx时钟 */
        HAL_NVIC_SetPriority(GTIM_TIMX_CAP_IRQn, 5, 3);     /* 抢占1，子优先级3 */
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CAP_IRQn);             /* 开启ITMx中断 */
    }
}
uint8_t g_timxchy_cap_sta = 0;    /* 输入捕获状态 */
uint16_t g_timxchy_cap_val = 0;   /* 输入捕获值 */

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cap_chy_handle);  /* 定时器HAL库共用处理函数 */
}
uint32_t rise_time = 0, fall_time = 0;  // 高电平宽度
uint32_t period = 0, high_time = 0;    // 低电平宽度
uint32_t new_rise = 0;
uint8_t  capture_count = 0; // 捕获完成标志
float fre = 0, duty_cycle = 0;
/**
 * @brief       定时器输入捕获中断处理回调函数
 * @param       htim:定时器句柄指针
 * @note        该函数在HAL_TIM_IRQHandler中会被调用
 * @retval      无
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

    if (htim->Instance == GTIM_TIMX_CAP)
    {
        switch (capture_count) {
            case 0: // 第一个上升沿
                rise_time = 0;
                capture_count = 1;
                __HAL_TIM_SET_COUNTER(&g_timx_cap_chy_handle, 0);  
                TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);
                TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING);
                break;
                
            case 1: // 下降沿
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
            case 2: // 第二个上升沿（计算周期）
                new_rise = HAL_TIM_ReadCapturedValue(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);;
                if (new_rise > rise_time) {
                    period = new_rise - rise_time;
                } else {
                    period = (0xFFFF - rise_time) + new_rise;
                }
                
                // 计算频率和占空比
                fre = 1000000.0f / period;  // 1MHz时钟
                duty_cycle = (high_time * 100.0f) / period;
                
                rise_time = new_rise;
                capture_count = 3;  // 准备下一次测量
                break;
        }
    }
}

/**
 * @brief       定时器更新中断回调函数
 * @param        htim:定时器句柄指针
 * @note        此函数会被定时器中断函数共同调用的
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_CAP)
    {
//        if ((g_timxchy_cap_sta & 0X80) == 0)            /* 还未成功捕获 */
//        {
//            if (g_timxchy_cap_sta & 0X40)               /* 已经捕获到高电平了 */
//            {
//                if ((g_timxchy_cap_sta & 0X3F) == 0X3F) /* 高电平太长了 */
//                {
//                    TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY);                     /* 一定要先清除原来的设置 */
//                    TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);/* 配置TIM5通道1上升沿捕获 */
//                    g_timxchy_cap_sta |= 0X80;          /* 标记成功捕获了一次 */
//                    g_timxchy_cap_val = 0XFFFF;
//                }
//                else      /* 累计定时器溢出次数 */
//                {
//                    g_timxchy_cap_sta++;
//                }
//            }
//        }
    }
}
