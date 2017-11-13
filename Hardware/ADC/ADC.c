/******************************************************************************

                 版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

******************************************************************************
 文 件 名   : ADC.c
 版 本 号   : 初稿
 作    者   :
 生成日期   : 2013年5月10日
 最近修改   :
 功能描述   :
 函数列表   :
             adcdeal
             ADC_int
             addo
             optics_collec
******************************************************************************/

#include "config.h"
#include "math.h"
#include "ADC.h"
#include "LPC177x_8x.h"
#include "drv595.h"
#include "uart1.h"

#ifdef   TYPE_OPM

#define CHTEMP   1
#define minadc  50   	              //minadc=20;太低了，应该考虑高一些（30~50），提高抗干扰能力。
#define maxadc  1000
extern uint8 hardware_way[CHANNEL_NUM];  //真实的硬件档位
float power[CHANNEL_NUM] ;
float power_count[CHANNEL_NUM];
float q_power[CHANNEL_NUM];
uint8 warn[CHANNEL_NUM];			    //告警状态

extern void  OSTimeDly (unsigned int ticks);

/*****************************************************************************
**函 数 名: ADC_int
**功能描述: ADC 初始化
**输入参数: rate ADC的换率应
**输出参数: 无
**返 回 值: LPC_ADC->DR
*****************************************************************************/
void ADC_int(uint32 rate)
{
    //使能以ADC控制器电源
    LPC_SC->PCONP |= (1U<<12);


    LPC_IOCON->P0_23 = 0x01;     //ADC0[0]
    LPC_IOCON->P0_24 = 0x01;     //ADC0[1]

//#if ADC_USE_NUM >=3	
//    LPC_IOCON->P0_25 = 0x01;     //ADC0[2]
//#endif
//#if ADC_USE_NUM >=4
//    LPC_IOCON->P0_26 = 0x01;     //ADC0[3]
//#endif
//#if ADC_USE_NUM >=5	
//    LPC_IOCON->P1_30 = 0x03;     //ADC0[4]
//#endif
//#if ADC_USE_NUM >=6	
//    LPC_IOCON->P1_31 = 0x03;     //ADC0[5]
//#endif
//#if ADC_USE_NUM >=7	
//    LPC_IOCON->P0_12 = 0x03;     //ADC0[6]
//#endif
//#if ADC_USE_NUM >=8	
//    LPC_IOCON->P0_13 = 0x03;     //ADC0[7]
//#endif

    //进行ADC模块设置，其中x<<n表示第n位设置为x(若x超过一位，则向高位顺延)
    LPC_ADC->CR = 0;
    LPC_ADC->CR = (1 << 0)              |     // SEL = 0 ，选择通道1
                  ((PeripheralClock / 1000000 - 1) << 8) |     // CLKDIV = Fpclk / 1000000 - 1 ，即转换时钟为1MHz
                  (0 << 16)                    |     // BURST = 0 ，软件控制转换操作
                  (0 << 17)                    |     // CLKS = 0 ，使用11clock转换
                  (1 << 21)                    |     // PDN = 1 ， 正常工作模式(非掉电转换模式)
                  (0 << 22)                    |     // TEST1:0 = 00 ，正常工作模式(非测试模式)
                  (1 << 24)                    |     // START = 1 ，启动ADC转换
                  (0 << 27);                         // EDGE = 0 (CAP/MAT引脚下降沿触发ADC转换)

    while( (LPC_ADC->GDR&0x80000000) == 0 );    // 等待转换结束
}

