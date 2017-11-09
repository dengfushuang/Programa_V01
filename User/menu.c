/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : menu.c
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2013年4月28日
  最近修改   :
  功能描述   : LCD16032两行多级菜单
******************************************************************************/
#include "config.h"
#include "main.h"
#include "menu.h"
#include "OSW.h"
#include "lpc177x_8x_eeprom.h"

#define MENULEVEL   12                  //当前菜单表个数
#define OPTIONMETE  LOST_MAX_4-MAIN+1   //当前选项个数，包括一个功能函数选项
#define Function    OPTIONMETE+1        //功能函数选项 

uint8 FirstLineDisIndex = 0;            //屏幕第一行显示的索引号
uint8 SelectLine = 0;                   //当前选择的行
uint8 SelectIndex = 0;                  //当前选定行对应的索引号
uint8 LastIndex = 0;                    //进入功能函数前的索引号，判断具体功能使用
uint8 Enter_Function = 0;               //进入功能函数的标示
uint8 Enter_counter=0;                  //按Enter键次数的计数变量
uint8 Key_temp=0;
uint16 Key_temp16=0;
float power_temp=0;

void show_Set_ok( void );
void FunctionAction (void);
uint8 MenuCancelOption (void);
extern void  OSTimeDly (uint32 ticks);

//-------------------------------------
//  菜单名声定义
//-------------------------------------
typedef enum
{
    //***************************主菜单********************************/
    MAIN = 0,
    //***************************第一级菜单****************************/
    MODE,
    OSW_STARE,
    POWER,
    WAVE,
    BACK_AUTO,
    BACK_TYPE,
    BACK_DELAY,
    ICMP_INTERVAL,
    TIME_OUT,
    LOST_MAX,
    START_DELAY,
    CMD_ADDRESS,
    LCD_BTIME,
    BEE,
    UART_BPS,
    LANGUAGE,
    RESTORE,
    //***************************第二级菜单****************************/
    MODE_1,
    MODE_2,
    MODE_3,
    MODE_4,
    OSW_STARE_1,
    OSW_STARE_2,
    OSW_STARE_3,
    OSW_STARE_4,
    POWER_1,
    POWER_2,
    POWER_3,
    POWER_4,
    POWER_5,
    POWER_6,
    POWER_7,
    POWER_8,
    WAVE_1,
    WAVE_2,
    WAVE_3,
    WAVE_4,
    WAVE_5,
    WAVE_6,
    WAVE_7,
    WAVE_8,
    BACK_AUTO_1,
    BACK_AUTO_2,
    BACK_AUTO_3,
    BACK_AUTO_4,
    BACK_TYPE_1,
    BACK_TYPE_2,
    BACK_TYPE_3,
    BACK_TYPE_4,
    BACK_DELAY_1,
    BACK_DELAY_2,
    BACK_DELAY_3,
    BACK_DELAY_4,
    ICMP_INTERVAL_1,
    ICMP_INTERVAL_2,
    ICMP_INTERVAL_3,
    ICMP_INTERVAL_4,
    TIME_OUT_1,
    TIME_OUT_2,
    TIME_OUT_3,
    TIME_OUT_4,
    LOST_MAX_1,
    LOST_MAX_2,
    LOST_MAX_3,
    LOST_MAX_4,

} MENU_NAME;

//--------------------------------------
//
//  菜单表分类数组
//
//--------------------------------------
uint8 const Level[MENULEVEL][3] =
{
    //每层表单对应 {开始索引号, 结束索引号 , 选项数目}

    {MAIN, MAIN, 1},
    {MODE, RESTORE, RESTORE-MODE+1},
    {MODE_1, MODE_2, 2},
    {OSW_STARE_1, OSW_STARE_2, 2},
    {POWER_1, POWER_8, 8},

    {WAVE_1, WAVE_8, 8},
    {BACK_AUTO_1, BACK_AUTO_2, 2},
    {BACK_TYPE_1, BACK_TYPE_2, 2},
    {BACK_DELAY_1 , BACK_DELAY_2, 2},
    {ICMP_INTERVAL_1, ICMP_INTERVAL_2, 2},
    
    {TIME_OUT_1,  TIME_OUT_2, 2},
    {LOST_MAX_1, LOST_MAX_2, 2},
};
//-------------------------------------
//  菜单结构定义
//-------------------------------------
struct Option
{
    uint8 KeyLevel;        //菜单选项所属菜单表号
    uint8 EnterIndex;      //选项进入索引号
    uint8 CancelIndex;     //选项退出索引号
    uint8 KeyWord[20];     //菜单选项文字描述数组
    //uint8 WordMete;      //菜单选项描述文字字节数
    MENU_NAME name;
};

