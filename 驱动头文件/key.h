#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f1xx.h"

enum KEYS_PRES
{
	KEY_NONE_PRES=0,
	KEY_PC13_START_PRES=1,
	KEY_PC14_STOP_PRES=2,
	KEY_PC15_PAUSE_PRES=3,
};

#define KEY_PC13_START				HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)
#define KEY_PC14_STOP        	HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14)
#define KEY_PC15_PAUSE        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15)

GPIO_InitTypeDef GPIO_InitStruct;

void KEY_Init()
{
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
}

int count_delay=0;
const int const_delay_num=70;

unsigned char KEY_Scan()
{
	static unsigned char key1_up=1;
	if(count_delay<=const_delay_num&&(KEY_PC13_START==0||KEY_PC14_STOP==0
		||KEY_PC15_PAUSE==0))
	{
		count_delay++;
	}
	else if(count_delay>const_delay_num&&KEY_PC13_START==0&&key1_up)
	{
		key1_up=0;
		count_delay=0;
		return KEY_PC13_START_PRES;
	}
	else if(count_delay>const_delay_num&&KEY_PC14_STOP==0&&key1_up)
	{
		key1_up=0;
		count_delay=0;
		return KEY_PC14_STOP_PRES;
	}
	else if(count_delay>const_delay_num&&KEY_PC15_PAUSE==0&&key1_up)
	{
		key1_up=0;
		count_delay=0;
		return KEY_PC15_PAUSE_PRES;
	}
	
	else if(KEY_PC13_START==1&&KEY_PC14_STOP==1
					&&KEY_PC15_PAUSE==1)
		key1_up=1;
	return KEY_NONE_PRES;
}

#define INC_ENCODER_L_GPIO GPIOB
#define INC_ENCODER_L_GPIO_CLKEN 	__HAL_RCC_GPIOB_CLK_ENABLE()
#define INC_ENCODER_L_PIN GPIO_PIN_9
#define INC_ENCODER_L_READ (INC_ENCODER_L_GPIO->IDR&INC_ENCODER_L_PIN)

#define INC_ENCODER_R_GPIO GPIOB
#define INC_ENCODER_R_GPIO_CLKEN 	__HAL_RCC_GPIOB_CLK_ENABLE()
#define INC_ENCODER_R_PIN GPIO_PIN_8
#define INC_ENCODER_R_READ (INC_ENCODER_R_GPIO->IDR&INC_ENCODER_R_PIN)

void Inc_Encoder_GPIO_Init()
{
	INC_ENCODER_R_GPIO_CLKEN;
	INC_ENCODER_L_GPIO_CLKEN;

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	GPIO_InitStruct.Pin = INC_ENCODER_R_PIN;
  HAL_GPIO_Init(INC_ENCODER_R_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = INC_ENCODER_L_PIN;
  HAL_GPIO_Init(INC_ENCODER_L_GPIO,&GPIO_InitStruct);
	
}

unsigned char count_inc_encoder_delay_l=0,count_inc_encoder_delay_r=0;

void Delay()
{
	int i=1000;
	while(i--);
}

unsigned char	Inc_Encoder_Read()
{
	if(INC_ENCODER_R_READ==0&&INC_ENCODER_L_READ)
	{
		Delay();
		if(INC_ENCODER_R_READ==0&&INC_ENCODER_L_READ==0)
		{
			return 1;
		}
	}//+
	else if(INC_ENCODER_R_READ&&INC_ENCODER_L_READ==0)
	{
		Delay();
		if(INC_ENCODER_R_READ==0&&INC_ENCODER_L_READ==0)
		{
			return 2;
		}
	}//-
	return 0;
}

//unsigned char	Inc_Encoder_Read()
//{
//	static char L_LAST=0, R_LAST=0;
//	
//	if((INC_ENCODER_L_READ != L_LAST) && (INC_ENCODER_R_READ == R_LAST))
//	{

//		//Delay_ms(20);printf("编码器返回：+\n");
//		if(count_inc_encoder_delay>=20&&INC_ENCODER_R_READ != R_LAST)
//		{
//			count_inc_encoder_delay=0;
//			return 1;
//		}
//	}
//	else if((INC_ENCODER_L_READ == L_LAST) && (INC_ENCODER_R_READ != R_LAST))
//	{
//		count_inc_encoder_delay++;
//		//Delay_ms(20);printf("编码器返回：-\n");
//		if(count_inc_encoder_delay>=20&&INC_ENCODER_L_READ != L_LAST)
//		{
//			count_inc_encoder_delay=0;
//			return 2;
//		}
//	}

//	L_LAST = INC_ENCODER_L_READ;
//	R_LAST = INC_ENCODER_R_READ;
//	return 0;
//}

//void EXTI_Init()
//{
//	GPIO_InitTypeDef GPIO_Initure;

//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	__HAL_RCC_GPIOC_CLK_ENABLE();

//	GPIO_Initure.Pin=GPIO_PIN_15;
//	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
//	GPIO_Initure.Pull=GPIO_PULLUP;
//	HAL_GPIO_Init(GPIOA,&GPIO_Initure);

//	GPIO_Initure.Pin=GPIO_PIN_5;
//	HAL_GPIO_Init(GPIOC,&GPIO_Initure);

//	//中断线5-PC5
//	HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,1);
//	//抢占优先级为2，子优先级为1
//	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 

//	//中断线15-PA15
//	HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,0);
// 	//抢占优先级为2，子优先级为0
//	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//}

//void fun()
//{
//	int i=2000;
//	while(i--);
//}

//void EXTI9_5_IRQHandler()
//{	  
//	if(KEY0==0)
//	{
//		//count_delay++;
//		fun();
//		if(KEY0==0)
//		{
//			//count_delay=0;
//			printf("KEY0\n");
//			TIM2->CNT=0;
//			count_cap=0;
//			TIM4->ARR=10000;
//			TIM4->CNT=0;
////			TIM4->DIER|=TIM_DIER_UIE;
//			TIM4->CR1|=TIM_CR1_CEN;
//			EXTI->PR=1<<5;
//		}
//	}
//}

//void EXTI15_10_IRQHandler(void)
//{						 
//  if(KEY1==0)
//	{
//		printf("KEY1\n");
//	}
// 	EXTI->PR=1<<15;
//}

#endif
