#include "2ddisplay.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "EmWinHZFont.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103ս��������
//STemWin 2D��ͼ 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

const GUI_POINT aPoints[]={
	{40,20},
	{0,20},
	{20,0}
};

const GUI_POINT aPoints1[]={
	{0,20},
	{40,20},
	{20,0}
};

const GUI_POINT aPointArrow[]={
	{ 0, -5},
	{-40, -35},
	{-10, -25},
	{-10, -85},
	{ 10, -85},
	{ 10, -25},
	{ 40, -35},
};

GUI_POINT aEnlargedPoints[GUI_COUNTOF(aPoints)]; 	//����ηŴ������
GUI_POINT aMagnifiedPoints[GUI_COUNTOF(aPoints1)];

//2Dͼ�λ�������
void display_2d(void)
{
	int i;
	GUI_SetBkColor(GUI_BLUE);		//���ñ�����ɫ
	GUI_Clear();
	
	GUI_SetColor(GUI_YELLOW);
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringHCenterAt("ALIENTEK 2D DISPALY",400,10);
	
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font16_ASCII);
	
	GUI_SetBkColor(GUI_GREEN);
	GUI_ClearRect(10,50,100,150);	//��һ���������������ɫ����
	GUI_SetBkColor(GUI_BLUE);		//�����Ļ���ɫ
	
	GUI_DrawGradientH(110,50,210,150,0X4117BB,0XC6B6F5);	//������ˮƽ��ɫ�ݶ����ľ���
	GUI_DrawGradientV(220,50,320,150,0X4117BB,0XC6B6F5);	//�����ô�ֱ��ɫ�ݶ����ľ���
	GUI_DrawGradientRoundedH(330,50,430,150,25,0X4117BB,0XC6B6F5);	//����ˮƽ��ɫ�ݶ�����Բ�Ǿ���
	GUI_DrawGradientRoundedV(440,50,540,150,25,0X4117BB,0XC6B6F5);	//���ƴ�ֱ��ɫ�ݶ�����Բ�Ǿ���
	
	GUI_DrawRect(550,50,650,150);	//�ڵ�ǰ������ָ����λ�û��ƾ���
	GUI_FillRect(660,50,760,150);	//�ڵ�ǰ������ָ��λ�û������ľ�������
	
	GUI_SetPenSize(5);				//���û�����ɫ,��λ���ص�
	GUI_DrawLine(10,160,110,260);   //��������
	
	for(i=0;i<50;i+=3) GUI_DrawCircle(200,220,i); 	//����Բ���޷����ư뾶����180��Բ
	GUI_FillCircle(320,220,50); 					//��ָ��λ�û���ָ���ߴ������Բ,�޷����ư뾶����180��Բ

	GUI_SetPenSize(2);
	GUI_SetColor(GUI_CYAN);
	GUI_DrawEllipse(450,220,70,50);	//��ָ��λ�û���ָ���ߴ����Բ������ �޷��������180��rx/ry����
	GUI_SetColor(GUI_MAGENTA);
	GUI_FillEllipse(450,220,65,45);	//��ָ��λ�û���ָ���ߴ��������Բ

	drawarcscale();
	draw_graph();
}

//Alpha�����ʾ
void alpha_display(void)
{
	GUI_EnableAlpha(1);				//ʹ��Alpha����
	GUI_SetBkColor(GUI_WHITE);		//���ñ�����ɫ
	GUI_Clear();
	
	GUI_SetTextMode(GUI_TM_TRANS);	//�ı�͸����ʾ
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font32_1);
	GUI_DispStringHCenterAt("ALIENTEK ALPHA TEST",400,220);
	
	GUI_SetAlpha(0x40);						//����alphaֵ0x00-0XFF
	GUI_SetColor(GUI_RED);	
	GUI_FillRect(100,50,300,250);			//���ƺ�ɫ����
	
	GUI_SetAlpha(0x80);	
	GUI_SetColor(GUI_GREEN);
	GUI_FillRect(200,150,400,350);			//������ɫ����
	
	GUI_SetAlpha(0xC0);
	GUI_SetColor(GUI_BLUE);
	GUI_FillRect(300,250,500,450);			//������ɫ����
	
	GUI_SetAlpha(0x80);
	GUI_SetColor(GUI_YELLOW);				
	GUI_FillRect(400,150,600,350);			//���ƻ�ɫ����
	
	GUI_SetAlpha(0x40);			
	GUI_SetColor(GUI_BROWN);				
	GUI_FillRect(500,50,700,250);			//������ɫ����
	GUI_SetAlpha(0);						//�ָ�alphaͨ��
}

