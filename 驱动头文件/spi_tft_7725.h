#include "stm32f1xx.h"
#include "delay.h"

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        160

#define RED  								0xf800
#define GREEN								0x07e0
#define BLUE 								0x001f
#define WHITE								0xffff
#define BLACK								0x0000
#define YELLOW  						0xFFE0
#define GRAY0   						0xEF7D
//��ɫ0 3165 00110 001011 00101
#define GRAY1   						0x8410 
//��ɫ1      00000 000000 00000
#define GRAY2   						0x4208

#define LCD_CTRL   	  			GPIOB

#define LCD_CS        			GPIO_PIN_11
#define LCD_RST     				GPIO_PIN_12
#define LCD_SCL        			GPIO_PIN_13
#define LCD_RS         			GPIO_PIN_14
#define LCD_SDA        			GPIO_PIN_15

#define	LCD_CS_SET  	LCD_CTRL->BSRR=LCD_CS
#define	LCD_CS_CLR  	LCD_CTRL->BRR=LCD_CS

#define	LCD_RS_SET  	LCD_CTRL->BSRR=LCD_RS
#define	LCD_RS_CLR  	LCD_CTRL->BRR=LCD_RS

#define	LCD_SDA_SET  	LCD_CTRL->BSRR=LCD_SDA
#define	LCD_SDA_CLR  	LCD_CTRL->BRR=LCD_SDA

#define	LCD_SCL_SET  	LCD_CTRL->BSRR=LCD_SCL
#define	LCD_SCL_CLR  	LCD_CTRL->BRR=LCD_SCL

#define	LCD_RST_SET  	LCD_CTRL->BSRR=LCD_RST    
#define	LCD_RST_CLR  	LCD_CTRL->BRR=LCD_RST

void LCD_GPIO_Init()
{
	RCC->APB2ENR|=0x08;
	LCD_CTRL->CRH&=0x00000fff;
	LCD_CTRL->CRH|=0x33333000;  
}

void SPI_WriteData(unsigned char Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
	  LCD_SDA_SET; //�������
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

//��Һ����дһ��8λָ��
void Lcd_WriteIndex(unsigned char Index)
{
   //SPI д����ʱ��ʼ
   LCD_CS_CLR;
   LCD_RS_CLR;
	 SPI_WriteData(Index);
   LCD_CS_SET;
}

//��Һ����дһ��8λ����
void Lcd_WriteData(unsigned char Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
   SPI_WriteData(Data);
   LCD_CS_SET; 
}
//��Һ����дһ��16λ����
void LCD_WriteData_16Bit(short Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
	 SPI_WriteData(Data>>8); 	//д���8λ����
	 SPI_WriteData(Data); 			//д���8λ����
   LCD_CS_SET; 
}

void Lcd_WriteReg(unsigned char Index,unsigned char Data)
{
	Lcd_WriteIndex(Index);
  Lcd_WriteData(Data);
}

void Lcd_Reset(void)
{
	LCD_RST_CLR;
	Delay_ms(100);
	LCD_RST_SET;
	Delay_ms(50);
}

void Lcd_Init(void)
{	
	LCD_GPIO_Init();
	Lcd_Reset();

	Lcd_WriteIndex(0x11);
	Delay_ms(120);
		
	Lcd_WriteIndex(0xB1);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3C);
	Lcd_WriteData(0x3C);
	Lcd_WriteIndex(0xB2);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3C);
	Lcd_WriteData(0x3C);
	Lcd_WriteIndex(0xB3);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3C);
	Lcd_WriteData(0x3C);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x3C);
	Lcd_WriteData(0x3C);
	//------------------------------------End ST7735S Frame Rate-----------------------------------------//
	Lcd_WriteIndex(0xB4); //Dot inversion
	Lcd_WriteData(0x03);
	Lcd_WriteIndex(0xC0);
	Lcd_WriteData(0x28);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x04);
	Lcd_WriteIndex(0xC1);
	Lcd_WriteData(0XC0);
	Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x0D);
	Lcd_WriteData(0x00);
	Lcd_WriteIndex(0xC3);
	Lcd_WriteData(0x8D);
	Lcd_WriteData(0x2A);
	Lcd_WriteIndex(0xC4);
	Lcd_WriteData(0x8D);
	Lcd_WriteData(0xEE);
	//---------------------------------End ST7735S Power Sequence-------------------------------------//
	Lcd_WriteIndex(0xC5); //VCOM
	Lcd_WriteData(0x1A);
	Lcd_WriteIndex(0x36); //MX, MY, RGB mode
	Lcd_WriteData(0xC0);
	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	Lcd_WriteIndex(0xE0);
	Lcd_WriteData(0x04);
	Lcd_WriteData(0x22);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x0A);
	Lcd_WriteData(0x2E);
	Lcd_WriteData(0x30);
	Lcd_WriteData(0x25);
	Lcd_WriteData(0x2A);
	Lcd_WriteData(0x28);
	Lcd_WriteData(0x26);
	Lcd_WriteData(0x2E);
	Lcd_WriteData(0x3A);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x13);
	Lcd_WriteIndex(0xE1);
	Lcd_WriteData(0x04);
	Lcd_WriteData(0x16);
	Lcd_WriteData(0x06);
	Lcd_WriteData(0x0D);
	Lcd_WriteData(0x2D);
	Lcd_WriteData(0x26);
	Lcd_WriteData(0x23);
	Lcd_WriteData(0x27);
	Lcd_WriteData(0x27);
	Lcd_WriteData(0x25);
	Lcd_WriteData(0x2D);
	Lcd_WriteData(0x3B);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x04);
	Lcd_WriteData(0x13);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
	Lcd_WriteIndex(0x3A); //65k mode
	Lcd_WriteData(0x05);
	Lcd_WriteIndex(0x29); //Display on
}


/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void Lcd_SetRegion(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{		
   Lcd_WriteIndex(0x2a);
   Lcd_WriteData(x1>>8);
   Lcd_WriteData(x1);
   Lcd_WriteData(x2>>8);
   Lcd_WriteData(x2);
  
   Lcd_WriteIndex(0x2b);
   Lcd_WriteData(y1>>8);
   Lcd_WriteData(y1);
   Lcd_WriteData(y2>>8);
   Lcd_WriteData(y2);

   Lcd_WriteIndex(0x2C);

}

/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(short x,short y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�������
����ֵ����
*************************************************/
void Gui_DrawPoint(short x,short y,short Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16Bit(Data);
}    

/*****************************************
 �������ܣ���TFTĳһ�����ɫ                          
 ���ڲ�����color  ����ɫֵ                                 
******************************************/
unsigned int Lcd_ReadPoint(short x,short y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//���������ֽ�
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}
/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(short Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteIndex(0x2C);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    }   
}