//-------------------------------------
//
// 具体菜单选项定义，定义一个结构数组
// 存储在编码区，节省内存RAM
//
//-------------------------------------
struct  Option const sOption[2][OPTIONMETE] =
{
    {
        //***************************主菜单********************************/
        {0,1,0, "      主菜单        ",MAIN},

        //***************************第一级菜单****************************/
        {1,MODE_1,MAIN,         "1.工作模式          ",MODE},
        {1,OSW_STARE_1,MAIN,    "2.通道选择          ",OSW_STARE},
        {1,POWER_1,MAIN,        "3.切换功率          ",POWER},
        {1,WAVE_1,MAIN,         "4.波长选择          ",WAVE},
        {1,BACK_AUTO_1,MAIN,    "5.手动返回自动延时  ",BACK_AUTO},
        {1,BACK_TYPE_1,MAIN,    "6.回切方式设置      ",BACK_TYPE},
        {1,BACK_DELAY_1,MAIN,   "7.回切延时设置      ",BACK_DELAY},
        {1,ICMP_INTERVAL_1,MAIN,"8.发心跳间隔设置    ",ICMP_INTERVAL},
        {1,TIME_OUT_1,MAIN,     "9.收心跳超时设置    ",TIME_OUT},
        {1,LOST_MAX_1,MAIN,     "10. 连续丢包旁路设置",LOST_MAX},
        {1,Function,MAIN,       "11. 开机延时设置    ",START_DELAY},
        {1,Function,MAIN,       "12. 设备指令地址设置",CMD_ADDRESS},
        {1,Function,MAIN,       "13. LCD 背光        ",LCD_BTIME},
        {1,Function,MAIN,       "14. 告警声设置      ",BEE},
        {1,Function,MAIN,       "15. 波特率设置      ",UART_BPS},
        {1,Function,MAIN,       "16. 语言设置        ",LANGUAGE},
        {1,Function,MAIN,       "17. 恢复出厂设置    ",RESTORE},

        //***************************第二级菜单****************************/
        {2,Function,MODE,"1.1R工作模式        ",MODE_1},
        {2,Function,MODE,"2.2R工作模式        ",MODE_2},
        {2,Function,MODE,"3.3R工作模式        ",MODE_3},
        {2,Function,MODE,"4.4R工作模式        ",MODE_4},

        {3,Function,OSW_STARE,"1.1R通道选择        ",OSW_STARE_1},
        {3,Function,OSW_STARE,"2.2R通道选择        ",OSW_STARE_2},
        {3,Function,OSW_STARE,"3.3R通道选择        ",OSW_STARE_3},
        {3,Function,OSW_STARE,"4.4R通道选择        ",OSW_STARE_4},

        {4,Function,POWER,"1.1R1 切换功率      ",POWER_1},
        {4,Function,POWER,"2.1R2 切换功率      ",POWER_2},
        {4,Function,POWER,"3.1R3 切换功率      ",POWER_3},
        {4,Function,POWER,"4.1R4 切换功率      ",POWER_4},
        {4,Function,POWER,"5.2R1 切换功率      ",POWER_5},
        {4,Function,POWER,"6.2R2 切换功率      ",POWER_6},
        {4,Function,POWER,"7.2R3 切换功率      ",POWER_7},
        {4,Function,POWER,"8.2R4 切换功率      ",POWER_8},

        {5,Function,WAVE,"1.1R1 波长选择      ",WAVE_1},
        {5,Function,WAVE,"2.1R2 波长选择      ",WAVE_2},
        {5,Function,WAVE,"3.1R3 波长选择      ",WAVE_3},
        {5,Function,WAVE,"4.1R4 波长选择      ",WAVE_4},
        {5,Function,WAVE,"5.2R1 波长选择      ",WAVE_5},
        {5,Function,WAVE,"6.2R2 波长选择      ",WAVE_6},
        {5,Function,WAVE,"7.2R3 波长选择      ",WAVE_7},
        {5,Function,WAVE,"8.2R4 波长选择      ",WAVE_8},

        {6,Function,BACK_AUTO,"1.1R手动返回自动延时",BACK_AUTO_1},
        {6,Function,BACK_AUTO,"2.2R手动返回自动延时",BACK_AUTO_2},
        {6,Function,BACK_AUTO,"3.3R手动返回自动延时",BACK_AUTO_3},
        {6,Function,BACK_AUTO,"4.4R手动返回自动延时",BACK_AUTO_4},

        {7,Function,BACK_TYPE,"1.1R回切方式        ",BACK_TYPE_1},
        {7,Function,BACK_TYPE,"2.2R回切方式        ",BACK_TYPE_2},
        {7,Function,BACK_TYPE,"3.3R回切方式        ",BACK_TYPE_3},
        {7,Function,BACK_TYPE,"4.4R回切方式        ",BACK_TYPE_4},

        {8,Function,BACK_DELAY,"1.1R回切延时        ",BACK_DELAY_1},
        {8,Function,BACK_DELAY,"2.2R回切延时        ",BACK_DELAY_2},
        {8,Function,BACK_DELAY,"3.3R回切延时        ",BACK_DELAY_3},
        {8,Function,BACK_DELAY,"4.4R回切延时        ",BACK_DELAY_4},

        {9,Function,ICMP_INTERVAL,"1.1R心跳间设置      ",ICMP_INTERVAL_1},
        {9,Function,ICMP_INTERVAL,"2.2R心跳间设置      ",ICMP_INTERVAL_2},
        {9,Function,ICMP_INTERVAL,"3.3R心跳间设置      ",ICMP_INTERVAL_3},
        {9,Function,ICMP_INTERVAL,"4.4R心跳间设置      ",ICMP_INTERVAL_4},

        {10,Function,TIME_OUT,"1.1R收心跳超时      ",TIME_OUT_1},
        {10,Function,TIME_OUT,"2.2R收心跳超时      ",TIME_OUT_2},
        {10,Function,TIME_OUT,"3.3R收心跳超时      ",TIME_OUT_3},
        {10,Function,TIME_OUT,"4.4R收心跳超时      ",TIME_OUT_4},

        {11,Function,LOST_MAX,"1.1R连续丢包旁路    ",LOST_MAX_1},
        {11,Function,LOST_MAX,"2.2R连续丢包旁路    ",LOST_MAX_2},
        {11,Function,LOST_MAX,"3.3R连续丢包旁路    ",LOST_MAX_3},
        {11,Function,LOST_MAX,"4.4R连续丢包旁路    ",LOST_MAX_4},

    },
    {
        //***************************主菜单********************************/
        {0,1,0, "      主菜单        ",MAIN},
        //***************************第一级菜单****************************/
        {1,MODE_1,MAIN,         "1.Working Mode      ",MODE},
        {1,OSW_STARE_1,MAIN,    "2.Channel Select    ",OSW_STARE},
        {1,POWER_1,MAIN,        "3.Power Switch      ",POWER},
        {1,WAVE_1,MAIN,         "4.Wave Select       ",WAVE},
        {1,BACK_AUTO_1,MAIN,    "5.Man to Auto delay ",BACK_AUTO},
        {1,BACK_TYPE_1,MAIN,    "6.Back Mode set     ",BACK_TYPE},
        {1,BACK_DELAY_1,MAIN,   "7.Back delay set    ",BACK_DELAY},
        {1,ICMP_INTERVAL_1,MAIN,"8.Sent ICMP gap     ",ICMP_INTERVAL},
        {1,TIME_OUT_1,MAIN,     "9.Get ICMP Timeout  ",TIME_OUT},
        {1,LOST_MAX_1,MAIN,     "10.Last lost ICMP   ",LOST_MAX},
        {1,Function,MAIN,       "11.Power-on delay   ",START_DELAY},
        {1,Function,MAIN,       "12. Command Address ",CMD_ADDRESS},
        {1,Function,MAIN,       "13. LCD Backlight   ",LCD_BTIME},
        {1,Function,MAIN,       "14. Buzzer Alarm    ",BEE},
        {1,Function,MAIN,       "15. Baud Rate Set   ",UART_BPS},
        {1,Function,MAIN,       "16. Language        ",LANGUAGE},
        {1,Function,MAIN,       "17. Restore Set     ",RESTORE},

        //***************************第二级菜单****************************/
        {2,Function,MODE,"1.1R Working Mode   ",MODE_1},
        {2,Function,MODE,"2.2R Working Mode   ",MODE_2},
        {2,Function,MODE,"3.3R Working Mode   ",MODE_3},
        {2,Function,MODE,"4.4R Working Mode   ",MODE_4},

        {3,Function,OSW_STARE,"1.1R Channel Select ",OSW_STARE_1},
        {3,Function,OSW_STARE,"2.2R Channel Select ",OSW_STARE_2},
        {3,Function,OSW_STARE,"3.3R Channel Select ",OSW_STARE_3},
        {3,Function,OSW_STARE,"4.4R Channel Select ",OSW_STARE_4},

        {4,Function,POWER,"1.1R1 Power Switch  ",POWER_1},
        {4,Function,POWER,"2.1R2 Power Switch  ",POWER_2},
        {4,Function,POWER,"3.1R3 Power Switch  ",POWER_3},
        {4,Function,POWER,"4.1R4 Power Switch  ",POWER_4},
        {4,Function,POWER,"5.2R1 Power Switch  ",POWER_5},
        {4,Function,POWER,"6.2R2 Power Switch  ",POWER_6},
        {4,Function,POWER,"7.2R3 Power Switch  ",POWER_7},
        {4,Function,POWER,"8.2R4 Power Switch  ",POWER_8},

        {5,Function,WAVE,"1.1R1 Wave Select   ",WAVE_1},
        {5,Function,WAVE,"2.1R2 Wave Select   ",WAVE_2},
        {5,Function,WAVE,"3.1R3 Wave Select   ",WAVE_3},
        {5,Function,WAVE,"4.1R4 Wave Select   ",WAVE_4},
        {5,Function,WAVE,"5.2R1 Wave Select   ",WAVE_5},
        {5,Function,WAVE,"6.2R2 Wave Select   ",WAVE_6},
        {5,Function,WAVE,"7.2R3 Wave Select   ",WAVE_7},
        {5,Function,WAVE,"8.2R4 Wave Select   ",WAVE_8},

        {6,Function,BACK_AUTO,"1.1R Man to Auto DLY",BACK_AUTO_1},
        {6,Function,BACK_AUTO,"2.2R Man to Auto DLY",BACK_AUTO_2},
        {6,Function,BACK_AUTO,"3.3R Man to Auto DLY",BACK_AUTO_3},
        {6,Function,BACK_AUTO,"4.4R Man to Auto DLY",BACK_AUTO_4},

        {7,Function,BACK_TYPE,"1.1R Back Mode      ",BACK_TYPE_1},
        {7,Function,BACK_TYPE,"2.2R Back Mode      ",BACK_TYPE_2},
        {7,Function,BACK_TYPE,"3.3R Back Mode      ",BACK_TYPE_3},
        {7,Function,BACK_TYPE,"4.4R Back Mode      ",BACK_TYPE_4},

        {8,Function,BACK_DELAY,"1.1R Back delay     ",BACK_DELAY_1},
        {8,Function,BACK_DELAY,"2.2R Back delay     ",BACK_DELAY_2},
        {8,Function,BACK_DELAY,"3.3R Back delay     ",BACK_DELAY_3},
        {8,Function,BACK_DELAY,"4.4R Back delay     ",BACK_DELAY_4},

        {9,Function,ICMP_INTERVAL,"1.1R ICMP gap set   ",ICMP_INTERVAL_1},
        {9,Function,ICMP_INTERVAL,"2.2R ICMP gap set   ",ICMP_INTERVAL_2},
        {9,Function,ICMP_INTERVAL,"3.3R ICMP gap set   ",ICMP_INTERVAL_3},
        {9,Function,ICMP_INTERVAL,"4.4R ICMP gap set   ",ICMP_INTERVAL_4},

        {10,Function,TIME_OUT,"1.1R Get ICMP TIMout",TIME_OUT_1},
        {10,Function,TIME_OUT,"2.2R Get ICMP TIMout",TIME_OUT_2},
        {10,Function,TIME_OUT,"3.3R Get ICMP TIMout",TIME_OUT_3},
        {10,Function,TIME_OUT,"4.4R Get ICMP TIMout",TIME_OUT_4},

        {11,Function,LOST_MAX,"1.1R Last lost ICMP ",LOST_MAX_1},
        {11,Function,LOST_MAX,"2.2R Last lost ICMP ",LOST_MAX_2},
        {11,Function,LOST_MAX,"3.3R Last lost ICMP ",LOST_MAX_3},
        {11,Function,LOST_MAX,"4.4R Last lost ICMP ",LOST_MAX_4},
    }
};

