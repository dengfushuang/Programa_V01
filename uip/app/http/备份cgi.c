/**
 * \addtogroup httpd
 * @{
 */

/**
 * \file
 * HTTP server script language C functions file.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 * This file contains functions that are called by the web server
 * scripts. The functions takes one argument, and the return value is
 * interpreted as follows. A zero means that the function did not
 * complete and should be invoked for the next packet as well. A
 * non-zero value indicates that the function has completed and that
 * the web server should move along to the next script line.
 *
 */

/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: cgi.c,v 1.23.2.4 2003/10/07 13:22:27 adam Exp $
 *
 */

#include "..\uip1.0\uip.h"
//#include "uip.h"
#include "cgi.h"
#include "httpd.h"
#include "fs.h"
//#include "..\APP\config.h"

#include <stdio.h>
#include <string.h>
#include "..\driver\uart0.h"

/**************************************************************************************/
/***********************************添加变量部分**************************************/
extern short max_abort_timer;


/**************************************************************************************/
static u8_t print_stats(u8_t next);
static u8_t file_stats(u8_t next);
//static u8_t tcp_stats(u8_t next);

cgifunction cgitab[] = {
  print_stats,   /* CGI function "a" */
  file_stats,    /* CGI function "b" */
//  tcp_stats      /* CGI function "c" */
};

//------------------------------------网页显示代码---------------------------------------
const u8_t web_data1[]=
{
    "<form action=\"/ipset/\" method=\"get\">"
	"<center> <table id=\"table1\" width=\"280\">"
		"<tr><td colSpan=\"2\">"
				"<div align=\"center\">"
					"<font color=\"#3366ff\" size=\"5\">"
					"<span style=\"background-color: #ffffff\">Network Parameters</span>" // <span style=\"BACKGROUND-COLOR: #ffffff\">设置</span></font></div>"
    "</td></tr></table> </center>"

   //"<p align=\"center\"><font color=\"#3366FF\">"
   //"<span style=\"background-color: #FFFFFF\">网口参数</span></font><font class=\"ws12\" color=\"#FF9900\"></font></div><br>"
	
//  "<br><div class=\"wpmd\" align=center>"
//    "IP:<input name=\"ftdvname\" value=\"IPort\" maxlength=15 type=\"text\">"
//    "&nbsp;&nbsp;&nbsp;设备名:"
//    "<select name=\"fsipmd\">"
//      "<option value=\"1\" selected>Static</option>"
//      "<option value=\"0\" >DHCP</option></select></div>"   
    "<br><br><br>"
    "<div class=\"wpmd\"align=center>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp;&nbsp;IP:"        //IP地址
    "<input name=\"ftip1\"value=\"%u\"type=\"text\"size=2 maxlength=3>"
    "<input name=\"ftip2\"value=\"%u\"type=\"text\"size=2 maxlength=3>"
    "<input name=\"ftip3\"value=\"%u\"type=\"text\"size=2 maxlength=3>"
    "<input name=\"ftip4\"value=\"%u\"type=\"text\"size=2 maxlength=3></div>"

    "<div class=\"wpmd\"align=center>&nbsp;&nbsp;&nbsp;&nbsp;Gateway:"  //默认网关
    "<input name=\"ftgw1\"value=\"%u\"type=\"text\"size=2 maxlength=3>"
    "<input name=\"ftgw2\"value=\"%u\"type=\"text\"size=2 maxlength=3>"
    "<input name=\"ftgw3\"value=\"%u\"type=\"text\"size=2 maxlength=3>"
    "<input name=\"ftgw4\"value=\"%u\"type=\"text\"size=2 maxlength=3></div>"
    

};   
const u8_t web_data2[]=
{    
    "<div class=\"wpmd\" align=center>&nbsp;&nbsp;&nbsp;&nbsp;SubMark:" //子网掩码
    "<input name=\"ftsm1\"type=\"text\"value=\"%u\"size=2 maxlength=3>"
    "<input name=\"ftsm2\"type=\"text\"value=\"%u\"size=2 maxlength=3>"
    "<input name=\"ftsm3\"type=\"text\"value=\"%u\"size=2 maxlength=3>"
    "<input name=\"ftsm4\"type=\"text\"value=\"%u\"size=2 maxlength=3></div>" 
   /* "DNS服务器:<input name=\"ftdns1\" type=\"text\"value=\"192\" size=2 maxlength=3>"
    "<input name=\"ftdns2\"type=\"text\"value=\"168\"size=2 maxlength=3>"
    "<input name=\"ftdns3\"type=\"text\"value=\"1\"size=2 maxlength=3>"
    "<input name=\"ftdns4\"type=\"text\"value=\"1\"size=2 maxlength=3>" */
  //"<br><br><div class=\"wpmd\" align=center><font class=\"ws12\"color=\"#FF9900\">端口参数</font></div>"
  //"<br><div class=\"wpmd\"align=center>串口端口:"
  //  "<input name=\"ftcmp\"value=\"3003\" type=\"text\" maxlength=5 size=5>"
  //  "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;网页端口:"
  //  "<input name=\"ftwebp\"value=\"80\"type=\"text\" maxlength=5 size=5>"
    "<div class=\"wpmd\" align=center>Inactivity time:"  //TCP超时断开时间
    "<input name=\"tcp_timeout\"type=\"text\"value=\"%u\"size=4 maxlength=5>(10ms)</div>" 

    "<br><br><br><div class=\"wpmd\"align=center>"
    "<input name=\"formbutton1\" type=\"submit\" value=\"Apply\">"       //保存
    "<input name=\"formbutton2\" type=\"reset\" value=\"Reset Value\">"  //重设
  "</div></form>"
};
const u8_t  web_data3[]=
{
    "<form action=\"/uartset/\" method=\"get\">"
    "<TABLE>"
      "<TR>"
        "<TD colspan=2><div align=center><font color=\"#3366FF\" size=\"5\">"
			"<span style=\"background-color: #FFFFFF\">Serial Parameters</span></font></div>"  // 串口参数
      "</TR>"
      "<TR>"
        "<TD colspan=2><div align=center><br>"
            "<font color=\"#3366FF\"><span style=\"background-color: #FFFFFF\"></span></font>"  //Serial Parameters
      "</TR>"
      "<TR>"
        "<TD><div class=\"wpmd\" align=right>Baud rate:</div></TD>"  //波特率
        "<TD><div class=\"wpmd\" align=left>"
                     
              "<select name=\"bps\">"
              "<option value=\"1\">2400</option>"
              "<option value=\"2\">4800</option>"
              "<option value=\"3\">9600</option>"
              "<option value=\"4\">14400</option>"
              "<option value=\"5\">19200</option>"
              "<option value=\"6\">38400</option>"
              "<option value=\"7\">56000</option>"
              "<option value=\"8\">57600</option>"
              "<option value=\"9\"selected>115200</option>"
            "</select>"
          
        "</div></TD>"
      "</TR>"
      "<TR>"
        "<TD><div class=\"wpmd\" align=right>Data bits:</div></TD>" //数据位
        "<TD><div class=\"wpmd\" align=left>"
            "<select name=\"dabit\">"
              "<option value=\"0\">5</option>"
              "<option value=\"1\">6</option>"
              "<option value=\"2\">7</option>"
              "<option value=\"3\" selected>8</option>"
            "</select>"
          "</div></TD>"
};

