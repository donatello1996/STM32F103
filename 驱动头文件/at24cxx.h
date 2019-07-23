#ifndef _AT24CXX_H
#define _AT24CXX_H
#include "stm32f1xx.h"
#include "delay.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767  

#define EE_TYPE AT24C256

#define I2C_GPIO_CLKEN	__HAL_RCC_GPIOA_CLK_ENABLE()
#define I2C_GPIO 				GPIOA
#define I2C_SCL_PIN 		GPIO_PIN_11
#define I2C_SDA_PIN 		GPIO_PIN_12

#define SDA_IN()  {I2C_GPIO->CRH&=0XFFF0FFFF;I2C_GPIO->CRH|=0X00080000;}
#define SDA_OUT() {I2C_GPIO->CRH&=0XFFF0FFFF;I2C_GPIO->CRH|=0X00030000;}

#define I2C_SCL_SET 	I2C_GPIO->BSRR|=I2C_SCL_PIN
#define I2C_SCL_CLR 	I2C_GPIO->BSRR|=((int)(I2C_SCL_PIN<<16))

#define I2C_SDA_SET 	I2C_GPIO->BSRR|=I2C_SDA_PIN
#define I2C_SDA_CLR 	I2C_GPIO->BSRR|=((int)(I2C_SDA_PIN<<16))

#define I2C_SDA_READ  HAL_GPIO_ReadPin(I2C_GPIO,I2C_SDA_PIN)

GPIO_InitTypeDef GPIO_Initure;

void I2C_Init()
{  
	I2C_GPIO_CLKEN;
	GPIO_Initure.Pin=I2C_SCL_PIN|I2C_SDA_PIN;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_NOPULL;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(I2C_GPIO,&GPIO_Initure);
	
	I2C_SDA_SET;
	I2C_SCL_SET;  
}

void I2C_Start()
{
	SDA_OUT();
	I2C_SDA_SET;	  	  
	I2C_SCL_SET;
	Delay_us(4);
 	I2C_SDA_CLR;
	Delay_us(4);
	I2C_SCL_CLR;
}	  

void I2C_Stop()
{
	SDA_OUT();
	I2C_SCL_CLR;
	I2C_SDA_CLR;
 	Delay_us(4);
	I2C_SCL_SET; 
	Delay_us(4);
	I2C_SDA_SET;
						   	
}

unsigned char I2C_Wait_Ack()
{
	unsigned char delay=0;
	SDA_IN(); 
	I2C_SDA_SET;
	Delay_us(1);	   
	I2C_SCL_SET;
	Delay_us(1);	 
	while(I2C_SDA_READ)
	{
		delay++;
		if(delay>10)
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C_SCL_CLR;  
	return 0;  
} 

void I2C_Ack()
{
	I2C_SCL_CLR;
	SDA_OUT();
	I2C_SDA_CLR;
	Delay_us(2);
	I2C_SCL_SET;
	Delay_us(2);
	I2C_SCL_CLR;
}
    
void I2C_NAck()
{
	I2C_SCL_CLR;
	SDA_OUT();
	I2C_SDA_SET;
	Delay_us(2);
	I2C_SCL_SET;
	Delay_us(2);
	I2C_SCL_CLR;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void I2C_Send_Byte(unsigned char txd)
{                        
  unsigned char t;   
	SDA_OUT(); 	    
	I2C_SCL_CLR;
//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
		if((txd&0x80)>>7)
			I2C_SDA_SET;
		else I2C_SDA_CLR;
			txd<<=1;  
		Delay_us(2);
		I2C_SCL_SET;
		Delay_us(2); 
		I2C_SCL_CLR;	
		Delay_us(2);
	}	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
unsigned char I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
    for(i=0;i<8;i++ )
	{
        I2C_SCL_CLR; 
        Delay_us(2);
		I2C_SCL_SET;
        receive<<=1;
        if(I2C_SDA_READ)receive++;   
		Delay_us(1); 
    }					 
    if (!ack)
        I2C_NAck();//发送nACK
    else
        I2C_Ack(); //发送ACK   
    return receive;
}


unsigned char AT24CXX_ReadOneByte(int ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    I2C_Start();  
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0XA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(ReadAddr>>8);
	}
	else I2C_Send_Byte(0XA0+((ReadAddr/256)<<1));	   
	I2C_Wait_Ack(); 
  I2C_Send_Byte(ReadAddr%256);
	I2C_Wait_Ack();	    
	I2C_Start();  	 	   
	I2C_Send_Byte(0XA1);		   
	I2C_Wait_Ack();	 
  temp=I2C_Read_Byte(0);		   
  I2C_Stop(); 
	return temp;
}

