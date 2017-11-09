#ifndef __USER_APP_TASK_H
#define __USER_APP_TASK_H
#include "main.h"
#include "ADC.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef  TYPE_OPM
void  TaskCollectA(void *pdata);
void  TaskCollectB(void *pdata);
void  TaskSwitch(void *pdata);
#endif
	

#ifdef __cplusplus
}
#endif	
#endif
