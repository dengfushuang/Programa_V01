#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif
	
/*********************************************************************************************************
  板卡类型
*********************************************************************************************************/
#define TYPE_OPM
#define FSW    0
#define OTDR   1
#define WDM    2
#define OPM    3
#define OS     4

#define CHANNEL_NUM 16   //最大通道数为16路
	
	

#ifdef  TYPE_OPM
#define AMP_CHANNEL  8   //运算放大器比例通道数 8通道
#define ADC_USE_NUM  2   //ADC使用通道数
#endif

	
	
	
	
	
	
	
	
	
	
	
	

#ifdef __cplusplus
}
#endif
#endif
