#include "user_app_task.h"
#ifdef  TYPE_OPM
extern struct EPROM_DATA EPROM;           //����EPROM���ò����Ľṹ��
extern uint8 hardware_way[CHANNEL_NUM];  //��ʵ��Ӳ����λ
extern uint8 PD_INFO[120];
extern uint8 MD_INFO[120];
#endif



/*********************************************************************************************************
** ��������: TaskCollect
** ��������: R1��R2ͨ�����ʲɼ�
** �䡡��:   pdata ���񸽼Ӳ���(ָ��)
** �䡡��: ��
** ����ʱ�� :
********************************************************************************************************/
#ifdef  TYPE_OPM
void  TaskCollectA(void *pdata)
{
    uint8 i,input_channel;
    float  dBm[4];
    OS_CPU_SR  cpu_sr;

    for ( input_channel = 0 ; input_channel	< (CHANNEL_NUM/2) ;input_channel++ )
    {
        hardware_way[input_channel] = 4;        //��ʼ��ģ�⿪��λ��
    }
    OSTimeDly(100);
    ADC_int(400);                   //ADC��ʼ��
    OSTimeDly(100);
    while(1)
    {
        for(input_channel = 0 ; input_channel < (CHANNEL_NUM/2) ; input_channel ++)
        { 
            //ѡ��ڼ�·
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
             //-----------ͨ���������ã�������ر�------------//      
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
            {   //�رգ�Ĭ����-50.00
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
        hardware_way[input_channel] = 4;        //��ʼ��ģ�⿪��λ��
    }
    OSTimeDly(100);
    while(1)
    {
        for(input_channel = 8;input_channel < CHANNEL_NUM ; input_channel++)
        { 
            //ѡ��ڼ�·
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

            //-----------ͨ���������ã�������ر�------------//      
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
            {   //�رգ�Ĭ����-50.00
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
** ��������: TaskSwitch
** ��������: �澯
** �䡡��: pdata        ���񸽼Ӳ���(ָ��)
** �䡡��: ��
** ����ʱ�� :
** ˵��:  �⹦�ʺ�PING�жϡ�������ʱ���ֶ������Զ������л�
********************************************************************************************************/
void  TaskSwitch(void *pdata)
{
//    uint8 err;
    uint8 i,j;
    uint16 temp16;
    char AD_INFO[120];

    OS_CPU_SR  cpu_sr;
    
    OSTimeDly(2000+EPROM.Start_delay*1000);   //������ʱ
    
    while(1)
    {
    //��������16������POWER,��ֹ�����ڲ�ѯ����ʱ��AD���ڲɼ�
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
        {   //�������飬���ڲ�ѯ
            MD_INFO[i]=AD_INFO[i];
            if(i==(CHANNEL_NUM*6))
            {
                MD_INFO[i]='\0';
            }
        }
        OS_EXIT_CRITICAL();
/////////////////�澯����q_power///////////////////////////
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
        {   //�������飬���ڲ�ѯ
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
