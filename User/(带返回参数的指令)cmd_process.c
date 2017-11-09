/**********************************************************************************************************
*文件名：Cmd_process.c
*创建时间: 2012-10-18
*功能描述: 处理串口和TCP的设置和查询指令
*作   者 ：
*公   司 ：
**********************************************************************************************************/
#include <stdio.h>
#include <math.h>
#include <main.h>
#include <string.h>
#include "includes.h"
#include "tcp_server.h"
#include "httpd.h"
#include "uip.h"
#include "uart0.h"
#include "queue.h"
#include "lpc177x_8x_eeprom.h"
#include "timer.h"
#include "ADC.h"
#include "OSW.h"
//*************************************************************************/
//** 函数名称:  change_ascii_date(char *x, uint8 len ,uint8 *err)
//** 功能描述:  读取指令中的字符串，并且转换成16位的十进制数
//** 输　入:    *x     读取字符的起始指针
//**            len    读取字符的长度
//**	*err      返回错误类型
//** 输　出:    16位十进制数
//*************************************************************************/
uint16 change_ascii_date(char *x, uint8 len ,uint8 *err)
{
    uint16 sum=0;
    uint16 temp;
    uint8  i;
    for(; len; len--)
    {
        temp=1;
        for(i=0; i<(len-1); i++) 			    //构造10的n次方
        {
            temp=temp*10;
        }
        if( (*x-'0')>9 )
        {
            *err=1;                             //判断字符大小是否在0~9之内
            return 0;
        }
        sum += ( *x-'0') * temp;                //一位一位的累加
        x++;
    }

    *err=0;
    return sum;
}


