#include "stm32f1xx.h"

IWDG_HandleTypeDef hiwdg;

void IWDG_Init(int prer,int rlr)
{
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = prer;
  hiwdg.Init.Reload = rlr;
  HAL_IWDG_Init(&hiwdg);
	HAL_IWDG_Start(&hiwdg);
}

TIM_HandleTypeDef TIM4_Handler;

void TIM4_IRQHandler()
{
	if(TIM4->SR&0X0001)
	{
		IWDG->KR=0XAAAA;
	}
	TIM4->SR&=~(1<<0);
	//清除中断标志位 	
}

void TIM4_Init(int arr,int psc)
{
	__HAL_RCC_TIM4_CLK_ENABLE();
	TIM4_Handler.Instance=TIM4;
	TIM4_Handler.Init.Prescaler=psc;
	TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
	TIM4_Handler.Init.Period=arr;
	TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&TIM4_Handler);
	HAL_TIM_Base_Start_IT(&TIM4_Handler);
	
	HAL_NVIC_SetPriority(TIM4_IRQn,0,1); 
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
}
