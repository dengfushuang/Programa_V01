#include "main.h"
#include "DS18B20.h"
#include "delay.h"


uint8 T_18b20flag;

/*********************************************************************************************************
** 函数名称: DS18B20_Reset()
** 功能描述: DS18B20复位
** 输　出:   Ack：DS18B20状态
********************************************************************************************************/
uint8 DS18B20_Reset(void)
{
  uint8 Ack;
	
//	DQ_OUT;
	DQ_L;
	delay_nus(700);
	DQ_H;
	
	delay_nus(75);				//总线拉低>480us复位所有设备
	DQ_IN;
	delay_nus(1);
	Ack = DQ;
/*	if(Ack)
	{
        UART0Write_Str("ER\n");    
    }
    else
    {
        UART0Write_Str("OK\n");    
    }
    */
    DQ_OUT;
	delay_nus(500);	
	DQ_H;
	//delay_nus(2);
	return Ack;
}

/*********************************************************************************************************
** 函数名称: DS18B20_Write()
** 功能描述: 向DS18B20写指令
** 输　入:   cmd：DS18B20操作指令
********************************************************************************************************/
void DS18B20_Write(uint8 cmd)
{
  uint8 i,data;  
  
	for(i=0;i<8;i++)  
	{  
		data = cmd>>i;
		data &= 0x01;
		//DQ_OUT;
		DQ_L;
		delay_nus(2);
		if(data==1)
		{
			DQ_H;
		}
		else
		{
			DQ_L;
		}
		delay_nus(65);
		DQ_H;
		delay_nus(2);
	}

}

/*********************************************************************************************************
** 函数名称: uint8 DS18B20_Read(void)
** 功能描述: 从DS18B20读数据
** 输　出:   data：DS18B20数据
********************************************************************************************************/
uint8 DS18B20_Read(void)
{
	uint8 i; 
	uint8 data=0;

	for(i=0;i<8;i++)
	{
		//DQ_OUT;
    DQ_L;
    delay_nus(2);
	  DQ_H;
		
	  delay_nus(10);
	  DQ_IN;
	  delay_nus(1);
		if(DQ==1)
		{
            data |= 0x01<<i;
		}
		else
		{
            data |= 0x00<<i;
		}
        
		delay_nus(60);
		DQ_OUT;
		delay_nus(1);
		DQ_H;
	}
	return data;
}

/*********************************************************************************************************
** 函数名称: float DS18B20_GetTemp(uint8 T)
** 功能描述: 获取温度值
** 输  入：  c：以摄氏温标输出温度，t：以华氏温标输出温度
** 输　出:   temperature_c（摄氏温度），temperature_f（华氏温度）
********************************************************************************************************/
uint16 DS18B20_GetTemperature(uint8 T)
{  
	 int temperature_c,temperature_f;
	 uint8 temperature_msb,temperature_lsb;
	 float tt;
   DS18B20_Reset();
	 DS18B20_Write(0xcc);
	 DS18B20_Write(0x44);
	// delay_nus(110);
	 DS18B20_Reset();
	 DS18B20_Write(0xcc);
	 DS18B20_Write(0xbe);
	 temperature_lsb = DS18B20_Read();
	// delay_nus(5);
	 temperature_msb = DS18B20_Read();
/*	 if(temperature_msb <= 0x80)
	 {
	   temperature_lsb = temperature_lsb/2;
	 }
	 temperature_msb = temperature_msb & 0x80;
	 if(temperature_msb >= 0x80)
	 {
	   temperature_lsb = (~temperature_lsb)+1;
		 temperature_lsb = (temperature_lsb/2);
		 temperature_lsb = ((-1)*temperature_lsb);
	 }
	 temperature_c = (int)temperature_lsb;
	 temperature_f = 32+temperature_c*9/5;
    if(T==0)
	 return temperature_c;
	 else 
	 return temperature_f;
	 */
    temperature_c = temperature_msb;                //先把高八位有效数据赋于temp
    temperature_c <<= 8;                            //把以上8位数据从temp低八位移到高八位
    temperature_c = temperature_c|temperature_lsb;  //两字节合成一个整型变量
    if(temperature_c > 2047)T_18b20flag = 0;
    else                    T_18b20flag = 1;
    
    tt = (float)temperature_c * 0.0625;     //得到真实十进制温度值
                                    //因为DS18B20可以精确到0.0625度
                                    //所以读回数据的最低位代表的是0.0625度
    temperature_c = tt*10.0+0.5;    //放大十倍
    temperature_f = 32+temperature_c*9/5;
	  if(T==0)
	  return temperature_c;
	  else 
	  return temperature_f;
}

