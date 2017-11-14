/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : main.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2017年8月7日
  最近修改   :
  功能描述   :
******************************************************************************/
#include "config.h"
#include "main.h"
#include "user_config.h"
#include "gpio_config.h"
#include "user_app_task.h"
#include "queue.h"
#include "Cmd_process.h"

#include "lpc177x_8x_eeprom.h"
#include "uart1.h"
#include "uart2.h"

#include "ADC.h"
//********************************************************************************
//************************************全局变量定义*******************************
//********************************************************************************
const  char  SVersion[] = "4.10\0";//模块软件版本号
const  char  type[6][6] = {"FSW\0", "OTDR\0", "OPM-1\0", "OPM-2\0","WDM\0", "OS\0"};
uint8  cfm[20];                    //确认buf
uint8  run_flag;                   //LED 运行灯
uint8  scan_flag;                  //扫描标志
uint16 srch;                       //扫描实时通道
uint8  stime;                      //扫描时间
uint8  ADDR;                       //业务盘地址
#ifdef TYPE_FSW
uint8  OPS_CH;                     //光开关通道数
#endif
uint8  u1RcvBuf[120];              //任务TaskUart1Revice 用的数组
struct EPROM_DATA EPROM;           //保存EPROM设置参数的结构体
#ifdef  TYPE_OPM
    uint8 hardware_way[CHANNEL_NUM];  //真实的硬件档位  
    uint8 PD_INFO[120];
    uint8 MD_INFO[120];
#endif

//OS_EVENT *Uart2RcvMbox;            //串口 2 接收邮箱
OS_EVENT *Uart1RcvMbox; 

OS_STK TaskWDTStk[TASK_STK_SIZE];

OS_STK TaskUart1CmdStk[TASK_STK_SIZE*2];
OS_STK TaskUart1RcvStk[TASK_STK_SIZE*2];

#ifdef TYPE_FSW
OS_STK TaskSvrStk[TASK_STK_SIZE*2];
#endif

#ifdef  TYPE_OPM
    OS_STK TaskCollectAStk[TASK_STK_SIZE*2];
    OS_STK TaskCollectBStk[TASK_STK_SIZE*2];
	OS_STK TaskSwitchStk[TASK_STK_SIZE - 1];
#endif
/********************************************************************************************************
** 函数名称: restore_set()
** 功能描述: 出厂默认设置
********************************************************************************************************/
void restore_set(void)
{
    EPROM.BPS = 9;            //串口波特率(115200)

    //再给EEPROM初始化,防止EEPROM出问题,无法保存
    LPC1778_EEPROM_Init();
}

/********************************************************************************************************
** 函数名称: systemInt
** 功能描述: 初始化
********************************************************************************************************/
void READ_EPROM_Init(void)
{
    LPC1778_EEPROM_Init();     //I2C 初始化

    EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
    delay_nms(20);
    //再读一次防止出现误码
    EEPROM_Read_Str( 0x00, (uint8 *)&EEPROM_BASE_ADDRESS, sizeof(struct EPROM_DATA) );
	  
	EPROM.BPS = 9;
	stime = 1;
}

