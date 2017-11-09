/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : ADC.h
  版 本 号   : 初稿
  作    者   : 123
  生成日期   : 2013年4月19日
  最近修改   :
  功能描述   : ADC.c 的头文件
  函数列表   :
******************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__
#include "main.h"
#include "gpio_config.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


extern uint8 hardware_way[CHANNEL_NUM];
extern float power[CHANNEL_NUM];  
extern float power_count[CHANNEL_NUM];
extern float q_power[CHANNEL_NUM];
extern uint8 warn[CHANNEL_NUM];			//告警状态

extern void adcdeal(uint8 input_channel_num,uint32 way, float adc);
extern void ADC_int(uint32 rate);
extern uint32 addo(uint8 adnum);
extern void CHANNEL(uint8 input_channel_num, uint8 way_temp);
//extern void CHANNELB(uint8 input_channel_num, uint8 way_temp);
extern void optics_collect(uint8 input_channel_num ,uint8 way_temp);
extern void log114_adcdeal(uint8 ADnum ,uint32 adc);
extern float test_optics_collect( uint8 input_channel_num ,uint8 way_temp);
extern float test_optics_collect_B( uint8 input_channel_num ,uint8 way_temp);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ADC_H__ */
