
#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__
					  

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/
#define  DEF_ENABLED    1

#define  OS_VIEW_MODULE                  DEF_DISABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/	
#define  APP_TASK_START_PRIO             (                 1) 

#define  LWIP_TASK_START_PRIO            (                 2)
#define  LWIP_TASK_END_PRIO              (                 4)

#define  APP_TASK_ETHERNETIF_INPUT_PRIO  (				   7)
#define  APP_TASK_BLINK_PRIO          	 (                 3) 

#define  APP_TASK_LWIP_PRIO              (                 10)

#define  APP_TASK_PIP_PRIO              (                 9)
#define  APP_TASK_HTTP_PRIO              (                 9)
#define  APP_TASK_UDP_PRIO              (                 9)
#define  APP_TASK_TCP_PRIO              (                 9)
#define  APP_TASK_TCPREC_PRIO              (                 10)
#define  APP_TASK_OSVIEW_TERMINAL_PRIO	 (OS_LOWEST_PRIO - 6)

#define  OS_VIEW_TASK_PRIO               (OS_LOWEST_PRIO - 3)
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)



/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define  APP_TASK_LWIP_STK_SIZE                          256u
#define  APP_TASK_ETHERNETIF_INPUT_STK_SIZE				 512u

#define  APP_TASK_START_STK_SIZE                          128u
#define  APP_TASK_BLINK_STK_SIZE                         128u
#define  APP_TASK_OSVIEW_TERMINAL_STK_SIZE   			 128u
#define  OS_VIEW_TASK_STK_SIZE                           128u
#define  APP_TASK_PIP_STK_SIZE                          128u
#define  APP_TASK_HTTP_STK_SIZE                          128u
#define  APP_TASK_UDP_STK_SIZE                          128u
#define  APP_TASK_TCP_STK_SIZE                          128u
#define  APP_TASK_TCPREC_STK_SIZE                          256u
/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_DISABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED



#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

