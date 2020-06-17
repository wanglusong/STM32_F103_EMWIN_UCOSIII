#include "2ddisplay.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "EmWinHZFont.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103战舰开发板
//STemWin 2D绘图 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
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

GUI_POINT aEnlargedPoints[GUI_COUNTOF(aPoints)]; 	//多边形放大后坐标
GUI_POINT aMagnifiedPoints[GUI_COUNTOF(aPoints1)];

//2D图形绘制例程
void display_2d(void)
{
	int i;
	GUI_SetBkColor(GUI_BLUE);		//设置背景颜色
	GUI_Clear();
	
	GUI_SetColor(GUI_YELLOW);
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringHCenterAt("ALIENTEK 2D DISPALY",400,10);
	
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font16_ASCII);
	
	GUI_SetBkColor(GUI_GREEN);
	GUI_ClearRect(10,50,100,150);	//在一个矩形区域填充绿色背景
	GUI_SetBkColor(GUI_BLUE);		//背景改回蓝色
	
	GUI_DrawGradientH(110,50,210,150,0X4117BB,0XC6B6F5);	//绘制用水平颜色梯度填充的矩形
	GUI_DrawGradientV(220,50,320,150,0X4117BB,0XC6B6F5);	//绘制用垂直颜色梯度填充的矩形
	GUI_DrawGradientRoundedH(330,50,430,150,25,0X4117BB,0XC6B6F5);	//绘制水平颜色梯度填充的圆角矩形
	GUI_DrawGradientRoundedV(440,50,540,150,25,0X4117BB,0XC6B6F5);	//绘制垂直颜色梯度填充的圆角矩形
	
	GUI_DrawRect(550,50,650,150);	//在当前窗口中指定的位置绘制矩形
	GUI_FillRect(660,50,760,150);	//在当前窗口中指定位置绘制填充的矩形区域
	
	GUI_SetPenSize(5);				//设置画笔颜色,单位像素点
	GUI_DrawLine(10,160,110,260);   //绘制线条
	
	for(i=0;i<50;i+=3) GUI_DrawCircle(200,220,i); 	//绘制圆，无法绘制半径大于180的圆
	GUI_FillCircle(320,220,50); 					//在指定位置绘制指定尺寸的填充的圆,无法绘制半径大于180的圆

	GUI_SetPenSize(2);
	GUI_SetColor(GUI_CYAN);
	GUI_DrawEllipse(450,220,70,50);	//在指定位置绘制指定尺寸的椭圆的轮廓 无法处理大于180的rx/ry参数
	GUI_SetColor(GUI_MAGENTA);
	GUI_FillEllipse(450,220,65,45);	//在指定位置绘制指定尺寸的填充的椭圆

	drawarcscale();
	draw_graph();
}

//Alpha混合显示
void alpha_display(void)
{
	GUI_EnableAlpha(1);				//使能Alpha功能
	GUI_SetBkColor(GUI_WHITE);		//设置背景颜色
	GUI_Clear();
	
	GUI_SetTextMode(GUI_TM_TRANS);	//文本透明显示
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font32_1);
	GUI_DispStringHCenterAt("ALIENTEK ALPHA TEST",400,220);
	
	GUI_SetAlpha(0x40);						//设置alpha值0x00-0XFF
	GUI_SetColor(GUI_RED);	
	GUI_FillRect(100,50,300,250);			//绘制红色矩形
	
	GUI_SetAlpha(0x80);	
	GUI_SetColor(GUI_GREEN);
	GUI_FillRect(200,150,400,350);			//绘制绿色矩形
	
	GUI_SetAlpha(0xC0);
	GUI_SetColor(GUI_BLUE);
	GUI_FillRect(300,250,500,450);			//绘制蓝色矩形
	
	GUI_SetAlpha(0x80);
	GUI_SetColor(GUI_YELLOW);				
	GUI_FillRect(400,150,600,350);			//绘制黄色矩形
	
	GUI_SetAlpha(0x40);			
	GUI_SetColor(GUI_BROWN);				
	GUI_FillRect(500,50,700,250);			//绘制棕色矩形
	GUI_SetAlpha(0);						//恢复alpha通道
}

//绘制弧线 绘制简易仪表盘
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

//绘制线图
void draw_graph(void)
{
	I16 aY[400];
	int i;
	for(i=0;i<GUI_COUNTOF(aY);i++)	aY[i] = rand()%100;
	GUI_ClearRect(0,300,400,400);   //每次显示前先清除显示区域
	GUI_DrawGraph(aY,GUI_COUNTOF(aY),0,300);
}

//绘制多边形
void draw_polygon(void)
{

//	GUI_SetBkColor(GUI_BLUE);
//	GUI_SetColor(GUI_YELLOW);
//	GUI_Clear();
	
	//GUI_DrawLineRel(300,200);//绘制按x、y,相对坐标.
	
//	GUI_SetPenSize(3);//线的粗细
//	GUI_SetColor(GUI_MAGENTA);
//	GUI_DrawLineTo(20,200);//横坐标20，纵坐标200
//  GUI_DrawLineTo(200,250);//横坐标200，纵坐标250
	
//	GUI_SetPenSize(3);//线的粗细
//	GUI_SetColor(GUI_GRAY);	
//	GUI_DrawLine(20,20,80,80);//从某个指定起点到指定终点线条
	
	
	//绘制坐标轴
	GUI_SetColor(GUI_BLACK);
	GUI_DrawHLine(380,20,300);//绘制水平线
	GUI_SetColor(GUI_BLACK);	
	GUI_DrawVLine(40,80,400);//绘制垂直线
  //设置纵坐标单位	
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font16_1);
	GUI_DispStringHCenterAt("KW",25,90);
	//绘制横坐标单位
	GUI_SetColor(GUI_BLACK);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt("时间/s",260,390);	   //范围 40-300(横坐标) 380-80(纵坐标)    260/60  
	
	//设置横坐标值
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
//	GUI_FillRect(40,380-20,45,380);//绘制用电情况
	
	
	//GUI_SetLineStyle(GUI_LS_SOLID);//线的样式
	
	//GUI_DrawPolyLine();
}


