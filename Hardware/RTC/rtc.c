/****************************************************************************
 *   $Id:: rtc.c 5781 2010-12-02 00:39:36Z usb00423                         $
 *   Project: NXP LPC17xx RTC example
 *
 *   Description:
 *     This file contains RTC code example which include RTC initialization,
 *     RTC interrupt handler, and APIs for RTC access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC177x_8x.h"
#include "rtc.h"

volatile uint32_t alarm_on = 0;
volatile uint32_t PLLReConfig = 0;

void RTC_time_Init(void)
{
    RTCTime local_time, alarm_time;

    RTCInit();

    local_time.RTC_Sec = 0;
    local_time.RTC_Min = 11;
    local_time.RTC_Hour = 16;
    local_time.RTC_Mday = 8;
    local_time.RTC_Wday = 3;
    local_time.RTC_Yday = 12;		/* current date 05/12/2010 */
    local_time.RTC_Mon = 9;
    local_time.RTC_Year = 2013;
    RTCSetTime( local_time );		/* Set local time */

    alarm_time.RTC_Sec = 0;
    alarm_time.RTC_Min = 0;
    alarm_time.RTC_Hour = 0;
    alarm_time.RTC_Mday = 1;
    alarm_time.RTC_Wday = 0;
    alarm_time.RTC_Yday = 1;		/* alarm date 01/01/2011 */
    alarm_time.RTC_Mon = 1;
    alarm_time.RTC_Year = 2013;
    RTCSetAlarm( alarm_time );		/* set alarm time */

    NVIC_EnableIRQ(RTC_IRQn);

#if 1
    RTCSetAlarmMask(~(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR));
#endif
#if 0
    RTCSetCntIncIt(IMSEC);
#endif
#if 0
    RTCSetCntIncIt(IMMIN);
#endif
#if 0
    RTCSetCntIncIt(IMHOUR);
#endif
#if 0
    RTCSetCntIncIt(IMDOM);
#endif
#if 0
    RTCSetCntIncIt(IMDOW);
#endif
#if 0
    RTCSetCntIncIt(IMDOY);
#endif
#if 0
    RTCSetCntIncIt(IMMON);
#endif
#if 0
    RTCSetCntIncIt(IMYEAR);
#endif

    RTCStart();
}

/*****************************************************************************
** Function name:		RTC_IRQHandler
**
** Descriptions:		RTC interrupt handler, it executes based on the
**						the alarm setting
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTC_IRQHandler (void)
{
    uint32_t i;
    RTCTime local_time;
    for(i = 0; i < 20000000; i++);
    LPC_RTC->ILR |= (ILR_RTCCIF | ILR_RTCALF);		/* clear interrupt flag */
    RTCStop();
    local_time.RTC_Sec = 50;
    local_time.RTC_Min = 59;
    local_time.RTC_Hour = 23;
    local_time.RTC_Mday = 31;
    local_time.RTC_Wday = 6;
    local_time.RTC_Yday = 366;
    local_time.RTC_Mon = 12;
    local_time.RTC_Year = 2008;
    RTCSetTime( local_time );		/* Set local time */
    RTCStart();

}

/*****************************************************************************
** Function name:		RTCInit
**
** Descriptions:		Initialize RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCInit( void )
{
    alarm_on = 0;

    /* Enable CLOCK into RTC */
    LPC_SC->PCONP |= (1 << 9);

    /* If RTC is stopped, clear STOP bit. */
    if ( LPC_RTC->RTC_AUX & (0x1<<4) )
    {
        LPC_RTC->RTC_AUX |= (0x1<<4);
    }

    /*--- Initialize registers ---*/
    LPC_RTC->CCR = 0;
    LPC_RTC->ILR = (ILR_RTCCIF | ILR_RTCALF);
    LPC_RTC->AMR = 0xFF;
    LPC_RTC->CIIR = 0;
    LPC_RTC->CALIBRATION = 0x0;
    return;
}