/********************************************************************************************************
** 函数名称: HW_Init
** 功能描述: 硬件初始化
********************************************************************************************************/
void HW_Init(void)
{	
	ONLINE_PIN_INIT;
	OFFLINE;
	RUN_LED_INIT;	
#ifdef TYPE_FSW
	OPS_PIN_INIT;
	OPS_RST;
	delay_nms(100);
	OPS_NRST;
    EPROM.TYPE = FSW;
	LPC_GPIO4->CLR |= (0x7F<<5);//光开关复位到第一路 
#endif
	
#ifdef TYPE_OTDR
    OTDR_INIT;
	OTDR_PWR_ON;
    EPROM.TYPE = OTDR;
#endif
	
#ifdef TYPE_WDM
    EPROM.TYPE = WDM;
#endif
		
#ifdef TYPE_OPM
    OPM_CTR_PIN_INIT;
	#ifdef TYPE_OPM_ONLINE 
		EPROM.TYPE = OPM1;
	#endif
	#ifdef TYPE_OPM_OFFLINE 
		EPROM.TYPE = OPM2;
	#endif
	ADC_int(400);
#endif

#ifdef TYPE_OS
    OS_PIN_INIT;
    EPROM.TYPE = OS;
#endif
	
	READ_EPROM_Init();
	UART0Init();
	UART1_Init(115200);
	UART2_Init(115200);
	ONLINE;
	ADDR = ((LPC_GPIO4->PIN>>0) & 0x1f) + 1;
	EPROM.address = ADDR;

	/*******************开启看门狗********************/
	/**LPC1778的看门狗使用内部RC时钟(500KHz),经过4次分频(500K/4=125K ,即十六进制为0X1E848)**/
	LPC_WDT->TC  = 0X1E848;    //设置WDT定时值为1秒.
	LPC_WDT->MOD = 0x03;       //设置WDT工作模式,启动WDT	

}

/********************************************************************************************************
** 函数名称: TaskInfo
** 功能描述: 发送状态
********************************************************************************************************/
#ifdef TYPE_FSW
void scan(void)
{
	  uint8  i, temp;
	
		if(scan_flag)
		{
			begin:
			  if(scan_flag == 1)
				{
						for(i=0; i<7; i++)
						{
								temp = ((EPROM.sbch - 1) >> i) & 0x01;
								if(temp)
										LPC_GPIO4->SET |= (1<<(5+i));
								else
										LPC_GPIO4->CLR |= (1<<(5+i));
						}
						srch = EPROM.sbch;
						OPS_CH = EPROM.sbch - 1;
						scan_flag = 2;
				}
				else
				{
					  srch++;
					  if(EPROM.sech > EPROM.sbch)
						{
								if(srch > EPROM.sech)
								{
										scan_flag = 1;
										goto begin;
								}
						}
						else if(EPROM.sech < EPROM.sbch)
						{
								if(srch > EPROM.maxch)
										srch = 1;
								if((srch > EPROM.sech) && (srch < EPROM.sbch))
								{
										scan_flag = 1;
										goto begin;
								}
						}
						else 
						{
								scan_flag = 1;
								goto begin;
						}
						OPS_CH = srch - 1;
						for(i=0; i<7; i++)
						{
								temp = ((srch - 1) >> i) & 0x01;
								if(temp)
										LPC_GPIO4->SET |= (1<<(5+i));
								else
										LPC_GPIO4->CLR |= (1<<(5+i));
						}
				}
		}
}
#endif
/********************************************************************************************************
** 函数名称: TaskWDT
** 功能描述: 看门狗复位
** 说  明: 看门狗使用内部RC时钟(4MHz),经过4次分频。T=0x1000000*1us=1s (看门狗复位定时时间)
********************************************************************************************************/
void TaskWDT(void *pdata)
{
	uint8 delay_temp = 0;
		while(1)
		{
				LPC_WDT->FEED = 0xAA;
				LPC_WDT->FEED = 0x55;
				
				if(run_flag)
				{
						RUN_LED_H;
						run_flag = 0;
				}
				else
				{
						RUN_LED_L;
						run_flag = 1;
						sprintf((char *)cfm, "<%02d_%01u_%s>\n", ADDR,EPROM.TYPE, EPROM.MN);
						//UART1Write_Str((uint8 *)cfm);
				}
				delay_temp ++;
				if(delay_temp > 10)
				{
					delay_temp = 0;
					ADDR = ((LPC_GPIO4->PIN>>0) & 0x1f) + 1;
					EPROM.address = ADDR;
				}
				OSTimeDly(500);
		}
}

