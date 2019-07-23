#ifndef _SPI_TFT_20_H_
#define _SPI_TFT_20_H_
#include "stm32f1xx.h"
#include "delay.h"
#include "font.h"
#include "picture.h"

#define LCD_X_SIZE	        176
#define LCD_Y_SIZE	        220

#define USE_HORIZONTAL  		1	
//定义是否使用横屏 		0,不使用.1,使用.

#ifdef USE_HORIZONTAL
//如果定义了横屏 
#define X_MAX_PIXEL	        220
#define Y_MAX_PIXEL	        176
#else
#define X_MAX_PIXEL	        176
#define Y_MAX_PIXEL	        220
#endif

//VCC:可以接5V也可以接3.3V
//可以将LCD_CS接地，LCD_LED接3.3V，LCD_RST接至单片机复位端，

#define   BLACK         0x0000
#define   BLUE          0x001F
#define   GREEN         0x07E0
#define   CYAN          0x07FF
#define   RED           0xF800
#define   MAGENTA       0xF81F
#define   YELLOW        0xFFE0
#define   WHITE         0xFFFF
#define   NAVY          0x000F
#define   DGREEN        0x03E0
#define   DCYAN         0x03EF
#define   MAROON        0x7800
#define   PURPLE        0x780F
#define   OLIVE         0x7BE0
#define   LGRAY         0xC618
#define   DGRAY         0x7BEF
#define   GRAY1   			0x8410
#define   GRAY2   			0x4208

#define SPITFT20_CTRL_GPIO_CLOEN 	__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPITFT20_SPI_GPIO_CLOEN 	__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPITFT20_CTRL_GPIO				GPIOB
#define SPITFT20_SPI_GPIO   			GPIOB

#define SPITFT20_RS								GPIO_PIN_14	//PB11--->>TFT --RS/DC
#define SPITFT20_CS								GPIO_PIN_12 //MCU_PB11--->>TFT --CS/CE

#define SPITFT20_SCL							GPIO_PIN_13	//PB13--->>TFT --SCL/SCK
#define SPITFT20_SDA							GPIO_PIN_15	//PB15 MOSI--->>TFT --SDA/DIN

#define	SPITFT20_CS_SET  					SPITFT20_CTRL_GPIO->BSRR=SPITFT20_CS
#define SPITFT20_CS_CLR  					SPITFT20_CTRL_GPIO->BRR=SPITFT20_CS

#define	SPITFT20_RS_SET  					SPITFT20_CTRL_GPIO->BSRR=SPITFT20_RS
#define	SPITFT20_RS_CLR  					SPITFT20_CTRL_GPIO->BRR=SPITFT20_RS

uint8_t SPI2_WriteByte(uint8_t Byte)
{
	while((SPI2->SR&SPI_FLAG_TXE)==RESET);
	//等待发送区空	  
	SPI2->DR=Byte;
	while((SPI2->SR&SPI_FLAG_RXNE)==RESET);
	//等待接收完一个byte  
	return SPI2->DR;
}

void SPI2_Init()	
{
	SPI_HandleTypeDef hspi2;
	GPIO_InitTypeDef GPIO_InitStruct;
	 
	//配置SPI2管脚
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_SPI2_CLK_ENABLE();
	SPITFT20_CTRL_GPIO_CLOEN;
	SPITFT20_SPI_GPIO_CLOEN;
	
	GPIO_InitStruct.Pin=SPITFT20_SCL|SPITFT20_SDA;
  GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPITFT20_SPI_GPIO,&GPIO_InitStruct);

	GPIO_InitStruct.Pin=SPITFT20_RS|SPITFT20_CS;
  GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SPITFT20_CTRL_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=GPIO_PIN_1;
  GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
  hspi2.Instance=SPI2;
  hspi2.Init.Mode=SPI_MODE_MASTER;
  hspi2.Init.Direction=SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize=SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity=SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase=SPI_PHASE_2EDGE;
  hspi2.Init.NSS=SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit=SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode=SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial=10;
  HAL_SPI_Init(&hspi2);
	__HAL_SPI_ENABLE(&hspi2);

}