/*****************************************************************************
** Function name:		RTCStart
**
** Descriptions:		Start RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCStart( void )
{
    /*--- Start RTC counters ---*/
    LPC_RTC->ILR = (ILR_RTCCIF | ILR_RTCALF);
    LPC_RTC->CCR |= CCR_CLKEN;
    LPC_SC->PCONP &= ~(1 << 9);
    return;
}

/*****************************************************************************
** Function name:		RTCStop
**
** Descriptions:		Stop RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCStop( void )
{
    /*--- Stop RTC counters ---*/
    LPC_RTC->CCR &= ~CCR_CLKEN;
    return;
}

/*****************************************************************************
** Function name:		RTC_CTCReset
**
** Descriptions:		Reset RTC clock tick counter
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTC_CTCReset( void )
{
    /*--- Reset CTC ---*/
    LPC_RTC->CCR |= CCR_CTCRST;
    return;
}

/*****************************************************************************
** Function name:		RTCSetTime
**
** Descriptions:		Setup RTC timer value
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCSetTime( RTCTime Time )
{
    LPC_RTC->SEC = Time.RTC_Sec;
    LPC_RTC->MIN = Time.RTC_Min;
    LPC_RTC->HOUR = Time.RTC_Hour;
    LPC_RTC->DOM = Time.RTC_Mday;
    LPC_RTC->DOW = Time.RTC_Wday;
    LPC_RTC->DOY = Time.RTC_Yday;
    LPC_RTC->MONTH = Time.RTC_Mon;
    LPC_RTC->YEAR = Time.RTC_Year;
    return;
}

/*****************************************************************************
** Function name:		RTCSetAlarm
**
** Descriptions:		Initialize RTC timer
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void RTCSetAlarm( RTCTime Alarm )
{
    LPC_RTC->ALSEC = Alarm.RTC_Sec;
    LPC_RTC->ALMIN = Alarm.RTC_Min;
    LPC_RTC->ALHOUR = Alarm.RTC_Hour;
    LPC_RTC->ALDOM = Alarm.RTC_Mday;
    LPC_RTC->ALDOW = Alarm.RTC_Wday;
    LPC_RTC->ALDOY = Alarm.RTC_Yday;
    LPC_RTC->ALMON = Alarm.RTC_Mon;
    LPC_RTC->ALYEAR = Alarm.RTC_Year;
    return;
}

/*****************************************************************************
** Function name:		RTCGetTime
**
** Descriptions:		Get RTC timer value
**
** parameters:			None
** Returned value:		The data structure of the RTC time table
**
*****************************************************************************/
RTCTime RTCGetTime( void )
{
    RTCTime LocalTime;

    LocalTime.RTC_Sec = LPC_RTC->SEC;
    LocalTime.RTC_Min = LPC_RTC->MIN;
    LocalTime.RTC_Hour = LPC_RTC->HOUR;
    LocalTime.RTC_Mday = LPC_RTC->DOM;
    LocalTime.RTC_Wday = LPC_RTC->DOW;
    LocalTime.RTC_Yday = LPC_RTC->DOY;
    LocalTime.RTC_Mon = LPC_RTC->MONTH;
    LocalTime.RTC_Year = LPC_RTC->YEAR;
    return ( LocalTime );
}

/*****************************************************************************
** Function name:		RTCSetAlarmMask
**
** Descriptions:		Set RTC timer alarm mask
**
** parameters:			Alarm mask setting
** Returned value:		None
**
*****************************************************************************/
void RTCSetAlarmMask( uint32_t AlarmMask )
{
    /*--- Set alarm mask ---*/
    LPC_RTC->AMR = AlarmMask;
    return;
}

/*****************************************************************************
** Function name:		RTCSetCntIncIt
**
** Descriptions:		Set RTC counter increment interrupt
**
** parameters:			counter increment interrupt setting
** Returned value:		None
**
*****************************************************************************/
void RTCSetCntIncIt( uint32_t CntIncIt )
{
    /*--- Set counter increment interrupt ---*/
    LPC_RTC->CIIR = CntIncIt;
    return;
}

/*****************************************************************************
**                            End Of File
******************************************************************************/

