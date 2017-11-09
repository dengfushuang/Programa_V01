#include "log.h"
#include "ADC.h"
#include "uart.h"
#include "uart0.h"
#include "lpc177x_8x_eeprom.h"


/*******************************************************************************************************
** 函数名称: Save_EEPROM_LOG()
** 功能描述: 存储日志
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void LOG_WRITE(struct LOG_DATA *LOG_DATA)
{
	uint16_t i, data_temp16;
	OS_CPU_SR cpu_sr;
	
	/***获取当前时刻***/
	UART1Write_Str((uint8_t *)"AT+CCLK?\r\n");
	while((LOG_DATA->time[0] = UART1Get()) != '\"')
	{
		i++;
		if(i == 2000)
		{
			UART0Write_Str((uint8_t *)"LOG WRITE FAIL\n");
			return;
		}
	}
	for(i=1;i<24;i++)
		LOG_DATA->time[i] = (char)UART1Get();
	LOG_DATA->time[23] = '\0';
	
	/***获取R1光功率***/
	if(power[0] >= 0)
	{
		LOG_DATA->R1_Power[0] = '+';
		data_temp16 = power[0]*100;
	}
	else
	{
		LOG_DATA->R1_Power[0] = '-';
		data_temp16 = (0 - power[0])*100;
	}
	LOG_DATA->R1_Power[1] = data_temp16/1000+'0';
	LOG_DATA->R1_Power[2] = data_temp16%1000/100+'0';
	LOG_DATA->R1_Power[3] = '.';
	LOG_DATA->R1_Power[4] = data_temp16%100/10+'0';
	LOG_DATA->R1_Power[5] = data_temp16%10+'0';
	LOG_DATA->R1_Power[6] = '\0';
	
	/***获取R2光功率***/
	if(power[1] >= 0)
	{
		LOG_DATA->R2_Power[0] = '+';
		data_temp16 = power[1]*100;
	}
	else
	{
		LOG_DATA->R2_Power[0] = '-';
		data_temp16 = (0 - power[1])*100;
	}
	LOG_DATA->R2_Power[1] = data_temp16/1000+'0';
	LOG_DATA->R2_Power[2] = data_temp16%1000/100+'0';
	LOG_DATA->R2_Power[3] = '.';
	LOG_DATA->R2_Power[4] = data_temp16%100/10+'0';
	LOG_DATA->R2_Power[5] = data_temp16%10+'0';
	LOG_DATA->R2_Power[6] = '\0';
	
	/***获取电源状态***/
	LOG_DATA->pwr_stat = POWER_STAT;
	
	//此处加10ms，是因为发送指令<BP01_X_ACC_1>时，指令保存数据时操作EEPROM
	//在此同时，光开关切换需要保存日志，也要操作了EEPROM，造成了复位。
  OSTimeDly(10);
  OS_ENTER_CRITICAL();  
	EPROM.LOG_NUM++;
	EEPROM_Write((LOG_ADDR + EPROM.LOG_NUM * 40)%64, (LOG_ADDR + EPROM.LOG_NUM * 40)/64, (uint8*)LOG_DATA, MODE_8_BIT, sizeof(struct LOG_DATA));
	OS_EXIT_CRITICAL();
	if(EPROM.LOG_NUM > 50)
		EPROM.LOG_NUM = 0;
	Save_To_EPROM((uint8 *)&EPROM.LOG_NUM, 1);
}