void SPITFT20_WriteIndex(uint8_t index)
{
   SPITFT20_CS_CLR;
   SPITFT20_RS_CLR;  
   SPI2_WriteByte(index);
   SPITFT20_CS_SET;
}

void SPITFT20_WriteData(uint8_t data)
{
   SPITFT20_CS_CLR;
   SPITFT20_RS_SET;
   SPI2_WriteByte(data);
   SPITFT20_CS_SET;
}

void SPITFT20_WriteReg(uint8_t index,uint16_t data)
{
	SPITFT20_WriteIndex(index);
	SPITFT20_WriteData(data>>8);
	SPITFT20_WriteData(data);	
}

void SPITFT20_Init()
{	
	SPI2_Init();
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
	Delay_ms(100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
	Delay_ms(100);
	
	//LCD Init For 2.2inch LCD Panel with ILI9225.	
	SPITFT20_WriteReg(0x10, 0x0000); // Set SAP,DSTB,STB
	SPITFT20_WriteReg(0x11, 0x0000); // Set APON,PON,AON,VCI1EN,VC
	SPITFT20_WriteReg(0x12, 0x0000); // Set BT,DC1,DC2,DC3
	SPITFT20_WriteReg(0x13, 0x0000); // Set GVDD
	SPITFT20_WriteReg(0x14, 0x0000); // Set VCOMH/VCOML voltage
	Delay_ms(40); // Delay 20 ms
	
	// Please follow this power on sequence
	SPITFT20_WriteReg(0x11, 0x0018); // Set APON,PON,AON,VCI1EN,VC
	SPITFT20_WriteReg(0x12, 0x1121); // Set BT,DC1,DC2,DC3
	SPITFT20_WriteReg(0x13, 0x0063); // Set GVDD
	SPITFT20_WriteReg(0x14, 0x3961); // Set VCOMH/VCOML voltage
	SPITFT20_WriteReg(0x10, 0x0800); // Set SAP,DSTB,STB
	Delay_ms(10); // Delay 10 ms
	SPITFT20_WriteReg(0x11, 0x1038); // Set APON,PON,AON,VCI1EN,VC
	Delay_ms(30); // Delay 30 ms
	
	
	SPITFT20_WriteReg(0x02, 0x0100); // set 1 line inversion

#if USE_HORIZONTAL//如果定义了横屏
	//R01H:SM=0,GS=0,SS=0 (for details,See the datasheet of ILI9225)
	SPITFT20_WriteReg(0x01, 0x031C); // set the display line number and display direction
	//R03H:BGR=1,ID0=1,ID1=1,AM=1 (for details,See the datasheet of ILI9225)
	SPITFT20_WriteReg(0x03, 0x1038); // set GRAM write direction .
#else//竖屏
	//R01H:SM=0,GS=0,SS=1 (for details,See the datasheet of ILI9225)
	SPITFT20_WriteReg(0x01, 0x011C); // set the display line number and display direction 
	//R03H:BGR=1,ID0=1,ID1=1,AM=0 (for details,See the datasheet of ILI9225)
	SPITFT20_WriteReg(0x03, 0x1030); // set GRAM write direction.
#endif

	SPITFT20_WriteReg(0x07, 0x0000); // Display off
	SPITFT20_WriteReg(0x08, 0x0808); // set the back porch and front porch
	SPITFT20_WriteReg(0x0B, 0x1100); // set the clocks number per line
	SPITFT20_WriteReg(0x0C, 0x0000); // CPU interface
	SPITFT20_WriteReg(0x0F, 0x0501); // Set Osc
	SPITFT20_WriteReg(0x15, 0x0020); // Set VCI recycling
	SPITFT20_WriteReg(0x20, 0x0000); // RAM Address
	SPITFT20_WriteReg(0x21, 0x0000); // RAM Address
	
	//------------------------ Set GRAM area --------------------------------//
	SPITFT20_WriteReg(0x30, 0x0000); 
	SPITFT20_WriteReg(0x31, 0x00DB); 
	SPITFT20_WriteReg(0x32, 0x0000); 
	SPITFT20_WriteReg(0x33, 0x0000); 
	SPITFT20_WriteReg(0x34, 0x00DB); 
	SPITFT20_WriteReg(0x35, 0x0000); 
	SPITFT20_WriteReg(0x36, 0x00AF); 
	SPITFT20_WriteReg(0x37, 0x0000); 
	SPITFT20_WriteReg(0x38, 0x00DB); 
	SPITFT20_WriteReg(0x39, 0x0000); 
	
	
	// ---------- Adjust the Gamma 2.2 Curve -------------------//
	SPITFT20_WriteReg(0x50, 0x0603); 
	SPITFT20_WriteReg(0x51, 0x080D); 
	SPITFT20_WriteReg(0x52, 0x0D0C); 
	SPITFT20_WriteReg(0x53, 0x0205); 
	SPITFT20_WriteReg(0x54, 0x040A); 
	SPITFT20_WriteReg(0x55, 0x0703); 
	SPITFT20_WriteReg(0x56, 0x0300); 
	SPITFT20_WriteReg(0x57, 0x0400); 
	SPITFT20_WriteReg(0x58, 0x0B00); 
	SPITFT20_WriteReg(0x59, 0x0017); 
	
	
	
	SPITFT20_WriteReg(0x0F, 0x0701); // Vertical RAM Address Position
	SPITFT20_WriteReg(0x07, 0x0012); // Vertical RAM Address Position
	Delay_ms(50);
	SPITFT20_WriteReg(0x07, 0x1017); // Vertical RAM Address Position  
	
}

void Lcd_SetXY(uint16_t Xpos, uint16_t Ypos)
{	
#if USE_HORIZONTAL//如果定义了横屏  	    	
	SPITFT20_WriteReg(0x21,Xpos);
	SPITFT20_WriteReg(0x20,Ypos);
#else//竖屏	
	SPITFT20_WriteReg(0x20,Xpos);
	SPITFT20_WriteReg(0x21,Ypos);
#endif
	SPITFT20_WriteIndex(0x22);		
} 

//设置显示窗口
void Lcd_SetRegion(uint8_t xStar, uint8_t yStar,uint8_t xEnd,uint8_t yEnd)
{
#if USE_HORIZONTAL//如果定义了横屏	
	SPITFT20_WriteReg(0x38,xEnd);
	SPITFT20_WriteReg(0x39,xStar);
	SPITFT20_WriteReg(0x36,yEnd);
	SPITFT20_WriteReg(0x37,yStar);
	SPITFT20_WriteReg(0x21,xStar);
	SPITFT20_WriteReg(0x20,yStar);
#else//竖屏	
	SPITFT20_WriteReg(0x36,xEnd);
	SPITFT20_WriteReg(0x37,xStar);
	SPITFT20_WriteReg(0x38,yEnd);
	SPITFT20_WriteReg(0x39,yStar);
	SPITFT20_WriteReg(0x20,xStar);
	SPITFT20_WriteReg(0x21,yStar);
#endif
	SPITFT20_WriteIndex(0x22);	
}

void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetXY(x,y);
	SPITFT20_WriteData(Data>>8);
	SPITFT20_WriteData(Data);

}    

