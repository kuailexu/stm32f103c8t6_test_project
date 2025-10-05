/**
  ******************************************************************************
  * @file    common.h
  * @brief   Common definitions for STM32 projects
  ******************************************************************************
  */

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f1xx_hal.h"  // 根据实际芯片修改，如 stm32f4xx_hal.h

/* Exported types ------------------------------------------------------------*/
typedef uint8_t   u8;
typedef int8_t    s8;
typedef uint16_t  u16;
typedef int16_t   s16;
typedef uint32_t  u32;
typedef int32_t   s32;
typedef float     f32;
typedef double    f64;

/* Exported constants --------------------------------------------------------*/
#define PI          3.141592653589793f
#define TRUE        1
#define FALSE       0
#define ENABLE      1
#define DISABLE     0
#define SET         GPIO_PIN_SET
#define RESET       GPIO_PIN_RESET

/* Exported macros -----------------------------------------------------------*/
// Bit manipulation
#define BIT(n)                  (1UL << (n))
#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))
#define TOGGLE_BIT(REG, BIT)    ((REG) ^= (BIT))
#define READ_BIT(REG, BIT)      ((REG) & (BIT))

// Array size
#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof((arr)[0]))

// Min/Max/Constrain
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))
#define CONSTRAIN(x, low, high) (((x) < (low)) ? (low) : (((x) > (high)) ? (high) : (x)))

// Debug printf (requires retarget.c)
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...)   printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)   ((void)0)
#endif

/* Exported functions --------------------------------------------------------*/
void Error_Handler(void);
void SystemClock_Config(void);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_H__ */
