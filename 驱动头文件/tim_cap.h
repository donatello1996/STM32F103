#ifndef _TIM_CAP_H_
#define _TIM_CAP_H_
#include "stm32f1xx.h"

//���벶��״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���
unsigned char TIM5CH1_CAPTURE_STA;

//���벶��ֵ 
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
	//�����ز���
	TIM5_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;
	//ӳ�䵽TI1��
	TIM5_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;
	//���������Ƶ������Ƶ
	TIM5_CH1Config.ICFilter=0;
	//���������˲��������˲�
	HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH1Config,TIM_CHANNEL_1);

	HAL_TIM_IC_Start_IT(&TIM5_Handler,TIM_CHANNEL_1);
	//����TIM5�Ĳ���ͨ��1�����ҿ��������ж�
	__HAL_TIM_ENABLE_IT(&TIM5_Handler,TIM_IT_UPDATE);
	//ʹ�ܸ����ж�

	HAL_NVIC_SetPriority(TIM5_IRQn,2,0);
	HAL_NVIC_EnableIRQ(TIM5_IRQn); 
}

void TIM5_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM5_Handler);				//��ʱ�����ô�����
}
 
//��ʱ�������жϣ�����������жϴ���ص������� �ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//�����жϣ����������ʱִ��
{
	if((TIM5CH1_CAPTURE_STA&0X80)==0)
		//��δ�ɹ�����ߵ�ƽ����
	{
		//printf("11111111\n");
		if(TIM5CH1_CAPTURE_STA&0X40)
			//�Ѿ����񵽸ߵ�ƽ��
		{
			//printf("22222222\n");
			if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)
				//�ߵ�ƽ̫����
			{
				TIM5CH1_CAPTURE_STA|=0X80;
				//��ǳɹ�����һ�θߵ�ƽ����
				TIM5CH1_CAPTURE_VAL=0XFFFF;
			}else TIM5CH1_CAPTURE_STA++;
		}	 
	}		
}

//��ʱ�����벶���жϴ���ص��������ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if((TIM5CH1_CAPTURE_STA&0X80)==0)				//��δ�ɹ�����
	{
		//printf("33333333\n");
		if(TIM5CH1_CAPTURE_STA&0X40)
		//����һ���½��� 		
		{	  		
			//printf("44444444\n");
			TIM5CH1_CAPTURE_STA|=0X80;
			//��ǳɹ�����һ�θߵ�ƽ����
      TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_1);
			//��ȡ��ǰ�Ĳ���ֵ.
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);
			//һ��Ҫ�����ԭ��������
      TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
			//��ʱ��5ͨ��1����Ϊ�����ز���
		}
		else
		//��δ��ʼ,��һ�β���������
		{
			TIM5CH1_CAPTURE_STA=0;
			//���
			TIM5CH1_CAPTURE_VAL=0;
			TIM5CH1_CAPTURE_STA|=0X40;
			//��ǲ�����������
			__HAL_TIM_DISABLE(&TIM5_Handler);
			//�رն�ʱ��5
			__HAL_TIM_SET_COUNTER(&TIM5_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);
			//һ��Ҫ�����ԭ��������
			TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);
			//��ʱ��5ͨ��1����Ϊ�½��ز���
			__HAL_TIM_ENABLE(&TIM5_Handler);
			//ʹ�ܶ�ʱ��5
		}		    
	}		
}

#endif

