/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : OSW.C
  版 本 号   :
  作    者   : 123
  生成日期   : 2013年5月31日
  函数列表   :
              BypassSwitch
              MostSwitch
  修改历史   :
******************************************************************************/

#include "config.h"
#include "main.h"
#include "LPC177x_8x.h"
#include "OSW.h"
#include "AT24C512.h"
#include "lpc177x_8x_eeprom.h"
#include "log.h"


/*****************************************************************************
**函 数 名: Save_LOG
**功能描述: 保存 LOG 到 AT24C512 中 
**输入参数: uint8 Link  
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void Save_LOG(uint8 Link)
{
//	SYS_LOG.Sec  = LPC_RTC->SEC;
//	SYS_LOG.Min  = LPC_RTC->MIN;
//	SYS_LOG.Hour = LPC_RTC->HOUR;
//	SYS_LOG.Mday = LPC_RTC->DOM;
//	SYS_LOG.Mon  = LPC_RTC->MONTH;
//	SYS_LOG.Year = LPC_RTC->YEAR;

//	SYS_LOG.Link = Link;
//	SYS_LOG.State = OSW_state[Link]; 
//	SYS_LOG.Mode = EPROM.Autoflag[Link];
//    //SYS_LOG.Switch_Condition = 
//	Write_AT24C512( EPROM.AT24C512_Page++, (uint8 *)&SYS_LOG, sizeof(struct LOG) ); 
//	OSTimeDly(15);
//	
//	if( EPROM.AT24C512_NUM < 511 )   EPROM.AT24C512_NUM++; 
//	
//	if(EPROM.AT24C512_Page > 511)    EPROM.AT24C512_Page = 0; 
//	Save_To_EPROM((uint8 *)&EPROM.AT24C512_Page, 4); 
//	OSTimeDly(15);
}

/***********************************************************************************
** 函数名称: MostSwitch(),
** 功能描述: 切换开关到主路
** 输　入: uint8 ch    0~3的通道
** 输　出: 无
************************************************************************************/
void MostSwitch(uint8 ch) //切换到 主路
{
    if(OSW_state[ch] != 1)
    {
        switch(ch)
        {
        case 0:
        {
            SW_CON1_L;
            SW_CON2_H;
            break;
        }
        case 1:
        {
            SW_CON3_L;
            SW_CON4_H;
            break;
        }
        
        default: break;          
        }
        OSW_state[ch] = 1;
    }
}

/***********************************************************************************
** 函数名称: BypassSwitch(),
** 功能描述: 切换开关到旁路
** 输　入: uint8 ch    0~3的通道
** 输　出: 无
************************************************************************************/
void BypassSwitch(uint8 ch)//切换到 旁路
{
    if(OSW_state[ch] != 0)
    {
        switch(ch)
        {
        case 0:
        {
            SW_CON1_H;
            SW_CON2_H;
            break;
        }
        case 1:
        {
            SW_CON3_H;
            SW_CON4_H;
            break;
        }

        default: break;          
        }
        OSW_state[ch] = 0;
    }
}