/*********************************************************************************************************
** 函数名称: void addo(uint8 ch)
** 功能描述: adc转换
** 输　入: AD号数:0~7
** 输　出: ADC结果
** 运算时间 :
********************************************************************************************************/
uint32 addo(uint8 ch)
{
    uint32 ADC_Data;
//	uint8 ab[5];
//	uint32 temp1,temp2;
    if(ch==0)		LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x01|(1 << 24);    // 设置通道1，并进行第一次转换

    else if(ch==1)	LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x02|(1 << 24);    // 设置通道2，并进行第一次转换

    else if(ch==2)	LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x04|(1 << 24);    // 设置通道3，并进行第一次转换

    else if(ch==3)	LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x08|(1 << 24);    // 设置通道4，并进行第一次转换

    else if(ch==4)	LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x10|(1 << 24);    // 设置通道5，并进行第一次转换

    else if(ch==5)	LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x20|(1 << 24);    // 设置通道6，并进行第一次转换

    else if(ch==6)	LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x40|(1 << 24);    // 设置通道7，并进行第一次转换

    else if(ch==7)	LPC_ADC->CR = (LPC_ADC->CR&0x00FFFF00)|0x80|(1 << 24);    // 设置通道8，并进行第一次转换
//------------------------------------------
    
    while( (LPC_ADC->GDR&0x80000000)==0 );               // 等待转换结束
    LPC_ADC->CR = LPC_ADC->CR | (1 << 24);               // 再次启运转换
    while( (LPC_ADC->GDR&0x80000000)==0 );               // 等待转换结束

    ADC_Data = LPC_ADC->GDR;                             // 读取ADC结果
    ADC_Data = (ADC_Data>>6) & 0x3FF;                      // 提取AD转换值 (bit4~bit15 为12位的ADC值)，现在只取10位的ADC值
//	ab[0] = ADC_Data/1000 + '0';
//	temp1 = ADC_Data%1000;
//	ab[1] = temp1/100 + '0';
//	temp2 = ADC_Data%100;
//	ab[2] = temp2/10 + '0';
//	ab[3] = ADC_Data%10 + '0';
//    UART1Put_str(ab,4);
    return ADC_Data;
}

/*****************************************************************************
**函 数 名: adcdeal
**功能描述:
**输入参数: uint8 RTnum
             uint32 way_temp
             float  adc
**输出参数: 无
**返 回 值:
**调用函数:
**被调函数:
*****************************************************************************/
void adcdeal(uint8 input_channel_num,uint32 way, float adc)
{
    if(adc > 800)
    {
        if(way>0)
        {
            way=way-1;
            adc=adc/10;
        }
        else
        {
            way=way;
            adc=800;
        }
    }
    else if(adc < 80)
    {
        if(way<7)
        {
            way=way+1;
            adc=adc*10;
        }
        else
        {
            way=way;  	   //解决了死机问题
            adc=80;
        }
    }
    //*****************************防止超过10倍*****************************
    if(adc > 800)
    {
        if(way>0)
        {
            way=way-1;
            adc=adc/10;
        }
        else
        {
            way=way;
            adc=800;
        }
    }
    else if(adc < 80)
    {
        if(way<7)
        {
            way=way+1;
            adc=adc*10;
        }
        else
        {
            way=way;  	 //解决了死机问题
            adc=80;
        }
    }

    if(way>=CHTEMP)      //负值
    {
        adc=800.0/adc;
        adc=10*log10(adc);
        adc=adc+10*(way-CHTEMP);
        adc=(-1)*adc;
    }
    else                 //正值
    {
        adc=adc/80.0;
        adc=10*log10(adc);
        adc=adc+10*((CHTEMP-1)-way);
    }

    //adc += ((float)EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]])/10.0; //加上校准补偿系数
    //-------------------------------------------
    if(adc > 30.0)      adc = 30.0;
    if(adc < -50.0)     adc = -50.0;
    //=================================
    power[input_channel_num] = adc;
    if(power[input_channel_num]<=EPROM.q_power[input_channel_num])
    {
        warn[input_channel_num]=1;      
        //ChannelLED((input_channel_num/2)*6+input_channel_num%2+2, 0);	//对应通道的指示灯灭
    }
    else
    {
        warn[input_channel_num]=0;
        //ChannelLED((input_channel_num/2)*6+input_channel_num%2+2, 1); //对应通道的指示灯亮
    }
}

