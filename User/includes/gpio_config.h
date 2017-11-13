#ifndef __GPIO_CONFIG_H
#define __GPIO_CONFIG_H
#include "LPC177x_8x.h"
#include "user_config.h"
#ifdef __cplusplus
extern "C" {
#endif

	
#ifdef  TYPE_OPM
	//1
	#define CHANNEL_PIN0_WAY(X)   LPC_GPIO3->CLR|=(7<<24);LPC_GPIO3->SET|=(X<<24) //AD ��ȡ�������ƽ�
	//2
	#define CHANNEL_PIN1_WAY(X)   LPC_GPIO0->CLR|=(7u<<29);LPC_GPIO0->SET|=(X<<29) //AD ��ȡ�������ƽ�
	//3
	#define CHANNEL_PIN2_WAY(X)   LPC_GPIO1->CLR|=(7<<18); LPC_GPIO1->SET|=(X<<18)  //AD ��ȡ�������ƽ�
	//4
	#define CHANNEL_PIN3_WAY(X)   LPC_GPIO1->CLR|=(7<<21);LPC_GPIO1->SET|=(X<<21) //AD ��ȡ�������ƽ�
	//5
	#define CHANNEL_PIN4_WAY(X)   LPC_GPIO1->CLR|=(7<<24);LPC_GPIO1->SET|=(X<<24) //AD ��ȡ�������ƽ�
	//6
	#define CHANNEL_PIN5_WAY(X)   LPC_GPIO3->CLR|=(7<<5);LPC_GPIO3->SET|=(X<<5) //AD ��ȡ�������ƽ�
	//7
	#define CHANNEL_PIN6_WAY(X)   LPC_GPIO1->CLR|=(7<<27);LPC_GPIO1->SET|=(X<<27) //AD ��ȡ�������ƽ�
	//8
	#define CHANNEL_PIN7_WAY(X)   LPC_GPIO2->CLR|=(7<<11);LPC_GPIO2->SET|=(X<<11)             //AD ��ȡ�������ƽ�
	//9
	#define CHANNEL_PIN8_WAY(X)   LPC_GPIO4->CLR|=(7<<5);LPC_GPIO4->SET|=(X<<5) //AD ��ȡ�������ƽ�
	//10
	#define CHANNEL_PIN9_WAY(X)   LPC_GPIO4->CLR|=(7<<10);LPC_GPIO4->SET|=(X<<10) //AD ��ȡ�������ƽ�
	//11
	#define CHANNEL_PIN10_WAY(X)   LPC_GPIO2->CLR|=(7<<6);LPC_GPIO2->SET|=(X<<6) //AD ��ȡ�������ƽ�
	//12
	#define CHANNEL_PIN11_WAY(X)   LPC_GPIO2->CLR|=(7<<3);LPC_GPIO2->SET|=(X<<3) //AD ��ȡ�������ƽ�
	//13
	#define CHANNEL_PIN12_WAY(X)   LPC_GPIO4->CLR|=(7<<11); LPC_GPIO4->SET|=(X<<11)  //AD ��ȡ�������ƽ�
	//14
	#define CHANNEL_PIN13_WAY(X)   LPC_GPIO2->CLR|=(7<<0);LPC_GPIO2->SET|=(X<<0) //AD ��ȡ�������ƽ�
	//15
	#define CHANNEL_PIN14_WAY(X)   LPC_GPIO0->CLR|=(7<<9);LPC_GPIO0->SET|=(X<<9) //AD ��ȡ�������ƽ�
	//16
	#define CHANNEL_PIN15_WAY(X)   LPC_GPIO0->CLR|=(7<<4);LPC_GPIO0->SET|=(X<<4) //AD ��ȡ�������ƽ�
	//17
	#define ADC_CHANNEL1_IN(X)   LPC_GPIO3->CLR|=(7<<2);LPC_GPIO3->SET|=(X<<2) //AD ��ȡ�������ƽ�
	//18
	#define ADC_CHANNEL2_IN(X)   LPC_GPIO1->CLR|=(7<<15);LPC_GPIO1->SET|=(X<<15) //AD ��ȡ�������ƽ�	
	
#endif	

#ifdef  TYPE_OS
   #define OS_PIN_INIT  LPC_GPIO4->DIR |= (255 << 5 )
   
   #define OS_H(x)       LPC_GPIO4->SET |= (1 << (x+4))
   #define OS_L(x)       LPC_GPIO4->CLR |= (1 << (x+4))  
   #define OS_ALL_H      LPC_GPIO4->SET |= (255 << 5)
   #define OS_ALL_L      LPC_GPIO4->CLR |= (255 << 5)
#endif

#ifdef __cplusplus
}
#endif
#endif
