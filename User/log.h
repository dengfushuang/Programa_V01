#ifndef __LOG_H__
#define __LOG_H__	 

#include "main.h"

typedef struct LOG_DATA {

	  char time[24];
	  uint8_t R1_Power[7];
	  uint8_t R2_Power[7];
	  uint8_t pwr_stat;
}LOG_DATA;

void LOG_WRITE(struct LOG_DATA *LOG_DATA);


#endif



