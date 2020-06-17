#include "GUI.h"
#include "DIALOG.h"
#include "BUTTON.h"
#include "led.h"
#include "buttonbmp.h"
#include "buttonbmpdisp.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniSTM32开发板
//STemWin BUTTON位图显示
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_0 (GUI_ID_USER + 0x03)

GUI_BITMAP buttonbmp_tab[2];

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 200, 240, 120, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "Button",     ID_BUTTON_0, 30, 20, 80, 30, 0, 0x0, 0 },
};

//对话框回调函数
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	static u8 ledflag=0;

	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			break;
		case WM_INIT_DIALOG:
			//初始化对话框
			hItem = pMsg->hWin;
			FRAMEWIN_SetTitleHeight(hItem, 30);
			FRAMEWIN_SetText(hItem, "ALIENTEK BUTTONBMP DISP");
			FRAMEWIN_SetFont(hItem, GUI_FONT_24_ASCII);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetTextColor(hItem, 0x0000FFFF);
		
			//初始化BUTTON0
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			BUTTON_SetBitmapEx(hItem,0,&buttonbmp_tab[0],0,0);
			BUTTON_SetText(hItem, "");
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case ID_BUTTON_0: //BUTTON_0的通知代码，控制LED1

					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
							LED1=~LED1;		//LED1反转
							break;	
					}
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

//创建一个对话框
WM_HWIN CreateFramewin(void) 
{
	WM_HWIN hWin;
	buttonbmp_tab[0]=bmBUTTONOFF;
	buttonbmp_tab[1]=bmBUTTONON;
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	return hWin;
}

//BUTTON按钮上显示位图
void Buttonbmp_Demo(void)
{
	WM_HWIN hWin;
	hWin=CreateFramewin();
	while(1)
	{
		GUI_Delay(100);
	}
}

