uint16  Cmd_process( char* sprintf_buf )
{
    uint8  i;
    uint8  err;
    char   *cp,*cptemp,*cpdata1,*cpdata2;
    uint16  sprintf_len=0;
    uint8  link_num;
    uint8  RT;
    uint8  data_temp8;
    uint16 data_temp16;
    uint8  data_temp[6];
    uint8  ipaddr[4]= {0};
//    float  power_temp=0;
//	uint8  address_temp;

    if( strstr(&sprintf_buf[0],"<BP") != NULL )
    {
        //<BP_ADR_?>
        if( strstr((char*)&sprintf_buf[3],"_ADR_?>") != NULL )
        {
            sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_OK>",EPROM.address);
        }
        //<BP01_ADR_XX>  更改设备地址
        else if( sprintf_buf[3]==(EPROM.address%10/10+'0') && sprintf_buf[4]==(EPROM.address%10+'0') && sprintf_buf[5]=='_' )
        {
            if( ( cp=strstr((char*)&sprintf_buf[6],"ADR_"))!=NULL && sprintf_buf[12]=='>' )
            {
                cptemp=cp+4;
                data_temp16 = change_ascii_date(cptemp, 2, &err );
                if( data_temp16<100 && err==0 )             //判断数值的大小,且在字符0~9之间
                {
                    EPROM.address = data_temp16;
                    Save_To_EPROM((uint8 *)&EPROM.address, 1);
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_ADR_OK>",EPROM.address);
                }
                else  goto send_err;
            }

            //<BP01_RESET> 复位
            else if( strstr((char*)&sprintf_buf[6],"RESET>")!=NULL  )
            {
                sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_RESET_OK>",EPROM.address);
                Reset_Handler();   	    //实行软件复位 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }

            //<BP01_RESTORE>  恢复出厂设置
            else if( strstr((char*)&sprintf_buf[6],"RESTORE>") !=NULL )
            {
                restore_set();          //恢复出厂设置
                Reset_Handler();        //实行软件复位 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }

            //<BP01_KEY_?>  设置按键使用权限
            else if( (cp=strstr((char*)&sprintf_buf[6],"KEY_"))!=NULL && sprintf_buf[11]=='>' )
            {
                if(sprintf_buf[10]=='?' )
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_KEY_%u>",EPROM.address,EPROM.key_admin);
                else if( sprintf_buf[10]=='0' || sprintf_buf[10]=='1')
                {
                    EPROM.key_admin = sprintf_buf[10]-'0';
                    Save_To_EPROM((uint8 *)&EPROM.key_admin, 1);
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_KEY_%1u_OK>",EPROM.address,EPROM.key_admin);
                }
                else  goto send_err;
            }
            //<BP01_BEEONE_OFF>   单次关蜂鸣器
            else if( (cp=strstr((char*)&sprintf_buf[6],"BEEONE_OFF>"))!=NULL )
            {
                BEE_OFF;       //关蜂鸣器
                BEE_ON_flag=0; //单次关闭蜂鸣器
                sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_BEEONE_OFF_OK>",EPROM.address);
            }
            //<BP01_BEE_X>  蜂鸣器告警设置
            else if( (cp=strstr((char*)&sprintf_buf[6],"BEE_"))!=NULL && sprintf_buf[11]=='>' )
            {
                if(sprintf_buf[10]=='?')
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_BEE_%u>",EPROM.address,EPROM.BEEflag );
                else if( sprintf_buf[10]=='0' || sprintf_buf[10]=='1')
                {
                    EPROM.BEEflag = sprintf_buf[10]-'0';
                    Save_To_EPROM((uint8 *)&EPROM.BEEflag, 1);
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_BEE_OK>",EPROM.address);
                }
                else  goto send_err;
            }
            //<BP01_LCDT_X>  设置LCD背光
            else if( (cp=strstr((char*)&sprintf_buf[6],"LCDT_"))!=NULL && sprintf_buf[12]=='>' )
            {
                if(sprintf_buf[11]=='?')
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_LCDT_%u>",EPROM.address,EPROM.LCDT );
                else if( sprintf_buf[11]>='1' && sprintf_buf[11]<='6' )
                {
                    EPROM.LCDT = sprintf_buf[11]-'0';
                    Save_To_EPROM((uint8 *)&EPROM.LCDT, 1);
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_LCDT_OK>",EPROM.address);
                }
                else  goto send_err;
            }
            //<BP01_X_M_Y>   手/自动模式设置. 各链路工作模式设置和查询
            else if( (cp=strstr((char*)&sprintf_buf[7],"_M_"))!=NULL && sprintf_buf[11]=='>' )
            {
                if( sprintf_buf[6]>= '1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    if(sprintf_buf[10]=='?')
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_M_%1u>", \
                                              EPROM.address, sprintf_buf[6], EPROM.Autoflag[sprintf_buf[6]-'1'] );
                    }
                    else if( sprintf_buf[10]=='0' || sprintf_buf[10]=='1' )
                    {
                        EPROM.Autoflag[sprintf_buf[6]-'1'] = sprintf_buf[10]-'0';
                        Save_To_EPROM((uint8 *)&EPROM.Autoflag[sprintf_buf[6]-'1'], 1);

                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_M_%c_OK>", \
                                              EPROM.address, sprintf_buf[6],sprintf_buf[10]);
                    }
                    else  goto send_err;
                }
                else  goto send_err;
            }
            //<BP01_X_S_Y>   切换通道   各链路主、旁状态设置和查询
            else if( (cp=strstr((char*)&sprintf_buf[7],"_S_"))!=NULL && sprintf_buf[11]=='>' )
            {
                if( sprintf_buf[6]>= '1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    if(sprintf_buf[10]=='?')
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_S_%1u>", \
                                              EPROM.address, sprintf_buf[6], EPROM.Autoflag[sprintf_buf[6]-'1'] );
                    }
                    else if( sprintf_buf[10]=='0' || sprintf_buf[10]=='1' )
                    {
                        //先更改为手动模式
                        EPROM.Autoflag[sprintf_buf[6]-'1'] = 0;
                        Save_To_EPROM((uint8 *)&EPROM.Autoflag[sprintf_buf[6]-'1'], 1);

                        if( sprintf_buf[10]=='0') BypassSwitch(sprintf_buf[6]-'1');
                        else                      MostSwitch(sprintf_buf[6]-'1');

                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_S_%c_OK>", \
                                              EPROM.address, sprintf_buf[6],sprintf_buf[10]);
                    }
                    else  goto send_err;
                }
                else  goto send_err;
            }

            //<BP01_X_ACC_Y> 设置或查询  回切方式设置.
            else if( (cp=strstr((char*)&sprintf_buf[7],"_ACC_"))!=NULL && sprintf_buf[13]=='>' )
            {
                if( sprintf_buf[6]>= '1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    if(sprintf_buf[12]=='?')
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_ACC_%1u>", \
                                              EPROM.address, sprintf_buf[6],EPROM.accessflag[sprintf_buf[6]-'1'] );
                    }
                    else if( (sprintf_buf[12]=='0' || sprintf_buf[12]=='1') )
                    {
                        EPROM.accessflag[sprintf_buf[6]-'1'] = sprintf_buf[12]-'0';
                        Save_To_EPROM((uint8 *)&EPROM.accessflag[sprintf_buf[6]-'1'], 1);

                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_ACC_%c_OK>", \
                                              EPROM.address,sprintf_buf[6],sprintf_buf[12]);
                    }
                    else  goto send_err;
                }
                else  goto send_err;
            }

            //<BP01_X_Q_YYYY> 设置或查询 自动回切延时
            else if( (cp=strstr((char*)&sprintf_buf[7],"_Q_"))!=NULL && (sprintf_buf[11]=='>' || sprintf_buf[14]=='>') )
            {
                if( sprintf_buf[6]>='1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    if(sprintf_buf[10]=='?')    //查询命令
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_Q_%04u>",
                                              EPROM.address, sprintf_buf[6] ,EPROM.BackOSW_Delay[sprintf_buf[6]-'1']);
                    }
                    else
                    {
                        data_temp16=change_ascii_date((char*)&sprintf_buf[10], 4 ,&err);
                        if( data_temp16<=9999 && err==0 )    //判断是否是小于9999
                        {
                            EPROM.BackOSW_Delay[sprintf_buf[6]-'1'] = data_temp16;
                            Save_To_EPROM((uint8 *)&EPROM.BackOSW_Delay[sprintf_buf[6]-'1'], 2);
                            sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_Q_%04u_OK>",EPROM.address,sprintf_buf[6],data_temp16);
                        }
                        else  goto send_err;
                    }
                }
                else  goto send_err;
            }

            //<BP01_X_R_YYYY>  设置或查询 手动模式返回自动模式延时
            else if( (cp=strstr((char*)&sprintf_buf[7],"_R_"))!=NULL && (sprintf_buf[11]=='>' || sprintf_buf[14]=='>') )
            {
                if( sprintf_buf[6]>='1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    if(sprintf_buf[10]=='?')    //查询命令
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_R_%04u>",
                                              EPROM.address, sprintf_buf[6] ,EPROM.Auto_Manual_delay[sprintf_buf[6]-'1']);
                    }
                    else
                    {
                        data_temp16=change_ascii_date((char*)&sprintf_buf[10], 4 ,&err);
                        if( data_temp16<=9999 && err==0 )    //判断是否是小于9999
                        {
                            EPROM.Auto_Manual_delay[sprintf_buf[6]-'1'] = data_temp16;
                            Save_To_EPROM((uint8 *)&EPROM.Auto_Manual_delay[sprintf_buf[6]-'1'], 2);
                            sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_R_%04u_OK>",EPROM.address,sprintf_buf[6],data_temp16);
                        }
                        else  goto send_err;
                    }
                }
                else  goto send_err;
            }
            //<BP01_SY_XXXX> 设置或查询 开机延时
            else if( (cp=strstr((char*)&sprintf_buf[6],"SY_"))!=NULL && (sprintf_buf[10]=='>' || sprintf_buf[13]=='>') )
            {
                cptemp=cp+3;
                if(*cptemp=='?')    //查询命令
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_SY_%04u>",EPROM.address, EPROM.Start_delay);
                }
                else
                {
                    data_temp16=change_ascii_date(cptemp, 4 ,&err);
                    if( data_temp16<=9999 && err==0 )    //判断是否是小于9999
                    {
                        EPROM.Start_delay=data_temp16;
                        Save_To_EPROM((uint8 *)&EPROM.Start_delay, 2);
                    }
                    else  goto send_err;

                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_SY_%04u_OK>",EPROM.address,EPROM.Start_delay);
                }
            }
            //<BP01_X_LINKSTS_?>   设置或查询当前某链路状态
            //返回:<BP01_X_LINKSTS_Y_Z>
            else if( (cp=strstr((char*)&sprintf_buf[7],"_LINKSTS_?"))!=NULL &&  sprintf_buf[17]=='>')
            {
                if( sprintf_buf[6]>='1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_LINKSTS_%1u_%1u>",EPROM.address, \
                                          sprintf_buf[6],EPROM.Autoflag[sprintf_buf[6]-'1'],OSW_start[sprintf_buf[6]-'1']);
                }
                else  goto send_err;
            }
            //<BP01_XRY_THRESHOLD_XXX.XX> 设置或查询切换功率阀值
            //<BP01_XRY_THRESHOLD_-30.00>
            //<BP01_XRY_THRESHOLD_?>
            else if( (cp=strstr((char*)&sprintf_buf[9],"_THRESHOLD_"))!=NULL &&  (sprintf_buf[21]=='>' || sprintf_buf[26]=='>' ))
            {
                link_num = sprintf_buf[6]-'1'; 
                RT = sprintf_buf[8]-'1'; 
                cpdata1 = &sprintf_buf[20];
                cpdata2 = &sprintf_buf[24];
                
                if( (link_num >= 0 && link_num<=( CHANNEL_NUM -1 )) && (RT>=0 && RT<=1) )
                {
                    if(*cpdata1=='?')
                    {
                        if(EPROM.q_power[link_num*2+RT] >= 0)
                        {
                            data_temp[0] = '+';
                            data_temp16 =  EPROM.q_power[link_num*2+RT]*100;
                        }
                        else
                        {
                            data_temp[0] = '-';
                            data_temp16 = (0 - EPROM.q_power[link_num*2+RT])*100;
                        }
                        data_temp[1] = data_temp16/1000+'0';
                        data_temp[2] = data_temp16%1000/100+'0';
                        data_temp[3] = '.';
                        data_temp[4] = data_temp16%100/10+'0';
                        data_temp[5] = data_temp16%10+'0';

                        sprintf_len = sprintf((char *)cpdata1,"%s>",data_temp);                                               
                        sprintf_len+=21;
                    }
                    else if(*cpdata1=='-')      
                    {
                        data_temp8=change_ascii_date( cpdata1+1, 2, &err );
                        if( data_temp8<=99 && err==0 )        
                        {
                            data_temp[0]=data_temp8;
                        }
                        else  goto send_err;

                        data_temp8=change_ascii_date( cpdata2, 2, &err );
                        if( data_temp8<=99 && err==0 )        
                        {
                            data_temp[1]=data_temp8;
                        }
                        else  goto send_err;   
                        
                       EPROM.q_power[link_num*2+RT] = -(data_temp[0]+(data_temp[1]*0.01));
                       Save_To_EPROM((uint8 *)&EPROM.q_power[link_num*2+RT], 4); 

                       sprintf_len = sprintf((char *)cpdata2+2,"_OK>");                                               
                       sprintf_len+=26; 

                    }
                    else if(*cpdata1=='+')
                    {
                        data_temp8=change_ascii_date( cpdata1+1, 2, &err );
                        if( data_temp8<=99 && err==0 )        
                        {
                            data_temp[0]=data_temp8;
                        }
                        else  goto send_err;

                        data_temp8=change_ascii_date( cpdata2, 2, &err );
                        if( data_temp8<=99 && err==0 )        
                        {
                            data_temp[1]=data_temp8;
                        }
                        else  goto send_err;   
                        
                       EPROM.q_power[link_num*2+RT] = data_temp[0]+(data_temp[1]*0.01);
                       Save_To_EPROM((uint8 *)&EPROM.q_power[link_num*2+RT], 4);  

                       sprintf_len = sprintf((char *)cpdata2+2,"_OK>");                                               
                       sprintf_len+=26; 
                       
                    }
                    else  goto send_err;
                }
                else  goto send_err;
            }
            
            //<BP01_XRY_WAVE_?> 设置或查询 波长
            //<BP01_XRY_WAVE_1> 
            else if( (cp=strstr(&sprintf_buf[9],"_WAVE_"))!=NULL && sprintf_buf[16]=='>')
            {
                link_num = sprintf_buf[6]-'1'; 
                RT = sprintf_buf[8]-'1'; 
                cpdata1 = &sprintf_buf[15];
                if( (link_num >= 0 && link_num<=( CHANNEL_NUM -1 )) && (RT>=0 && RT<=1) )
                {
                    if( *cpdata1=='?' )
                    {
                        *cpdata1=EPROM.wavelength[link_num*2+RT]+'0';
                        sprintf_len = 17;
                    }
                    else
                    {
                       EPROM.wavelength[link_num*2+RT] = *cpdata1-'0';
                       Save_To_EPROM((uint8 *)&EPROM.wavelength[link_num*2+RT], 1);  
                       
                       sprintf_len = sprintf((char *)cpdata1+1,"_OK>");                                               
                       sprintf_len+=20;                    
                    
                    }
                }
                else  goto send_err;
            }        
            
            //<BP01_X_POWER_?>  查询 当前光功率 和 波长
            //返回 <BP01_X_POWER_-50.00_1310_-50.00_1550>
            else if( (cp=strstr(&sprintf_buf[7],"_POWER_?>"))!=NULL && sprintf_buf[15]=='>')
            {
                link_num = sprintf_buf[6]-'1';
                if( link_num >= 0 && link_num <= ( CHANNEL_NUM -1 ) )
                {
                    //R1 功率 和 波长
                    if(power[link_num] >= 0)
                    {
                        data_temp[0] = '+';
                        data_temp16 =  power[link_num*2]*100;
                    }
                    else
                    {
                        data_temp[0] = '-';
                        data_temp16 = (0 - power[link_num*2])*100;
                    }
                    data_temp[1] = data_temp16/1000+'0';
                    data_temp[2] = data_temp16%1000/100+'0';
                    data_temp[3] = '.';
                    data_temp[4] = data_temp16%100/10+'0';
                    data_temp[5] = data_temp16%10+'0';

                    if(EPROM.wavelength[link_num*2] == 0)
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%1u_POWER_%s_1310" ,\
                                              EPROM.address,link_num+1,data_temp);
                    }
                    else
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%1u_POWER_%s_1550" ,\
                                              EPROM.address,link_num+1,data_temp);
                    }
                    //R2 功率 和 波长
                    if(power[link_num] >= 0)
                    {
                        data_temp[0] = '+';
                        data_temp16 =  power[link_num*2+1]*100;
                    }
                    else
                    {
                        data_temp[0] = '-';
                        data_temp16 = (0 - power[link_num*2+1])*100;
                    }
                    data_temp[1] = data_temp16/1000+'0';
                    data_temp[2] = data_temp16%1000/100+'0';
                    data_temp[3] = '.';
                    data_temp[4] = data_temp16%100/10+'0';
                    data_temp[5] = data_temp16%10+'0';

                    if(EPROM.wavelength[link_num*2+1] == 0)
                    {
                        sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],"_%s_1310>" ,data_temp);
                    }
                    else
                    {
                        sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],"_%s_1550>" ,data_temp);
                    }
                }
                else goto  send_err;
            }
            //<BP01_POWER_?>   工作电源查询 <BP01_POWER_12_11>
            //<BP01_POWER_12>  电源配置 <BP01_OK>
            else if( (cp=strstr((char*)&sprintf_buf[6],"POWER_"))!=NULL && (sprintf_buf[13]=='>' || sprintf_buf[14]=='>'))
            {
                if(sprintf_buf[12]=='?')
                {
                    POW1=POW1_STATE;
                    POW1=POW2_STATE;
                    if(POW1==0)POW1=1;
                    else	POW1=0;
                    if(POW2==0)POW2=1;
                    else	POW2=0;
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_POWER_%1u%1u%1u%1u>", \
                                          EPROM.address, EPROM.POW1flag, EPROM.POW2flag,POW1,POW2 );
                }
                else if( (sprintf_buf[12]=='0' || sprintf_buf[12]=='1') ||
                         (sprintf_buf[13]=='0' || sprintf_buf[13]=='1') )
                {
                    EPROM.POW1flag =  sprintf_buf[12]-'0';
                    Save_To_EPROM((uint8 *)&EPROM.POW1flag, 1);

                    EPROM.POW2flag =  sprintf_buf[13]-'0';
                    Save_To_EPROM((uint8 *)&EPROM.POW2flag, 1);
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_POWER_OK>",EPROM.address);
                }
                else
                {
                    goto send_err;
                }

            }

            //<BP01_BAUD_X>  设置串口波特率  1:2400 2:4800 3:9600 4:14400 5:19200 6:38400 7:56000 8:57600 9:115200
            else if( strstr((char*)&sprintf_buf[6],"BAUD_") !=NULL && sprintf_buf[12]=='>')
            {
                if(sprintf_buf[11]=='?')
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_BAUD_%1u>",\
                                          EPROM.address,EPROM.BPS);
                }
                else if( sprintf_buf[11]>='1' && sprintf_buf[11]<='9')
                {
                    EPROM.BPS = sprintf_buf[11]-'0';
                    Save_To_EPROM((uint8 *)&EPROM.BPS, 1);

                    sprintf((char *)&sprintf_buf[12],"_OK>");                 
                    sprintf_len=17;
                    
                    //sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_BAUD_%c_OK>",\
                                          //EPROM.address,sprintf_buf[11]);
                }
                else  goto send_err;
            }

            //<BP01_IP_XXX_XXX_XXX_XXX>   设置或查询IP 地址
            else if( (cp=strstr(&sprintf_buf[6],"IP_"))!=NULL )
            {
                if(*(cp+4)!='>' && *(cp+18)!='>')  goto send_err;

                cptemp=cp+3;
                if(*cptemp=='?')  //查询命令
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_IP_%03u_%03u_%03u_%03u>",
                                          EPROM.address,
                                          htons(EPROM.hostaddr[0]) >> 8,
                                          htons(EPROM.hostaddr[0]) & 0xff,
                                          htons(EPROM.hostaddr[1]) >> 8,
                                          htons(EPROM.hostaddr[1]) & 0xff);
                }
                else
                {
                    for(i=0; i<4; i++)        					//读取4个字节的参数
                    {
                        data_temp16=change_ascii_date( cptemp, 3, &err );
                        if( data_temp16<=255 && err==0 )        //判断是否符合IP的格式
                        {
                            data_temp[i]=(uint8)data_temp16;
                            cptemp+=4;
                        }
                        else  goto send_err;
                    }
                    EEPROM_Write_Str((uint8 *)&EPROM.hostaddr, data_temp, 4);

                    sprintf((char *)&sprintf_buf[24],"_OK>");                 
                    sprintf_len=29;
                }
            }

            //<BP01_GW_XXX_XXX_XXX_XXX>  设置或查询 网关地址
            else if( (cp=strstr(&sprintf_buf[6],"GW_"))!=NULL )
            {
                if(*(cp+4)!='>' && *(cp+18)!='>')  goto send_err;
                cptemp=cp+3;
                if(*cptemp=='?')  //查询命令
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_GW_%03u_%03u_%03u_%03u>",
                                          EPROM.address,
                                          htons(EPROM.draddr[0]) >> 8,
                                          htons(EPROM.draddr[0]) & 0xff,
                                          htons(EPROM.draddr[1]) >> 8,
                                          htons(EPROM.draddr[1]) & 0xff);
                }
                else
                {
                    for(i=0; i<4; i++)   //读取4个字节的参数
                    {
                        data_temp16=change_ascii_date( cptemp, 3, &err );
                        if( data_temp16<=255 && err==0 )   //判断是否符合GW的格式
                        {
                            data_temp[i]=(uint8)data_temp16;
                            cptemp+=4;
                        }
                        else  goto send_err;
                    }
                    EEPROM_Write_Str((uint8 *)&EPROM.draddr , data_temp, 4);
                    
                    sprintf((char *)&sprintf_buf[24],"_OK>");                 
                    sprintf_len=29;
                }
            }

            //<BP01_SM_XXX_XXX_XXX_XXX>  设置或查询 掩码地址
            else if( (cp=strstr(&sprintf_buf[6],"SM_"))!=NULL )
            {
                if(*(cp+4)!='>' && *(cp+18)!='>')  goto send_err;
                cptemp=cp+3;
                if(*cptemp=='?')  //查询命令
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_SM_%03u_%03u_%03u_%03u>",
                                          EPROM.address,
                                          htons(EPROM.netmask[0]) >> 8,
                                          htons(EPROM.netmask[0]) & 0xff,
                                          htons(EPROM.netmask[1]) >> 8,
                                          htons(EPROM.netmask[1]) & 0xff);
                }
                else
                {
                    for(i=0; i<4; i++)   //读取4个字节的参数
                    {
                        data_temp16=change_ascii_date( cptemp, 3, &err );
                        if( data_temp16<=255 && err==0 )  //判断是否符合SM的格式
                        {
                            data_temp[i]=(uint8)data_temp16;
                            cptemp+=4;
                        }
                        else  goto send_err;
                    }
                    EEPROM_Write_Str((uint8 *)&EPROM.netmask, data_temp, 4);
                    
                    sprintf((char *)&sprintf_buf[24],"_OK>" );                 
                    sprintf_len=29;
                }
            }

            //<BP01_PIPX_XXX_XXX_XXX_XXX>  设置或查询 PING 的目标地址
            else if( (cp=strstr(&sprintf_buf[6],"PIP"))!=NULL )
            {
                if(*(cp+6)!='>' && *(cp+20)!='>')  goto send_err;
                cptemp=cp+5;
                if( sprintf_buf[9]>='1' && sprintf_buf[9]<= ( CHANNEL_NUM +'0' ) )
                {
                    if( *cptemp=='?' )   //查询命令
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_PIP%c_%03u_%03u_%03u_%03u>",
                                              EPROM.address,
                                              sprintf_buf[9],                // 通道序号
                                              htons(uip_pingaddr[sprintf_buf[9]-'1'][0]) >> 8,
                                              htons(uip_pingaddr[sprintf_buf[9]-'1'][0]) & 0xff,
                                              htons(uip_pingaddr[sprintf_buf[9]-'1'][1]) >> 8,
                                              htons(uip_pingaddr[sprintf_buf[9]-'1'][1]) & 0xff);
                    }
                    else
                    {
                        for(i=0; i<4; i++)      //读取4个字节的参数
                        {
                            data_temp16=change_ascii_date( cptemp, 3, &err );
                            if( data_temp16<=255 && err==0 )  //判断是否符合PIP的格式
                            {
                                data_temp[i]=(uint8)data_temp16;
                                cptemp+=4;
                            }
                            else  goto send_err;
                        }
                        //马上更新PING IP
                        uip_ipaddr(ipaddr,data_temp[0],data_temp[1],data_temp[2],data_temp[3]);
                        uip_setpingaddr( (sprintf_buf[9]-'1'), ipaddr );

                        EEPROM_Write_Str((uint8 *)&EPROM.pingaddr[sprintf_buf[9]-'1'], data_temp, 4);

                        sprintf((char *)&sprintf_buf[26],"_OK>");                 
                        sprintf_len=31;
                    }
                }
                else  goto send_err;
            }

            //<BP01_TCPP_XXXXX>  设置或查询  TCP 连接端口
            else if( (cp=strstr(&sprintf_buf[6],"TCPP_"))!=NULL )
            {
                if(*(cp+6)!='>' && *(cp+10)!='>')  goto send_err;
                cptemp=cp+5;
                if(*cptemp=='?')   //查询命令
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_TCPP_%05u>",EPROM.address, EPROM.TCP_lport);
                }
                else
                {
                    data_temp16=change_ascii_date(cptemp, 5 ,&err);
                    if( data_temp16<=65534 && err==0 )   //判断是否符合HI的格式
                    {
                        //EPROM.TCP_lport = data_temp16;
                        EEPROM_Write_Str((uint8 *)&EPROM.TCP_lport,(uint8*)&data_temp16, 2);
                    }
                    else  goto send_err;
                   
                    sprintf((char *)&sprintf_buf[16],"_OK>" );                 
                    sprintf_len=21;                   
                }
            }

            //<BP01_X_HI_YYYY>  设置或查询 发ICMP包的时间间隔
            else if( (cp=strstr(&sprintf_buf[7],"_HI_"))!=NULL && (sprintf_buf[12]=='>' || sprintf_buf[15]=='>'))
            {
                if( sprintf_buf[6]>='1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    cptemp=&sprintf_buf[11];
                    if(*cptemp=='?')    //查询命令
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_HI_%04u>",\
                                              EPROM.address, sprintf_buf[6], EPROM.Send_ICMP_interval[ sprintf_buf[6]-'1' ] );
                    }
                    else if( sprintf_buf[6]>='1' && sprintf_buf[6]<='4' )
                    {
                        data_temp16=change_ascii_date(cptemp, 4 ,&err);
                        if( data_temp16<=9999 && err==0 )   //判断是否符合HI的格式
                        {
                            EPROM.Send_ICMP_interval[sprintf_buf[6]-'1']=data_temp16; //马上更新
                        }
                        else  goto send_err;

                        EEPROM_Write_Str((uint8 *)&EPROM.Send_ICMP_interval[ sprintf_buf[6]-'1' ],(uint8 *)&data_temp16,2);

                        sprintf((char *)&sprintf_buf[15],"_OK>" );                 
                        sprintf_len=20;   
                        
                    }
                }
                else goto  send_err;
            }

            //<BP01_X_PO_YYYY>   设置或查询 收心跳数据包的超时时间
            else if( (cp=strstr(&sprintf_buf[7],"_PO_"))!=NULL && (sprintf_buf[12]=='>' || sprintf_buf[15]=='>'))
            {
                if( sprintf_buf[6]>='1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    cptemp=&sprintf_buf[11];
                    if(*cptemp=='?')    //查询命令
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_PO_%04u>",\
                                              EPROM.address, sprintf_buf[6], EPROM.time_out[ sprintf_buf[6]-'1' ] );
                    }
                    else if( sprintf_buf[6]>='1' && sprintf_buf[6]<='4' )
                    {
                        data_temp16=change_ascii_date(cptemp, 4 ,&err);
                        if( data_temp16<=9999 && err==0 )   //判断是否符合PO的格式
                        {
                            EPROM.time_out[sprintf_buf[6]-'1']=data_temp16; //马上更新
                        }
                        else  goto send_err;

                        EEPROM_Write_Str((uint8 *)&EPROM.time_out[ sprintf_buf[6]-'1' ],(uint8 *)&data_temp16,2);

                        sprintf((char *)&sprintf_buf[15],"_OK>" );                 
                        sprintf_len=20;                   
                    }
                }
                else goto  send_err;
            }
            //<BP01_X_LP_YYYY> 设置或查询 连续丢数据包个数切换到旁路
            else if( (cp=strstr(&sprintf_buf[7],"_LP_"))!=NULL && (sprintf_buf[12]=='>' || sprintf_buf[15]=='>'))
            {
                if( sprintf_buf[6]>='1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    cptemp=&sprintf_buf[11];
                    if(*cptemp=='?')    //查询命令
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_LP_%04u>",\
                                              EPROM.address, sprintf_buf[6], EPROM.lost_max[ sprintf_buf[6]-'1' ] );
                    }
                    else if( sprintf_buf[6]>='1' && sprintf_buf[6]<='4' )
                    {
                        data_temp16=change_ascii_date(cptemp, 4 ,&err);
                        if( data_temp16<=9999 && err==0 )   //判断是否符合PO的格式
                        {
                            EPROM.lost_max[sprintf_buf[6]-'1']=data_temp16; //马上更新
                        }
                        else  goto send_err;

                        EEPROM_Write_Str((uint8 *)&EPROM.lost_max[ sprintf_buf[6]-'1' ],(uint8 *)&data_temp16,2);

                        sprintf((char *)&sprintf_buf[15],"_OK>" );                 
                        sprintf_len=20;                           
                    }
                }
                else goto  send_err;
            }

            //<BP01_X_PROTECT_?>   设置或查询 光开关保护功能
            else if( (cp=strstr(&sprintf_buf[7],"_PROTECT_"))!=NULL && sprintf_buf[17]=='>')
            {
                if( sprintf_buf[6]>='1' && sprintf_buf[6]<= ( CHANNEL_NUM +'0' ) )
                {
                    if(sprintf_buf[16]=='?')
                    {
                        sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_%c_PROTECT_%1u>",EPROM.address,sprintf_buf[6],EPROM.PROTECT[sprintf_buf[6]-'1']);
                    }
                    else if( sprintf_buf[16]=='0' || sprintf_buf[16]=='1' )
                    {
                        EPROM.PROTECT[sprintf_buf[6]-'1']=sprintf_buf[16]-'0';
                        Save_To_EPROM((uint8 *)&EPROM.PROTECT[sprintf_buf[6]-'1'], 1);

                        sprintf((char *)&sprintf_buf[17],"_OK>" );                 
                        sprintf_len=22;                          
                    }
                    else goto  send_err;
                }
                else  goto  send_err;
            }

            //<BP01_LINK_?>  查询链路数
            else if( (cp=strstr(&sprintf_buf[6],"LINK_"))!=NULL )
            {
                if( sprintf_buf[11]== '?' )
                {
                    sprintf_len = sprintf((char *)sprintf_buf,"<BP%02u_LINK_%1u>",EPROM.address, CHANNEL_NUM);
                }
                else  goto  send_err;
            }
            else  goto  send_err;
        }
        else  goto  send_err;
    }

    //************************************************************************************
    //************************************以下是内部指令********************************
    //************************************************************************************
    else if( (cp=strstr(&sprintf_buf[0],"<RESTORE>"))!=NULL )
    {
        restore_set();
        sprintf_len = sprintf((char *)sprintf_buf,"<RESTORE_OK>");

        //Reset_CMD = 1;
        Reset_Handler();
    }
    //<SET_MAC_48_A6_D2_00_00_00>   设置或查询MAC
    else if( (cp=strstr(&sprintf_buf[0],"<SET_MAC_"))!=NULL )
    {
        if(*(cp+10)!='>' && *(cp+26)!='>')  goto send_err;

        cptemp=cp+9;
        for(i=0; i<6; i++)
        {
            if( *cptemp >= '0' && *cptemp <= '9' )
                data_temp[i] = (*cptemp - '0')*16;
            else if( *cptemp >= 'A' && *cptemp <= 'F' )
                data_temp[i] = (*cptemp - 'A' )*16+160;
            else  goto send_err;

            cptemp++;
            if( *cptemp >= '0' && *cptemp <= '9' )
                data_temp[i] += (*cptemp - '0');
            else if( *cptemp >='A' && *cptemp <='F' )
                data_temp[i] += (*cptemp - 'A' ) + 10;
            else  goto send_err;

            cptemp+=2;
        }
        EEPROM_Write_Str( (uint8 *)EPROM.MAC_addr, data_temp, 6);
        OSTimeDly (1);
        sprintf_len = sprintf((char *)sprintf_buf,"<SET_MAC_SETOK>");

    }
    else if( (cp=strstr(&sprintf_buf[0],"<MAC_?>"))!=NULL )
    {
        sprintf_len = sprintf((char *)sprintf_buf,"<MAC_%02X_%02X_%02X_%02X_%02X_%02X>",
                              EPROM.MAC_addr[0],
                              EPROM.MAC_addr[1],
                              EPROM.MAC_addr[2],
                              EPROM.MAC_addr[3],
                              EPROM.MAC_addr[4],
                              EPROM.MAC_addr[5] );
    }
    else
    {
send_err:
        sprintf_len = sprintf((char *)sprintf_buf,"<CMD_ERR>");
    }

    return sprintf_len;
}

