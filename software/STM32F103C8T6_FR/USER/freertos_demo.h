#ifndef __FREERTOS_H__
#define __FREERTOS_H__

#include "FreeRTOS.h" 
#include "task.h"

extern uint16_t ledrpwmval;
extern TaskHandle_t StartTask_Handler;
extern TaskHandle_t	ADCTask_Handler;
extern TaskHandle_t	PWMTask_Handler;
extern TaskHandle_t	DISTask_Handler;
extern TaskHandle_t	KEYTask_Handler;
void freertos_demo(void);

#endif
