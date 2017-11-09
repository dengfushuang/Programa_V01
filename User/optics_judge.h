/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : optics_judge.h
  版 本 号   : 初稿
  作    者   : 123
  生成日期   : 2013年4月26日
  最近修改   :
  功能描述   : optics_judge.c 的头文件
  函数列表   :
******************************************************************************/

#ifndef __OPTICS_JUDGE_H__
#define __OPTICS_JUDGE_H__
#include "main.h"

extern uint8_t Optics_Bypass_flag[CHANNEL_NUM];
extern uint8_t Autoflag[CHANNEL_NUM];
extern uint32_t BackAuto_Time_Tick[CHANNEL_NUM];


extern void optics_judge(uint8_t Link_num , uint8_t ch1 , uint8_t ch2);




#endif /* __OPTICS_JUDGE_H__ */
