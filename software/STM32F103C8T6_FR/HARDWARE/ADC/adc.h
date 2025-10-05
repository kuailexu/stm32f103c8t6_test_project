#ifndef _ADC_H
#define _ADC_H

#include "common.h"
#include "sys.h"
#define ADC_DMA_CH_NUM       1  
#define ADC_DMA_BUF_SIZE        50 * ADC_DMA_CH_NUM      
extern uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

/******************************************************************************************/


#define ADC_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_ADCX_CHY_GPIO_PIN               GPIO_PIN_1 
#define ADC_ADCX_CHY_GPIO_PIN2              GPIO_PIN_3 
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) 

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CHY                        ADC_CHANNEL_1                                
#define ADC_ADCX_CHY2                       ADC_CHANNEL_3 
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)  
/******************************************************************************************/
#define ADC_ADCX_DMACx						DMA1_Channel1
#define ADC_ADCX_DMACx_IRQn					DMA1_Channel1_IRQn
#define ADC_ADCX_DMACx_IRQHandler			DMA1_Channel1_IRQHandler

#define ADC_ADCX_DMACx_IS_TC()				(DMA1->ISR & (1 << 1))
#define ADC_ADCX_DMACx_CLR_TC()             do{ DMA1->IFCR |= 1 << 1; }while(0)
/******************************************************************************************/
void adc_init(void);                                           
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime);
void adc_dma_init(uint32_t mar); 
void adc_nch_dma_init(uint32_t mar);
void adc_dma_enable(uint16_t cndtr); 
uint32_t adc_get_result(uint32_t ch);                         
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);       

#endif
