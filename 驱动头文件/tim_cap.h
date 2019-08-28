#ifndef _TIM_CAP_H_
#define _TIM_CAP_H_
#include "stm32f1xx.h"
#include "tim_cap.h"

//输入捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数
unsigned char TIM2CH1_CAPTURE_STA;

//输入捕获值 
int	TIM2CH1_CAPTURE_VAL;

TIM_HandleTypeDef TIM2_Handler;

void TIM2_CAP_Init(int arr,int psc)
{  
	TIM_IC_InitTypeDef TIM2_CH1Config;  
  GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_Initure.Pin=GPIO_PIN_0; 
	GPIO_Initure.Mode=GPIO_MODE_AF_INPUT; 
	GPIO_Initure.Pull=GPIO_PULLDOWN; 
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	TIM2_Handler.Instance=TIM2;
	TIM2_Handler.Init.Prescaler=psc;
	TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
	TIM2_Handler.Init.Period=arr;
	TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_IC_Init(&TIM2_Handler);

	TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;
	//上升沿捕获
	TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;
	//映射到TI1上
	TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;
	//配置输入分频，不分频
	TIM2_CH1Config.ICFilter=0;
	//配置输入滤波器，不滤波
	HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);
	
//	HAL_TIM_IC_Start(&TIM2_Handler,TIM_CHANNEL_1);

	HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_1);
	//开启TIM2的捕获通道1，并且开启捕获中断
	__HAL_TIM_ENABLE_IT(&TIM2_Handler,TIM_IT_UPDATE);
	//使能更新中断

	HAL_NVIC_SetPriority(TIM2_IRQn,1,1);
	HAL_NVIC_EnableIRQ(TIM2_IRQn); 
}

//定时器更新中断（计数溢出）中断处理回调函数
void TIM_PeriodElapsedCB()
	//更新中断（溢出）发生时执行
{
	if((TIM2CH1_CAPTURE_STA&0X80)==0)
		//还未成功捕获高电平脉宽
	{
		//printf("11111111\n");
		if(TIM2CH1_CAPTURE_STA&0X40)
			//已经捕获到高电平了
		{
			//printf("22222222\n");
			if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)
				//高电平太长了
			{
				TIM2CH1_CAPTURE_STA|=0X80;
				//标记成功捕获到一次高电平脉宽
				TIM2CH1_CAPTURE_VAL=0XFFFF;
			}else TIM2CH1_CAPTURE_STA++;
		}	 
	}		
}

//定时器输入捕获中断处理回调函数
void TIM_IC_CaptureCB()
//捕获中断发生时执行
{
	if((TIM2CH1_CAPTURE_STA&0X80)==0)				//还未成功捕获
	{

		if(TIM2CH1_CAPTURE_STA&0X40)
		//捕获到一个下降沿 		
		{	  		
			TIM2CH1_CAPTURE_STA|=0X80;
			//标记成功捕获到一次高电平脉宽
      TIM2CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);
			//获取当前的捕获值.
			TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);
			//一定要先清除原来的设置
      TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
			//定时器5通道1设置为上升沿捕获
		}
		else
		//还未开始,第一次捕获上升沿
		{
			TIM2CH1_CAPTURE_STA=0;
			//清空
			TIM2CH1_CAPTURE_VAL=0;
			TIM2CH1_CAPTURE_STA|=0X40;
			//标记捕获到了上升沿
			__HAL_TIM_DISABLE(&TIM2_Handler);
			//关闭定时器5
			__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);
			//一定要先清除原来的设置
			TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);
			//定时器5通道1设置为下降沿捕获
			__HAL_TIM_ENABLE(&TIM2_Handler);
		}		    
	}		
}

//void TIM2_IRQHandler()
//{
//  if(TIM2->SR&TIM_FLAG_CC1)
//  {
//    if(TIM2->DIER&TIM_IT_CC1)
//    {
//			TIM2->SR&=~TIM_FLAG_CC1;
//			TIM2_Handler.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
//			if((TIM2_Handler.Instance->CCMR1 & TIM_CCMR1_CC1S) != 0x00U)
//			{
//				TIM_IC_CaptureCB();
//			}
//			TIM2_Handler.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
//    }
//  }

//  if(TIM2->SR&TIM_FLAG_UPDATE)
//  {
//    if(TIM2->DIER&TIM_IT_UPDATE)
//    {
//      TIM2->SR&=~TIM_FLAG_UPDATE;
//      TIM_PeriodElapsedCB();
//    }
//  }
//}

void TIM2_Etr_Init(short arr,short psc)
{    
	RCC->APB1ENR|=1<<0;
	//使能TIM2时钟 
	RCC->APB2ENR|=1<<2;
	//使能PORTA时钟 

	GPIOA->CRL&=0XFFFFFFF0;
	GPIOA->CRL|=0X00000008;
	GPIOA->ODR|=0<<0;

	TIM2->ARR=arr;
	TIM2->PSC=psc;

	TIM2->CCMR1|=1<<0;
	//CC1S=01   选择输入端 IC1映射到TI1上
	TIM2->CCMR1|=1<<4;
	//IC1F=0001 配置输入滤波器 以Fck_int采样，2个事件后有效
	TIM2->CCMR1|=0<<10;
	//IC2PS=00  配置输入不分频 
	TIM2->CCER|=0<<1;
//CC1P=0    上升沿捕获
	TIM2->CCER|=1<<0;
//CC1E=1     允许捕获计数器的值到捕获寄存器中
	TIM2->SMCR=0x4000;
//启动外部计数模式2
	TIM2->CR1|=0x01;
//使能定时器2
	TIM2->DIER|=TIM_FLAG_UPDATE;
//允许更新中断  
	HAL_NVIC_SetPriority(TIM2_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn); 
//  TIM2->DIER|=1<<1;
//允许捕获中断                
      
}

int count,count_cap=0;

void TIM2_IRQHandler()
{
	if(TIM2->SR&TIM_FLAG_UPDATE)
	{
		count_cap++;
		TIM2->SR&=~TIM_FLAG_UPDATE;
	}
}

#endif
