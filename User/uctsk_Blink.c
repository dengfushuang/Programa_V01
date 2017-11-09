

/* Includes ------------------------------------------------------------------*/
#include <includes.h>            
#include "uart0.h"
#include "lcd16032.h"
/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
static  void  uctsk_Blink        (void);
//static  void  LED_On             (void) ;
//static  void  LED_Off            (void) ;
#define  RUN_H      LPC_GPIO0->FIOSET=1<<9 		
#define  RUN_L      LPC_GPIO0->FIOCLR=1<<9  

void  App_BlinkTaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_Blink,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_BLINK_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_BLINK_PRIO, "Task LED Blink", &os_err);
	#endif

}

static void uctsk_Blink (void) 
{                 
    //LCD_Init();
	LPC_GPIO0->FIODIR|=1<<9;
   	for(;;)
   	{   
	  RUN_H ;
			OSTimeDly(500);
			RUN_L;
			OSTimeDly(500);
   }
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
