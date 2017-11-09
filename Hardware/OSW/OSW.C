/******************************************************************************

                  ��Ȩ���� (C), 2001-2013, ���ֺ������ͨ�ż������޹�˾

 ******************************************************************************
  �� �� ��   : OSW.C
  �� �� ��   :
  ��    ��   : 123
  ��������   : 2013��5��31��
  �����б�   :
              BypassSwitch
              MostSwitch
  �޸���ʷ   :
******************************************************************************/

#include "config.h"
#include "main.h"
#include "LPC177x_8x.h"
#include "OSW.h"
#include "AT24C512.h"
#include "lpc177x_8x_eeprom.h"
#include "log.h"


/*****************************************************************************
**�� �� ��: Save_LOG
**��������: ���� LOG �� AT24C512 �� 
**�������: uint8 Link  
**�������: ��
**�� �� ֵ: ��
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
** ��������: MostSwitch(),
** ��������: �л����ص���·
** �䡡��: uint8 ch    0~3��ͨ��
** �䡡��: ��
************************************************************************************/
void MostSwitch(uint8 ch) //�л��� ��·
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
** ��������: BypassSwitch(),
** ��������: �л����ص���·
** �䡡��: uint8 ch    0~3��ͨ��
** �䡡��: ��
************************************************************************************/
void BypassSwitch(uint8 ch)//�л��� ��·
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