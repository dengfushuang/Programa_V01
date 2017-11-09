
/*
ancheel changed it for STM32 2010-6-6
www.anchey.com
*/

#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

#include <stdint.h>

typedef uint32_t clock_time_t;

//extern volatile clock_time_t UIP_sys_ticks;

//#define clock_time() UIP_sys_ticks//我们使用宏定义来提升系统性能

#define CLOCK_CONF_SECOND   OS_TICKS_PER_SEC

#endif /* __CLOCK_ARCH_H__ */