void AT24CXX_WriteOneByte(int WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
  I2C_Start();  
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0XA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(WriteAddr>>8);
	}
	else I2C_Send_Byte(0XA0+((WriteAddr/256)<<1)); 
	I2C_Wait_Ack();	   
    I2C_Send_Byte(WriteAddr%256);
	I2C_Wait_Ack(); 	 										  		   
	I2C_Send_Byte(DataToWrite);   
	I2C_Wait_Ack();  		    	   
  I2C_Stop();
	Delay_ms(5);	 
}

void AT24CXX_WriteLenByte(int WriteAddr,int DataToWrite,unsigned char Len)
{  	
	unsigned char t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

int AT24CXX_ReadLenByte(int ReadAddr,unsigned char Len)
{  	
	unsigned char t;
	int temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

void AT24CXX_Read(int ReadAddr,unsigned char *pBuffer,int NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  

void AT24CXX_Write(int WriteAddr,unsigned char *pBuffer,int NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

#define FREQ_U_RX_ADDR 								100
#define FREQ_TX_ADDR 									108
#define SUBVOICE_U_RX_ADDR 						116
#define SUBVOICE_TX_ADDR 							117
#define FREQ_V_RX_ADDR 								118
#define SUBVOICE_V_RX_ADDR 						127
#define GBW_ADDR 		   								128
#define TX_POWER_ADDR 								129
#define MIC_LEVEL_ADDR 								130
#define SCRAM_LEVEL_ADDR 							131
#define U_CURRENT_CHANNEL_ADDR				132
#define V_CURRENT_CHANNEL_ADDR				133
#define BACKLIGHTNESS_ADDR  					134
#define CONTRAST_RATIO_ADDR  					135
#define FM_RADIO_FREQ_ADDR 		   			136
#define SQ_ADDR 		   								141
#define TOT_ADDR 		   								142
#define FM_VOLUME_ADDR 		   					143
#define OVER_VOLUME_ADDR 		   				144
#define CURRENT_NICKNAME_ADDR 		   	145
//145+8

#define U_CHANNEL_ADDR								1000

#define V_CHANNEL_ADDR								6000

#define FLAG_BACKLIGHT_ADDR						5000
#define FLAG_REMOTE_EXTI_ADDR  				5001
#define FLAG_REMOTE_EXTI_CLEAR_ADDR  	5002
#define FLAG_KDU_REMOTE_ADDR  				5003
#define FLAG_VFO_ADDR  								5004
#define FLAG_VFO_CLEAR_ADDR  					5005
#define FLAG_VFO_SUBMIT_ADDR  				5006
#define FLAG_VFO_SUBMIT_CLEAR_ADDR  	5007
#define FLAG_RT_VFO_ADDR  						5008
#define FLAG_RT_VFO_CLEAR_ADDR  			5009
#define FLAG_RT_VFO_SELECT_ADDR  			5010
#define FLAG_F3_ADDR  								5011
#define FLAG_F4_SHORT_ADDR  					5012
//#define FLAG_A20_SET_ADDR  						5013
#define FLAG_VU_SWITCH_ADDR  					5014
#define FLAG_CF_SWITCH_ADDR  					5015
#define FM_UV_ADDR  									5016
#define FLAG_FM_SUBMIT_ADDR  					5017
#define FLAG_FM_SUBMIT_CLEAR_ADDR  		5018
#define FLAG_ENCODER_SW_ADDR 		   		5019
#define FLAG_WFMMOD_ADDR 		   				5020
#define FLAG_TRF_ADDR 		   					5021
//发射频率存储
void Freq_TX_Save(float tx_freq)								
{
	unsigned char freq_save[8];
	if(140.0<=tx_freq<=170.0||400.0<=tx_freq<=470.0)
	{
		sprintf((char*)freq_save,"%3.4f",tx_freq);
		AT24CXX_Write(FREQ_TX_ADDR,freq_save,8);
	}
	else 
		AT24CXX_Write(FREQ_TX_ADDR,(unsigned char*)"435.025",8);
}

//发射频率载入
float Freq_TX_Load()														
{
	float tx_freq=0;
	unsigned char freq_load[8];
	AT24CXX_Read(FREQ_TX_ADDR,freq_load,8);
	tx_freq=(freq_load[0]-'0')*100+(freq_load[1]-'0')*10+(freq_load[2]-'0');
	tx_freq+=(freq_load[4]-'0')*0.1+(freq_load[5]-'0')*0.01+(freq_load[6]-'0')*0.001+(freq_load[7]-'0')*0.0001;
	if(140.0<=tx_freq<=170.0||400.0<=tx_freq<=470.0)
		return tx_freq;
	else return 435.025;
}


//发射亚音存储
void SubVoice_TX_Save(unsigned char subvoice)		//发射亚音存储
{
	if(0<subvoice||subvoice<=122)
		AT24CXX_WriteOneByte(SUBVOICE_TX_ADDR,subvoice);
	else AT24CXX_WriteOneByte(SUBVOICE_TX_ADDR,0);
}

//发射亚音载入
unsigned char SubVoice_TX_Load()
{
	int result=AT24CXX_ReadOneByte(SUBVOICE_TX_ADDR);
	if(0>result||result>122)
	{
		AT24CXX_WriteOneByte(SUBVOICE_TX_ADDR,1);
		return 1;
	}
	return AT24CXX_ReadOneByte(SUBVOICE_TX_ADDR);
}
//

//U收频率存储
void Freq_U_RX_Save(float rx_freq)							
{
	unsigned char freq_save[8];
	if(400.0<=rx_freq<=470.0)
	{
		sprintf((char*)freq_save,"%3.4f",rx_freq);
		AT24CXX_Write(FREQ_U_RX_ADDR,freq_save,8);
	}
	else
		AT24CXX_Write(FREQ_U_RX_ADDR,(unsigned char*)"435.025",8);
}
//V收频率存储
void Freq_V_RX_Save(float rx_freq)							
{
	unsigned char freq_save[8];
	if(140.0<=rx_freq<=170.0)
	{
		sprintf((char*)freq_save,"%3.4f",rx_freq);
		AT24CXX_Write(FREQ_V_RX_ADDR,freq_save,8);
	}
	else 
		AT24CXX_Write(FREQ_V_RX_ADDR,(unsigned char*)"145.550",8);
}

//
//U收频率载入
float Freq_U_RX_Load()
{
	float rx_freq=0;
	unsigned char freq_load[8];
	AT24CXX_Read(FREQ_U_RX_ADDR,freq_load,8);
	rx_freq=(freq_load[0]-'0')*100+(freq_load[1]-'0')*10+(freq_load[2]-'0');
	rx_freq+=(freq_load[4]-'0')*0.1+(freq_load[5]-'0')*0.01+(freq_load[6]-'0')*0.001+(freq_load[7]-'0')*0.0001;
	if(400.0<=rx_freq<=470.0)
		return rx_freq;
	else
	{
		Freq_U_RX_Save(435.025);
		return 435.025;
	}
}

//V收频率载入
float Freq_V_RX_Load()
{
	float rx_freq=0;
	unsigned char freq_load[8];
	AT24CXX_Read(FREQ_V_RX_ADDR,freq_load,8);
	rx_freq=(freq_load[0]-'0')*100+(freq_load[1]-'0')*10+(freq_load[2]-'0');
	rx_freq+=(freq_load[4]-'0')*0.1+(freq_load[5]-'0')*0.01+(freq_load[6]-'0')*0.001+(freq_load[7]-'0')*0.0001;
	if(140.0<=rx_freq<=170.0)
		return rx_freq;
	else
	{
		Freq_V_RX_Save(145.550);
		return 145.550;
	}
}
//

//U收亚音存储
void SubVoice_U_RX_Save(unsigned char subvoice) 
{
	if(0<subvoice||subvoice<=122)
		AT24CXX_WriteOneByte(SUBVOICE_U_RX_ADDR,subvoice);
	else AT24CXX_WriteOneByte(SUBVOICE_U_RX_ADDR,0);
}
//V收亚音存储
void SubVoice_V_RX_Save(unsigned char subvoice) 
{
	if(0<subvoice||subvoice<=122)
		AT24CXX_WriteOneByte(SUBVOICE_V_RX_ADDR,subvoice);
	else AT24CXX_WriteOneByte(SUBVOICE_V_RX_ADDR,0);
}


//

//U接收亚音载入
unsigned char SubVoice_U_RX_Load()
{
	int result=AT24CXX_ReadOneByte(SUBVOICE_U_RX_ADDR);
	if(0>result||result>122)
	{
		AT24CXX_WriteOneByte(SUBVOICE_U_RX_ADDR,0);
		return 0;
	}
	return 
		AT24CXX_ReadOneByte(SUBVOICE_U_RX_ADDR);
}

//V接收亚音载入
unsigned char SubVoice_V_RX_Load()
{
	int result=AT24CXX_ReadOneByte(SUBVOICE_V_RX_ADDR);
	if(0>result||result>122)
	{
		AT24CXX_WriteOneByte(SUBVOICE_V_RX_ADDR,0);
		return 0;
	}
	return 
		AT24CXX_ReadOneByte(SUBVOICE_V_RX_ADDR);
}


void V_Channel_Save(unsigned char channel,float rx_freq,float tx_freq,unsigned char rx_subvoice,unsigned char tx_subvoice,
										unsigned char power,unsigned char gbw,unsigned char nn[])
{
	unsigned char channel_save[16];
	if(1>channel&&channel>100)				//	想改||
		channel=1;
	AT24CXX_WriteOneByte(V_CHANNEL_ADDR+channel*30,channel);
	if(140.0>rx_freq||rx_freq>170.0)
		rx_freq=145.55;
	if(140.0>tx_freq||tx_freq>170.0)
		tx_freq=145.55;
	sprintf((char*)channel_save,"%3.4f%3.4f",rx_freq,tx_freq);
	AT24CXX_Write(V_CHANNEL_ADDR+channel*30+1,channel_save,16);
	if(0>rx_subvoice||rx_subvoice>121)
		rx_subvoice=0;
	AT24CXX_WriteOneByte(V_CHANNEL_ADDR+channel*30+17,rx_subvoice);
	if(0>tx_subvoice||tx_subvoice>121)
		tx_subvoice=0;	
	AT24CXX_WriteOneByte(V_CHANNEL_ADDR+channel*30+18,tx_subvoice);
	if(power!=0&&power!=1)
		power=1;
	AT24CXX_WriteOneByte(V_CHANNEL_ADDR+channel*30+19,power);
	if(gbw!=0&&gbw!=1)
		gbw=1;
	AT24CXX_WriteOneByte(V_CHANNEL_ADDR+channel*30+20,gbw);
	AT24CXX_Write(V_CHANNEL_ADDR+channel*30+21,nn,7);
}

void V_Channel_Load(unsigned char channel,float* rx_freq,float* tx_freq,unsigned char* rx_subvoice,unsigned char* tx_subvoice,
										unsigned char* power,unsigned char *gbw,unsigned char nn[8])
{
	unsigned char freq_load[21];
	AT24CXX_Read(V_CHANNEL_ADDR+channel*30+1,freq_load,19);
	*rx_freq=(freq_load[0]-'0')*100+(freq_load[1]-'0')*10+(freq_load[2]-'0');
	*rx_freq+=(freq_load[4]-'0')*0.1+(freq_load[5]-'0')*0.01+(freq_load[6]-'0')*0.001+(freq_load[7]-'0')*0.0001;
	if(140.0>*rx_freq||*rx_freq>170.0)
		*rx_freq=145.55;
	*tx_freq=(freq_load[8]-'0')*100+(freq_load[9]-'0')*10+(freq_load[10]-'0');
	*tx_freq+=(freq_load[12]-'0')*0.1+(freq_load[13]-'0')*0.01+(freq_load[14]-'0')*0.001+(freq_load[15]-'0')*0.0001;
		if(140.0>*tx_freq||*tx_freq>170.0)
		*tx_freq=145.55;
	*rx_subvoice=freq_load[16];
	if(0>*rx_subvoice||*rx_subvoice>121)
		*rx_subvoice=0;
	*tx_subvoice=freq_load[17];
	if(0>*tx_subvoice||*tx_subvoice>121)
		*tx_subvoice=0;
	*power=freq_load[18];
	if(*power!=1&&*power!=0)
		*power=1;
	*gbw=freq_load[19];
	if(*gbw!=1&&*gbw!=0)
		*gbw=1;
	AT24CXX_Read(V_CHANNEL_ADDR+channel*30+20,nn,7);
}

void U_Channel_Save(unsigned char channel,float rx_freq,float tx_freq,unsigned char rx_subvoice,unsigned char tx_subvoice,
										unsigned char power,unsigned char gbw,unsigned char nn[])
{
	unsigned char channel_save[16];
	if(1>channel&&channel>100)
		channel=1;
	AT24CXX_WriteOneByte(U_CHANNEL_ADDR+channel*30,channel);
	if(400.0>rx_freq||rx_freq>470.0)
		rx_freq=435.025;
	if(400.0>tx_freq||tx_freq>470.0)
		tx_freq=435.025;
	sprintf((char*)channel_save,"%3.4f%3.4f",rx_freq,tx_freq);
	AT24CXX_Write(U_CHANNEL_ADDR+channel*30+1,channel_save,16);
	if(0>rx_subvoice||rx_subvoice>121)
		rx_subvoice=0;
	AT24CXX_WriteOneByte(U_CHANNEL_ADDR+channel*30+17,rx_subvoice);
	if(0>tx_subvoice||tx_subvoice>121)
		tx_subvoice=0;	
	AT24CXX_WriteOneByte(U_CHANNEL_ADDR+channel*30+18,tx_subvoice);
	if(power!=0&&power!=1)
		power=1;
	AT24CXX_WriteOneByte(U_CHANNEL_ADDR+channel*30+19,power);
	if(gbw!=0&&gbw!=1)
		gbw=1;
	AT24CXX_WriteOneByte(U_CHANNEL_ADDR+channel*30+20,gbw);
	AT24CXX_Write(U_CHANNEL_ADDR+channel*30+21,nn,7);
}

void U_Channel_Load(unsigned char channel,float* rx_freq,float* tx_freq,unsigned char* rx_subvoice,unsigned char* tx_subvoice,
										unsigned char* power,unsigned char *gbw,unsigned char nn[8])
{
	unsigned char freq_load[30];
	AT24CXX_Read(U_CHANNEL_ADDR+channel*30+1,freq_load,19);
	*rx_freq=(freq_load[0]-'0')*100+(freq_load[1]-'0')*10+(freq_load[2]-'0');
	*rx_freq+=(freq_load[4]-'0')*0.1+(freq_load[5]-'0')*0.01+(freq_load[6]-'0')*0.001+(freq_load[7]-'0')*0.0001;
	if(400.0>*rx_freq||*rx_freq>470.0)
		*rx_freq=435.025;
	*tx_freq=(freq_load[8]-'0')*100+(freq_load[9]-'0')*10+(freq_load[10]-'0');
	*tx_freq+=(freq_load[12]-'0')*0.1+(freq_load[13]-'0')*0.01+(freq_load[14]-'0')*0.001+(freq_load[15]-'0')*0.0001;
		if(400.0>*tx_freq||*tx_freq>470.0)
		*tx_freq=435.025;
	*rx_subvoice=freq_load[16];
	if(0>*rx_subvoice||*rx_subvoice>121)
		*rx_subvoice=0;
	*tx_subvoice=freq_load[17];
	if(0>*tx_subvoice||*tx_subvoice>121)
		*tx_subvoice=0;
	*power=freq_load[18];
	if(*power!=1&&*power!=0)
		*power=1;
	*gbw=freq_load[19];
	if(*gbw!=1&&*gbw!=0)
		*gbw=1;
	AT24CXX_Read(U_CHANNEL_ADDR+channel*30+20,nn,7);
}

//发射功率
void TX_Power_Save(unsigned char value)
{
	if(value)
		AT24CXX_WriteOneByte(TX_POWER_ADDR,1);
	else if(value==0)
			AT24CXX_WriteOneByte(TX_POWER_ADDR,0);
}

unsigned char TX_Power_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(TX_POWER_ADDR);
	if(result)
		return 1;
	else if(result>1||result<0)
	{
		AT24CXX_WriteOneByte(TX_POWER_ADDR,1);
		return 1;
	}
	else return 0;
}

//
//宽窄频
void GBW_Save(unsigned char value)
{
	if(value)
		AT24CXX_WriteOneByte(GBW_ADDR,1);
	else if(value==0)
			AT24CXX_WriteOneByte(GBW_ADDR,0);
}

unsigned char GBW_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(GBW_ADDR);
	if(result!=0&&result!=1)
	{
		AT24CXX_WriteOneByte(GBW_ADDR,1);
		return 1;
	}
	else return result;
}

//
//静噪
void SQL_Save(unsigned char value)
{
	if(0<=value&&value<=8)
		AT24CXX_WriteOneByte(SQ_ADDR,value);
	else
		AT24CXX_WriteOneByte(SQ_ADDR,1);
}

unsigned char SQL_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(SQ_ADDR);
	if(0<=result&&result<=8)
		return result;
	else 
	{
		AT24CXX_WriteOneByte(SQ_ADDR,1);
		return 1;
	}
}

//
//咪灵敏度
void MIC_LEVEL_Save(unsigned char value)
{
	if(1<=value&&value<=8)
		AT24CXX_WriteOneByte(MIC_LEVEL_ADDR,value);
	else 
		AT24CXX_WriteOneByte(MIC_LEVEL_ADDR,0);
}

unsigned char MIC_LEVEL_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(MIC_LEVEL_ADDR);
	if(1<=result&&result<=8)
		return result;
	else 
	{
		AT24CXX_WriteOneByte(MIC_LEVEL_ADDR,8);
		return 8;
	}
}

