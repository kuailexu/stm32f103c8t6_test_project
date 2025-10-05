#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* 头文件 */
#include "stm32f1xx.h"  // 替换原sys.h，确保CMSIS兼容性
#include <stdint.h>

/* 基础配置项 */
#define configUSE_PREEMPTION                    1       // 抢占式调度
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1       // 启用硬件任务选择优化（Cortex-M3支持）
#define configCPU_CLOCK_HZ                      (SystemCoreClock)  // 72MHz
#define configTICK_RATE_HZ                      1000    // 1ms系统节拍
#define configMAX_PRIORITIES                    12      // 最大优先级
#define configMINIMAL_STACK_SIZE                (128)   // 空闲任务栈（字）
#define configTOTAL_HEAP_SIZE                   ((size_t)(6*1024)) // 6KB堆（C8T6仅20KB RAM）
#define configMAX_TASK_NAME_LEN									( 16 )
#define configUSE_16_BIT_TICKS									0

/* 内存管理 */
#define configSUPPORT_STATIC_ALLOCATION         0       // 纯动态内存
#define configSUPPORT_DYNAMIC_ALLOCATION        1

/* 中断配置（Cortex-M3特定） */
#define configPRIO_BITS                         4       // STM32使用4位优先级
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15      // 最低中断优先级
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5  // FreeRTOS可管理最高优先级
#define configKERNEL_INTERRUPT_PRIORITY         (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* 硬件相关重定义 */
#define vPortSVCHandler    SVC_Handler          // 必须与启动文件一致
#define xPortPendSVHandler PendSV_Handler

/* 裁剪可选功能（节省资源） */
#define configUSE_IDLE_HOOK                     0       // 禁用空闲钩子
#define configUSE_TICK_HOOK                     0       // 禁用Tick钩子
#define configCHECK_FOR_STACK_OVERFLOW          0       // 栈溢出检测方法2
#define configUSE_MUTEXES                       1       // 启用互斥量
#define configUSE_RECURSIVE_MUTEXES             1       // 启用递归互斥量
#define configUSE_TIMERS                        1       // 启用软件定时器
#define configTIMER_QUEUE_LENGTH         				10 // 命令队列长度
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES-1)
#define configTIMER_TASK_STACK_DEPTH            (configMINIMAL_STACK_SIZE * 2)

/* 包含的API函数 */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/* 断言配置 */
#define configASSERT(x) if((x)==0) {taskDISABLE_INTERRUPTS(); for(;;);}

#endif /* FREERTOS_CONFIG_H */
