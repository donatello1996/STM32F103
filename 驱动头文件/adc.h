#ifndef __ADC_H_
#define	__ADC_H_
#include "stm32f1xx.h"

#define ADC_GPIO                        GPIOA
#define ADC_GPIO_CLKEN  								__HAL_RCC_GPIOA_CLK_ENABLE()
#define ADC_GPIO_PIN1                   GPIO_PIN_1               
#define ADC_GPIO_PIN2                   GPIO_PIN_4

#define ADC_NUMOFCHANNEL               	2
#define ADC_NUMOFSAMP               	  1

#define ADC_USE_DMA                     0

ADC_HandleTypeDef hadcx;
RCC_PeriphCLKInitTypeDef ADC_CLKInit;
DMA_HandleTypeDef hdma_adcx;
ADC_ChannelConfTypeDef sConfig;
GPIO_InitTypeDef GPIO_InitStruct;

int adc_value[ADC_NUMOFSAMP][ADC_NUMOFCHANNEL];
int DMA_Transfer_Complete_Count=0;
	
void MX_ADCx_Init()
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	ADC_GPIO_CLKEN;
  __HAL_RCC_DMA1_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=ADC_GPIO_PIN1|ADC_GPIO_PIN2;
  GPIO_InitStruct.Mode=GPIO_MODE_ANALOG;
  HAL_GPIO_Init(ADC_GPIO,&GPIO_InitStruct);
	
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;	
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV2;	
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);

  hadcx.Instance = ADC1;
  hadcx.Init.ScanConvMode = DISABLE;
  hadcx.Init.ContinuousConvMode = DISABLE;
  hadcx.Init.DiscontinuousConvMode = DISABLE;
  hadcx.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadcx.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadcx.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadcx);

  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx, &sConfig);
  HAL_ADCEx_Calibration_Start(&hadcx);
	
#if ADC_USE_DMA==1

	hdma_adcx.Instance = DMA1_Channel1;
	hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adcx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_adcx.Init.Mode = DMA_CIRCULAR;
	hdma_adcx.Init.Priority = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&hdma_adcx);
	__HAL_LINKDMA(&hadcx,DMA_Handle,hdma_adcx);
	

  HAL_ADC_Start_DMA(&hadcx,(unsigned int*)adc_value,ADC_NUMOFCHANNEL*ADC_NUMOFSAMP); 

#endif

//	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn,0,0);
//	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

#if ADC_USE_DMA==0

short Get_Adc(int ch)   
{
	sConfig.Channel=ch;
	HAL_ADC_ConfigChannel(&hadcx,&sConfig); 
  HAL_ADC_Start(&hadcx);
//ÂÖÑ¯×ª»»
	ADC1->CR2 |= ((uint32_t)0x00500000);
	while(!(ADC1->SR&ADC_FLAG_EOC));
	return (short)ADC1->DR;
}

#endif

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  DMA_Transfer_Complete_Count++; 
}

void DMA1_Channel1_IRQHandler()
{
  HAL_DMA_IRQHandler(&hdma_adcx);
}

#endif
