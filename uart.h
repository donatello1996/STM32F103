#ifndef _UART_H_
#define _UART_H_
#include <stdio.h>
#include <string.h>
#include "stm32f1xx.h"
//#include "at24cxx.h"

UART_HandleTypeDef huart1,huart2,huart3;
DMA_HandleTypeDef hdma_usart1_rx,hdma_usart2_rx;
GPIO_InitTypeDef GPIO_InitStruct;

#define BUFFERSIZE 255
unsigned char rx1_buf[BUFFERSIZE], uart1_recv_end_flag=1, rx1_len=0;
unsigned char rx2_buf[BUFFERSIZE], uart2_recv_end_flag=1, rx2_len=0;
unsigned char flag_a20_set_success=0;

void UART1_Init(int baud)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();	
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

  GPIO_InitStruct.Pin=GPIO_PIN_9;
  GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

  GPIO_InitStruct.Pin=GPIO_PIN_10;
  GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull=GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
  huart1.Instance=USART1;
  huart1.Init.BaudRate=baud;
  huart1.Init.WordLength=UART_WORDLENGTH_8B;
  huart1.Init.StopBits=UART_STOPBITS_1;
  huart1.Init.Parity=UART_PARITY_NONE;
  huart1.Init.Mode=UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl=UART_HWCONTROL_NONE;
  huart1.Init.OverSampling=UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	HAL_NVIC_SetPriority(USART1_IRQn,1,1);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
	
	hdma_usart1_rx.Instance=DMA1_Channel5;
  hdma_usart1_rx.Init.Direction=DMA_PERIPH_TO_MEMORY;
  hdma_usart1_rx.Init.PeriphInc=DMA_PINC_DISABLE;
  hdma_usart1_rx.Init.MemInc=DMA_MINC_ENABLE;
  hdma_usart1_rx.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;
  hdma_usart1_rx.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;
  hdma_usart1_rx.Init.Mode=DMA_NORMAL;
  hdma_usart1_rx.Init.Priority=DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hdma_usart1_rx);
  __HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);
	
}

void UART2_Init(int baud)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=GPIO_PIN_3;
  GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull=GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  huart2.Instance = USART2;
  huart2.Init.BaudRate = baud;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);

	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	HAL_NVIC_SetPriority(USART2_IRQn,0,0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	
	hdma_usart2_rx.Instance = DMA1_Channel6;
	hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_usart2_rx.Init.Mode = DMA_NORMAL;
	hdma_usart2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	HAL_DMA_Init(&hdma_usart2_rx);
	__HAL_LINKDMA(&huart2,hdmarx,hdma_usart2_rx);

}

void UART3_Init(int baud)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();	
	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_AFIO_REMAP_USART3_PARTIAL();
	
  GPIO_InitStruct.Pin=GPIO_PIN_10;
  GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

  GPIO_InitStruct.Pin=GPIO_PIN_11;
  GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull=GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
  huart3.Instance=USART3;
  huart3.Init.BaudRate=baud;
  huart3.Init.WordLength=UART_WORDLENGTH_8B;
  huart3.Init.StopBits=UART_STOPBITS_1;
  huart3.Init.Parity=UART_PARITY_NONE;
  huart3.Init.Mode=UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl=UART_HWCONTROL_NONE;
  huart3.Init.OverSampling=UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);
	
//	__HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
//	HAL_NVIC_SetPriority(USART3_IRQn,0,0);
//  HAL_NVIC_EnableIRQ(USART3_IRQn);
//	
//	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
//	HAL_NVIC_SetPriority(USART3_IRQn,0,0);
//  HAL_NVIC_EnableIRQ(USART3_IRQn);
//	
//	hdma_usart3_rx.Instance=DMA1_Channel3;
//  hdma_usart3_rx.Init.Direction=DMA_PERIPH_TO_MEMORY;
//  hdma_usart3_rx.Init.PeriphInc=DMA_PINC_DISABLE;
//  hdma_usart3_rx.Init.MemInc=DMA_MINC_ENABLE;
//  hdma_usart3_rx.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;
//  hdma_usart3_rx.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;
//  hdma_usart3_rx.Init.Mode=DMA_NORMAL;
//  hdma_usart3_rx.Init.Priority=DMA_PRIORITY_LOW;
//  HAL_DMA_Init(&hdma_usart3_rx);
//  __HAL_LINKDMA(&huart3,hdmarx,hdma_usart3_rx);
	
}

void UART2_Send(char ch)
{
	while((USART2->SR&0X40)==0);
  USART2->DR=ch; 
}

void UART3_Send(char ch)
{
	while((USART3->SR&0X40)==0);
  USART3->DR=ch; 
}

void UART2_Put_String(unsigned char s[])
{
	int i=0;
	while(s[i]!='\0')
	{
		UART2_Send(s[i]);
		i++;
	}
}

void USART1_IRQHandler()
{
	uint32_t temp;
	if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);
		temp=__HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
		rx1_len=BUFFERSIZE-temp; 
		uart1_recv_end_flag=1;
	}
}