/*****************************************************************************
**函 数 名: FunctionAction
**功能描述: 具体功能散转函数
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void FunctionAction (void)
{
    unsigned short q_power_temp;

    switch ( SelectIndex )
    {
        //**************************开机延时设置**************************/
    case START_DELAY:
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )        //计数按Enter键的次数
            {
                Enter_counter=1;
                Key_temp16=EPROM.Start_delay;
            }
            else if( Enter_counter == 1 )   //计数按Enter键的次数
            {
                EPROM.Start_delay=Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.Start_delay, 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"PWR-ON delay:      S");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,7);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,8);
        }
        else
        {
            LCD_PutStr(0,0,"  开机延时:       秒");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,6);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,7);
        }
        break;

        //****************************新地址设置**************************/
    case CMD_ADDRESS:
        if( KeyStyle == Up )
        {
            Key_temp++;
            if(Key_temp>99) Key_temp=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp--;
            if(Key_temp==0xff) Key_temp=99;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0)
            {
                Enter_counter=1;
                Key_temp = EPROM.address;
            }
            else if( Enter_counter == 1 )  //计数按Enter键的次数
            {
                EPROM.address = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.address, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"New Address:         ");
        }
        else
        {
            LCD_PutStr(0,0,"新指令地址:         ");
        }
        LCD_PutStr(1,0,"                    ");
        LCD_SetCursor(0,6);
        LCD_WriteData(Key_temp%100/10+'0');
        LCD_WriteData(Key_temp%10+'0');
        LCD_SetCursor(0,6);
        break;

        //*************************LCD 背光延时时间设置 **************************/
    case LCD_BTIME :

        if( KeyStyle == Up )
        {
            Key_temp++;
            if(Key_temp > 6 ) Key_temp=1;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp--;
            if(Key_temp == 0 ) Key_temp=6;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = EPROM.LCDT;
            }
            else if(Enter_counter++ >=1)  //计数按Enter键的次数
            {
                EPROM.LCDT = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.LCDT, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"15s   | 30s   |  1m ");
            LCD_PutStr(1,0," 2m   |  5m   | Long");
        }
        else
        {
            LCD_PutStr(0,0,"15秒  | 30秒  |  1分");
            LCD_PutStr(1,0," 2分  |  5分  | 长亮");
        }
        switch ( Key_temp )
        {
        case 1 :
            LCD_SetCursor(0,0);
            break;
        case 2 :
            LCD_SetCursor(0,4);
            break;
        case 3 :
            LCD_SetCursor(0,8);
            break;
        case 4 :
            LCD_SetCursor(1,0);
            break;
        case 5 :
            LCD_SetCursor(1,4);
            break;
        case 6 :
            LCD_SetCursor(1,8);
            break;
        default:
            break;
        }
        break;

        //***************************告警声设置设置 **************************/
    case BEE :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.BEEflag;
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.BEEflag=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.BEEflag, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Turn on           ");
            LCD_PutStr(1,0,"2.Turn off          ");
        }
        else
        {
            LCD_PutStr(0,0,"1.开启              ");
            LCD_PutStr(1,0,"2.关闭              ");
        }
        LCD_SetCursor((!Key_temp),0);
        break;

        // *************************波特率设置   **************************/
    case UART_BPS :
        if( KeyStyle == Up )
        {
            Key_temp++;
            if(Key_temp > 9 ) Key_temp=1;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp--;
            if(Key_temp == 0 ) Key_temp=9;
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.BPS;
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.BPS=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.BPS, 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Baud rate:          ");
            LCD_PutStr(1,0,"Setting must restart");
        }
        else
        {
            LCD_PutStr(0,0,"  波特率:           ");
            LCD_PutStr(1,0,"  设置后重启才生效  ");
        }

        switch ( Key_temp )
        {
        case 1 :
            LCD_PutStr(0,5,"2400");
            break;
        case 2 :
            LCD_PutStr(0,5,"4800");
            break;
        case 3 :
            LCD_PutStr(0,5,"9600");
            break;
        case 4 :
            LCD_PutStr(0,5,"14400");
            break;
        case 5 :
            LCD_PutStr(0,5,"19200");
            break;
        case 6 :
            LCD_PutStr(0,5,"38400");
            break;
        case 7 :
            LCD_PutStr(0,5,"56000");
            break;
        case 8 :
            LCD_PutStr(0,5,"57600");
            break;
        case 9 :
            LCD_PutStr(0,5,"115200");
            break;
        default:
            break;
        }
        LCD_SetCursor(0,5);
        break;

        // *************************语言设置   **************************/
    case LANGUAGE :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.Language;
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.Language=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.Language, 1);
                show_Set_ok();
            }
        }
        LCD_PutStr(0,0,"1.中文              ");
        LCD_PutStr(1,0,"2.English           ");
        LCD_SetCursor(Key_temp,0);
        break;

        // *************************恢复出厂设置   **************************/
    case RESTORE :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = 0;
            }
            else if( Enter_counter == 1 )
            {
                if(Key_temp==1)
                {
                    restore_set();          //恢复出厂设置
                    if ( EPROM.Language )
                    {
                        LCD_PutStr(0,0,"   Restore Set OK   ");
                        LCD_PutStr(1,0,"                    ");
                    }
                    else
                    {
                        LCD_PutStr(0,0,"  恢复出厂设置成功  ");
                        LCD_PutStr(1,0,"                    ");
                    }
                    OSTimeDly(1000);

                    Reset_Handler();    //复位 !!!!!!!!!!!!!!
                }
                else
                {
                    MenuCancelOption();
                    break;
                }

            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Cancel            ");
            LCD_PutStr(1,0,"2.Confirm           ");
        }
        else
        {
            LCD_PutStr(0,0,"1.取消              ");
            LCD_PutStr(1,0,"2.确定              ");
        }
        LCD_SetCursor(Key_temp,0);
        break;

        // *************************4路的工作模式 设置  **************************/
    case MODE_1 :
    case MODE_2 :
    case MODE_3 :
    case MODE_4 :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = EPROM.Autoflag[SelectIndex-MODE_1];
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                EPROM.Autoflag[SelectIndex-MODE_1]=Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.Autoflag[SelectIndex-MODE_1], 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Auto Mode         ");
            LCD_PutStr(1,0,"2.Manual Mode       ");
        }
        else
        {
            LCD_PutStr(0,0,"1.自动模式          ");
            LCD_PutStr(1,0,"2.手动模式          ");
        }

        LCD_SetCursor((!Key_temp),0);
        break;

        // ***************************4路的通道选择 设置  **************************/
    case OSW_STARE_1 :
    case OSW_STARE_2 :
    case OSW_STARE_3 :
    case OSW_STARE_4 :
        if( KeyStyle == Up || KeyStyle == Dwon )
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if( Enter_counter == 0 )
            {
                Key_temp = OSW_state[SelectIndex-OSW_STARE_1];
                Enter_counter = 1;
            }
            else if( Enter_counter == 1 )
            {
                //先更改为手动模式
                EPROM.Autoflag[SelectIndex-OSW_STARE_1] = 0;
                Save_To_EPROM((uint8 *)&EPROM.Autoflag[SelectIndex-OSW_STARE_1], 1);

                if ( Key_temp == 1)         MostSwitch(SelectIndex-OSW_STARE_1);
                else if ( Key_temp == 0)    BypassSwitch(SelectIndex-OSW_STARE_1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Primary           ");
            LCD_PutStr(1,0,"2.Secondary         ");
        }
        else
        {
            LCD_PutStr(0,0,"1.主路-Pri (在线)   ");
            LCD_PutStr(1,0,"2.旁路-BPS          ");
        }
        LCD_SetCursor((!Key_temp),0);
        break;

        //*******************************切换功率 设置 **************************/
    case POWER_1 :
    case POWER_2 :
    case POWER_3 :
    case POWER_4 :
    case POWER_5 :
    case POWER_6 :
    case POWER_7 :
    case POWER_8 :
        if( KeyStyle == Up )
        {
            power_temp += 0.5;
            if(power_temp > 30.0)  power_temp = -50.0;
        }
        else if( KeyStyle == Dwon)
        {
            power_temp -= 0.5;
            if(power_temp < -50.0)  power_temp = 30.0;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                power_temp = EPROM.q_power[SelectIndex-POWER_1];
            }
            else if( Enter_counter == 1 )                 //计数按Enter键的次数
            {
                EPROM.q_power[SelectIndex-POWER_1]=power_temp;
                Save_To_EPROM((uint8 *)&EPROM.q_power[SelectIndex-POWER_1], 4);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"SwitchPWR:       dBm");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"切换功率:        dBm");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,5);
        if(power_temp <0 )
            LCD_WriteData('-');
        q_power_temp = power_temp*100;           //放大100倍便于显示
        LCD_WriteData(q_power_temp%10000/1000+'0');
        LCD_WriteData(q_power_temp%1000/100+'0');
        LCD_WriteData('.');
        LCD_WriteData(q_power_temp%100/10+'0');
        LCD_WriteData(q_power_temp%10+'0');
        LCD_SetCursor(0,5);
        break;

        //************************************波长选择**************************/
    case WAVE_1 :
    case WAVE_2 :
    case WAVE_3 :
    case WAVE_4 :
    case WAVE_5 :
    case WAVE_6 :
    case WAVE_7 :
    case WAVE_8 :
        if( KeyStyle == Up || KeyStyle == Dwon )
        {
            Key_temp=(!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = EPROM.wavelength[SelectIndex-WAVE_1];
            }
            else if(Enter_counter == 1 )  //计数按Enter键的次数
            {
                EPROM.wavelength[SelectIndex-WAVE_1] = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.wavelength[SelectIndex-WAVE_1], 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Wave Select:850nm   ");
            LCD_PutStr(1,0,"Wave Select:1550nm  ");
        }
        else
        {
            LCD_PutStr(0,0,"  波长选择: 850nm   ");
            LCD_PutStr(1,0,"  波长选择: 1550nm  ");
        }
        LCD_SetCursor(Key_temp,7);
        break;

        //*********************************手动返回自动延时*************************/
    case BACK_AUTO_1 :
    case BACK_AUTO_2 :
    case BACK_AUTO_3 :
    case BACK_AUTO_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.Auto_Manual_delay[SelectIndex-BACK_AUTO_1];
            }
            else if(Enter_counter==1)  //计数按Enter键的次数
            {
                EPROM.Auto_Manual_delay[SelectIndex-BACK_AUTO_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.Auto_Manual_delay[SelectIndex-BACK_AUTO_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Return Time:     m  ");
            LCD_PutStr(1,0,"0000m is no return  ");
        }
        else
        {
            LCD_PutStr(0,0,"  返回时间:     分钟");
            LCD_PutStr(1,0,"0000分钟为不自动返回");
        }
        LCD_SetCursor(0,6);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,7);
        break;

        //**********************************回切方式设置**************************/
    case BACK_TYPE_1 :
    case BACK_TYPE_2 :
    case BACK_TYPE_3 :
    case BACK_TYPE_4 :
        if( KeyStyle == Up || KeyStyle == Dwon)
        {
            Key_temp = (!Key_temp);
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter = 1;
                Key_temp = EPROM.accessflag[SelectIndex-BACK_TYPE_1];
            }
            else if(Enter_counter==1)  //计数按Enter键的次数
            {
                EPROM.accessflag[SelectIndex-BACK_TYPE_1] = Key_temp;
                Save_To_EPROM((uint8 *)&EPROM.accessflag[SelectIndex-BACK_TYPE_1], 1);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"1.Auto Back         ");
            LCD_PutStr(1,0,"2.Manual Back       ");
        }
        else
        {
            LCD_PutStr(0,0,"1.自动回切          ");
            LCD_PutStr(1,0,"2.不自动回切        ");
        }
        LCD_SetCursor( (!Key_temp), 0);
        break;

        //**********************************回切延时设置**************************/
    case BACK_DELAY_1 :
    case BACK_DELAY_2 :
    case BACK_DELAY_3 :
    case BACK_DELAY_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter==0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.BackOSW_Delay[SelectIndex-BACK_DELAY_1];
            }
            else if(Enter_counter==1)  //计数按Enter键的次数
            {
                EPROM.BackOSW_Delay[SelectIndex-BACK_DELAY_1]=Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.BackOSW_Delay[SelectIndex-BACK_DELAY_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Back delay:      S  ");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"  回切时间:       秒");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,6);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,7);
        break;


        //********************************心跳时间间隔设置 **************************/
    case ICMP_INTERVAL_1 :
    case ICMP_INTERVAL_2 :
    case ICMP_INTERVAL_3 :
    case ICMP_INTERVAL_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.Send_ICMP_interval[SelectIndex-ICMP_INTERVAL_1];
            }
            else if(Enter_counter==1)  //计数按Enter键的次数
            {
                EPROM.Send_ICMP_interval[SelectIndex-ICMP_INTERVAL_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.Send_ICMP_interval[SelectIndex-ICMP_INTERVAL_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Gap Time:     (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"间隔时间:     (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,5);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,6);
        break;

        //***********************************接收超时设置  **************************/
    case TIME_OUT_1 :
    case TIME_OUT_2 :
    case TIME_OUT_3 :
    case TIME_OUT_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.time_out[SelectIndex-TIME_OUT_1];
            }
            else if( Enter_counter == 1 )  //计数按Enter键的次数
            {
                EPROM.time_out[SelectIndex-TIME_OUT_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.time_out[SelectIndex-TIME_OUT_1], 2);
                show_Set_ok();
            }
        }

        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Timeout:      (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        else
        {
            LCD_PutStr(0,0,"超时时间:     (10ms)");
            LCD_PutStr(1,0,"                    ");
        }
        LCD_SetCursor(0,5);
        LCD_WriteData(Key_temp16%10000/1000+'0');
        LCD_WriteData(Key_temp16%1000/100+'0');
        LCD_WriteData(Key_temp16%100/10+'0');
        LCD_WriteData(Key_temp16%10+'0');
        LCD_SetCursor(0,6);
        break;

        //*********************************连续丢包旁路设置  **************************/
    case LOST_MAX_1 :
    case LOST_MAX_2 :
    case LOST_MAX_3 :
    case LOST_MAX_4 :
        if( KeyStyle == Up )
        {
            Key_temp16++;
            if(Key_temp16>9999) Key_temp16=0;
        }
        else if( KeyStyle == Dwon)
        {
            Key_temp16--;
            if(Key_temp16==0xffff)  Key_temp16=9999;
        }
        else if( KeyStyle == Enter )
        {
            if ( Enter_counter == 0 )
            {
                Enter_counter=1;
                Key_temp16 = EPROM.lost_max[SelectIndex-LOST_MAX_1];
            }
            else if(Enter_counter==1)  //计数按Enter键的次数
            {
                EPROM.lost_max[SelectIndex-LOST_MAX_1] = Key_temp16;
                Save_To_EPROM((uint8 *)&EPROM.lost_max[SelectIndex-LOST_MAX_1], 2);
                show_Set_ok();
            }
        }
        if ( EPROM.Language )
        {
            LCD_PutStr(0,0,"Last lost ICMP:     ");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,8);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,9);
        }
        else
        {
            LCD_PutStr(0,0,"连续丢包:           ");
            LCD_PutStr(1,0,"                    ");
            LCD_SetCursor(0,5);
            LCD_WriteData(Key_temp16%10000/1000+'0');
            LCD_WriteData(Key_temp16%1000/100+'0');
            LCD_WriteData(Key_temp16%100/10+'0');
            LCD_WriteData(Key_temp16%10+'0');
            LCD_SetCursor(0,6);
        }
        break;

    default:
        break;
    }
}

