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
#include "main.h"
#include "uip.h"
//#include "uip.h"
#include "cgi.h"
#include "httpd.h"
#include "fs.h"
//#include "..\APP\config.h"

#include <stdio.h>
#include <string.h>
#include "uart0.h"

/**************************************************************************************/
/***********************************添加变量部分**************************************/
extern short  max_abort_timer;
extern uint8  UART_LCR_flag;
extern uint8  UART0_pbs;

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
"<html>\r\n"
"<form action=\"/ipset/\" method=\"get\">\r\n"
"<center>\r\n"
    "<table id=\"table1\" width=\"280\">\r\n"
        "<tr>\r\n"
            "<td colspan=\"2\">\r\n"
                "<div align=\"center\">\r\n"
                    "<font color=\"#3366ff\" size=\"5\"><span style=\"background-color: #ffffff\"> Network Parameters</span>\r\n"
            "</td>\r\n"
        "</tr>\r\n"
    "</table>\r\n"
"</center>\r\n"
//"<br>\r\n"
//"<br>\r\n"
"<br>\r\n"
"<div class=\"wpmd\" align=\"center\">"
"&nbsp;IP Address:"
  "<input name=\"ftip1\" value=\"%u\"type=\"text\" size=\"2\" maxlength=\"3\" onChange=\"return checkIP()\">"
  "<input name=\"ftip2\" value=\"%u\" type=\"text\" size=\"2\"maxlength=\"3\"onchange=\"return checkIP()\">"
  "<input name=\"ftip3\" value=\"%u\" type=\"text\" size=\"2\" maxlength=\"3\"onchange=\"return checkIP()\">"
  "<input name=\"ftip4\" value=\"%u\" type=\"text\" size=\"2\" maxlength=\"3\"onchange=\"return checkIP()\"></div>"
"<div class=\"wpmd\" align=\"center\">"

"&nbsp;&nbsp;&nbsp;&nbsp;GateWay:"
  "<input name=\"ftgw1\" value=\"%u\" type=\"text\" size=\"2\"maxlength=\"3\" onChange=\"return checkGW()\">"
  "<input name=\"ftgw2\" value=\"%u\" type=\"text\" size=\"2\" maxlength=\"3\" onChange=\"return checkGW()\">"
  "<input name=\"ftgw3\" value=\"%u\" type=\"text\" size=\"2\" maxlength=\"3\"onchange=\"return checkGW()\">"
  "<input name=\"ftgw4\" value=\"%u\" type=\"text\" size=\"2\" maxlength=\"3\" onChange=\"return checkGW()\"></div>"
"<div class=\"wpmd\" align=\"center\">"
};

const u8_t web_data2[]=
{
 "&nbsp;&nbsp;&nbsp;&nbsp;SubMark:"
 "<input name=\"ftsm1\" type=\"text\" value=\"%u\" size=\"2\"maxlength=\"3\" onChange=\"return checkSM()\">"
 "<input name=\"ftsm2\" type=\"text\" value=\"%u\" size=\"2\" maxlength=\"3\"onchange=\"return checkSM()\">"
 "<input name=\"ftsm3\" type=\"text\" value=\"%u\" size=\"2\" maxlength=\"3\" onChange=\"return checkSM()\">"
 "<input name=\"ftsm4\" type=\"text\" value=\"%u\" size=\"2\" maxlength=\"3\" onChange=\"return checkSM()\"></div>"

"<div class=\"wpmd\" align=\"center\">"
"&nbsp;&nbsp;&nbsp;&nbsp;</div>"
"<div class=\"wpmd\" align=\"center\">"
    "&nbsp;Test Interval:\r\n"
"<input name=\"interval\" type=\"text\" value=\"%u\" size=\"4\" maxlength=\"5\">(10ms)</div>\r\n"
"<div class=\"wpmd\" align=\"center\"></div>\r\n"
"<br>\r\n"
"<div class=\"wpmd\" align=\"center\">\r\n"
"<input name=\"formbutton1\" type=\"submit\" value=\"Apply\"><input name=\"formbutton2\" type=\"reset\"\r\n"
        "value=\"Reset Value\"></div>\r\n"
"\r\n"
"</form>\r\n"
"</td></tr></table>\r\n"
};
const u8_t web_data3[]=
{
"<script language =javascript type=\"text/javascript\">"
//检测IP是否正确
"function checkIP()"
"{\r\n"
	"var ip = new Array();"
	"ip[0]=document.getElementsByName(\"ftip1\")[0];"
	"ip[1]=document.getElementsByName(\"ftip2\")[0];"
	"ip[2]=document.getElementsByName(\"ftip3\")[0];"
	"ip[3]=document.getElementsByName(\"ftip4\")[0];"
	"for(var i=0;i<ip.length;i++){\r\n"
		"if(ip[i].value>254||ip[i].value<0)"
		"{\r\n"
			"ip[i].style.backgroundColor = \"#ff0000\";"
			"alert(\"设置IP不符合！IP段的值应该在0-255之间\");"
		"}else\r\n"
		"{\r\n"
			"ip[i].style.backgroundColor = \"#ffffff\";"
		"}\r\n"
	"}\r\n"
"}\r\n"
//检测子网掩码是否正确
"function checkSM()\r\n"
"{\r\n"
	"var sm = new Array();\r\n"
	"sm[0]=document.getElementsByName(\"ftsm1\")[0];"
	"sm[1]=document.getElementsByName(\"ftsm2\")[0];"
	"sm[2]=document.getElementsByName(\"ftsm3\")[0];"
	"sm[3]=document.getElementsByName(\"ftsm4\")[0];"
	"for(var i=0;i<sm.length;i++){"
	"if(sm[i].value>255||sm[i].value<0)"
	    "{\r\n"
			"sm[i].style.backgroundColor = \"#ff0000\";\r\n"
			"alert(\"设置子网掩码不符合！掩码段的值应该在0-255之间！\");"
		"}else\r\n"
		"{\r\n"
			"sm[i].style.backgroundColor = \"#ffffff\";"
		"}\r\n"
	"}\r\n"
"}\r\n"
//检测用户默认网关是否正确
"function checkGW()"
"{\r\n"
	"var gw = new Array();"
	"gw[0]=document.getElementsByName(\"ftgw1\")[0];"
	"gw[1]=document.getElementsByName(\"ftgw2\")[0];"
	"gw[2]=document.getElementsByName(\"ftgw3\")[0];"
	"gw[3]=document.getElementsByName(\"ftgw4\")[0];"
	"for(var i=0;i<gw.length;i++){\r\n"
		"if(gw[i].value>255||gw[i].value<0)"
		"{\r\n"
			"gw[i].style.backgroundColor = \"#ff0000\";"
			"alert(\"设置的默认网关不符合！网关段值应该在0-255之间！\");"
		"}else\r\n"
		"{\r\n"
			"gw[i].style.backgroundColor = \"#ffffff\";"
		"}\r\n"
	"}\r\n"
"}\r\n"
"</script>\r\n"
};