//
//加密
void SCRAM_LEVEL_Save(unsigned char value)
{
	if(0<=value&&value<=8)
		AT24CXX_WriteOneByte(SCRAM_LEVEL_ADDR,value);
	else AT24CXX_WriteOneByte(SCRAM_LEVEL_ADDR,0);
}

unsigned char SCRAM_LEVEL_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(SCRAM_LEVEL_ADDR);
	if(0<=result&&result<=8)
		return result;
	else 
	{
		AT24CXX_WriteOneByte(SCRAM_LEVEL_ADDR,0);
		return 0;
	}
}

//
//发射延时
void TOT_Save(unsigned char value)
{
	if(0<=value&&value<=9)
		AT24CXX_WriteOneByte(TOT_ADDR,value);
	else 
		AT24CXX_WriteOneByte(TOT_ADDR,0);
}

unsigned char TOT_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(TOT_ADDR);
	if(0<=result&&result<=9)
		return result;
	else 
	{
		AT24CXX_WriteOneByte(TOT_ADDR,0);
		return 0;
	}
}

//
//收音机音量
void FM_VOLUME_Save(unsigned char value)
{
	if(0<=value&&value<=15)
		AT24CXX_WriteOneByte(FM_VOLUME_ADDR,value);
}

unsigned char FM_VOLUME_Load()
{
	return AT24CXX_ReadOneByte(FM_VOLUME_ADDR);
}


