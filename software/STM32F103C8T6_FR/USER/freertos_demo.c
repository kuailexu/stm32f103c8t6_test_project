#include "freertos_demo.h"
#include "usart.h" 
#include "led.h"
#include "QDTFT_demo.h"
#include "adc.h"
#include "key.h"
#include "gtim.h"
/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t	StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */
/* ADC采集 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define ADC_PRIO      2                   /* 任务优先级 */
#define ADC_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t	ADCTask_Handler;  /* 任务句柄 */
void ADC_GET(void *pvParameters);             /* 任务函数 */
/* PWM生成 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define PWM_PRIO      3                   /* 任务优先级 */
#define PWM_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t	PWMTask_Handler;  /* 任务句柄 */
void PWM_OUTPUT(void *pvParameters);             /* 任务函数 */
/* 显示 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define DIS_PRIO      4                   /* 任务优先级 */
#define DIS_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t	DISTask_Handler;  /* 任务句柄 */
void Display(void *pvParameters);             /* 任务函数 */
/* 按键检测 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define KEY_PRIO      5                   /* 任务优先级 */
#define KEY_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t	KEYTask_Handler;  /* 任务句柄 */
void KEY(void *pvParameters);             /* 任务函数 */
float adcx[ADC_DMA_CH_NUM];
int16_t data_volt_list[LCD_WIDTH];
uint16_t ledrpwmval = 5;
uint32_t temp = 0;
float last_temp = 0;
int16_t wave_buf[ADC_DMA_BUF_SIZE];
extern TIM_HandleTypeDef g_timx_pwm_chy_handle;
extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */ 

void freertos_demo(void)
{
    
    Lcd_Clear(GRAY0);
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler();
}

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )ADC_GET,
                (const char*    )"task1",
                (uint16_t       )ADC_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )ADC_PRIO,
                (TaskHandle_t*  )&ADCTask_Handler);
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )PWM_OUTPUT,
                (const char*    )"task2",
                (uint16_t       )PWM_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )PWM_PRIO,
                (TaskHandle_t*  )&PWMTask_Handler);
		    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )Display,
                (const char*    )"task3",
                (uint16_t       )DIS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DIS_PRIO,
                (TaskHandle_t*  )&DISTask_Handler);
		    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )KEY,
                (const char*    )"task4",
                (uint16_t       )KEY_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )KEY_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

void ADC_GET(void *pvParameters)
{   
    uint8_t t = 0;
    float last_t = 0;
    uint32_t sum;
    
	adc_dma_enable(ADC_DMA_BUF_SIZE);
    while(1)
    {   if (capture_count==3)
        {
            capture_count = 0;          /* 开启下一次捕获*/
//            g_adc_dma_sta = 0;
        }
        if (g_adc_dma_sta == 1) {
            for(int i = 0; i < ADC_DMA_BUF_SIZE; i++)
            {
                wave_buf[i] = (int16_t)(g_adc_dma_buf[i]*3.3 /4096.f);
            }
            g_adc_dma_sta = 0;
			adc_dma_enable(ADC_DMA_BUF_SIZE);
        }
//        if(g_adc_dma_sta == 1)
//        {
//            for(int i=0;i<ADC_DMA_BUF_SIZE;i++)
//            {
//                sum += g_adc_dma_buf[i];
//            }
//            adcx[0] = sum / ADC_DMA_BUF_SIZE;
//            g_adc_dma_sta = 0;                                  /* 清除DMA采集完成状态标志 */
//            adc_dma_enable(ADC_DMA_BUF_SIZE);
//        }
        vTaskDelay(50);                                              
    }
}

void PWM_OUTPUT(void *pvParameters)
{		
    uint8_t dir = 1;
    while(1)
    {		
        /* 修改比较值控制占空比 */
        __HAL_TIM_SET_COMPARE(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY, ledrpwmval); /* 方波 */
        vTaskDelay(50);
    }
}

void Display(void *pvParameters)
{
    uint16_t i,j;
    uint16_t y;
    float last_rom = 0;
    while(1)
    {
        // static ContentItem_t myItems[] = {
        //     { .x = 0, .y = 0, .width = 54, .height = 8, .type = ITEM_TYPE_NUM, .data = &fre,.drawFunction = LCD_Num_8x6}, // Item 0
        //     { .x = 0, .y = 8, .width = 54, .height = 8, .type = ITEM_TYPE_NUM, .data = &duty_cycle,.drawFunction = LCD_Num_8x6}, // Item 1
        // // 注意：x,y可以先设为0，由布局计算函数自动计算实际位置
        // };
        // // 2. 初始化布局管理器
        // mainLayout.items = myItems;
        // mainLayout.itemCount = sizeof(myItems) / sizeof(ContentItem_t);
        // mainLayout.currentFocusIndex = 0; // 默认焦点在第一个项上
        // mainLayout.direction = LAYOUT_VERTICAL; // 垂直排列
        // mainLayout.spacing = 10; // 项之间间隔10像素
        // Gui_DrawCurve(0,50,ADC_DMA_BUF_SIZE,80,wave_buf,2,RED);
				// Gui_box2(0,0,LCD_WIDTH,16,0);		
        // LCD_Num_8x6(0, 0, RED, GRAY0, fre,NULL);
        // LCD_Num_8x6(0, 8, RED, GRAY0, duty_cycle,NULL);			
        // LCD_Num_8x6(130, 15*FONT_HEIGH, RED, GRAY0, xPortGetFreeHeapSize(),&last_rom);
        // last_rom = xPortGetFreeHeapSize();
        LED0_TOGGLE();
        vTaskDelay(1000);
    }
}

void KEY(void *pvParameters)
{
    while(1)
    {		
        if (Button_Debounce(Read_KEY1, KEY_ID_1) == KEY_EVENT_PRESSED) {
            Lcd_Clear(GRAY0);
        }
        vTaskDelay(50);
    }
}
