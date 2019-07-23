#ifndef _TIM_CAP_H_
#define _TIM_CAP_H_
#include "stm32f1xx.h"

//输入捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数
unsigned char TIM5CH1_CAPTURE_STA;

//输入捕获值 
int	TIM5CH1_CAPTURE_VAL;

TIM_HandleTypeDef TIM5_Handler;

void TIM5_CAP_Init(int arr,int psc)
{  
	TIM_IC_InitTypeDef TIM5_CH1Config;  
  GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_TIM5_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_Initure.Pin=GPIO_PIN_0; 
	GPIO_Initure.Mode=GPIO_MODE_AF_INPUT; 
	GPIO_Initure.Pull=GPIO_PULLDOWN; 
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	TIM5_Handler.Instance=TIM5;
	TIM5_Handler.Init.Prescaler=psc;
	TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
	TIM5_Handler.Init.Period=arr;
	TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_IC_Init(&TIM5_Handler);

	TIM5_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;
	//上升沿捕获
	TIM5_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;
	//映射到TI1上
	TIM5_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;
	//配置输入分频，不分频
	TIM5_CH1Config.ICFilter=0;
	//配置输入滤波器，不滤波
	HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH1Config,TIM_CHANNEL_1);

	HAL_TIM_IC_Start_IT(&TIM5_Handler,TIM_CHANNEL_1);
	//开启TIM5的捕获通道1，并且开启捕获中断
	__HAL_TIM_ENABLE_IT(&TIM5_Handler,TIM_IT_UPDATE);
	//使能更新中断

	HAL_NVIC_SetPriority(TIM5_IRQn,2,0);
	HAL_NVIC_EnableIRQ(TIM5_IRQn); 
}

void TIM5_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM5_Handler);				//定时器共用处理函数
}
 
//定时器更新中断（计数溢出）中断处理回调函数， 该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//更新中断（溢出）发生时执行
{
	if((TIM5CH1_CAPTURE_STA&0X80)==0)
		//还未成功捕获高电平脉宽
	{
		//printf("11111111\n");
		if(TIM5CH1_CAPTURE_STA&0X40)
			//已经捕获到高电平了
		{
			//printf("22222222\n");
			if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)
				//高电平太长了
			{
				TIM5CH1_CAPTURE_STA|=0X80;
				//标记成功捕获到一次高电平脉宽
				TIM5CH1_CAPTURE_VAL=0XFFFF;
			}else TIM5CH1_CAPTURE_STA++;
		}	 
	}		
}

//定时器输入捕获中断处理回调函数，该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	if((TIM5CH1_CAPTURE_STA&0X80)==0)				//还未成功捕获
	{
		//printf("33333333\n");
		if(TIM5CH1_CAPTURE_STA&0X40)
		//捕获到一个下降沿 		
		{	  		
			//printf("44444444\n");
			TIM5CH1_CAPTURE_STA|=0X80;
			//标记成功捕获到一次高电平脉宽
      TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_1);
			//获取当前的捕获值.
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);
			//一定要先清除原来的设置
      TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
			//定时器5通道1设置为上升沿捕获
		}
		else
		//还未开始,第一次捕获上升沿
		{
			TIM5CH1_CAPTURE_STA=0;
			//清空
			TIM5CH1_CAPTURE_VAL=0;
			TIM5CH1_CAPTURE_STA|=0X40;
			//标记捕获到了上升沿
			__HAL_TIM_DISABLE(&TIM5_Handler);
			//关闭定时器5
			__HAL_TIM_SET_COUNTER(&TIM5_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);
			//一定要先清除原来的设置
			TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);
			//定时器5通道1设置为下降沿捕获
			__HAL_TIM_ENABLE(&TIM5_Handler);
			//使能定时器5
		}		    
	}		
}

#endif