void OVER_VOLUME_Save(unsigned char value)
{
	if(0<=value&&value<=15)
		AT24CXX_WriteOneByte(FM_VOLUME_ADDR,value);
	else 
		AT24CXX_WriteOneByte(FM_VOLUME_ADDR,1);
}

unsigned char OVER_VOLUME_Load()
{	
	unsigned char result=AT24CXX_ReadOneByte(FM_VOLUME_ADDR);
	if(0<=result&&result<=15)
	return result;
	else 
	{
		AT24CXX_WriteOneByte(FM_VOLUME_ADDR,1);
		return 1;
	}
}


void U_Current_Channel_Save(unsigned char value)
{
	if(1<=value&&value<=99)
		AT24CXX_WriteOneByte(U_CURRENT_CHANNEL_ADDR,value);
	else 
		AT24CXX_WriteOneByte(U_CURRENT_CHANNEL_ADDR,1);
}

unsigned char U_Current_Channel_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(U_CURRENT_CHANNEL_ADDR);
	if(1<=result&&result<=99)
		return result;
	else 
	{
		AT24CXX_WriteOneByte(U_CURRENT_CHANNEL_ADDR,1);
		return 1;
	}
}

void V_Current_Channel_Save(unsigned char value)
{
	if(1<=value&&value<=99)
		AT24CXX_WriteOneByte(V_CURRENT_CHANNEL_ADDR,value);
	else AT24CXX_WriteOneByte(V_CURRENT_CHANNEL_ADDR,1);
}