float adc_deal(uint8 input_channel_num,uint32 way, float adc)
{
    if(adc > 800)
    {
        if(way>0)
        {
            way=way-1;
            adc=adc/10;
        }
        else
        {
            way=way;
            adc=800;
        }
    }
    else if(adc < 80)
    {
        if(way<7)
        {
            way=way+1;
            adc=adc*10;
        }
        else
        {
            way=way;  	   //解决了死机问题
            adc=80;
        }
    }

if(adc > 800)
    {
        if(way>0)
        {
            way=way-1;
            adc=adc/10;
        }
        else
        {
            way=way;
            adc=800;
        }
    }
    else if(adc < 80)
    {
        if(way<7)
        {
            way=way+1;
            adc=adc*10;
        }
        else
        {
            way=way;  	   //解决了死机问题
            adc=80;
        }
    }
    
    if(way>=CHTEMP)      //负值
    {
        adc=800.0/adc;
        adc=10*log10(adc);
        adc=adc+10*(way-CHTEMP);
        adc=(-1)*adc;
    }
    else                 //正值
    {
        adc=adc/80.0;
        adc=10*log10(adc);
        adc=adc+10*((CHTEMP-1)-way);
    }

//    if(adc > 10.0)      adc = 10.0;
//	if(adc < -70.0)     adc = -70.0;

    return adc;
}
/*****************************************************************************
**函 数 名: CHANNEL
**功能描述:
**输入参数: uint8 RTnum
             uint8 way_temp
**输出参数: 无
**返 回 值:
*****************************************************************************/
void CHANNEL(uint8 input_channel_num, uint8 way_temp)
{
    switch ( input_channel_num )
    {
    case 0 :
        CHANNEL_PIN0_WAY(way_temp);
        break;
    case 1 :
        CHANNEL_PIN1_WAY(way_temp);
        break;
    case 2 :
        CHANNEL_PIN2_WAY(way_temp);
        break;
    case 3 :
        CHANNEL_PIN3_WAY(way_temp);
        break;
    case 4 :
        CHANNEL_PIN4_WAY(way_temp);
        break;
    case 5 :
        CHANNEL_PIN5_WAY(way_temp);
        break;
    case 6 :
        CHANNEL_PIN6_WAY(way_temp);
        break;
    case 7 :
        CHANNEL_PIN7_WAY(way_temp);
        break;
	case 8 :
        CHANNEL_PIN8_WAY(way_temp);
        break;
    case 9 :
        CHANNEL_PIN9_WAY(way_temp);
        break;
    case 10 :
        CHANNEL_PIN10_WAY(way_temp);
        break;
    case 11 :
        CHANNEL_PIN11_WAY(way_temp);
        break;
    case 12 :
        CHANNEL_PIN12_WAY(way_temp);
        break;
    case 13 :
        CHANNEL_PIN13_WAY(way_temp);
        break;
    case 14 :
        CHANNEL_PIN14_WAY(way_temp);
        break;
    case 15 :
        CHANNEL_PIN15_WAY(way_temp);
        break;

    default:
        break;
    }
}
//void CHANNELB(uint8 RTnum, uint8 way_temp)
//{
//    switch ( RTnum )
//    {
//    case 8 :
//        CHANNEL_PIN8(way_temp);
//        break;
//    case 9 :
//        CHANNEL_PIN9(way_temp);
//        break;
//    case 10 :
//        CHANNEL_PIN10(way_temp);
//        break;
//    case 11 :
//        CHANNEL_PIN11(way_temp);
//        break;
//    case 12 :
//        CHANNEL_PIN12(way_temp);
//        break;
//    case 13 :
//        CHANNEL_PIN13(way_temp);
//        break;
//    case 14 :
//        CHANNEL_PIN14(way_temp);
//        break;
//    case 15 :
//        CHANNEL_PIN15(way_temp);
//        break;