/*****************************************************************************
**函 数 名: show_Set_ok
**功能描述: 显示 设置成功 函数
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void show_Set_ok( void )
{
    if ( EPROM.Language )
    {
        LCD_PutStr(0,0,"                    ");
        LCD_PutStr(1,0,"      Set OK        ");
    }
    else
    {
        LCD_PutStr(0,0,"                    ");
        LCD_PutStr(1,0,"      设置成功      ");
    }
    OSTimeDly(1000);
}

/*****************************************************************************
**函 数 名: DISlm16032
**功能描述: 刷新菜单功能函数
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void DISlm16032 (void)
{
    uint8 i;
    uint8 LineMete;

    LineMete = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2];     //循环量,显示行数
    LCD_WriteCmd(0x30);                                     //进入液晶普通指令模式
    LCD_WriteCmd(0x01);                                     //清屏
    OSTimeDly(2);
    LCD_WriteCmd(0x06);                                     //ENTRY MODE

    do                                                      //分别显示各行菜单项
    {
        LCD_SetCursor(0,0);
        for(i=0; i<20; i++)
            LCD_WriteData(sOption[EPROM.Language][FirstLineDisIndex].KeyWord[i]);
        if(--LineMete == 0) break;
        LCD_SetCursor(1,0);
        for(i=0; i<20; i++)
            LCD_WriteData(sOption[EPROM.Language][FirstLineDisIndex+1].KeyWord[i]);
        if(--LineMete == 0) break;
    }
    while(0);
    LCD_WriteCmd(0x0d);                                     //画面显示开,光标显示关,光标反白显示开
    LCD_SetCursor(SelectLine-1,0);                          //被选着行光标反白显示
}

/*****************************************************************************
**函 数 名: MenuLevelStart
**功能描述: 判断当前索引是否是表单第一项功能函数
**输入参数: void
**输出参数: 无
**返 回 值: 返回Bit标志，是第一项返回1，不是返回0
*****************************************************************************/
uint8 MenuLevelStart (void)    //判断当前索引是否为当前层第一个选项
{
    uint8 i = MENULEVEL;
    do
    {
        i--;
        if(SelectIndex == Level[i][0]) return 1;
    }
    while(i);
    return 0;
}

