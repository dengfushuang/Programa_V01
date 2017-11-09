/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name: config.h
** Last modified Date:  2004-09-17
** Last Version: 1.0
** Descriptions: User Configurable File
**
**------------------------------------------------------------------------------------------------------
** Created By: Chenmingji
** Created date:   2004-09-17
** Version: 1.0
** Descriptions: First version
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************/
#ifndef __CONFIG_H 
#define __CONFIG_H
//这一段无需改动
//This segment should not be modified
// #ifndef TRUE
// #define TRUE  1
// #endif

// #ifndef FALSE
// #define FALSE 0
// #endif

typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          fp32;                    /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         fp64;                    /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */

/********************************/
/*      uC/OS-II specital code  */
/*      uC/OS-II的特殊代码      */
/********************************/

#define     USER_USING_MODE    0x10                    /*  User mode ,ARM 32BITS CODE 用户模式,ARM代码                  */
// 
                                                     /*  Chosen one from 0x10,0x30,0x1f,0x3f.只能是0x10,0x30,0x1f,0x3f之一       */
#include "Includes.h"


/********************************/
/*      ARM的特殊代码           */
/*      ARM specital code       */
/********************************/
//这一段无需改动
//This segment should not be modify

#include "LPC177x_8x.h"

#include "core_cm3.h"

/********************************/
/*     应用程序配置             */
/*Application Program Configurations*/
/********************************/
//以下根据需要改动
//This segment could be modified as needed.
#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h>

#include 	"uart0.h"
//#include 	"..\driver\uart1.h"
//#include 	"..\driver\queue.h"
//#define 	UART0_SEND_QUEUE_LENGTH   240    /* 给UART0发送数据队列分配的空间大小 */
//#define 	UART1_SEND_QUEUE_LENGTH   240    /* 给UART1发送数据队列分配的空间大小 */

//#include  "i2c.h"
//#include  "I2CINT.H"
#define   CAT1025		0xA0		/* CAT1025的I2C地址 */
#include "drv595.h"
#include "delay.h"

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
