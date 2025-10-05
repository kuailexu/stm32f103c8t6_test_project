#include "adc.h"
#include "delay.h"


ADC_HandleTypeDef g_adc_handle;   /* ADC句柄 */

/**
 * @brief       ADC初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道, 但是不支持ADC3
 *              我们使用12位精度, ADC采样时钟=12M, 转换时间为: 采样周期 + 12.5个ADC周期
 *              设置最大采样周期: 239.5, 则转换时间 = 252 个ADC周期 = 21us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    g_adc_handle.Instance = ADC_ADCX;                        /* 选择哪个ADC */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;       /* 数据对齐方式：右对齐 */
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;       /* 扫描模式， */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;          /* 关闭连续转换模式 */
    g_adc_handle.Init.NbrOfConversion = 1;                   /* 赋值范围是1~16，本实验用到1个规则通道序列 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;       /* 禁止规则通道组间断模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;               /* 配置间断模式的规则通道个数，禁止规则通道组间断模式后，此参数忽略 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* 触发转换方式：软件触发 */
    HAL_ADC_Init(&g_adc_handle);                             /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_handle);              /* 校准ADC */
}

/**
 * @brief       ADC底层驱动，引脚配置，时钟使能
                此函数会被HAL_ADC_Init()调用
 * @param       hadc:ADC句柄
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC_ADCX)
    {
        GPIO_InitTypeDef gpio_init_struct;
        RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
        
        ADC_ADCX_CHY_CLK_ENABLE();                                /* 使能ADCx时钟 */
        ADC_ADCX_CHY_GPIO_CLK_ENABLE();                           /* 开启GPIO时钟 */

        /* 设置ADC时钟 */
        adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;    /* ADC外设时钟 */
        adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;       /* 分频因子6时钟为72M/6=12MHz */
        HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                 /* 设置ADC时钟 */

        /* 设置AD采集通道对应IO引脚工作模式 */
        gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;             /* ADC通道IO引脚 */
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;                 /* 模拟 */
        HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_1CYCLE_5, 1.5个ADC时钟周期        ADC_SAMPLETIME_7CYCLES_5, 7.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_13CYCLES_5, 13.5个ADC时钟周期     ADC_SAMPLETIME_28CYCLES_5, 28.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_41CYCLES_5, 41.5个ADC时钟周期     ADC_SAMPLETIME_55CYCLES_5, 55.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_71CYCLES_5, 71.5个ADC时钟周期     ADC_SAMPLETIME_239CYCLES_5, 239.5个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channle1的rank=1，channle2 的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channle1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道DMA设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_ch_conf;
    
    adc_ch_conf.Channel = ch;                            /* 通道 */
    adc_ch_conf.Rank = rank;                             /* 序列 */
    adc_ch_conf.SamplingTime = stime;                    /* 采样时间 */
    HAL_ADC_ConfigChannel(adc_handle, &adc_ch_conf);     /* 通道配置 */
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道值 0~17，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      无
 */
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&g_adc_handle , ch, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* 设置通道，序列和采样时间 */
    HAL_ADC_Start(&g_adc_handle);                            /* 开启ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, 10);            /* 轮询转换 */
    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);        /* 返回最近一次ADC1规则组的转换结果 */
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       ch      : 通道号, 0~17
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* 获取times次数据 */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;        /* 返回平均值 */
}
DMA_HandleTypeDef g_dma_adc_handle = {0};                                   /* 定义要搬运ADC数据的DMA句柄 */
ADC_HandleTypeDef g_adc_dma_handle = {0};                                   /* 定义ADC（DMA读取）句柄 */
uint8_t g_adc_dma_sta = 0;         
void adc_dma_init(uint32_t mar)
{
    GPIO_InitTypeDef gpio_init_struct;
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
    ADC_ChannelConfTypeDef adc_ch_conf = {0};

    ADC_ADCX_CHY_CLK_ENABLE();                                              /* 使能ADCx时钟 */
    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                                         /* 开启GPIO时钟 */

    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)                 /* 大于DMA1_Channel7, 则为DMA2的通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                                        /* DMA2时钟使能 */
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1时钟使能 */
    }

    /* 设置ADC时钟 */
    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                  /* ADC外设时钟 */
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                     /* 分频因子6时钟为72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                               /* 设置ADC时钟 */

    /* 设置AD采集通道对应IO引脚工作模式 */
    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;                           /* ADC通道对应的IO引脚 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                               /* 模拟 */
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);

    /* 初始化DMA */
    g_dma_adc_handle.Instance = ADC_ADCX_DMACx;                             /* 设置DMA通道 */
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* 从外设到存储器模式 */
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* 外设流控模式 */
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_adc_handle);

    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);         /* 将DMA与adc联系起来 */

    g_adc_dma_handle.Instance = ADC_ADCX;                                   /* 选择哪个ADC */
    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* 数据对齐方式：右对齐 */
    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                  /* 非扫描模式，仅用到一个通道 */
    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;                      /* 使能连续转换模式 */
    g_adc_dma_handle.Init.NbrOfConversion = 1;                              /* 赋值范围是1~16，本实验用到1个规则通道序列 */
    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* 禁止规则通道组间断模式 */
    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                          /* 配置间断模式的规则通道个数，禁止规则通道组间断模式后，此参数忽略 */
    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* 触发转换方式：软件触发 */
    HAL_ADC_Init(&g_adc_dma_handle);                                        /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle);                         /* 校准ADC */

    /* 配置ADC通道 */
    adc_ch_conf.Channel = ADC_ADCX_CHY;                                     /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                  /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* 采样时间，设置最大采样周期:239.5个ADC周期 */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* 通道配置 */

    /* 配置DMA数据流请求中断优先级 */
    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);

    HAL_DMA_Start_IT(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);       /* 启动DMA，并开启中断 */
    HAL_ADC_Start_DMA(&g_adc_dma_handle, &mar, 0);                          /* 开启ADC，通过DMA传输结果 */
}
DMA_HandleTypeDef g_dma_nch_adc_handle = {0};
ADC_HandleTypeDef g_adc_nch_dma_handle = {0};
//uint8_t g_adc_dma_sta = 0;