/*****************************************************************************
**函 数 名: MenuLevelEnd
**功能描述: 判断当前索引是否是表单最后一项功能函数
**输入参数: void
**输出参数: 无
**返 回 值: 没有参数，返回Bit标志，是最后一项返回1，不是返回0
*****************************************************************************/
uint8 MenuLevelEnd (void)
{
    uint8 i = MENULEVEL;
    do
    {
        i--;
        if(SelectIndex == Level[i][1]) return 1;
    }
    while(i);
    return 0;
}

/*****************************************************************************
**函 数 名: MenuUpOneOption
**功能描述: 菜单上移一项函数
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void MenuUpOneOption (void)
{
    if(Enter_Function != 1)
    {
        if(MenuLevelStart ())                                                   //如果当前为表单第一项
        {
            if(Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2]>=2)                      //并且表单中选项数目大于等于2个
            {
                FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-1;  //第一行显示索引号为倒数第二项
                SelectIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1];          //选择索引为表单最后一项
                SelectLine = 2;                                                 //标记选择行为第二行
                //DISlm16032 ();                                                //刷新屏幕显示
            }
            else                                                                //如果选项数目并不大于四个
            {
                SelectIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1];          //选择索引为当前表单最后一个
                SelectLine = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2];           //显示行表单数目(最后一个)
                //DISlm16032 ();                                                //刷新屏幕显示
            }
        }
        else                                                                    //如果当前不是开始索引
        {
            if(SelectLine==1)                                                   //并且已经在屏幕最上边一行
            {
                FirstLineDisIndex--;                //显示索引上移
                SelectIndex--;                      //选择索引自减
                SelectLine = 1;                     //选择行还是第一行
                //DISlm16032 ();                    //刷新屏幕
            }
            else                                    //如果不是第一行
            {
                SelectLine--;                       //选择行自减
                SelectIndex--;                      //选择索引自减
                //DISlm16032 ();                    //刷新屏幕显示
            }
        }

        DISlm16032 ();                              //刷新屏幕显示
    }
    else
    {
        FunctionAction ();                          //如果是功能选择项，进入功能分支判断函数
    }
}

/*****************************************************************************
**函 数 名: MenuDownOneOption
**功能描述: 菜单下移一项函数
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void MenuDownOneOption (void)
{
    if(Enter_Function != 1)
    {
        if(MenuLevelEnd ())                     //如果当前是表单最后一个索引
        {
            FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0];    //第一行显示索引为表单第一个选项
            SelectIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0];          //选择索引为表单第一个选项索引
            SelectLine = 1;                     //选择行为第一行
            //DISlm16032 ();                    //刷新显示
        }
        else                                    //如果不是最后的索引
        {
            if(SelectLine!=2)                   //如果当前不是屏幕最底行
            {
                SelectIndex++;                  //选择索引自加
                SelectLine++;                   //选择行下移
                //DISlm16032 ();                  //刷新显示
            }
            else                                //如果是屏幕最低行
            {
                FirstLineDisIndex++;            //第一行显示下移
                SelectIndex++;                  //选择索引自加
                //DISlm16032();                 //刷新显示
            }
        }

        DISlm16032();                           //刷新显示
    }
    else
    {
        FunctionAction ();                      //如果是功能选择项，进入功能分支判断函数
    }
}

/*****************************************************************************
**函 数 名: MenuEnterOption
**功能描述: 进入某项功能函数
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void MenuEnterOption (void)
{
    if(Enter_Function != 1)
    {
        if(sOption[EPROM.Language][SelectIndex].EnterIndex == Function)
        {
            LastIndex = SelectIndex;                            //标记进入前的索引号（以便判断具体功能）
            Enter_Function = 1;                                 //标记进入散转函数
            FunctionAction ();                                  //如果是功能选择项，进入功能分支判断函数
        }
        else
        {
            SelectIndex = sOption[EPROM.Language][SelectIndex].EnterIndex;      //更新选择索引为之前索引号对应进入索引
            if(SelectIndex != Function)                         //如果当前索引不是功能选择索引
            {
                FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0]; //第一行显示为进入表单第一项
                SelectLine = 1;                                 //设定第一行为选择行
                DISlm16032 ();                                  //刷新菜单
            }
        }
    }
    else
    {
        FunctionAction ();                                      //如果是功能选择项，进入功能分支判断函数
    }
}

/*****************************************************************************
**函 数 名: MenuCancelOption
**功能描述:  菜单退出功能函数
**输入参数: void
**输出参数: 无
**返 回 值: 没有参数，没有返回值
*****************************************************************************/
uint8 MenuCancelOption (void)
{
    if(Enter_Function != 1)
    {
        SelectIndex = sOption[EPROM.Language][SelectIndex].CancelIndex; //选择索引为选项返回索引
        if ( SelectIndex == 0 )
        {
            Menu_layer=0;                                              //退出回到 0级菜单(主菜单)
            Trends_display=1;
            return 0;
        }
    }
    else
    {
        SelectIndex = LastIndex;                                    //当前的索引号等于进入功能函数前的索引号
        Enter_Function = 0;                                         //退出功能函数
        Enter_counter = 0;                                          //退出功能函数后清零Enter_counter
    }

    if(Level[sOption[EPROM.Language][SelectIndex].KeyLevel][2]>=2)                  //如果返回表单选项数目大于2个
    {
        if(SelectIndex > Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-1) //根据返回选项确定显示首项
        {
            FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-1;
            SelectLine = 2-(Level[sOption[EPROM.Language][SelectIndex].KeyLevel][1]-SelectIndex);
        }
        else                                                        //一般显示方式
        {
            FirstLineDisIndex = SelectIndex;                        //第一行显示索引
            SelectLine = 1;                                         //选择第一行
        }
    }
    else                                                            //如果返回表单选项数目不足4个
    {
        FirstLineDisIndex = Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0];   //第一行显示索引为表单第一项
        SelectLine = SelectIndex - Level[sOption[EPROM.Language][SelectIndex].KeyLevel][0]+1; //选择行标志为当前选择索引对应行
    }

    DISlm16032 ();                                                  //刷新菜单
    return 0;
}

