#include "freertos_demo.h"
#include "usart.h" 
#include "led.h"
#include "QDTFT_demo.h"
#include "adc.h"
#include "key.h"
#include "gtim.h"
/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1                   /* �������ȼ� */
#define START_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t	StartTask_Handler;  /* ������ */
void start_task(void *pvParameters);        /* ������ */
/* ADC�ɼ� ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define ADC_PRIO      2                   /* �������ȼ� */
#define ADC_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t	ADCTask_Handler;  /* ������ */
void ADC_GET(void *pvParameters);             /* ������ */
/* PWM���� ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define PWM_PRIO      3                   /* �������ȼ� */
#define PWM_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t	PWMTask_Handler;  /* ������ */
void PWM_OUTPUT(void *pvParameters);             /* ������ */
/* ��ʾ ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define DIS_PRIO      4                   /* �������ȼ� */
#define DIS_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t	DISTask_Handler;  /* ������ */
void Display(void *pvParameters);             /* ������ */
/* ������� ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define KEY_PRIO      5                   /* �������ȼ� */
#define KEY_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t	KEYTask_Handler;  /* ������ */
void KEY(void *pvParameters);             /* ������ */
float adcx[ADC_DMA_CH_NUM];
int16_t data_volt_list[LCD_WIDTH];
uint16_t ledrpwmval = 5;
uint32_t temp = 0;
float last_temp = 0;
int16_t wave_buf[ADC_DMA_BUF_SIZE];
extern TIM_HandleTypeDef g_timx_pwm_chy_handle;
extern uint8_t g_adc_dma_sta;               /* DMA����״̬��־, 0,δ���; 1, ����� */ 

void freertos_demo(void)
{
    
    Lcd_Clear(GRAY0);
    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */
    vTaskStartScheduler();
}

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */
    /* ��������1 */
    xTaskCreate((TaskFunction_t )ADC_GET,
                (const char*    )"task1",
                (uint16_t       )ADC_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )ADC_PRIO,
                (TaskHandle_t*  )&ADCTask_Handler);
    /* ��������2 */
    xTaskCreate((TaskFunction_t )PWM_OUTPUT,
                (const char*    )"task2",
                (uint16_t       )PWM_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )PWM_PRIO,
                (TaskHandle_t*  )&PWMTask_Handler);
		    /* ��������2 */
    xTaskCreate((TaskFunction_t )Display,
                (const char*    )"task3",
                (uint16_t       )DIS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DIS_PRIO,
                (TaskHandle_t*  )&DISTask_Handler);
		    /* ��������2 */
    xTaskCreate((TaskFunction_t )KEY,
                (const char*    )"task4",
                (uint16_t       )KEY_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )KEY_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
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
            capture_count = 0;          /* ������һ�β���*/
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
//            g_adc_dma_sta = 0;                                  /* ���DMA�ɼ����״̬��־ */
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
        /* �޸ıȽ�ֵ����ռ�ձ� */
        __HAL_TIM_SET_COMPARE(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY, ledrpwmval); /* ���� */
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
        // // ע�⣺x,y��������Ϊ0���ɲ��ּ��㺯���Զ�����ʵ��λ��
        // };
        // // 2. ��ʼ�����ֹ�����
        // mainLayout.items = myItems;
        // mainLayout.itemCount = sizeof(myItems) / sizeof(ContentItem_t);
        // mainLayout.currentFocusIndex = 0; // Ĭ�Ͻ����ڵ�һ������
        // mainLayout.direction = LAYOUT_VERTICAL; // ��ֱ����
        // mainLayout.spacing = 10; // ��֮����10����
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
