#include "main.h"
#include "tcp_server.h"
#include "httpd.h"
#include "uip.h"
#include "uart0.h"
#include "queue.h"
#include "Cmd_process.h"
#include "OSW.h"
#include"AT24C512.H"

/*****************************************************************************
**函 数 名: tcp_server_init
**功能描述: 添加 TCP 收发指令端口到 监听列表，并初始化
**输入参数: void  
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void tcp_server_init(void)
{
    // httpd_init();
    uip_listen( htons(TCP_lport));
}

/*****************************************************************************
**函 数 名: TCP_Cmd_process
**功能描述: TCP 指令处理函数
**输入参数: void  
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void TCP_Cmd_process(void)
{
    uint16 len=0;
    uint16 sprintf_len=0;

    if( uip_connected() )                           //连接处于连接状态              
    {
        TCP_Abort_EN=1;
    }
    else if( uip_closed() )                         //连接处于关闭状态  
    {
        TCP_Abort_EN=0;
    }
    
    if( uip_newdata() )                             //收到新的数据 
    {
        if( ( len = Cmd_process( (char*)&uip_appdata[0]) ) > 0 )
            uip_send( uip_appdata, len );
    }
    else if( uip_poll() && LOG_PRINT_FLAG )  
    {    
        while ( EPROM.AT24C512_NUM - LOG_Print_Index  )
        {
            Read_AT24C512( (EPROM.AT24C512_NUM - LOG_Print_Index - 1), (uint8 *)&SYS_LOG, sizeof(struct LOG));
            sprintf_len += sprintf((char *)&uip_appdata[sprintf_len],"%03u.%4u/%02u/%02u %02u:%02u:%02u Link%1u %s %s\r\n",\
                            LOG_Print_Index+1,SYS_LOG.Year, SYS_LOG.Mon, SYS_LOG.Mday, SYS_LOG.Hour, SYS_LOG.Min, SYS_LOG.Sec,\
                            SYS_LOG.Link, LOG_Stade[SYS_LOG.State], LOG_Mode[SYS_LOG.Mode] );
            LOG_Print_Index++;  

            if( LOG_Print_Index%20 == 0 )  break;       //每次打印 20 个 LOG
        }                 
        uip_send((uint8*)uip_appdata, sprintf_len);    //通过TCP转发数据
    }  
/*    
    //如果因丢包而重发数据
    else if( uip_rexmit() )
    {
        uip_send(UART0_data_Buf, last_len);
        return;
    }
*/
}

/*****************************************************************************
**函 数 名: tcp_server_appcall
**功能描述: 通过端口号识别跳转到 TCP 指令处理函数
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void tcp_server_appcall	(void)
{
    //if(uip_conn->lport== HTONS(80) )
    //{
    //    httpd_appcall();
    //}
    //else
    if( uip_conn->lport==htons(TCP_lport) ) 
    {
        TCP_Cmd_process();      //调转到 TCP 指令处理函数
    }
}