const u8_t  web_data4[]=
{
      "</TR>"
      "<TR>"
        "<TD><div class=\"wpmd\" align=right>Stop bits:</div></TD>"  //停止位
        "<TD><div class=\"wpmd\" align=left>"
            "<select name=\"stbit\">"
              "<option value=\"0\" selected>1</option>"
              "<option value=\"4\">2</option>"
            "</select>"
          "</div></TD>"    
      "</TR>"
      "<TR>"
        "<TD><div class=\"wpmd\" align=right>Parity bits:</div></TD>"  //校验位
        "<TD><div class=\"wpmd\" align=left>"
            "<select name=\"pabit\">"
              "<option value=\"0\" selected>None</option>"
              "<option value=\"8\">Odd</option>"
              "<option value=\"24\">Even</option>"
              "<option value=\"40\">Mark</option>"
              "<option value=\"56\">Space</option>"
            "</select>"
          "</div></TD>"
      "</TR>"
      "<TR>"
        "<TD><div class=\"wpmd\" align=right>Local port:</div></TD>"  //端口
        "<TD><div class=\"wpmd\" align=left>&nbsp;<input name=\"port\" value=\"%u\"size=4 maxlength=4 type=\"text\"></div></TD>"
      "</TR>"
      "</TABLE>"
  	"<p>&nbsp;</div>"
  "<div class=\"wpmd\" align=center>"
    "<input name=\"formbutton1\" type=\"submit\" value=\"Apply\">"       //保存
    "<input name=\"formbutton2\" type=\"reset\" value=\"Reset Value\">"  //重设
  "</div>"
"</form>"  

};


static u8_t
print_stats(u8_t next)
{
 static  u8_t xx=0;
    switch(xx)
    {   
       case 0:
       {     
            uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data1,                                  
            htons(uip_hostaddr[0]) >> 8,
        	htons(uip_hostaddr[0]) & 0xff,
        	htons(uip_hostaddr[1]) >> 8,
        	htons(uip_hostaddr[1]) & 0xff,
        	 
            htons(uip_draddr[0]) >> 8,
        	htons(uip_draddr[0]) & 0xff,
        	htons(uip_draddr[1]) >> 8,
        	htons(uip_draddr[1]) & 0xff) );
            xx=1; break;
       };
       case 1:
       {
            uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data2, 
            htons(uip_netmask[0]) >> 8,
        	htons(uip_netmask[0]) & 0xff,
        	htons(uip_netmask[1]) >> 8,
        	htons(uip_netmask[1]) & 0xff, 
            max_abort_timer));
            xx=2; break;
       };
       case 2:
       {
            xx=0;
            return 1;    
       };
       default: return 0;

     }
 return 0;
}    
/*-----------------------------------------------------------------------------------*/
static u8_t
file_stats(u8_t next)
{
  static u8_t yy=0;
    switch(yy)
    {   
       case 0:
       {  
          uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data3));  
          yy=1;break;
       };
       case 1:
       {   
          uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data4,UART_lport) );  
          yy=2;break;
       };
       case 2:
       {
          yy=0;
          return 1;
       };
       default: return 0;      
   }      
return 0;
}

/*-----------------------------------------------------------------------------------*/
