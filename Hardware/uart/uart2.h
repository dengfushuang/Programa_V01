#include  "config.h"


#ifndef __UART2_H 
#define __UART2_H 

#define UART2_FIFO_LENGTH         8

uint8_t UART2_Init(uint32 BPS);
void UART2Putch(uint8 Data);   
void UART2Put_str(uint8 *Data, uint16 NByte);
void UART2Write_Str(uint8 *Data);

uint8 UART2Getch(void);
uint8 UART2Get(void);

#endif