unsigned char V_Current_Channel_Load()
{
	unsigned char result=AT24CXX_ReadOneByte(V_CURRENT_CHANNEL_ADDR);
	if(1<=result&&result<=99)
		return result;
	else 
	{
		AT24CXX_WriteOneByte(V_CURRENT_CHANNEL_ADDR,1);
		return 1;
	}
}

//
//别名
void Current_Nickname_Save(unsigned char nn[7])
{
	AT24CXX_Write(CURRENT_NICKNAME_ADDR,nn,7);
}

unsigned char* Current_Nickname_Load()
{
	unsigned char nn[7];
	AT24CXX_Read(CURRENT_NICKNAME_ADDR,nn,7);
	return nn;
}

//
//修改标志位
void Flag_SET(int flag_number,unsigned char value)
{
	if(value)
		AT24CXX_WriteOneByte(flag_number,1);
	else if(value==0)
			AT24CXX_WriteOneByte(flag_number,0);
}

//读取标志位
unsigned char Flag_GET(int flag_number)
{
	return AT24CXX_ReadOneByte(flag_number);
}

//
//
void FM_UV_Mode_Save(unsigned char value)
{
	if(1<=value&&value<=7)
		AT24CXX_WriteOneByte(FM_UV_ADDR,value);
	else 
		AT24CXX_WriteOneByte(FM_UV_ADDR,4);
}

