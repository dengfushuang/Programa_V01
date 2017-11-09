/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : User_TimeTicks.c
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 2013年5月7日
  最近修改   :
  功能描述   :  
  函数列表   :
              User_TimeTicks
******************************************************************************/
#include "main.h"
#include "clock-arch.h"

extern uint32  BLCD_TimeTicks;
extern uint8   ICMP_time_en[CHANNEL_NUM];
extern uint16  ICMP_time_couter[CHANNEL_NUM];
extern uint32  BackOSW_Time_Tick[CHANNEL_NUM];
extern uint32  BackAuto_Time_Tick[CHANNEL_NUM];
extern uint32  BH_Tick[CHANNEL_NUM];

void  User_TimeTicks(void)
{

}