//���ƻ��� ���Ƽ����Ǳ���
void drawarcscale(void) 
{
	int x0 = 630;
	int y0 = 380;
	int i;
	char ac[4];
	GUI_SetPenSize( 5 );
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(&GUI_FontComic18B_ASCII);
	GUI_SetColor(GUI_BLACK);
	GUI_DrawArc( x0,y0,150, 150,-30, 210 );
	for (i=0; i<= 23; i++) {
		float a = (-30+i*10)*3.1415926/180;
		int x = -141*cos(a)+x0;
		int y = -141*sin(a)+y0;
		if (i%2 == 0)
		GUI_SetPenSize( 5 );
		else
		GUI_SetPenSize( 4 );
		GUI_DrawPoint(x,y);
		if (i%2 == 0) {
			x = -123*cos(a)+x0;
			y = -130*sin(a)+y0;
			sprintf(ac, "%d", 10*i);
			GUI_SetTextAlign(GUI_TA_VCENTER);
			GUI_DispStringHCenterAt(ac,x,y);
		}
	}
}

//������ͼ
void draw_graph(void)
{
	I16 aY[400];
	int i;
	for(i=0;i<GUI_COUNTOF(aY);i++)	aY[i] = rand()%100;
	GUI_ClearRect(0,300,400,400);   //ÿ����ʾǰ�������ʾ����
	GUI_DrawGraph(aY,GUI_COUNTOF(aY),0,300);
}

//���ƶ����
void draw_polygon(void)
{

//	GUI_SetBkColor(GUI_BLUE);
//	GUI_SetColor(GUI_YELLOW);
//	GUI_Clear();
	
	//GUI_DrawLineRel(300,200);//���ư�x��y,�������.
	
//	GUI_SetPenSize(3);//�ߵĴ�ϸ
//	GUI_SetColor(GUI_MAGENTA);
//	GUI_DrawLineTo(20,200);//������20��������200
//  GUI_DrawLineTo(200,250);//������200��������250
	
//	GUI_SetPenSize(3);//�ߵĴ�ϸ
//	GUI_SetColor(GUI_GRAY);	
//	GUI_DrawLine(20,20,80,80);//��ĳ��ָ����㵽ָ���յ�����
	
	
	//����������
	GUI_SetColor(GUI_BLACK);
	GUI_DrawHLine(380,20,300);//����ˮƽ��
	GUI_SetColor(GUI_BLACK);	
	GUI_DrawVLine(40,80,400);//���ƴ�ֱ��
  //���������굥λ	
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font16_1);
	GUI_DispStringHCenterAt("KW",25,90);
	//���ƺ����굥λ
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt("ʱ��/s",260,390);	   //��Χ 40-300(������) 380-80(������)    260/60  
	
	//���ú�����ֵ
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font16_1);
	GUI_DispStringHCenterAt("20",75,390);	
	GUI_DispStringHCenterAt("40",105,390);	
	GUI_DispStringHCenterAt("60",135,390);
	GUI_DispStringHCenterAt("80",165,390);
	GUI_DispStringHCenterAt("100",195,390);
	GUI_DispStringHCenterAt("120",225,390);
	
//	// 20,50  //320()
//	GUI_SetAlpha(0x40);				
//	GUI_SetColor(GUI_RED);	
//	GUI_FillRect(40,380-20,45,380);//�����õ����
	
	
	//GUI_SetLineStyle(GUI_LS_SOLID);//�ߵ���ʽ
	
	//GUI_DrawPolyLine();
}


