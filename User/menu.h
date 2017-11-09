/******************************************************************************

                  版权所有 (C), 2001-2013, 桂林恒毅金宇通信技术有限公司

 ******************************************************************************
  文 件 名   : menu.h
  版 本 号   : 初稿
  作    者   : 123
  生成日期   : 2013年4月22日
  最近修改   :
  功能描述   : menu.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年4月22日
    作    者   : 123
    修改内容   : 创建文件

******************************************************************************/

#ifndef __MENU_H__
#define __MENU_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define Up 					1
#define Up_Long 		    2
#define Dwon				3
#define Dwon_Long			4
#define Enter				5
#define Enter_Long			6
#define Up_And_Dwon			7
#define Dwon_Long_long      8


extern unsigned char FirstLineDisIndex ;   //屏幕第一行显示的索引号
extern unsigned char SelectLine;           //当前选择的行
extern unsigned char SelectIndex;          //当前选定行对应的索引号
extern unsigned char LastIndex;            //进入功能函数前的索引号，判断具体功能使用
extern unsigned char Enter_Function;       //进入功能函数的标示
extern unsigned char Enter_counter;        //按Enter键次数的计数变量

extern void DISlm16032 (void);
extern void Function1(void);
extern void FunctionAction (void);
extern void show_Set_ok(void);
extern void KeyCodeAction (unsigned char KeyCode);
extern uint8 MenuCancelOption (void);
extern void MenuDownOneOption (void);
extern void MenuEnterOption (void);
extern unsigned char MenuLevelEnd (void);
extern unsigned char MenuLevelStart (void);
extern void MenuUpOneOption (void);
extern void NoThisFunction (void);
extern void showsuccess(void);
extern void Goto_mian_menu(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MENU_H__ */
