#include "user_app_task.h"
#ifdef  TYPE_OPM
extern struct EPROM_DATA EPROM;           //保存EPROM设置参数的结构体
extern uint8 hardware_way[CHANNEL_NUM];  //真实的硬件档位
extern uint8 PD_INFO[120];
extern uint8 MD_INFO[120];
#endif



/*********************************************************************************************************
** 函数名称: TaskCollect
** 功能描述: R1、R2通道功率采集
** 输　入:   pdata 任务附加参数(指针)
** 输　出: 无
** 运算时间 :
********************************************************************************************************/
#ifdef  TYPE_OPM
void  TaskCollectA(void *pdata)
{
    uint8 i,input_channel;
    float  dBm[4];
    OS_CPU_SR  cpu_sr;

    for ( input_channel = 0 ; input_channel	< (CHANNEL_NUM/2) ;input_channel++ )
    {
        hardware_way[input_channel] = 4;        //初始化模拟开关位置
    }
    OSTimeDly(100);
    ADC_int(400);                   //ADC初始化
    OSTimeDly(100);
    while(1)
    {
        for(input_channel = 0 ; input_channel < (CHANNEL_NUM/2) ; input_channel ++)
        { 
            //选择第几路
            OS_ENTER_CRITICAL();
            ADC_CHANNEL1_IN( input_channel );
            OS_EXIT_CRITICAL(); 
            OSTimeDly(5);
            CHANNEL(input_channel,hardware_way[input_channel]);  
            OSTimeDly(50);
            for(i=0;i<4;i++)
            {
                dBm[i] = test_optics_collect(input_channel,hardware_way[input_channel]);
                OSTimeDly(2);
            }
            power[input_channel] = (dBm[0] + dBm[1]+ dBm[2]+ dBm[3])/4.0;
             //-----------通道开关设置，开启或关闭------------//      
            if(EPROM.way_switch[input_channel]==1)
            {
                if(power[input_channel]<=EPROM.q_power[input_channel])
                {
                    warn[input_channel]=1;      
                }
                else
                {
                    warn[input_channel]=0;
                }
             }
            else    
            {   //关闭，默认是-50.00
                power[input_channel] =-50.00;
                warn[input_channel]=0; 
            }
            //-----------------------------------------------//
            
            OSTimeDly(10);
        }

        
        OSTimeDly(10);
    }
}

void  TaskCollectB(void *pdata)
{
    uint8 i,input_channel,temp;
    float  dBm[4];
    OS_CPU_SR  cpu_sr;

    for ( input_channel = 8 ; input_channel <CHANNEL_NUM ; input_channel++ )
    {
        hardware_way[input_channel] = 4;        //初始化模拟开关位置
    }
    OSTimeDly(100);
    while(1)
    {
        for(input_channel = 8;input_channel < CHANNEL_NUM ; input_channel++)
        { 
            //选择第几路
            OS_ENTER_CRITICAL();
			temp = input_channel - 8;
            ADC_CHANNEL2_IN(temp);
            OS_EXIT_CRITICAL();  
            OSTimeDly(5);
            CHANNEL(input_channel,hardware_way[input_channel]);  
            OSTimeDly(50);
            for(i=0;i<4;i++)
            {
                dBm[i] = test_optics_collect_B(input_channel,hardware_way[input_channel]);
                OSTimeDly(2);
            }
            
            power[input_channel] = (dBm[0] + dBm[1]+ dBm[2]+ dBm[3])/4;

            //-----------通道开关设置，开启或关闭------------//      
            if(EPROM.way_switch[input_channel]==1)
            {
                if(power[input_channel]<=EPROM.q_power[input_channel])
                {
                    warn[input_channel]=1;      
                }
                else
                {
                    warn[input_channel]=0;
                }
             }
            else    
            {   //关闭，默认是-50.00
                power[input_channel] =-50.00;
                warn[input_channel]=0; 
            }
            //-----------------------------------------------//
            
            OSTimeDly(10);
        }

        OSTimeDly(10);
    }
}
/*********************************************************************************************************
** 函数名称: TaskSwitch
** 功能描述: 告警
** 输　入: pdata        任务附加参数(指针)
** 输　出: 无
** 运算时间 :
** 说明:  光功率和PING判断、回切延时、手动返回自动、光切换
********************************************************************************************************/
void  TaskSwitch(void *pdata)
{
//    uint8 err;
    uint8 i,j;
    uint16 temp16;
    char AD_INFO[120];

    OS_CPU_SR  cpu_sr;
    
    OSTimeDly(2000+EPROM.Start_delay*1000);   //开机延时
    
    while(1)
    {
    //用来保存16个功率POWER,防止主控在查询功率时，AD正在采集
        j=0;
        for(i=0;i<CHANNEL_NUM;i++)
        {
            if (power[i] >= 0)
            {
                AD_INFO[j] =  '+';
                j++;
                temp16 =  power[i] * 100;
            }
            else
            {
                AD_INFO[j] =  '-';
                j++;
                temp16 = (0 - power[i]) * 100;
            }
            
            AD_INFO[j] = temp16 / 1000 + '0';
            j++;
            AD_INFO[j] = temp16 % 1000 / 100 + '0';
            j++;
            AD_INFO[j] = '.';
            j++;
            AD_INFO[j] = temp16 % 100 / 10 + '0';
            j++;
            AD_INFO[j] = temp16 % 10 + '0';
            j++;
        }
        
        OS_ENTER_CRITICAL();
        for(i=0;i<(CHANNEL_NUM*6+1);i++)
        {   //存入数组，利于查询
            MD_INFO[i]=AD_INFO[i];
            if(i==(CHANNEL_NUM*6))
            {
                MD_INFO[i]='\0';
            }
        }
        OS_EXIT_CRITICAL();
/////////////////告警功率q_power///////////////////////////
        j=0;
        for(i=0;i<CHANNEL_NUM;i++)
        {
            if (EPROM.q_power[i] >= 0)
            {
                AD_INFO[j] =  '+';
                j++;
                temp16 =  EPROM.q_power[i] * 100;
            }
            else
            {
                AD_INFO[j] =  '-';
                j++;
                temp16 = (0 - EPROM.q_power[i]) * 100;
            }
            
            AD_INFO[j] = temp16 / 1000 + '0';
            j++;
            AD_INFO[j] = temp16 % 1000 / 100 + '0';
            j++;
            AD_INFO[j] = '.';
            j++;
            AD_INFO[j] = temp16 % 100 / 10 + '0';
            j++;
            AD_INFO[j] = temp16 % 10 + '0';
            j++;

        }
        
        OS_ENTER_CRITICAL();
        for(i=0;i<(CHANNEL_NUM*6+1);i++)
        {   //存入数组，利于查询
            PD_INFO[i]=AD_INFO[i];
            if(i==(CHANNEL_NUM*6))
            {
                PD_INFO[i]='\0';
            }
        }
        OS_EXIT_CRITICAL();
        
        OSTimeDly(20);
    }
}
#endif

#ifdef  TYPE_OS

void  TaskOS(void *pdata)
{
	
}
#endif
