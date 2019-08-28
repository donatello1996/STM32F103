#ifndef _TIM_CAP_H_
#define _TIM_CAP_H_
#include "stm32f1xx.h"
#include "tim_cap.h"

//���벶��״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���
unsigned char TIM2CH1_CAPTURE_STA;

//���벶��ֵ 
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
	//�����ز���
	TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;
	//ӳ�䵽TI1��
	TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;
	//���������Ƶ������Ƶ
	TIM2_CH1Config.ICFilter=0;
	//���������˲��������˲�
	HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);
	
//	HAL_TIM_IC_Start(&TIM2_Handler,TIM_CHANNEL_1);

	HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_1);
	//����TIM2�Ĳ���ͨ��1�����ҿ��������ж�
	__HAL_TIM_ENABLE_IT(&TIM2_Handler,TIM_IT_UPDATE);
	//ʹ�ܸ����ж�

	HAL_NVIC_SetPriority(TIM2_IRQn,1,1);
	HAL_NVIC_EnableIRQ(TIM2_IRQn); 
}

//��ʱ�������жϣ�����������жϴ���ص�����
void TIM_PeriodElapsedCB()
	//�����жϣ����������ʱִ��
{
	if((TIM2CH1_CAPTURE_STA&0X80)==0)
		//��δ�ɹ�����ߵ�ƽ����
	{
		//printf("11111111\n");
		if(TIM2CH1_CAPTURE_STA&0X40)
			//�Ѿ����񵽸ߵ�ƽ��
		{
			//printf("22222222\n");
			if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)
				//�ߵ�ƽ̫����
			{
				TIM2CH1_CAPTURE_STA|=0X80;
				//��ǳɹ�����һ�θߵ�ƽ����
				TIM2CH1_CAPTURE_VAL=0XFFFF;
			}else TIM2CH1_CAPTURE_STA++;
		}	 
	}		
}

//��ʱ�����벶���жϴ���ص�����
void TIM_IC_CaptureCB()
//�����жϷ���ʱִ��
{
	if((TIM2CH1_CAPTURE_STA&0X80)==0)				//��δ�ɹ�����
	{

		if(TIM2CH1_CAPTURE_STA&0X40)
		//����һ���½��� 		
		{	  		
			TIM2CH1_CAPTURE_STA|=0X80;
			//��ǳɹ�����һ�θߵ�ƽ����
      TIM2CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);
			//��ȡ��ǰ�Ĳ���ֵ.
			TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);
			//һ��Ҫ�����ԭ��������
      TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
			//��ʱ��5ͨ��1����Ϊ�����ز���
		}
		else
		//��δ��ʼ,��һ�β���������
		{
			TIM2CH1_CAPTURE_STA=0;
			//���
			TIM2CH1_CAPTURE_VAL=0;
			TIM2CH1_CAPTURE_STA|=0X40;
			//��ǲ�����������
			__HAL_TIM_DISABLE(&TIM2_Handler);
			//�رն�ʱ��5
			__HAL_TIM_SET_COUNTER(&TIM2_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1);
			//һ��Ҫ�����ԭ��������
			TIM_SET_CAPTUREPOLARITY(&TIM2_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);
			//��ʱ��5ͨ��1����Ϊ�½��ز���
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
	//ʹ��TIM2ʱ�� 
	RCC->APB2ENR|=1<<2;
	//ʹ��PORTAʱ�� 

	GPIOA->CRL&=0XFFFFFFF0;
	GPIOA->CRL|=0X00000008;
	GPIOA->ODR|=0<<0;

	TIM2->ARR=arr;
	TIM2->PSC=psc;

	TIM2->CCMR1|=1<<0;
	//CC1S=01   ѡ������� IC1ӳ�䵽TI1��
	TIM2->CCMR1|=1<<4;
	//IC1F=0001 ���������˲��� ��Fck_int������2���¼�����Ч
	TIM2->CCMR1|=0<<10;
	//IC2PS=00  �������벻��Ƶ 
	TIM2->CCER|=0<<1;
//CC1P=0    �����ز���
	TIM2->CCER|=1<<0;
//CC1E=1     �������������ֵ������Ĵ�����
	TIM2->SMCR=0x4000;
//�����ⲿ����ģʽ2
	TIM2->CR1|=0x01;
//ʹ�ܶ�ʱ��2
	TIM2->DIER|=TIM_FLAG_UPDATE;
//��������ж�  
	HAL_NVIC_SetPriority(TIM2_IRQn,0,0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn); 
//  TIM2->DIER|=1<<1;
//�������ж�                
      
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