void adc_nch_dma_init(uint32_t mar)
{
    GPIO_InitTypeDef gpio_init_struct;
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
    ADC_ChannelConfTypeDef adc_ch_conf= {0};

    ADC_ADCX_CHY_CLK_ENABLE();                                /* 使能ADCx时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                            /* 开启GPIO时钟 */

    if((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)
    {
        __HAL_RCC_DMA2_CLK_ENABLE(); /* 开启DMA2时钟 */
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE(); /* 开启DMA1时钟 */
    }

    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;    /* ADC外设时钟 */
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;       /* 分频因子6时钟为72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                 /* 设置ADC时钟*/

    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN|ADC_ADCX_CHY_GPIO_PIN2;             /* ADC通道IO引脚 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                 /* 模拟 */ 
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);       

    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMACx;           /* DMA通道 */
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;/* 外设到存储器 */
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;    /* 外设地址不变 */
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;        /* 存储器地址递增 */
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  /* 外设数据宽度为32位 */
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;     /* 存储器数据宽度为32位 */
    g_dma_nch_adc_handle.Init.Mode = DMA_CIRCULAR;            /* 循环模式 */
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_HIGH;    /* 优先级高 */
    HAL_DMA_Init(&g_dma_nch_adc_handle);                      /* 初始化DMA */
    
    __HAL_LINKDMA(&g_adc_nch_dma_handle, DMA_Handle, g_dma_nch_adc_handle);   /* 将DMA与adc联系起来 */
    
    g_adc_nch_dma_handle.Instance = ADC_ADCX;                /* 选择哪个ADC */
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; /* 数据对齐方式：右对齐 */
    g_adc_nch_dma_handle.Init.ScanConvMode = ADC_SCAN_ENABLE; /* 扫描模式， */
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;         /* 开启连续转换模式 */
    g_adc_nch_dma_handle.Init.NbrOfConversion = 2;            /* 赋值范围是1~16，本实验用到2个规则通道序列 */
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;/* 禁止规则通道组间断模式 */
    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 0;        /* 配置间断模式的规则通道个数，禁止规则通道组间断模式后，此参数忽略 */
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* 触发转换方式：软件触发 */
    HAL_ADC_Init(&g_adc_nch_dma_handle);                      /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_nch_dma_handle);         /* 校准ADC */
    
    adc_ch_conf.Channel = ADC_ADCX_CHY;               /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;            /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; /* 采样时间 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 通道1配置 */
    adc_ch_conf.Channel = ADC_ADCX_CHY2;               /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_2;            /* 序列 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf); /* 通道2配置 */ 

    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 6, 0); /* 设置中断优先级 */
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);          /* 使能中断 */    

    HAL_DMA_Start_IT(&g_dma_nch_adc_handle, (uint32_t)&ADC_ADCX->DR, mar, 0); /* 开启DMA传输 */
    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, &mar, 0); /* 开启ADC，并使能DMA */

}

void adc_dma_enable(uint16_t cndtr)
{
    ADC_ADCX->CR2 &= ~(1 << 0); /* 关闭ADC */
    
    ADC_ADCX_DMACx->CCR &= ~(1 << 0); /* 关闭DMA */
    while (ADC_ADCX_DMACx->CCR & (1 << 0));
    ADC_ADCX_DMACx->CNDTR = cndtr; /* 设置DMA传输数据量 */
    ADC_ADCX_DMACx->CCR |= 1 << 0; /* 使能DMA */
    
    ADC_ADCX->CR2 |= 1 << 0; /* 开启ADC */
    ADC_ADCX->CR2 |= 1 << 22;  
}

void ADC_ADCX_DMACx_IRQHandler(void)
{
    if(ADC_ADCX_DMACx_IS_TC()) /* 如果DMA传输完成 */
    {
        g_adc_dma_sta = 1; /* 设置DMA传输完成标志 */
        ADC_ADCX_DMACx_CLR_TC(); /* 清除DMA传输完成标志 */
    }
}