const u8_t  web_data4[]=
{
	"<html>\r\n"
	"<body bgcolor=\"white\" onload =\"selectmenu()\">"
	"<center>"
    //"<table width=\"600\" border=\"0\" >"
	"<script language=\"javascript\" type=\"text/javascript\">"
	"function selectmenu()"
	"{document.getElementById(\"bps\").selectedIndex=%u;" 
	 "document.getElementById(\"dabit\").selectedIndex=%u;"
	 "document.getElementById(\"stbit\").selectedIndex=%u;"
	 "document.getElementById(\"pabit\").selectedIndex=%u;}"
	 "</script>"
};
const u8_t  web_data5[]=
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
                     
              "<select id=\"bps\"name=\"bps\">"
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
            "<select id=\"dabit\"name=\"dabit\">"
              "<option value=\"0\">5</option>"
              "<option value=\"1\">6</option>"
              "<option value=\"2\">7</option>"
              "<option value=\"3\"selected>8</option>"
            "</select>"
          "</div></TD>"
};

const u8_t  web_data6[]=
{
      "</TR>"
      "<TR>"
        "<TD><div class=\"wpmd\" align=right>Stop bits:</div></TD>"  //停止位
        "<TD><div class=\"wpmd\" align=left>"
            "<select id=\"stbit\"name=\"stbit\">"
              "<option value=\"0\"selected>1</option>"
              "<option value=\"4\">2</option>"
            "</select>"
          "</div></TD>"    
      "</TR>"
      "<TR>"
        "<TD><div class=\"wpmd\" align=right>Parity bits:</div></TD>"  //校验位
        "<TD><div class=\"wpmd\" align=left>"
            "<select id=\"pabit\"name=\"pabit\">"
              "<option value=\"0\"selected>None</option>"
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
  	"<p> </p>"
  "<div class=\"wpmd\" align=center>"
    "<input name=\"formbutton1\" type=\"submit\" value=\"Apply\">"       //保存
    "<input name=\"formbutton2\" type=\"reset\" value=\"Reset Value\">"  //重设
  "</div>"
  "</center>"
"</form>\r\n"  

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
            EPROM.max_abort_timer));
            xx=2; break;
       };
       case 2:
       {
       		uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data3)); 
       		xx=3; break;
       }
       case 3:
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
	u8_t temp1;
	
    switch(yy)
    {   
	   case 0:
       {  
		  if( EPROM.uart_flag&0x08 )  //有校验位时
		  {
			 temp1= (((EPROM.uart_flag&0x30)>>4)+1);		
		  }
		  else						  //无校验位时				
		  {
				temp1=0;
		  } 
  		  uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data4,
				  (EPROM.uart_bps-1),        	  //波特率
				  (EPROM.uart_flag&0x03),    	  //数据位
				  ((EPROM.uart_flag&0x04)>>2),    //停止位
				  temp1));	  					  //校验位	 
		          
          yy=1;break;
	   }
       case 1:
       {  
          uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data5));  
          yy=2;break;
       };
       case 2:
       {   
          uip_send(uip_appdata, sprintf((char *)uip_appdata,(const char *)web_data6,EPROM.TCP_lport) );  
          yy=3;break;
       };
       case 3:
       {
          yy=0;
          return 1;
       }; 
       default: return 0;      
   }      
   return 0;
}

/*-----------------------------------------------------------------------------------*/
