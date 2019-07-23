#ifndef __ADC_H
#define __ADC_H
#include "stm32f1xx.h"

ADC_HandleTypeDef ADC1_Handler;
ADC_ChannelConfTypeDef ADC1_ChanConf;
RCC_PeriphCLKInitTypeDef ADC_CLKInit;
GPIO_InitTypeDef GPIO_Initure;

void MY_ADC_Init()
{ 
  __HAL_RCC_ADC1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();	
	
  GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_4;
  GPIO_Initure.Mode=GPIO_MODE_ANALOG;
  GPIO_Initure.Pull=GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;	
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV2;	
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);
	
  ADC1_Handler.Instance=ADC1;
  ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;
  ADC1_Handler.Init.ScanConvMode=DISABLE;
	//非扫描模式
  ADC1_Handler.Init.ContinuousConvMode=DISABLE;
	//关闭连续转换
  ADC1_Handler.Init.NbrOfConversion=1;
	//1个转换在规则序列中 也就是只转换规则序列1 
  ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;
	//禁止不连续采样模式
  ADC1_Handler.Init.NbrOfDiscConversion=0;
	//不连续采样通道数为0
  ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;
	//软件触发
  HAL_ADC_Init(&ADC1_Handler);
	
	HAL_ADCEx_Calibration_Start(&ADC1_Handler);	
	//校准ADC
  
	ADC1_ChanConf.Rank=1;
	//第1个序列，序列1
	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;
	//采样时间               
	ADC1_ChanConf.Channel=ADC_CHANNEL_4;
	HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);
	//通道配置
	HAL_ADC_Start(&ADC1_Handler); 
}

short Get_Adc(int ch)   
{
	ADC1_ChanConf.Channel=ch;
	HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);
  HAL_ADC_Start(&ADC1_Handler);
	
	ADC1->CR2 |= ((uint32_t)0x00500000);
	while(!(ADC1->SR&ADC_FLAG_EOC));
	return ADC1->DR;
}

#endif 
