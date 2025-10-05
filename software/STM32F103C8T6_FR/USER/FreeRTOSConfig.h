#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* ͷ�ļ� */
#include "stm32f1xx.h"  // �滻ԭsys.h��ȷ��CMSIS������
#include <stdint.h>

/* ���������� */
#define configUSE_PREEMPTION                    1       // ��ռʽ����
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1       // ����Ӳ������ѡ���Ż���Cortex-M3֧�֣�
#define configCPU_CLOCK_HZ                      (SystemCoreClock)  // 72MHz
#define configTICK_RATE_HZ                      1000    // 1msϵͳ����
#define configMAX_PRIORITIES                    12      // ������ȼ�
#define configMINIMAL_STACK_SIZE                (128)   // ��������ջ���֣�
#define configTOTAL_HEAP_SIZE                   ((size_t)(6*1024)) // 6KB�ѣ�C8T6��20KB RAM��
#define configMAX_TASK_NAME_LEN									( 16 )
#define configUSE_16_BIT_TICKS									0

/* �ڴ���� */
#define configSUPPORT_STATIC_ALLOCATION         0       // ����̬�ڴ�
#define configSUPPORT_DYNAMIC_ALLOCATION        1

/* �ж����ã�Cortex-M3�ض��� */
#define configPRIO_BITS                         4       // STM32ʹ��4λ���ȼ�
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15      // ����ж����ȼ�
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5  // FreeRTOS�ɹ���������ȼ�
#define configKERNEL_INTERRUPT_PRIORITY         (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Ӳ������ض��� */
#define vPortSVCHandler    SVC_Handler          // �����������ļ�һ��
#define xPortPendSVHandler PendSV_Handler

/* �ü���ѡ���ܣ���ʡ��Դ�� */
#define configUSE_IDLE_HOOK                     0       // ���ÿ��й���
#define configUSE_TICK_HOOK                     0       // ����Tick����
#define configCHECK_FOR_STACK_OVERFLOW          0       // ջ�����ⷽ��2
#define configUSE_MUTEXES                       1       // ���û�����
#define configUSE_RECURSIVE_MUTEXES             1       // ���õݹ黥����
#define configUSE_TIMERS                        1       // ���������ʱ��
#define configTIMER_QUEUE_LENGTH         				10 // ������г���
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES-1)
#define configTIMER_TASK_STACK_DEPTH            (configMINIMAL_STACK_SIZE * 2)

/* ������API���� */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/* �������� */
#define configASSERT(x) if((x)==0) {taskDISABLE_INTERRUPTS(); for(;;);}

#endif /* FREERTOS_CONFIG_H */
