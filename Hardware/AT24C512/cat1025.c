#include"config.h"
#include "main.H"
#include "cat1025.h"
#include "LPC177x_8x.h"


#define SDA_1_H LPC_GPIO0->SET|=(1<<19)
#define SDA_1_L LPC_GPIO0->CLR|=(1<<19)

#define SCL_1_H LPC_GPIO0->SET|=(1<<20)
#define SCL_1_L LPC_GPIO0->CLR|=(1<<20)

#define  SDA        ((LPC_GPIO0->PIN>>19)&0x01) 
#define  SDA_OUT    LPC_GPIO0->DIR |=  (1u<<19)   
#define  SDA_IN     LPC_GPIO0->DIR &= ~(1u<<19)  

uint8_t ack;

void dlus()
{
	uint8_t i; 
	for(i=0;i<2;i++){;}
}
/*************************************************************************************************************
*函数名称：delay_nms()
*参   数 ：uint16_t n   延时N个毫秒
*返回值  : void
*功能描述：软件延时N个毫秒，外部调用
**************************************************************************************************************/
void DAC_delay_nms(uint16_t n) 
{
	uint16_t  a,b;
	for (a=0;a<n;a++)
	{
		for (b=0;b<8000;b++);
	}
}

/*************************************************************************************************************
*函数名称：delay_nus()
*参   数 ：uint16_t n   延时N个微秒
*返回值  : void
*功能描述：软件延时N个微秒，外部调用
**************************************************************************************************************/
void DAC_delay_nus(uint16_t n) 
{
	uint16_t  a,b;
	for (a=0;a<n;a++)
	{
		for (b=0;b<8;b++);
	}
}

//=====================   I2C存储器读写   ======================
void I2C_star()
{
SDA_1_H;
SCL_1_H;
DAC_delay_nms(1);
SDA_1_L;
DAC_delay_nms(1);
SCL_1_L;
DAC_delay_nms(1);
}

void I2C_stop()
{
	SDA_1_L;
	DAC_delay_nms(1);
	SCL_1_H;
	DAC_delay_nms(1);
	SDA_1_H;
	DAC_delay_nms(1);
}

//-------------  写单字节  -------------
void I2C_wbyte(uint8_t by)
{
	uint8_t i,j;
	j=by;ack=0;
	for(i=0;i<8;i++)
	{
		if((j&0x80)!=0){SDA_1_H;}
  	else if((j&0x80)==0){SDA_1_L;}
 		DAC_delay_nms(1);
		SCL_1_H;
		DAC_delay_nms(1);
		SCL_1_L;
		j=j<<1;
	}
	DAC_delay_nms(1);SDA_1_H;DAC_delay_nms(1);SCL_1_H;DAC_delay_nms(1);
	SDA_IN;
	DAC_delay_nms(1);
	if(SDA==0x00) 
	ack=1; 
	SDA_OUT;
	DAC_delay_nms(1);
	SCL_1_L;DAC_delay_nms(1);
}

//-------------  写多字节 -------------
void I2C_ramwrite(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t n)
{
	uint8_t i; 
	I2C_star();
	I2C_wbyte(sla);		 				//写器件地址,写操作
	if(ack==1){I2C_wbyte(suba);}		//写存储位置
	for(i=0;i<n;i++)				//写数据
	{
	 	if(ack==1){I2C_wbyte(*s);}
		s++;
	}
	I2C_stop();
	DAC_delay_nms(5);//延时
} 

//------------  读单字节 --------------
uint8_t I2C_rbyte()
{
	uint8_t i,msb; 
	msb=0;
	for(i=0;i<8;i++) 
	{
		SDA_1_H;msb=msb*2;SCL_1_H;
		SDA_IN;
		DAC_delay_nms(1);
        if(SDA==1) {msb=msb+1;}
        SDA_OUT;
		SCL_1_L;
		DAC_delay_nms(1);
	}
	return msb;
}


//------------  读多字节  -------------
void I2C_ramread(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t n)
{
	uint8_t i; 
	I2C_star();
	I2C_wbyte(sla);						//写器件地址,写操作
	if(ack==1){I2C_wbyte(suba);}		//写存储位置
	if(ack==1)
	{
		I2C_star();
    I2C_wbyte(sla|0x01);			//写器件地址,读操作
	}
	for(i=0;i<(n-1);i++)
	{
		if(ack==1)
		{
			*s=I2C_rbyte();
			s++;
			SDA_1_L;DAC_delay_nms(1);SCL_1_H;DAC_delay_nms(1);SCL_1_L;DAC_delay_nms(1);
		}
	}
	if(ack==1)
	{
		*s=I2C_rbyte();
	}
	I2C_stop();  
} 
//==================  I2C读写结束  ===================