void USART2_IRQHandler()
{
	int temp;
	if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);
		temp=__HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		rx2_len=BUFFERSIZE-temp; 
		uart2_recv_end_flag=1;
		//printf("%s %d\n", rx2_buf, rx2_len);
		if(rx2_buf[0]=='+'
			&&rx2_buf[1]=='D'
			&&rx2_buf[2]=='M'
			&&rx2_buf[3]=='O'
			&&rx2_buf[4]=='S'
			&&rx2_buf[5]=='E'
			&&rx2_buf[6]=='T'
			&&rx2_buf[7]=='G'
			&&rx2_buf[8]=='R'
			&&rx2_buf[9]=='O'
			&&rx2_buf[10]=='U'
			&&rx2_buf[11]=='P'
			&&rx2_buf[12]==':'
			&&rx2_buf[13]=='0')
		{
			flag_a20_set_success=1;
			printf("频率写入成功\n");
			for(temp=0; temp<255; temp++)
				rx2_buf[0]=0x00;
		}
	}
}

int fputc(int ch,FILE *f)
{
	while(!(USART1->SR&(1<<7)));
	USART1->DR=ch;
  return ch;
}
//
void UART1_Put_Char(unsigned char ch)
{
	while(!(USART1->SR&(1<<7)));
	USART1->DR=ch;
}

void UART1_Put_String(unsigned char s[])
{
	int i=0;
	while(s[i]!='\0')
	{
		UART1_Put_Char(s[i]);
		i++;
	}
}

void UART_DMA_Get()
{
	int i=0;
	if(uart1_recv_end_flag==1)
	{	
		uart1_recv_end_flag=0;
		printf("%s %d\n", rx1_buf, rx1_len);
		HAL_UART_Receive_DMA(&huart1, (unsigned char*)rx1_buf, BUFFERSIZE);
	}
	
	if(uart2_recv_end_flag==1)
	{	
		uart2_recv_end_flag=0;
		//printf("%s %d\n", rx2_buf, rx2_len);
		HAL_UART_Receive_DMA(&huart2, (unsigned char*)rx2_buf, BUFFERSIZE);
	}
}

//void Set_A002(unsigned char set_gbw,float tx_freq,float rx_freq,unsigned char rx_cxcss,unsigned char sq,unsigned char tx_cxcss,unsigned char power)
//{
//	unsigned char a002_send_buff[46]="AT+DMOSETGROUP=1,436.025,436.025,000,1,001,1\r\n";
//	int i=0;
//	if(set_gbw)
//		a002_send_buff[15]='1';
//	else a002_send_buff[15]='0';
//	
//	if(sq<0 || sq>8)sq=1;
//	if(power<0 || power>8)power=1;
//	
//	a002_send_buff[17]=(int)tx_freq/100%10+'0';
//	a002_send_buff[18]=(int)tx_freq/10%10+'0';
//	a002_send_buff[19]=(int)tx_freq%10+'0';
//	
//	a002_send_buff[21]=(int)(tx_freq*10)%10+'0';
//	a002_send_buff[22]=(int)(tx_freq*100)%10+'0';
//	a002_send_buff[23]=(int)(tx_freq*1000)%10+'0';
//	
//	a002_send_buff[25]=(int)rx_freq/100%10+'0';
//	a002_send_buff[26]=(int)rx_freq/10%10+'0';
//	a002_send_buff[27]=(int)rx_freq%10+'0';
//	
//	a002_send_buff[29]=(int)(rx_freq*10)%10+'0';
//	a002_send_buff[30]=(int)(rx_freq*100)%10+'0';
//	a002_send_buff[31]=(int)(rx_freq*1000)%10+'0';
//	
//	a002_send_buff[33]=rx_cxcss/100%10+'0';
//	a002_send_buff[34]=rx_cxcss/10%10+'0';
//	a002_send_buff[35]=rx_cxcss%10+'0';
//	
//	a002_send_buff[37]=sq+'0';
//	
//	a002_send_buff[39]=tx_cxcss/100%10+'0';
//	a002_send_buff[40]=tx_cxcss/10%10+'0';
//	a002_send_buff[41]=tx_cxcss%10+'0';
//	
//	a002_send_buff[43]=power+'0';
//	
//	for(i=0;i<=45;i++)
//		UART2_Send(a002_send_buff[i]);
//}


//void Set_A002_MIC(unsigned char miclvl,unsigned char scramlvl,unsigned char tot)
//{
//	unsigned char i=0,a002_send_buff[20]="AT+DMOSETMIC=1,0,0\r\n";
//	a002_send_buff[13]=miclvl+'0';
//	a002_send_buff[15]=scramlvl+'0';
//	a002_send_buff[17]=tot+'0';
//	
//	for(i=0;i<=19;i++)
//		UART2_Send(a002_send_buff[i]);
//}

//int Get_A002_RSSI()
//{
//	unsigned char i=0, a002_send_buff[16]="AT+DMOREADRSSI\r\n";
//	int rssi=0, len=0;
//	for(i=0;i<16;i++)
//		UART2_Send(a002_send_buff[i]);
//	Delay_ms(20);
//	len = strlen(rx2_buf);
//	if(len==16)
//		rssi = rx2_buf[13]-'0';
//	else if(len==17)
//		rssi = (rx2_buf[13]-'0')*16 + (rx2_buf[14]-'0');
//	
//	rssi=rssi*100/0xff;
//	for(i=0; i<255; i++)
//			rx2_buf[0]=0x00;
//	
//	if(rssi<0)rssi=0;
//	if(rssi>100)rssi=100;
//	return rssi;
//	
//}

#endif

