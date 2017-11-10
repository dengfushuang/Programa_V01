/**********************************************************************************************************
*文件名：Cmd_process.c
*创建时间: 2012-10-18
*功能描述: 处理串口和TCP的设置和查询指令
*作   者 ：
*公   司 ：
**********************************************************************************************************/
#include "cmd_process.h"
#include "uart1.h"
#include "uart2.h"
#include "lpc177x_8x_eeprom.h"
#include "ADC.h"

#ifdef TYPE_OPM
	   uint8  i2c_set;
	   uint8 re_set;
       uint8 info_set;
       uint8  c_set[4];
       uint8  w_set[4];
	   uint8  a_set[4];
extern uint8 PD_INFO[120];
extern uint8 MD_INFO[120];
#endif


uint16 cmd_process( char* sprintf_buf )
{
	  uint8  i;
	  uint16 sprintf_len = 0;
#ifdef TYPE_FSW
	  uint8 temp;
#endif	
#ifdef TYPE_OPM	
	  char  *cp,*cp1,*cp2;
	  uint8  link_num;
      uint8  data_temp;
	  uint32 data_temp2;
      uint8  temp_arry[8];
      uint8 err;	
#endif
    if( strstr(&sprintf_buf[0], "<AD") != NULL )
    {
				if( sprintf_buf[3]==(ADDR/10+'0') && sprintf_buf[4]==(ADDR%10+'0') && sprintf_buf[5]=='_' )
				{
						//<AD01_INFO_?>  <AD01_INFO_12345678>
					  if( (strstr((char*)&sprintf_buf[6], "INFO_") != NULL) && (sprintf_buf[12] == '>' || sprintf_buf[19] == '>') )
						{
							if(sprintf_buf[11] == '?')
							{
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_INFO_%s_%s>\n", ADDR, type[EPROM.TYPE], EPROM.MN); 
							}
						    else
							{
								for(i=0; i<8; i++)
								{
									EPROM.MN[i] = sprintf_buf[11+i];
								}
										
								EPROM.MN[8] = '\0';
								Save_To_EPROM((uint8_t *)&EPROM.MN[0], 9);
								sprintf((char *)cfm, "<%02d_%01u_%s>\n", ADDR, EPROM.TYPE, EPROM.MN);
								UART1Write_Str((uint8 *)cfm);
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_INFO_OK>\n", ADDR);
							}
						}
						
						//<AD01_RESET> 设备复位
						else if( strstr((char*)&sprintf_buf[6], "RESET>")!=NULL && (sprintf_buf[11] == '>') )
						{
								OFFLINE;
								sprintf_len = sprintf(sprintf_buf, "<AD%02u_RESET_OK>\n", ADDR);
								UART2Write_Str((uint8 *)sprintf_buf);
							  OSTimeDly(1000);
								Reset_Handler();
						}
						
						//<AD01_SV_?> 查询插盘软件版本
						else if( strstr((char*)&sprintf_buf[6], "SV_?>")!=NULL )
						{
							sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_SV%s>\n", ADDR, SVersion);
						}		
						
						//<AD01_RESUME>  恢复出厂设置
						else if( strstr((char*)&sprintf_buf[6], "RESUME>") != NULL )
						{
								UART2Write_Str((uint8 *)"<RESUME_OK>\n");
								restore_set();          //恢复出厂设置
								Reset_Handler();        //实行软件复位 
						}
					
					#ifdef TYPE_FSW
						//<AD01_MAX_?>
						else if( strstr((char*)&sprintf_buf[6], "MAX_?>") != NULL )
						{
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_MAX_%03u>\n", ADDR, EPROM.maxch);
								return sprintf_len;
						}

						//<AD01_F_YY>
						else if( (strstr((char *)&sprintf_buf[6], "F_") != NULL) && (sprintf_buf[10] == '>') )
						{
								stime = (sprintf_buf[8] - '0')*10 + (sprintf_buf[9] - '0');
								if(stime < 100)
								{
										if(stime == 0)
												stime = 1;
										scan_flag = 1;
										sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_F_OK>\n", ADDR);
										return sprintf_len;
								}
								else goto ERROR;
						}
						
						//<AD01_S_XX> 光开关切换
						else if( (strstr((char*)&sprintf_buf[6], "S_") != NULL) && (sprintf_buf[10] == '>') )
						{
								OPS_CH = ((sprintf_buf[8] - '0')*10 + (sprintf_buf[9] - '0')) - 1;
								if(OPS_CH <= 99)
								{
										for(i=0; i<7; i++)
										{
												temp = (OPS_CH >> i) & 0x01;
												if(temp)
														LPC_GPIO4->SET |= (1<<(5+i));
												else
														LPC_GPIO4->CLR |= (1<<(5+i));
										}
										scan_flag = 0;
										sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_S_OK>\n", ADDR);
										return sprintf_len;
								}
								else goto ERROR;
						}
						
						//<AD01_T_CHN_?>
						else if( (strstr((char *)&sprintf_buf[6], "T_CHN_?>") != NULL) && (sprintf_buf[13] == '>') )
						{
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_T_CHN_%02u>\n", ADDR, OPS_CH+1);
								return sprintf_len;
						}

						//<AD01_B_YYY_E_ZZZ>
						else if( (strstr((char *)&sprintf_buf[6], "B_") != NULL) && (strstr((char *)&sprintf_buf[11], "_E_") != NULL) && (sprintf_buf[17] == '>') )
						{
								EPROM.sbch = (sprintf_buf[8] - '0')*100 + (sprintf_buf[9] - '0')*10 + (sprintf_buf[10] - '0');
								if(EPROM.sbch > EPROM.maxch)
										goto ERROR;
								Save_To_EPROM((uint8_t *)&EPROM.sbch, 2);
								EPROM.sech = (sprintf_buf[14] - '0')*100 + (sprintf_buf[15] - '0')*10 + (sprintf_buf[16] - '0');
								if(EPROM.sech > EPROM.maxch)
										goto ERROR;
								Save_To_EPROM((uint8_t *)&EPROM.sech, 2);
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_OK>\n", ADDR);
								return sprintf_len;
						}
						
						else if(strstr((char *)&sprintf_buf[6], "B_?>") != NULL)
						{
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_B_%03d_E_%03d>\n", ADDR,EPROM.sbch, EPROM.sech);
								return sprintf_len;
						}
						
						//<AD01_SET_MAX_XXX>  设置最大通道数
						else if( strstr((char*)&sprintf_buf[6], "SET_MAX_") != NULL && (sprintf_buf[17] == '>') )
						{
								EPROM.maxch = (sprintf_buf[14] - '0')*100 + (sprintf_buf[15] - '0')*10 + (sprintf_buf[16] - '0');
								Save_To_EPROM((uint8_t *)&EPROM.maxch, 2);
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_SET_MAX_OK>\n", ADDR);
								return sprintf_len;
						}
					#endif	
						
          #ifdef TYPE_OTDR
						//<AD01_OTDRRESET>  设置最大通道数
						else if(strstr((char*)&sprintf_buf[6], "OTDRRESET>") != NULL)
						{
							  OTDR_PWR_OFF;
							  OSTimeDly(50);
								OTDR_PWR_ON;
								sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_OTDRRESET_OK>\n", ADDR);
								return sprintf_len;
						}
		  #endif

          #ifdef TYPE_OPM
						//<ADxx_yy_C_z>通道开关
						else if( (cp=strstr((char*)&sprintf_buf[8],"_C_"))!=NULL && sprintf_buf[12]=='>' )
						{  // value1=sprintf_buf[6]-'0';
						  //  value2=sprintf_buf[7]-'0';
							link_num = (sprintf_buf[6]-'0')*10+(sprintf_buf[7]-'0'); 
							if( link_num<= CHANNEL_NUM )
							{   
								if(link_num==0)//00表示所有通道
								{
									if(sprintf_buf[11]=='?')
									{   
										sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_%02u_C_",EPROM.address,link_num,CHANNEL_NUM);
										for(i=0;i<CHANNEL_NUM;i++)
										{
											sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],"%01u" ,EPROM.way_switch[i]);
										}
										sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],">");
									}
									else if(sprintf_buf[11]=='0' ||sprintf_buf[11]=='1')
									{
										for(i=0;i<CHANNEL_NUM;i++)
										{
										  EPROM.way_switch[i]=sprintf_buf[11]-'0'; 
										}
										i2c_set=1;
										c_set[0]=1;   //通道开关设置 
										c_set[1]=1;   //全部所有通道设置
									 sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_C_OK>",EPROM.address,link_num);
									 //
									 //Save_To_EPROM((uint8 *)&EPROM.way_switch, 16);
									
									}
									else goto ERROR;
								}
								else if(sprintf_buf[11]=='?')
								{   
									sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_C_%1u>",EPROM.address,link_num,EPROM.way_switch[link_num-1]);
								}
								else if(sprintf_buf[11]=='0' ||sprintf_buf[11]=='1')
								{   
									 EPROM.way_switch[link_num-1]=sprintf_buf[11]-'0'; 
									 i2c_set=1;
									 c_set[0]=1;                //通道开关设置 
									 c_set[2]=1;                //单个通道设置
									 c_set[3]=link_num-1;       //通道号
									 sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_C_OK>",EPROM.address,link_num);
								     //
									 //Save_To_EPROM((uint8 *)&EPROM.way_switch[c_set[3]], 1);
								} 
								else goto ERROR;
							}
							else goto ERROR;
						}
						 //<ADxx_yy_W_z>通道波长
						else if( (cp=strstr((char*)&sprintf_buf[8],"_W_"))!=NULL && sprintf_buf[12]=='>' )
						{   link_num = (sprintf_buf[6]-'0')*10+(sprintf_buf[7]-'0'); 
							cp = &sprintf_buf[11];
							if( link_num<= CHANNEL_NUM )
							{  
								if(link_num==0)//00表示所有通道
								{
									if( *cp=='?' )
									{   
										sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_%02u_W_",EPROM.address,link_num,CHANNEL_NUM);
										for(i = 0;i < CHANNEL_NUM;i ++)
										{
											sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],"%01u" ,EPROM.wavelength[i]);
										}
										sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],">");
									}
									else if( *cp=='0' || *cp =='1')
									{
										for(i=0;i<CHANNEL_NUM;i++)
										{
											EPROM.wavelength[i] = *cp-'0';
										}
										i2c_set=1;
										w_set[0]=1;     //波长设置
										w_set[1]=1;     //所有通道波长设置
										sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_W_OK>",EPROM.address,link_num);                    
									    //
										//Save_To_EPROM((uint8 *)&EPROM.wavelength, 16);
									}
									else goto ERROR;
								}
								else if( *cp == '?' )
								{
									*cp=EPROM.wavelength[link_num-1]+'0';
									sprintf_len = 13;
								}
								else if( *cp == '0' || *cp == '1' )
								{
									EPROM.wavelength[link_num-1] = *cp-'0'; 
									i2c_set=1;
									w_set[0]=1;             //波长设置
									w_set[2]=1;             //单个通道波长设置
									w_set[3]=link_num-1;    //波长设置通道号
									sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_W_OK>",EPROM.address,link_num);                    
								    //
									//Save_To_EPROM((uint8 *)&EPROM.wavelength[w_set[3]], 1);
								}
								else goto ERROR;
							}
							else goto ERROR;
						}
						//<ADxx_yy_A_pzz.zz>设置通道告警功率值
						//<ADxx_yy_A_?>
						else if( (cp=strstr((char*)&sprintf_buf[8],"_A_"))!=NULL && ((sprintf_buf[12]=='>')||(sprintf_buf[17]=='>')) )
						{
							link_num = (sprintf_buf[6]-'0')*10+(sprintf_buf[7]-'0'); 
						 
							cp1 = &sprintf_buf[11];
							cp2 = &sprintf_buf[15];
							
							if(link_num <= CHANNEL_NUM )
							{  
								if(link_num == 0)//00表示所有通道
								{
									if(*cp1 == '?')
									{
										uint8 order_size; 
										uint8 len;
										order_size=(CHANNEL_NUM-1)/8+1;
//										sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_%02u_%02u_A_%s>" ,\
//									    EPROM.address,link_num,CHANNEL_NUM,order_size,(char *)PD_INFO);
										sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_%02u_%02u_A_" ,\
									    EPROM.address,link_num,CHANNEL_NUM,order_size);
										len = 0;
										for(i = 0 ; i < CHANNEL_NUM ; i++)
										{
											if(EPROM.q_power[i] >= 0)
											{
												temp_arry[0] = '+';
												data_temp2 =  EPROM.q_power[i]*100;
											}
											else
											{
												temp_arry[0] = '-';
												data_temp2 = (0 - EPROM.q_power[i])*100;
											}
											temp_arry[1] = data_temp2/1000+'0';
											temp_arry[2] = data_temp2%1000/100+'0';
											temp_arry[3] = '.';
											temp_arry[4] = data_temp2%100/10+'0';
											temp_arry[5] = data_temp2%10+'0';
                                            memcpy((char *)&PD_INFO[6*i],temp_arry,6);
											len +=6;
										}
										PD_INFO[len] = '>';
										len += 1;
										memcpy((char *)&sprintf_buf[sprintf_len],PD_INFO,len);
										sprintf_len += len;
									}
									else if(*cp1=='-')      
									{
										data_temp=change_ascii_date( cp1+1, 2, &err );
										if( data_temp <= 99 && err == 0 )        
										{
											temp_arry[0] = data_temp;
										}
										else  goto ERROR;

										data_temp = change_ascii_date( cp2, 2, &err );
										if( data_temp <= 99 && err == 0 )        
										{
											temp_arry[1] = data_temp;
										}
										else  goto ERROR;   
									    for(i=0;i<CHANNEL_NUM;i++)
									    {
									        EPROM.q_power[i] = -(temp_arry[0]+(temp_arry[1]*0.01)); 
									    } 
										i2c_set=1;
										a_set[0]=1;
										a_set[1]=1;
									    sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_A_OK>",\
															EPROM.address,link_num); 
                                        /*for(i=0;i<CHANNEL_NUM;i++)
                                        {
											Save_To_EPROM((uint8 *)&EPROM.q_power[i], 4);										
									    }*/
									}
									else if(*cp1=='+')
									{
										data_temp = change_ascii_date( cp1+1, 2, &err );
										if( data_temp <= 99 && err == 0 )        
										{
											temp_arry[0] = data_temp;
										}
										else  goto ERROR;

										data_temp = change_ascii_date( cp2, 2, &err );
										if( data_temp <= 99 && err == 0 )        
										{
											temp_arry[1] = data_temp;
										}
										else  goto ERROR;   
									    for(i=0;i<CHANNEL_NUM;i++)
										{
									        EPROM.q_power[i] = temp_arry[0]+(temp_arry[1]*0.01); 
										}
										i2c_set=1;
										a_set[0]=1;
										a_set[1]=1;
										sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_A_OK>",\
															EPROM.address,link_num);
                                        //
										/*for(i=0;i<CHANNEL_NUM;i++)
                                        {
											Save_To_EPROM((uint8 *)&EPROM.q_power[i], 4);										
									    }*/
                                         										
									}
								}
								else if(*cp1 == '?')
								{
									if(EPROM.q_power[link_num-1] >= 0)
									{
										temp_arry[0] = '+';
										data_temp2 =  EPROM.q_power[link_num-1]*100;
									}
									else
									{
										temp_arry[0] = '-';
										data_temp2 = (0 - EPROM.q_power[link_num-1])*100;
									}
									temp_arry[1] = data_temp2/1000+'0';
									temp_arry[2] = data_temp2%1000/100+'0';
									temp_arry[3] = '.';
									temp_arry[4] = data_temp2%100/10+'0';
									temp_arry[5] = data_temp2%10+'0';
									temp_arry[6] = '\0';

									sprintf_len = sprintf((char *)cp1,"%s>",temp_arry);                                               
									sprintf_len+=16;
								}
								else if(*cp1=='-')      
								{
									data_temp = change_ascii_date( cp1+1, 2, &err );
									if( data_temp <= 99 && err == 0 )        
									{
										temp_arry[0] = data_temp;
									}
									else  goto ERROR;

									data_temp = change_ascii_date( cp2, 2, &err );
									if( data_temp <= 99 && err == 0 )        
									{
										temp_arry[1] = data_temp;
									}
									else  goto ERROR;   
									
								    EPROM.q_power[link_num-1] = -(temp_arry[0]+(temp_arry[1]*0.01)); 
									i2c_set=1;
									a_set[0]=1;
									a_set[2]=1;
									a_set[3]=link_num-1;
								    sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_A_OK>",\
														EPROM.address,link_num);
									//
                                    //Save_To_EPROM((uint8 *)&EPROM.q_power[a_set[3]], 4);									
								}
								else if(*cp1=='+')
								{
									data_temp = change_ascii_date( cp1+1, 2, &err );
									if( data_temp <= 99 && err == 0 )        
									{
										temp_arry[0] = data_temp;
									}
									else  goto ERROR;

									data_temp = change_ascii_date( cp2, 2, &err );
									if( data_temp <= 99 && err == 0 )        
									{
										temp_arry[1] = data_temp;
									}
									else  goto ERROR;   
									
								   EPROM.q_power[link_num-1] = temp_arry[0]+(temp_arry[1]*0.01);
							  
								   i2c_set=1;
								   a_set[0]=1;
								   a_set[2]=1;
								   a_set[3]=link_num-1;
								   sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_A_OK>",\
														EPROM.address,link_num);  
								   //
								   //Save_To_EPROM((uint8 *)&EPROM.q_power[a_set[3]], 4);
								}
								else  goto ERROR;
							}
							else  goto ERROR;
						}
						
						//<ADxx_yy_B_?>查询通道告警状态
						else if( (cp=strstr((char*)&sprintf_buf[8],"_B_"))!=NULL && sprintf_buf[12]=='>' )
						{   link_num = (sprintf_buf[6]-'0')*10+(sprintf_buf[7]-'0'); 
							if(link_num<= CHANNEL_NUM )
							{
								if(link_num==0)//00表示所有通道
								{
									if(sprintf_buf[11]=='?')
									{   sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_B_",EPROM.address,link_num);
										for(i=0;i<CHANNEL_NUM;i++)
										sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],"%01u" ,warn[i]);
										sprintf_len = sprintf_len+sprintf((char *)&sprintf_buf[sprintf_len],">");
									}
									else goto ERROR;
								}
								else if(sprintf_buf[11]=='?')
								{
									sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_B_%01u>",EPROM.address,link_num,warn[link_num-1]);
								}
								else goto ERROR;
							}
							else goto ERROR;
					    }
						//<ADxx_yy_POW_?>查询通道当前功率值
						else if( (cp=strstr((char*)&sprintf_buf[8],"_POW_?"))!=NULL && sprintf_buf[14]=='>' )
						{
							cp1 = &sprintf_buf[6];
							cp2 = &sprintf_buf[7];
							link_num = *cp1*10+*cp2;
							if(link_num<= CHANNEL_NUM )
							{
								if(link_num==0)//00表示所有通道
								{   
									uint8 order_size; 
									order_size=(CHANNEL_NUM-1)/8+1;
								    sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_%02u_%02u_POW_%s>" ,\
											  EPROM.address,link_num,CHANNEL_NUM,order_size,(char *)MD_INFO);
								}
							    else
							    {
									if(power[link_num-1] >= 0)
									{
										temp_arry[0] = '+';
										data_temp2 =  power[link_num-1]*100;
									}
								else
								{
									temp_arry[0] = '-';
									data_temp2 = (0 - power[link_num-1])*100;
								}
								temp_arry[1] = data_temp2/1000+'0';
								temp_arry[2] = data_temp2%1000/100+'0';
								temp_arry[3] = '.';
								temp_arry[4] = data_temp2%100/10+'0';
								temp_arry[5] = data_temp2%10+'0';

								sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_%02u_POW_%s>" ,\
											  EPROM.address,link_num,temp_arry);
								}
							}
							else goto ERROR;
							
						}
						//<ADxx_MAX_?>查询采集盘总通道数
						else if((cp=strstr((char*)&sprintf_buf[5],"_MAX_?"))!=NULL && sprintf_buf[11]=='>' )
						{
							cp1 = &sprintf_buf[3];
							cp2 = &sprintf_buf[4];
							data_temp = (*cp1-'0')*10+*cp2;
							if(EPROM.address == data_temp)
							{
								sprintf_len = sprintf((char *)sprintf_buf,"<AD%02u_MAX_%03u>" ,\
											  EPROM.address,CHANNEL_NUM);
							}
							else
							{
								sprintf_len = 0;
							}
						}
          #endif						
						else
						{
								ERROR:
										sprintf_len = sprintf((char *)sprintf_buf, "<AD%02u_ER>\n", ADDR);
										return sprintf_len;
						}
				}
    }
    return sprintf_len;
}

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



