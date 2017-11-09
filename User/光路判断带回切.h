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


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern uint8 Optics_Bypass_flag[CHANNEL_NUM];

extern uint8 Autoflag[CHANNEL_NUM];              
extern uint32 BackAuto_Time_Tick[CHANNEL_NUM];
extern uint32 BackSW_Time_Tick[CHANNEL_NUM];

extern void optics_judge(uint8 Link_num , uint8 ch1 ,uint8 ch2 );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __OPTICS_JUDGE_H__ */