void Lcd_Clear(uint16_t Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
			SPITFT20_WriteData(Color>>8);
			SPITFT20_WriteData(Color);
    }   
}

uint16_t LCD_BGR2RGB(uint16_t c)
{
  uint16_t  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}

void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc) 
{
//Bresenham算法 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//画线函数，使用Bresenham 画线算法
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color)   
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	


	Lcd_SetXY(x0,y0);
	dx=x1-x0;//计算x距离
	dy=y1-y0;//计算y距离

	if (dx>=0)
	{
		x_inc=1;
	}
	else
	{
		x_inc=-1;
		dx   =-dx;  
	} 
	
	if (dy>=0)
	{
		y_inc=1;
	} 
	else
	{
		y_inc=-1;
		dy   =-dy; 
	} 

	dx2=dx << 1;
	dy2=dy << 1;

	if (dx > dy)//x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
	{//且线的点数等于x距离，以x轴递增画点
		// initialize error term
		error=dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//要画的点数不会超过x距离
		{
			//画点
			Gui_DrawPoint(x0,y0,Color);
			
			// test if error has overflowed
			if (error >= 0) //是否需要增加y坐标值
			{
				error-=dx2;

				// move to next line
				y0+=y_inc;//增加y坐标值
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;//x坐标值每次画点后都递增1
		} // end for
	} // end if |slope| <= 1
	else//y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
	{//以y轴为递增画点
		// initialize error term
		error=dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			Gui_DrawPoint(x0,y0,Color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}



void Gui_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t bc)
{
	Gui_DrawLine(x,y,x+w,y,0xEF7D);
	Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
	Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void Gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode)
{
	if (mode==0)	{
		Gui_DrawLine(x,y,x+w,y,0xEF7D);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
		Gui_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		Gui_DrawLine(x,y,x+w,y,0x2965);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		Gui_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		Gui_DrawLine(x,y,x+w,y,0xffff);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
		Gui_DrawLine(x,y,x,y+h,0xffff);
	}
}



//功能描述: 在屏幕显示一凸起的按钮框
void DisplayButtonDown(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1,GRAY2);  //H
	Gui_DrawLine(x1+1,y1+1,x2,y1+1,GRAY1);  //H
	Gui_DrawLine(x1,  y1,  x1,y2,GRAY2);  //V
	Gui_DrawLine(x1+1,y1+1,x1+1,y2,GRAY1);  //V
	Gui_DrawLine(x1,  y2,  x2,y2,WHITE);  //H
	Gui_DrawLine(x2,  y1,  x2,y2,WHITE);  //V
}


