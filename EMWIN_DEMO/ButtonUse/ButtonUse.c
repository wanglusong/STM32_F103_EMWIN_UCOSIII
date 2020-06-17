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

u8 buttonflag=0;//ѡ���ĸ��õ��豸
u16 width_tt=41;//��ʼ��ʱ��
u16 width_tt_tt=45;//��ǰ��ʱ��  �����β�ֵΪ���

//�Ի�����Դ��
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "",   ID_FRAMEWIN_0, 0, 415, 320, 60, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "�յ�",      ID_BUTTON_0, 30,  1, 60, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "΢��¯",    ID_BUTTON_1, 120, 1, 60, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "����",      ID_BUTTON_2, 210, 1, 60, 40, 0, 0x0, 0 },
};

//�Ի���ص�����
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;

	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			//��ʼ���Ի���
			hItem = pMsg->hWin;
		
			//��ʼ��Tempadd
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
		  BUTTON_SetFont(hItem, &GUI_FontHZ16);
		
			//��ʼ��Timeadd
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetFont(hItem, &GUI_FontHZ16);		
	
			//��ʼ��set
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
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�											
						
						//�����Ļ���괦
						GUI_SetAlpha(0x00);				
						GUI_SetColor(GUI_WHITE);	
						GUI_FillRect(41,380-290,300,379);			
						width_tt=41;
						width_tt_tt=45;		

						GUI_SetColor(GUI_BLACK);
						GUI_SetFont(&GUI_FontHZ16);
						GUI_DispStringAt(" �յ��õ���� ",120,90);	
						
						TIM_Cmd(TIM3,ENABLE);
						buttonflag=1;//�յ�
						timett=0;//���ʱ��						
						
							break;	
					}
					break;
				case ID_BUTTON_1: 
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
						
						//�����Ļ���괦
						GUI_SetAlpha(0x00);				
						GUI_SetColor(GUI_WHITE);	
						GUI_FillRect(41,380-290,300,379);		
						width_tt=41;
						width_tt_tt=45;		
					
						GUI_SetColor(GUI_BLACK);
						GUI_SetFont(&GUI_FontHZ16);
						GUI_DispStringAt("΢��¯�õ����",120,90);
						
            TIM_Cmd(TIM3,ENABLE);						
						buttonflag=2;//΢��¯
						timett=0;//���ʱ��						
						
							break;	
					}
					break;
				case ID_BUTTON_2: 
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
						
						//�����Ļ���괦
						GUI_SetAlpha(0x00);				
						GUI_SetColor(GUI_WHITE);	
						GUI_FillRect(41,380-290,300,379);		
						width_tt=41;
						width_tt_tt=45;		
					
						GUI_SetColor(GUI_BLACK);
						GUI_SetFont(&GUI_FontHZ16);
						GUI_DispStringAt(" �����õ���� ",120,90);	
						
						TIM_Cmd(TIM3,ENABLE);
						buttonflag=3;//����
						timett=0;//���ʱ��						
						
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

//����һ���Ի���
WM_HWIN CreateFramewin(void) 
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	return hWin;
}

//BUTTO��ʾ����
void ButtonUse_Demo(void)
{
	u8 AirData=0,MicData=0,RefData=0;//�յ�,΢��¯,�����õ���
	
	WM_HWIN hWin;
	hWin=CreateFramewin();
	
	while(1)
	{
		
		if(sign==1)//���յ���λ������
		{
		  sign=0;
			if(Sys_buf[15]==';'){
				AirData=(Sys_buf[14]-0x30);
			}else
				AirData=(Sys_buf[14]-0x30)*10+(Sys_buf[15]-0x30);//�յ�
			if(AirData>=100)AirData=100;
			
			if(Sys_buf[22]==';'){
			  MicData=(Sys_buf[21]-0x30);
			}else
				MicData=(Sys_buf[21]-0x30)*10+(Sys_buf[22]-0x30);//΢��¯
			if(MicData>=100)MicData=100;
			
			if(Sys_buf[29]==';'){
				RefData=(Sys_buf[28]-0x30);
			}else
				RefData=(Sys_buf[28]-0x30)*10+(Sys_buf[29]-0x30);//����
			if(RefData>=100)RefData=100;
			
			printf("�յ��õ��� = %d kw,΢��¯�õ��� = %d kw,�����õ��� = %d kw\r\n",AirData,MicData,RefData);//������
			
			memset(Sys_buf,0,sizeof(Sys_buf));
		}
		//������ʾ
		if(buttonflag==1){//�յ� 
		
			if(timett>121)//��������������
			{ 
				//�����Ļ���괦
				GUI_SetAlpha(0x00);				
				GUI_SetColor(GUI_WHITE);	
				GUI_FillRect(41,380-270,300,379);	
				
				timett=0;//����
				//�����λ
			  width_tt=41;
				width_tt_tt=45;
			}
			
			if(timeflag==1)//260/60=4
			{
				timeflag=0;//�����ʱ����־				
				
				GUI_SetAlpha(0x40);				
				GUI_SetColor(GUI_RED);	
				GUI_FillRect(width_tt,379-AirData,width_tt_tt,379);//���ƿյ��õ����
				
				//ÿ�κ��������ҵ���
				width_tt+=4;//240 ���
				width_tt_tt+=4;//
				
			}
			
		}else if(buttonflag==2){//΢��¯
			if(timett>121)//��������������
			{ 
				//�����Ļ���괦
				GUI_SetAlpha(0x00);				
				GUI_SetColor(GUI_WHITE);	
				GUI_FillRect(41,380-270,300,379);	
				
				timett=0;//����
				//�����λ
			  width_tt=41;
				width_tt_tt=45;
			}
			
			if(timeflag==1)//260/60=4
			{
				timeflag=0;//�����ʱ����־				
				
				GUI_SetAlpha(0x40);				
				GUI_SetColor(GUI_RED);	
				GUI_FillRect(width_tt,379-MicData,width_tt_tt,379);//����΢��¯�õ����
				
				//ÿ�κ��������ҵ���
				width_tt+=4;//240 ���
				width_tt_tt+=4;//
				
			}			
		
		}else if(buttonflag==3){//����
		
			if(timett>121)//��������������
			{ 
				//�����Ļ���괦
				GUI_SetAlpha(0x00);				
				GUI_SetColor(GUI_WHITE);	
				GUI_FillRect(41,380-270,300,379);	
				
				timett=0;//����
				//�����λ
			  width_tt=41;
				width_tt_tt=45;
			}
			
			if(timeflag==1)//260/60=4
			{
				timeflag=0;//�����ʱ����־				
				
				GUI_SetAlpha(0x40);				
				GUI_SetColor(GUI_RED);	
				GUI_FillRect(width_tt,379-RefData,width_tt_tt,379);//���Ʊ����õ����
				
				//ÿ�κ��������ҵ���
				width_tt+=4;//240 ���
				width_tt_tt+=4;//
				
			}			
		}
		
		GUI_Delay(100);
	}
}