unsigned char FM_UV_Mode_Load()
{
	return AT24CXX_ReadOneByte(FM_UV_ADDR);
}

//背光
void Backlightness_Save(unsigned char value)
{
	if(0<=value&&value<=100)
		AT24CXX_WriteOneByte(BACKLIGHTNESS_ADDR,value);
	else 
		AT24CXX_WriteOneByte(BACKLIGHTNESS_ADDR,100);
}

unsigned char Backlightness_Load()
{
	return AT24CXX_ReadOneByte(BACKLIGHTNESS_ADDR);
}

//
//背景对比度
void Contrast_Ratio_Save(unsigned char value)
{
	if(2<=value&&value<=4)
		AT24CXX_WriteOneByte(CONTRAST_RATIO_ADDR,value);
	else 
		AT24CXX_WriteOneByte(CONTRAST_RATIO_ADDR,3);
}

unsigned char Contrast_Ratio_Load()
{
	return AT24CXX_ReadOneByte(CONTRAST_RATIO_ADDR);
}

//
//收音机频率
void FM_Radio_Freq_Save(int fm_freq)
//870~1080
{
	unsigned char freq_save[4];
	freq_save[0]=(int)(fm_freq)/1000%10;
	freq_save[1]=(int)(fm_freq)/100%10;
	freq_save[2]=(int)(fm_freq)/10%10;
	freq_save[3]=(int)(fm_freq)%10;
	AT24CXX_Write(FM_RADIO_FREQ_ADDR,freq_save,4);
}

int FM_Radio_Freq_Load()
{
	int fm_freq=0;
	unsigned char freq_load[4];
	AT24CXX_Read(FM_RADIO_FREQ_ADDR,freq_load,4);
	fm_freq=(freq_load[0])*1000+(freq_load[1])*100+(freq_load[2])*10+freq_load[3];
	return fm_freq;
}

#endif
