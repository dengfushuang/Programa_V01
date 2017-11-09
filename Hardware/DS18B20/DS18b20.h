#ifndef __DS18B20_H
#define __DS18B20_H 


#include "main.h"


#define  DQ        ((LPC_GPIO0->PIN>>31)&0x01) //DS18B20的DQ引脚
#define  DQ_OUT    LPC_GPIO0->DIR |= (1u<<31)   //将DQ设置为输出
#define  DQ_IN     LPC_GPIO0->DIR &= ~(1u<<31)  //将DQ设置为输入
#define  DQ_H      LPC_GPIO0->SET|=(1u<<31)     //拉高DQ电平
#define  DQ_L      LPC_GPIO0->CLR|=(1u<<31)     //拉低DQ电平

extern uint8 T_18b20flag;
   	
uint8 DS18B20_Reset(void);
void DS18B20_Write(uint8 cmd);
uint8 DS18B20_Read(void);
uint16 DS18B20_GetTemperature(uint8 T);
void DS18B20_Alarm(float Threshold_T,uint8 T);
extern uint16 DS18B20_Get_Tmp(void);
 
#endif