/*****************************************************************************
**函 数 名: KeyCodeAction
**功能描述: 菜单操作按键处理散转函数
**输入参数: uint8 KeyCode
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void KeyCodeAction (uint8 KeyCode)
{
    switch (KeyCode)
    {
    case Up:
        MenuUpOneOption();
        break;              //如果是向上按键，则菜单向上，以下类似
    case Dwon:
        MenuDownOneOption();
        break;
    case Enter:
        MenuEnterOption();
        break;
    case Enter_Long:
        MenuCancelOption();
        break;
    default:
        break;
    }
}

/*****************************************************************************
**函 数 名: Goto_mian_menu
**功能描述: LCD 显示返回初始主菜单界面
**输入参数: void
**输出参数: 无
**返 回 值: 无
*****************************************************************************/
void Goto_mian_menu(void)
{
    Menu_layer=0;
    Trends_display = 1;

    FirstLineDisIndex = 0;    //屏幕第一行显示的索引号
    SelectLine = 0;           //当前选择的行
    SelectIndex = 0;          //当前选定行对应的索引号
    LastIndex = 0;            //进入功能函数前的索引号，判断具体功能使用
    Enter_Function = 0;       //进入功能函数的标示
    Enter_counter=0;          //按Enter键次数的计数变量
    Menu_Index=0;
}

