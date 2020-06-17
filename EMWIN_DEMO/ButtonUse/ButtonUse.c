#include "DIALOG.h"
#include "ButtonUse.h"
#include "led.h"
#include "EmWinHZFont.h"
#include "timer.h"
#include "usart.h"	
#include "string.h"	
#include "delay.h"	
#include "timer.h"

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x03)

u8 buttonflag=0;//选择哪个用电设备
u16 width_tt=41;//起始的时间
u16 width_tt_tt=45;//当前的时间  两个次差值为宽度

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "",   ID_FRAMEWIN_0, 0, 415, 320, 60, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "空调",      ID_BUTTON_0, 30,  1, 60, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "微波炉",    ID_BUTTON_1, 120, 1, 60, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "冰箱",      ID_BUTTON_2, 210, 1, 60, 40, 0, 0x0, 0 },
};

//对话框回调函数
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			//初始化对话框
			hItem = pMsg->hWin;
		
			//初始化Tempadd
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
		  BUTTON_SetFont(hItem, &GUI_FontHZ16);
		
			//初始化Timeadd
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetFont(hItem, &GUI_FontHZ16);		
	
			//初始化set
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
			BUTTON_SetFont(hItem, &GUI_FontHZ16);			
			
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case ID_BUTTON_0:
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放											
						
						//清空屏幕坐标处
						GUI_SetAlpha(0x00);				
						GUI_SetColor(GUI_WHITE);	
						GUI_FillRect(41,380-290,300,379);			
						width_tt=41;
						width_tt_tt=45;		

						GUI_SetColor(GUI_BLACK);
						GUI_SetFont(&GUI_FontHZ16);
						GUI_DispStringAt(" 空调用电情况 ",120,90);	
						
						TIM_Cmd(TIM3,ENABLE);
						buttonflag=1;//空调
						timett=0;//清空时间						
						
							break;	
					}
					break;
				case ID_BUTTON_1: 
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
						
						//清空屏幕坐标处
						GUI_SetAlpha(0x00);				
						GUI_SetColor(GUI_WHITE);	
						GUI_FillRect(41,380-290,300,379);		
						width_tt=41;
						width_tt_tt=45;		
					
						GUI_SetColor(GUI_BLACK);
						GUI_SetFont(&GUI_FontHZ16);
						GUI_DispStringAt("微波炉用电情况",120,90);
						
            TIM_Cmd(TIM3,ENABLE);						
						buttonflag=2;//微波炉
						timett=0;//清空时间						
						
							break;	
					}
					break;
				case ID_BUTTON_2: 
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
						
						//清空屏幕坐标处
						GUI_SetAlpha(0x00);				
						GUI_SetColor(GUI_WHITE);	
						GUI_FillRect(41,380-290,300,379);		
						width_tt=41;
						width_tt_tt=45;		
					
						GUI_SetColor(GUI_BLACK);
						GUI_SetFont(&GUI_FontHZ16);
						GUI_DispStringAt(" 冰箱用电情况 ",120,90);	
						
						TIM_Cmd(TIM3,ENABLE);
						buttonflag=3;//冰箱
						timett=0;//清空时间						
						
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

	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	return hWin;
}

//BUTTO演示函数
void ButtonUse_Demo(void)
{
	u8 AirData=0,MicData=0,RefData=0;//空调,微波炉,冰箱用电量
	
	WM_HWIN hWin;
	hWin=CreateFramewin();
	
	while(1)
	{
		
		if(sign==1)//接收到上位机数据
		{
		  sign=0;
			if(Sys_buf[15]==';'){
				AirData=(Sys_buf[14]-0x30);
			}else
				AirData=(Sys_buf[14]-0x30)*10+(Sys_buf[15]-0x30);//空调
			if(AirData>=100)AirData=100;
			
			if(Sys_buf[22]==';'){
			  MicData=(Sys_buf[21]-0x30);
			}else
				MicData=(Sys_buf[21]-0x30)*10+(Sys_buf[22]-0x30);//微波炉
			if(MicData>=100)MicData=100;
			
			if(Sys_buf[29]==';'){
				RefData=(Sys_buf[28]-0x30);
			}else
				RefData=(Sys_buf[28]-0x30)*10+(Sys_buf[29]-0x30);//冰箱
			if(RefData>=100)RefData=100;
			
			printf("空调用电量 = %d kw,微波炉用电量 = %d kw,冰箱用电量 = %d kw\r\n",AirData,MicData,RefData);//调试用
			
			memset(Sys_buf,0,sizeof(Sys_buf));
		}
		//处理显示
		if(buttonflag==1){//空调 
		
			if(timett>121)//排满整个坐标轴
			{ 
				//清空屏幕坐标处
				GUI_SetAlpha(0x00);				
				GUI_SetColor(GUI_WHITE);	
				GUI_FillRect(41,380-270,300,379);	
				
				timett=0;//清零
				//坐标归位
			  width_tt=41;
				width_tt_tt=45;
			}
			
			if(timeflag==1)//260/60=4
			{
				timeflag=0;//清除定时器标志				
				
				GUI_SetAlpha(0x40);				
				GUI_SetColor(GUI_RED);	
				GUI_FillRect(width_tt,379-AirData,width_tt_tt,379);//绘制空调用电情况
				
				//每次横坐标向右递增
				width_tt+=4;//240 最大
				width_tt_tt+=4;//
				
			}
			
		}else if(buttonflag==2){//微波炉
			if(timett>121)//排满整个坐标轴
			{ 
				//清空屏幕坐标处
				GUI_SetAlpha(0x00);				
				GUI_SetColor(GUI_WHITE);	
				GUI_FillRect(41,380-270,300,379);	
				
				timett=0;//清零
				//坐标归位
			  width_tt=41;
				width_tt_tt=45;
			}
			
			if(timeflag==1)//260/60=4
			{
				timeflag=0;//清除定时器标志				
				
				GUI_SetAlpha(0x40);				
				GUI_SetColor(GUI_RED);	
				GUI_FillRect(width_tt,379-MicData,width_tt_tt,379);//绘制微波炉用电情况
				
				//每次横坐标向右递增
				width_tt+=4;//240 最大
				width_tt_tt+=4;//
				
			}			
		
		}else if(buttonflag==3){//冰箱
		
			if(timett>121)//排满整个坐标轴
			{ 
				//清空屏幕坐标处
				GUI_SetAlpha(0x00);				
				GUI_SetColor(GUI_WHITE);	
				GUI_FillRect(41,380-270,300,379);	
				
				timett=0;//清零
				//坐标归位
			  width_tt=41;
				width_tt_tt=45;
			}
			
			if(timeflag==1)//260/60=4
			{
				timeflag=0;//清除定时器标志				
				
				GUI_SetAlpha(0x40);				
				GUI_SetColor(GUI_RED);	
				GUI_FillRect(width_tt,379-RefData,width_tt_tt,379);//绘制冰箱用电情况
				
				//每次横坐标向右递增
				width_tt+=4;//240 最大
				width_tt_tt+=4;//
				
			}			
		}
		
		GUI_Delay(100);
	}
}