#ifdef TYPE_OPM
//EPROM要存入数据
//因为主控设置插盘数据时，EPROM存入数据需要较长的时间
//因此先给主控返回OK，插盘再把数据存入EPROM
void eprom_set(void)
{   uint8 i;
    if(i2c_set==1)
    {   i2c_set=0;
        if(re_set==1)//复位
        {
            re_set=0;
            Reset_Handler();   	    //实行软件复位 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
        else if(c_set[0]==1)//通道开关
        {   c_set[0]=0;
            if(c_set[1]==1)
            {   c_set[1]=0;
                Save_To_EPROM((uint8 *)&EPROM.way_switch, 16);
            }
            else if(c_set[2]==1)
            {   c_set[2]=0;
                Save_To_EPROM((uint8 *)&EPROM.way_switch[c_set[3]], 1);
            }
        }
        else if(w_set[0]==1)//波长
        {
            w_set[0]=0;
            if(w_set[1]==1)
            {   w_set[1]=0;
                Save_To_EPROM((uint8 *)&EPROM.wavelength, 16);  
            }
            else if(w_set[2]==1)
            {
                 w_set[2]=0;
                 Save_To_EPROM((uint8 *)&EPROM.wavelength[w_set[3]], 1);
            }
        }
        else if(a_set[0]==1)//告警功率
        {   a_set[0]=0;
            if(a_set[1]==1)
            {   a_set[1]=0;
                for(i=0;i<CHANNEL_NUM;i++)
               Save_To_EPROM((uint8 *)&EPROM.q_power[i], 4);  
            }
            else if(a_set[2]==1)
            {   a_set[2]=0;
                Save_To_EPROM((uint8 *)&EPROM.q_power[a_set[3]], 4);
            }
        }
    }

}
#endif