//功能描述: 在屏幕显示一凹下的按钮框
void DisplayButtonUp(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
	Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V
	
	Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
	Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
	Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}

void Gui_DrawOneFont_GBK16(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,uint8_t ch)
{
	uint8_t i,j;
	unsigned short k,x0;
	x0=x;

	if (ch==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (ch>32) ch-=32; else ch=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[ch*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
						}
					}
				x+=8;
			}
}

void Gui_DrawFont_GBK16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
	uint8_t i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
						}
					}
				x+=8;
			}
			s++;
		}
			
		else 
		{
		

			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		} 
		
	}
}

void Gui_DrawFont_GBK24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
	uint8_t i,j;
	unsigned short k;

	while(*s) 
	{
		if( *s < 0x80 ) 
		{
			k=*s;
			if (k>32) k-=32; else k=0;

		    for(i=0;i<16;i++)
			for(j=0;j<8;j++) 
				{
			    	if(asc16[k*16+i]&(0x80>>j))	
					Gui_DrawPoint(x+j,y+i,fc);
					else 
					{
						if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
					}
				}
			s++;x+=8;
		}
		else 
		{

			for (k=0;k<hz24_num;k++) 
			{
			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<24;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3]&(0x80>>j))
								Gui_DrawPoint(x+j,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+2]&(0x80>>j))	
								Gui_DrawPoint(x+j+16,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
								}
							}
				    }
			  }
			}
			s+=2;x+=24;
		}
	}
}
void Gui_DrawFont_Num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num)
{
	uint8_t i,j,k,c;

    for(i=0;i<32;i++)
	{
		for(j=0;j<4;j++) 
		{
			c=*(sz32+num*32*4+i*4+j);
			for (k=0;k<8;k++)	
			{
	
		    	if(c&(0x80>>k))	Gui_DrawPoint(x+j*8+k,y+i,fc);
				else {
					if (fc!=bc) Gui_DrawPoint(x+j*8+k,y+i,bc);
				}
			}
		}
	}
}

#endif