//    default:
//        break;
//    }
//}
/*****************************************************************************
 函 数 名  : optics_collect
 功能描述  : 控制换挡电路，采集光信号转换成光功率值
 输入参数  : uint8 RTnum
              uint8 way
 输出参数  : 无
 返 回 值  :
*****************************************************************************/
void optics_collect(uint8 input_channel_num ,uint8 way_temp)
{
    uint8  i;
    uint16 ADC_Data;
    float  ADC_just_temp;
	uint8 ADC_num;
    
    OS_CPU_SR  cpu_sr;
	if(input_channel_num < 8)
	{
		ADC_num = 0;
	}else
	{
		ADC_num = 1;
	}

ADC_STAR:

    ADC_Data = (uint16)addo(ADC_num);
    if(ADC_Data < minadc)							    //AD转换值小于20，换通道
    {
        if(way_temp < 7)
        {
            way_temp ++;

            OS_ENTER_CRITICAL();
            CHANNEL(input_channel_num,way_temp);     			    //设定到1通道
            OS_EXIT_CRITICAL();

            OSTimeDly(10);
            //UART0Putch(way_temp + '0');
            /*
               if(EPROM.q_power[RTnum] >= -23.0)
               {
                   OSTimeDly(2);
               }
               else if(EPROM.q_power[RTnum] < -23.0 && EPROM.q_power[RTnum] >= -33.0)
               {
                   if(way_temp==5 )		OSTimeDly(3);	//防-25dBm处误切。
                   else 			        OSTimeDly(2);
               }
               else if(EPROM.q_power[RTnum] < -33.0)
               {
                   if(way_temp>5)			OSTimeDly(3);	//防-35dBm处误切。
                   else 				    OSTimeDly(2);
               }  
                
               */
            goto ADC_STAR;
        }
    }
    else if(ADC_Data > maxadc)    	        //AD转换值大于1000，换通道
    {
        if(way_temp > 0)
        {
            way_temp --;

            OS_ENTER_CRITICAL();
            CHANNEL(input_channel_num,way_temp);     	//设定到1通道
            OS_EXIT_CRITICAL();

            OSTimeDly(10);	 		        //换挡延时时间，与探测器和电路有关，会影响到切换时间。取合适为宜。
            //UART0Putch(way_temp + '0');
            goto ADC_STAR;
        }
    }

    ADC_just_temp = (float)ADC_Data;
    //加上校准补偿系数
    if( EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] > 0 )
    {
       for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
       {
           ADC_just_temp =ADC_just_temp*1.023293;
       }
    }
    else if( EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] < 0 )
    {
       for ( i = 0 ; i<( ~(EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] - 1) ) ; i++ )
       {
           ADC_just_temp =ADC_just_temp*0.977237;
       }
    }     
    
    adcdeal( input_channel_num, way_temp, ADC_just_temp );
    hardware_way[input_channel_num]=way_temp;     //再把真实的档位放到全局变量 hardware_way

}

float test_optics_collect( uint8 input_channel_num ,uint8 way_temp)
{
    uint8  i;
    uint16 ADC_Data;
    float  ADC_just_temp;
    float  dBm_power;
	uint8 ADC_num;
	OS_CPU_SR  cpu_sr;
	if(input_channel_num < 8 )
	{
		ADC_num = 0;
	}
	else
	{
		ADC_num = 1;
	}
    
    //
    

ADC_STAR:

    ADC_Data = (uint16)addo(ADC_num);
    
    if(ADC_Data < minadc)							    //AD转换值小于20，换通道
    {
        if(way_temp < 7)
        {
            way_temp ++;

            OS_ENTER_CRITICAL();
            CHANNEL(input_channel_num,way_temp);     			    //设定到1通道
            //CHANNEL_PIN8(way_temp);
            OS_EXIT_CRITICAL();
            OSTimeDly(10);
           //delay_nms(5);
            goto ADC_STAR;
        }
    }
    else if(ADC_Data > maxadc)    	        //AD转换值大于1000，换通道
    {
        if(way_temp > 0)
        {
            way_temp --;

            OS_ENTER_CRITICAL();
            CHANNEL(input_channel_num,way_temp);     	//设定到1通道
           // CHANNEL_PIN8(way_temp);
            OS_EXIT_CRITICAL();
           // delay_nms(5);
            OSTimeDly(10);	 		        //换挡延时时间，与探测器和电路有关，会影响到切换时间。取合适为宜。
            goto ADC_STAR;
        }
    }     
    
    ADC_just_temp = (float)ADC_Data;

    //加上校准补偿系数
    if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 1 )
    {
       for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
       {
           ADC_just_temp =ADC_just_temp*1.023293;
       }
    }
    else if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 0 )
    {
       for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
       {
           ADC_just_temp =ADC_just_temp*0.977237;
       }
    }
   OS_ENTER_CRITICAL();
   dBm_power = adc_deal(input_channel_num, way_temp, ADC_just_temp);
   OS_EXIT_CRITICAL();
   
   power_count[input_channel_num]= dBm_power;
    hardware_way[input_channel_num]=way_temp;     //再把真实的档位放到全局变量 hardware_way
