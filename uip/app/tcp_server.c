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
**�� �� ��: tcp_server_init
**��������: ���� TCP �շ�ָ��˿ڵ� �����б�������ʼ��
**�������: void  
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void tcp_server_init(void)
{
    // httpd_init();
    uip_listen( htons(TCP_lport));
}

/*****************************************************************************
**�� �� ��: TCP_Cmd_process
**��������: TCP ָ�������
**�������: void  
**�������: ��
**�� �� ֵ: ��
*****************************************************************************/
void TCP_Cmd_process(void)
{
    uint16 len=0;
    uint16 sprintf_len=0;

    if( uip_connected() )                           //���Ӵ�������״̬              
    {
        TCP_Abort_EN=1;
    }
    else if( uip_closed() )                         //���Ӵ��ڹر�״̬  
    {
        TCP_Abort_EN=0;
    }
    
    if( uip_newdata() )                             //�յ��µ����� 
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

            if( LOG_Print_Index%20 == 0 )  break;       //ÿ�δ�ӡ 20 �� LOG
        }                 
        uip_send((uint8*)uip_appdata, sprintf_len);    //ͨ��TCPת������
    }  
/*    
    //����򶪰����ط�����
    else if( uip_rexmit() )
    {
        uip_send(UART0_data_Buf, last_len);
        return;
    }
*/
}

/*****************************************************************************
**�� �� ��: tcp_server_appcall
**��������: ͨ���˿ں�ʶ����ת�� TCP ָ�������
**�������: void
**�������: ��
**�� �� ֵ: ��
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
        TCP_Cmd_process();      //��ת�� TCP ָ�������
    }
}