/********************************************************************************************************
** 函数名称: TaskInfo
** 功能描述: 发送状态
********************************************************************************************************/
#ifdef TYPE_FSW
void TaskSvr(void* pdata)
{
		while(1)
		{
			  scan();
				OSTimeDly(100 * stime);
		}
}
#endif
/********************************************************************************************************
** 函数名称: TaskUart0Cmd
** 功能描述: 命令解析
********************************************************************************************************/
void TaskUart1Cmd(void* pdata)
{
    uint8  err;
    uint16 len;

    OSTimeDly(500);            //等待延时
    while(1)
    {
        OSMboxPend(Uart1RcvMbox, 0, &err);         // 等待接收邮箱数据
        if( (len = cmd_process((char*)&u1RcvBuf)) > 0 )
        {
            UART1Put_str(u1RcvBuf, len);
        }
		#ifdef TYPE_OPM	
			eprom_set();
		#endif
		}
}

/********************************************************************************************************
** 函数名称: TaskUart2Rcv
** 功能描述: μCOS-II的任务。从UART2接收数据，当接收完一帧数据后通过消
**           息邮箱传送到TaskStart任务。
********************************************************************************************************/
void TaskUart1Rcv(void* pdata)
{ 
		uint8 *cp;
		uint8 i,temp;

		while(1)
		{
				cp = u1RcvBuf;
				while((*cp = UART1Getch()) != '<') ;  // 接收数据头
				cp++;   								              //往下移一个字节
				for (i = 0; i < 50; i++)
				{
						temp = UART1Getch();
						*cp++ = temp;
						if (temp =='\n')
						{       temp = i;
								while(i < 48)
								{
										*cp++ = 0;                //空余的后面补0
										i++;
								}
								break;
						}
				}
				OSMboxAccept(Uart1RcvMbox);          //清空 邮箱Uart0ReviceMbox
				OSMboxPost(Uart1RcvMbox, (void *)u1RcvBuf);
		}
}
/*******************************************************************************************************
** 函数名称: main
** 功能描述: 主函数
********************************************************************************************************/
int main (void)
{
		SystemInit();
		HW_Init();
		IntDisAll();  //Note:由于使用UCOS, 在OS运行之前运行,注意别使能任何中断.
		OSInit();
		OS_CPU_SysTickInit(SystemCoreClock/OS_TICKS_PER_SEC);

		OSTaskCreate(TaskWDT,       (void *)0, &TaskWDTStk[TASK_STK_SIZE - 1],        1);
#ifdef TYPE_FSW	 
	    OSTaskCreate(TaskSvr,       (void *)0, &TaskSvrStk[TASK_STK_SIZE*2 - 1],      2);
	    OSTaskCreate(TaskUart1Cmd,  (void *)0, &TaskUart1CmdStk[TASK_STK_SIZE*2 - 1], 3);
		OSTaskCreate(TaskUart1Rcv,  (void *)0, &TaskUart1RcvStk[TASK_STK_SIZE*2 - 1], 4);
#endif


#ifdef TYPE_OPM
	    OSTaskCreate(TaskCollectA,  (void *)0, &TaskCollectAStk[TASK_STK_SIZE*2 - 1], 2);
	    OSTaskCreate(TaskCollectB,  (void *)0, &TaskCollectBStk[TASK_STK_SIZE*2 - 1], 3);
	    OSTaskCreate(TaskSwitch,    (void *)0, &TaskSwitchStk[TASK_STK_SIZE - 1],     4);
		OSTaskCreate(TaskUart1Cmd,  (void *)0, &TaskUart1CmdStk[TASK_STK_SIZE*2 - 1], 5);
		OSTaskCreate(TaskUart1Rcv,  (void *)0, &TaskUart1RcvStk[TASK_STK_SIZE*2 - 1], 6);
#endif 
        OSTaskCreate(TaskUart1Cmd,  (void *)0, &TaskUart1CmdStk[TASK_STK_SIZE*2 - 1], 5);
		OSTaskCreate(TaskUart1Rcv,  (void *)0, &TaskUart1RcvStk[TASK_STK_SIZE*2 - 1], 6);
#ifdef TYPE_OS
#endif
		//建立串口0的接收邮箱
		Uart1RcvMbox = OSMboxCreate(NULL);
		if(Uart1RcvMbox == NULL)
			  return 1;
		
		OSStart();
		return 0;
}

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
