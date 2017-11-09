#include "main.h"
#include "udp_server.h"
#include "httpd.h"
#include "uip.h"
#include "uart0.h"
#include "uip.h"
#include <string.h>
#include "uip_arp.h"


extern  uip_udp_listen( u16_t lport);    //UDP监听函数

extern struct uip_eth_addr uip_ethaddr;   //UIP的MAC地址

const char CMD[7]= {0XA1,0X15,0X02,0,0,0XFF};
const char data1[4] = {0x00,0x02,0x00,0x02};
const char data2[7] = {0xA1,0XFD,0X00,0X09,0X23,0X01,0X08};
const char data3[4] = {0X0B,0XBB,0X00,0XFF};

/***********************************************************************************************
* 函数名  :udp_server_init()
* 参数    :void
* 返回值  :void
* 功能描述:UDP初始化监听函数，启动调用
************************************************************************************************/
void udp_server_init(void)
{
    uip_udp_listen(8800);
    uip_udp_listen(8600);
}
/***********************************************************************************************
* 函数名  :UIP_UDP_APPCALL()
* 参数    :void
* 返回值  :void
* 功能描述:UDP回应函数，用于回应以太网转串口的搜索，否则PC搜索不到
************************************************************************************************/
void UIP_UDP_APPCALL(void)
{
    uint16 sprintf_len;
    
    if(uip_udp_conn->lport== HTONS(8800) )
    {
        if(uip_newdata())   //|| uip_rexmit()
        {
            //电脑上的ZNetCom 以太网串口转换设备配置工具软件发出的命令:  A1 15 02 00 00 FF
            //模块收到后回应: 00 02 00 02  00 14 97 06 F1 D7   A1 FD 00 09 23 01 08 C0 A8 00 B2 0B BB 00 FF
            //                             |-------MAC-----|                        |---原IP--|

            if( uip_appdata[0]==0XA1 && uip_appdata[1]==0X15 && uip_appdata[2]==0X02 && uip_appdata[3]==0 && uip_appdata[4]==0 && uip_appdata[5]==0Xff )
            {
                memcpy(uip_appdata,data1,4);
                memcpy(&uip_appdata[4], uip_ethaddr.addr, 6);   //回应MAC地址给上位机
                memcpy(&uip_appdata[10],data2,7);
                memcpy(&uip_appdata[17], uip_hostaddr,4);       //回应IP地址给上位机
                memcpy(&uip_appdata[21],data3,4);
                uip_send(uip_appdata, 25);
            }
        }
    }
    else if( uip_udp_conn->lport == HTONS(8600) )
    {
        Send_LOG_conn = uip_udp_conn;
        if( uip_poll() )
        {
//            Print_LOG(uip_appdata, uint16 num);
            uip_send((char *)uip_appdata, sprintf_len);             
        }
        else if( uip_newdata() )                             //收到新的数据 
        {
//             if( ( sprintf_len = Cmd_process( (char*)&uip_appdata[0]) ) > 0 )
//                 uip_send( uip_appdata, sprintf_len );
                
            //if( LOG_PRINT_FLAG )    UDP_CMD = 1; 
        }
    }
}

