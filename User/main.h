/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : main.h
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2013年8月20日

******************************************************************************/


#ifndef __MAIN_H 
#define __MAIN_H 
#include "user_config.h"
#include "includes.h"
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
  自定义的新变量
*******************************************************************************/
typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		  有符号8位整型变量  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          fp32;                    /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         fp64;                    /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */


/*********************************************************************************************************
  TASK STACK SIZES  任务堆栈大小
*********************************************************************************************************/
#define  TASK_STK_SIZE                128

/*********************************************************************************************************
  引脚定义
*********************************************************************************************************/
#ifdef   TYPE_OPM
#define  RUN_LED_INIT    LPC_GPIO1->DIR |= (1u<<31)
#define  RUN_LED_H       LPC_GPIO1->SET |= (1u<<31)
#define  RUN_LED_L       LPC_GPIO1->CLR |= (1u<<31)
#else
#define  RUN_LED_INIT    LPC_GPIO3->DIR |= (1<<2)
#define  RUN_LED_H       LPC_GPIO3->SET |= (1<<2)
#define  RUN_LED_L       LPC_GPIO3->CLR |= (1<<2)
#endif

#define  ONLINE_PIN_INIT LPC_GPIO5->DIR |= (1<<4)
#define  ONLINE          LPC_GPIO5->CLR |= (1<<4)
#define  OFFLINE         LPC_GPIO5->SET |= (1<<4)

#define  SW_CON_INIT     LPC_GPIO2->DIR |= (2<<5)
#define  SW_CON1_H       LPC_GPIO2->SET |= (1<<5)
#define  SW_CON1_L       LPC_GPIO2->CLR |= (1<<5)
#define  SW_CON2_H       LPC_GPIO2->CLR |= (1<<6)
#define  SW_CON2_L       LPC_GPIO2->CLR |= (1<<6)

#define  OPS_PIN_INIT    LPC_GPIO4->DIR |= (0x1ff<<5) 
#define  OPS_RST         LPC_GPIO4->CLR |= (1<<12)
#define  OPS_NRST        LPC_GPIO4->SET |= (1<<12)

#define  OTDR_INIT       LPC_GPIO2->DIR |= (1<<9);LPC_GPIO5->DIR |= (1<<0)
#define  OTDR_PWR_ON     LPC_GPIO5->SET |= (1<<0)       
#define  OTDR_PWR_OFF    LPC_GPIO5->CLR |= (1<<0)

//#define  CHANNEL_NUM       1            //链路数宏定义
#define  LOG_ADDR          1000         //日志存储地址
#define  EEPROM_BASE_ADDRESS  EPROM.BPS //flash模拟EPROM基地址

__packed struct EPROM_DATA {
	
		uint8  BPS;                  //串口波特率  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:56000 8:57600 9:115200

		uint8  TYPE;                 //业务盘类型: 0 FSW, 1 OTDR, 2 WDM, 3 OPM
		char   MN[9];                //业务盘机器码
		uint16 maxch;                //最大通道数
		uint16 sbch;                 //扫描启始通道
		uint16 sech;                 //扫描结束通道
#ifdef  TYPE_OPM
	    uint16 Start_delay;    		//开机延时时间	
	    uint8  address;             //设备地址: 00 ~ 99。
	    uint8 way_switch[CHANNEL_NUM];
		int16   ADC_just[CHANNEL_NUM][2];   //功率的校准补偿系数
	    uint8 fuhao[CHANNEL_NUM][2];
	    uint8 fuhao_just[CHANNEL_NUM][2];
	    uint16 DBM_delay[CHANNEL_NUM];       //功率基准值
		int8   ADC_just_36[CHANNEL_NUM][2];   //大于36dB功率的校准补偿系数
		uint8   wavelength[CHANNEL_NUM];          //波长            1 1550     0 1310
	    float   q_power[CHANNEL_NUM];             //告警功率切换点
#endif
#ifdef  TYPE_OS
	    uint8 OS_Channel[CHANNEL_NUM];       //光源状态：0-关闭、1-开启
#endif
	    
};

extern const  char  SVersion[];  //模块软件版本号
extern const  char  type[][5];
extern uint8  u2RcvBuf[];
extern uint8  cfm[];
extern uint8  ADDR;
extern uint8  OPS_CH;
extern uint8  scan_flag;
extern uint8  stime;
extern struct EPROM_DATA  EPROM;

extern void   restore_set(void);             //恢复出厂设置函数
extern void   Reset_Handler(void);           //软件复位函数

#ifdef __cplusplus
}
#endif

#endif
/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/