/*********************************************************************************************************
** 函数名称: void DS18B20_Alarm(float Threshold_T)
** 功能描述: 获取温度值
** 输  入：  Threshold_T：温度报警阈值，c：以摄氏温标输出温度，t：以华氏温标输出温度
********************************************************************************************************/
void DS18B20_Alarm(float Threshold_T,uint8 T)
{
  float temperature;
	
	temperature = DS18B20_GetTemperature(T);
	if(temperature >= Threshold_T)
	{
	  if(BEE_ON_flag ==1)
		{
		  BEE_ON;
			delay_nms(200);
			BEE_OFF;
			BEE_ON;
			delay_nms(200);
			BEE_OFF;
		}
		else BEE_OFF;
	}
}


void init_ds18b20(void)
{
    //DQ_H;                     //DQ复位,不要也可行。
    //delay_nus(7);                  //稍做延时
    DQ_L;                    //单片机拉低总线
    delay_nus(700);                //精确延时，维持至少480us
    DQ_H;                    //释放总线，即拉高了总线
    delay_nus(75);                //此处延时有足够,确保能让DS18B20发出存在脉冲。
 /*   DQ_IN;
    delay_nus(1); 
    if(DQ)
    {
        UART0Write_Str("ER\n");    
    }
    else
    {
        UART0Write_Str("OK\n");    
    }
    DQ_OUT;
    */
    delay_nus(495);
    DQ_H;
    delay_nus(2);
}
/*********************************************
 Ds18b20_read_byte
*********************************************/
uint8 Ds18b20_read_byte()
{
    uint8 i,dat;

    for(i=8;i>0;i--)
    {
     //   DQ_H;
      //  delay_nus(2);
       DQ_L;                  //将总线拉低，要在1us之后释放总线
                               //单片机要在此下降沿后的15us内读数据才会有效。
       delay_nus(2);                 //至少维持了1us,表示读时序开始
       dat >>= 1;               //让从总线上读到的位数据，依次从高位移动到低位。
       DQ_H;                  //释放总线，此后DS18B20会控制总线,把数据传输到总线上
       delay_nus(5);                 //延时7us,此处参照推荐的读时序图，尽量把控制器采样时间放到读时序后的15us内的最后部分
       DQ_IN;//设置成输入
       delay_nus(1);
       if(DQ==1)                   //控制器进行采样
       {
        dat |= 0x80;            //若总线为1,即DQ为1,那就把dat的最高位置1;若为0,则不进行处理,保持为0
       } 
       else
       {
        dat &= 0x7f;
       }
       delay_nus(65);
        DQ_OUT;
        delay_nus(1);
        DQ_H;
        delay_nus(2);
    }
    return (dat);
}
/*********************************************
 Ds18b20_write_byte
*********************************************/
void Ds18b20_write_byte(uint8 dat)
{
    uint8 i,temp;
    for(i=8;i>0;i--)
    {
     //   DQ_H;
     //   delay_nus(2);
       DQ_L;                       //拉低总线
       delay_nus(2);               //至少维持了1us,表示写时序(包括写0时序或写1时序)开始
       //DQ = dat&0x01;            //从字节的最低位开始传输
       temp = dat&0x01;            //指令dat的最低位赋予给总线,必须在拉低总线后的15us内,
       if(temp == 0x01)            //因为15us后DS18B20会对总线采样。
       {
          DQ_H;
       }
       else
       {
          DQ_L;
       }
       delay_nus(60);              //必须让写时序持续至少60us
       DQ_H;                       //写完后,必须释放总线,
       dat >>= 1;
       delay_nus(2);
    }
}


uint16 DS18B20_Get_Tmp(void)       //获取温度get the temperature
{
    float tt;
    uint8 a,b;
    uint16 temp;
    
    init_ds18b20();                //初始化
    Ds18b20_write_byte(0xcc);      //忽略ROM指令
    Ds18b20_write_byte(0x44);      //温度转换指令
    init_ds18b20();                //初始化
    Ds18b20_write_byte(0xcc);      //忽略ROM指令
    Ds18b20_write_byte(0xbe);      //读暂存器指令
    a = Ds18b20_read_byte();       //读取到的第一个字节为温度LSB
    b = Ds18b20_read_byte();       //读取到的第一个字节为温度MSB
    init_ds18b20();
    temp = b;                      //先把高八位有效数据赋于temp
    temp <<= 8;                    //把以上8位数据从temp低八位移到高八位
    temp = temp|a;                 //两字节合成一个整型变量

    if(temp > 2047) T_18b20flag = 0;//-
    else            T_18b20flag = 1;//+
     
    tt = temp*0.0625;              //得到真实十进制温度值
                                   //因为DS18B20可以精确到0.0625度
                                   //所以读回数据的最低位代表的是0.0625度
																		
    temp = tt*10+0.5;              //放大十倍
                                   //这样做的目的将小数点后第一位也转换为可显示数字
                                   //同时进行一个四舍五入操作。
    return temp;
}






