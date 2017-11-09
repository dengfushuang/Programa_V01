#ifndef __DRV595_H
#define __DRV595_H

#include "LPC177x_8x.h"

#define  HC595_PORT_INIT    LPC_GPIO0->DIR|= (5<<19)|(1<<22) 

#define  HC595_SER_H		LPC_GPIO0->SET|=(1<<22)     	
#define  HC595_SER_L	    LPC_GPIO0->CLR|=(1<<22)

#define  HC595_RCLK_H       LPC_GPIO0->SET|=(1<<20)     
#define  HC595_RCLK_L	    LPC_GPIO0->CLR|=(1<<20) 

#define  HC595_SCLK_H		LPC_GPIO0->SET|=(1<<19)    	
#define  HC595_SCLK_L	    LPC_GPIO0->CLR|=(1<<19)

//#define  HC595_GL_SET     LPC_GPIO1->SET|=(1<<24)     	
//#define  HC595_GL_CLR		LPC_GPIO1->CLR|=(1<<24)

/*************************************************************************************************************
*函数名称：HC595_Shift()
*参   数 ：uint8_t dat
*返回值  : void
*功能描述：给74HC595发送一个字节函数并加载
**************************************************************************************************************/
extern void HC595_Shift(uint16_t  dat);
extern void ChannelLED(uint8_t i ,uint8_t w);	 //i通道；w数值0都灭，1红，2绿

#endif /*结束*/