//档位补偿
    //dBm_power = POWER_JUST(RTnum,way_temp,dBm_power);
		if(way_temp == 1) dBm_power = dBm_power -0.5;
    if (dBm_power > 23.0)      dBm_power = 23.0;
    if (dBm_power < -50.0)     dBm_power = -50.0;
       
    return dBm_power;
}
float test_optics_collect_B( uint8 input_channel_num ,uint8 way_temp)
{
    uint8  i;
    uint16 ADC_Data;
    float  ADC_just_temp;
    float  dBm_power;
	uint8 ADC_num;
	OS_CPU_SR  cpu_sr;
	
	
	if(input_channel_num < 8 )
	{
		ADC_num = 0;
	}
	else
	{
		ADC_num = 1;
	}
  

ADC_STAR:

    ADC_Data = (uint16)addo(ADC_num);
    
    if(ADC_Data < minadc)							    //AD转换值小于20，换通道
    {
        if(way_temp < 7)
        {
            way_temp ++;

            OS_ENTER_CRITICAL();
            //CHANNEL(RTnum,way_temp);     			    //设定到1通道
            CHANNEL(input_channel_num,way_temp);
            OS_EXIT_CRITICAL();
            OSTimeDly(10);
          // delay_nms(10);
            goto ADC_STAR;
        }
    }
    else if(ADC_Data > maxadc)    	        //AD转换值大于1000，换通道
    {
        if(way_temp > 0)
        {
            way_temp --;

            OS_ENTER_CRITICAL();
            //CHANNEL(RTnum,way_temp);     	//设定到1通道
            CHANNEL(input_channel_num,way_temp);
            OS_EXIT_CRITICAL();
            //delay_nms(10);
            OSTimeDly(10);	 		        //换挡延时时间，与探测器和电路有关，会影响到切换时间。取合适为宜。
            goto ADC_STAR;
        }
    }     
    
    ADC_just_temp = (float)ADC_Data;

/*
    //加上校准补偿系数
    if( EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] > 0 )
    {
       for ( i = 0 ; i<EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] ; i++ )
       {
           ADC_just_temp =ADC_just_temp*1.023293;
       }
    }
    else if( EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] < 0 )
    {
       for ( i = 0 ; i<( ~(EPROM.ADC_just[RTnum][EPROM.wavelength[RTnum]] - 1) ) ; i++ )
       {
           ADC_just_temp =ADC_just_temp*0.977237;
       }
    }  
    */
    //加上校准补偿系数
    if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 1 )
    {
       for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
       {
           ADC_just_temp =ADC_just_temp*1.023293;
       }
    }
    else if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 0 )
    {
       for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
       {
           ADC_just_temp =ADC_just_temp*0.977237;
       }
    } 
            
   OS_ENTER_CRITICAL();
   dBm_power = adc_deal(input_channel_num, way_temp, ADC_just_temp);
   OS_EXIT_CRITICAL();
   
   power_count[input_channel_num]= dBm_power;
    hardware_way[input_channel_num]=way_temp;     //再把真实的档位放到全局变量 hardware_way
		if(way_temp == 1) dBm_power = dBm_power -0.5;
    if (dBm_power > 23.0)      dBm_power = 23.0;
    if (dBm_power < -50.0)     dBm_power = -50.0;

    return dBm_power;
}
void log114_adcdeal(uint8 input_channel_num ,uint32 adc)
{
    uint16 i,ad_flag;
//    uint32 temp16;
    uint32 ADC_Data;
    float  ADC_just_temp;
    
    //8路 功率转换    1dB = 37.3mv   (37.3/3300)*1024 =11.57   // 0dB : 67650
     ADC_Data=adc*1000;
     
     if(ADC_Data>67650)
      { //x > 67650 :  -n dBm =(x - 67650)/116
        ADC_Data=(ADC_Data - 67650)/EPROM.DBM_delay[input_channel_num];
        ad_flag=1; 
      }
      else if(ADC_Data<67650)
      { //x < 67650 :  +n dBm =(67650 - x)/116   
        ADC_Data=(67650 - ADC_Data)/EPROM.DBM_delay[input_channel_num];
         ad_flag=0;
      }
      else
      { //x = 67650   
        ADC_Data=0;
        ad_flag=1;    
      }
      
    ADC_just_temp = (float)ADC_Data;  
    //实际功率
    if(ad_flag==0)//功率值为正
       {
            //加上16dB
            power_count[input_channel_num] = ADC_just_temp/100;//实际功率值
            ADC_just_temp = ADC_just_temp/100;
            
             //加上校准补偿系数EPROM.fuhao_just
            if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 1 )
            {
               for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
               {
                   ADC_just_temp =ADC_just_temp + 0.1;
               }
            }
            else if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 0 )
            {
               for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
               {
                   ADC_just_temp =ADC_just_temp - 0.1;
               }
            }
      
            power[input_channel_num] = ADC_just_temp ;

            if( power[input_channel_num] > 30.0)       power[input_channel_num] = 30.0;
       }
    else   //功率值为负                
      {  
            //加上16dB
            power_count[input_channel_num] = (0 -(ADC_just_temp/100));//实际功率值
            ADC_just_temp = (0 -(ADC_just_temp/100));
            
            if(ADC_just_temp >= -37.0)
            {
                if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 1 )
                {
                   for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
                   {
                       ADC_just_temp =ADC_just_temp + 0.1;
                   }
                }
                else if( EPROM.fuhao[input_channel_num][EPROM.wavelength[input_channel_num]] == 0 )
                {
                   for ( i = 0 ; i<EPROM.ADC_just[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
                   {
                       ADC_just_temp =ADC_just_temp - 0.1;
                   }
                }
            }
            else if(ADC_just_temp < -37.0)
            {
                if( EPROM.fuhao_just[input_channel_num][EPROM.wavelength[input_channel_num]] == 1 )
                {
                   for ( i = 0 ; i<EPROM.ADC_just_36[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
                   {
                       ADC_just_temp =ADC_just_temp + 0.1;
                   }
                }
                else if( EPROM.fuhao_just[input_channel_num][EPROM.wavelength[input_channel_num]] == 0 )
                {
                   for ( i = 0 ; i< EPROM.ADC_just_36[input_channel_num][EPROM.wavelength[input_channel_num]] ; i++ )
                   {
                       ADC_just_temp =ADC_just_temp - 0.1;
                   }
                }
            }

            power[input_channel_num] =  ADC_just_temp ;
           
            if(power[input_channel_num] < -50.0)     power[input_channel_num] = -50.0;
      }
 
//-----------通道开关设置，开启或关闭------------//      
if(EPROM.way_switch[input_channel_num]==1)
{
    if(power[input_channel_num]<=EPROM.q_power[input_channel_num])
    {
        warn[input_channel_num]=1;      
    }
    else
    {
        warn[input_channel_num]=0;
    }
 }
else    
{   //关闭，默认是-50.00
    power[input_channel_num] =-50.00;
    warn[input_channel_num]=0; 
}
//-----------------------------------------------//
}
#endif
