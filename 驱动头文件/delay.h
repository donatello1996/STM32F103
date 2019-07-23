#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f1xx.h"

static int fac_us=0;
RCC_OscInitTypeDef RCC_OscInitStructure; 
RCC_ClkInitTypeDef RCC_ClkInitStructure;

void Clock_Init(int pll_mul)
{
	RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStructure.HSEState=RCC_HSE_ON;
	RCC_OscInitStructure.HSEPredivValue=RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;
	RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;
	RCC_OscInitStructure.PLL.PLLMUL=pll_mul;
	HAL_RCC_OscConfig(&RCC_OscInitStructure);

	RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;
	RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV2; 
	RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_2);
	//设置FLASH延时周期为2WS，也就是3个CPU周期。
	
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	fac_us=72;
}

void Delay_us(int nus)
{		
	int ticks;
	int told,tnow,tcnt=0;
	int reload=SysTick->LOAD;	 
	ticks=nus*fac_us;
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;
			//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;	
			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}

//延时nms
//nms:要延时的ms数
void Delay_ms(int nms)
{
	int i;
	for(i=0;i<nms;i++) 
		Delay_us(1000);
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

#endif
