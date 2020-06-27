#include "led.h"
#include "beep.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "ILI93xx.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "sram.h"
#include "timer.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "GUI.h"
#include "ff.h"
#include "exfuns.h"
#include "w25qxx.h"
#include "includes.h"
#include "fontupd.h"
#include "EmWinHZFont.h"
#include "WM.h"
#include "DIALOG.h"
#include "max6675.h"
#include "string.h"
#include "stdbool.h"
#include "pid.h"
#include "pwm.h"
#include "timer.h"
#include "fuzzypid.h"
#include "math.h"
#include "smithpid.h"
#include "USART2.h"
#include "esp8266.h"	

static uint8_t keyvlaue = 0;

typedef struct main_param
{
	uint16_t prestate;
	bool startstate;
	uint16_t start_stop;
	uint16_t settemp;
	float pidtemp;
	float temp;
	uint16_t firstsettemp;
	
	float esp_settemp;//设定温度
	float esp_currenttemp;//实时温度
	float esp_difftemp;//差值温度
	int adjusttime;//调节时间
	
}MAIN_PARAM;

MAIN_PARAM main_param_t;

//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO				4
//任务堆栈大小
#define TOUCH_STK_SIZE				128
//任务控制块
OS_TCB TouchTaskTCB;
//任务堆栈
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch任务
void touch_task(void *p_arg);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO 				5
//任务堆栈大小
#define LED0_STK_SIZE				128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0任务
void led0_task(void *p_arg);

//字库更新任务
//设置任务优先级
#define FONTUPDATA_TASK_PRIO		6
//任务堆栈大小
#define FONTUPDATA_STK_SIZE			128
//任务控制块
OS_TCB FontupdataTaskTCB;
//任务堆栈
CPU_STK FONTUPDATA_TASK_STK[FONTUPDATA_STK_SIZE];
//字库更新任务
void fontupdata_task(void *p_arg);

//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO			7
//任务堆栈大小
#define EMWINDEMO_STK_SIZE			1024
//任务控制块
OS_TCB EmwindemoTaskTCB;
//任务堆栈
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task任务
void emwindemo_task(void *p_arg);

int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();	    	//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			    //LED端口初始化
	TFTLCD_Init();			//LCD初始化	
	KEY_Init();	 			//按键初始化
	BEEP_Init();			//初始化蜂鸣器
	MAX6675_Init();
	FSMC_SRAM_Init();		//初始化SRAM
	my_mem_init(SRAMIN); 	//初始化内部内存池
	my_mem_init(SRAMEX);  	//初始化外部内存池
	memset(&main_param_t,0,sizeof(MAIN_PARAM));
	main_param_t.prestate = 1;
	main_param_t.settemp = 20;
	
	TIM3_Int_Init(999,7199);//1000*7200 / 72 000 000 = 1/10 = 100ms
	
	pid_Init();//普通pid
	smith_pid_Init();//smith_pid
	
	TIM5_PWM_Init(39,719);//40*720 / 72 000 000 = 1/100=100hz
	TIM_SetCompare2(TIM5,40);//关闭
	
	USART3_init(115200);
	
	exfuns_init();			//为fatfs文件系统分配内存
	f_mount(fs[0],"0:",1);	//挂载SD卡
	f_mount(fs[1],"1:",1);	//挂载FLASH
	while(font_init())		//初始化字库
	{
		LCD_ShowString(30,70,200,16,16,"Font Error!");
		while(SD_Init())	//检测SD卡
		{
			LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,90,200+30,70+16,WHITE);
			delay_ms(200);		    
		}
		update_font(30,90,16,"0:");	//如果字库不存在就更新字库
		delay_ms(2000);
		LCD_Clear(WHITE);	//清屏
		break;
	}
	TP_Init();				//触摸屏初始化
	
	OSInit(&err);			//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//开启CRC时钟
	GUI_Init();  			//STemWin初始化
	
	OS_CRITICAL_ENTER();	//进入临界区
	//STemWin Demo任务	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//触摸屏任务
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//LED0任务
	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"Led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//字库更新任务
	OSTaskCreate((OS_TCB*     )&FontupdataTaskTCB,		
				 (CPU_CHAR*   )"Fontupdata task", 		
                 (OS_TASK_PTR )fontupdata_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )FONTUPDATA_TASK_PRIO,     
                 (CPU_STK*    )&FONTUPDATA_TASK_STK[0],	
                 (CPU_STK_SIZE)FONTUPDATA_STK_SIZE/10,	
                 (CPU_STK_SIZE)FONTUPDATA_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}

//EMWINDEMO任务
void emwindemo_task(void *p_arg)
{
	uint16_t tempdot = 0;
	static uint16_t balancenum = 0;
	char Esp_buf[255];
	char Esp8266_buf[64];
	u16 Esp8266len=0;
	u16 Esp8266num=0;
	
	GUI_CURSOR_Show();
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();	
	
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt("无线模块初始化中...",50,140);	
	esp8266_init();//无线init
	
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_FontHZ24);
	GUI_DispStringAt("PID算法温度控制系统",8,10);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt("目标温度:00.00℃",50,60);
	GUI_DispStringAt("实时温度:00.00℃",50,100);
	GUI_DispStringAt("实时时间:000s",50,140);
	GUI_DispStringAt("当前功能:普通pid",50,180);  
	GUI_DispStringAt("当前状态:待机中",50,220);

  //上电获取环境温度为开始设置温度	
	main_param_t.firstsettemp = (uint16_t)(MAX6675_Read_data()*0.25);
	main_param_t.settemp = main_param_t.firstsettemp;//以环境温度为起始温度
	
	GUI_DispDecAt(main_param_t.firstsettemp%100,122,60,2);	//显示目标温度
	GUI_DispCharAt('.',138,60);
	GUI_DispDecAt(00,146,60,2);	//显示目标温度小数点
	
  GUI_DispDecAt(00,122,100,2);	//显示实时温度
	GUI_DispCharAt('.',138,100);
	GUI_DispDecAt(00,146,100,2);	//显示实时温度小数点
	
	GUI_DispDecAt(000,122,140,3);	//显示实时时间
	
	GUI_SetFont(&GUI_FontHZ16);
//	GUI_DispStringAt("待机中...",90,130);	
	
	//更换皮肤
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
	
	while(1)
	{
		if(time_param_t.timestatus == false)//未达平衡
		{
			GUI_SetColor(GUI_WHITE);
			GUI_DispDecAt(time_param_t.tts,122,140,3);//显示计时
		}
		
		tempdot = main_param_t.temp * 100;
		printf("采集温度是：%04.2f,当前温度是：%d,小数点温度：%d\r\n",main_param_t.temp,((uint16_t)main_param_t.temp)%100,tempdot%100);
		
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt((((uint16_t)main_param_t.temp))%100,122,100,2);	//显示实时温度
		GUI_DispCharAt('.',138,100);
		GUI_DispDecAt(tempdot % 100,146,100,2);	//显示实时温度小数点		
		
		//处理pid事件
		//普通pid
		if(main_param_t.prestate == 1 && main_param_t.startstate == true)
		{
			pid1.ActualSpeed = main_param_t.temp;
			main_param_t.pidtemp = pid_realize(main_param_t.settemp);
			printf("普通pid is starting= %f\r\n",main_param_t.pidtemp);
			
			if(fabs(main_param_t.temp - main_param_t.settemp) < 0.3 && time_param_t.timestatus == false)
			{
					
				  balancenum++;
				  //已达到平衡
					if(balancenum >= 300)
					{
						 balancenum = 0;
						 time_param_t.timestatus = true;
						 GUI_SetColor(GUI_GREEN);
						 TIM_Cmd(TIM3,DISABLE);//停止计时		
						 GUI_DispDecAt(time_param_t.tts,122,140,3);//显示计时
						 BEEP_ok();
					}
			}
			
			TIM_SetCompare2(TIM5,40 - main_param_t.pidtemp);//设置
		}
		//模糊pid
		else if(main_param_t.prestate == 2 && main_param_t.startstate == true)
		{
			TtF = main_param_t.temp;//实时温度
			SsF = main_param_t.settemp;//设定温度
			
			main_param_t.pidtemp = speed_pid(SsF,TtF);
			printf("模糊pid is starting= %f\r\n",main_param_t.pidtemp);
			
			if(fabs(main_param_t.temp - main_param_t.settemp) < 0.3 && time_param_t.timestatus == false)
			{
					
				  balancenum++;
				  //已达到平衡
					if(balancenum >= 10)
					{
						 time_param_t.timestatus = true;
						 GUI_SetColor(GUI_GREEN);
						 TIM_Cmd(TIM3,DISABLE);//停止计时		
						 GUI_DispDecAt(time_param_t.tts,122,140,3);//显示计时
						 BEEP_ok();
					}
			}			
			
			if(main_param_t.pidtemp > 40)main_param_t.pidtemp = 40;
			else if(main_param_t.pidtemp < 0)main_param_t.pidtemp = 0;
			
			printf("调整后的模糊pid is starting= %f\r\n",main_param_t.pidtemp);
			
			TIM_SetCompare2(TIM5,40 - main_param_t.pidtemp);//设置
		}
		//smith-pid
		else if(main_param_t.prestate == 3 && main_param_t.startstate == true)
		{
			
			main_param_t.pidtemp = smith_pid_out(main_param_t.settemp,main_param_t.temp);
			printf("smith-pid is starting= %f\r\n",main_param_t.pidtemp);
			
			if(fabs(main_param_t.temp - main_param_t.settemp) < 0.3 && time_param_t.timestatus == false)
			{
					
				  balancenum++;
				  //已达到平衡
					if(balancenum >= 10)
					{
						 time_param_t.timestatus = true;
						 GUI_SetColor(GUI_GREEN);
						 TIM_Cmd(TIM3,DISABLE);//停止计时		
						 GUI_DispDecAt(time_param_t.tts,122,140,3);//显示计时
						 BEEP_ok();
					}
			}			
			
			TIM_SetCompare2(TIM5,40 - main_param_t.pidtemp);//设置			
		}
		
		if(main_param_t.startstate == true)//传输上位机查看
		{
			 Esp8266num++;//传输一次
			
			 // 获取数据
			 main_param_t.esp_settemp = main_param_t.settemp;
			 main_param_t.esp_currenttemp = main_param_t.temp;
			 main_param_t.esp_difftemp = main_param_t.settemp - main_param_t.temp;
			 main_param_t.adjusttime = time_param_t.tts;
			
			 if(Esp8266num >= 3)//1s
			 {
				 Esp8266num = 0;
			   //传输
				 sprintf(Esp_buf,"##CM=3035;TY=%d;ST=%05.2f;CT=%05.2f;DT=%05.2f;TT=%03d;BT=%05.2f;CL=N\r\n",main_param_t.prestate,main_param_t.esp_settemp,main_param_t.esp_currenttemp,
						main_param_t.esp_difftemp,main_param_t.adjusttime,main_param_t.pidtemp);
				 Esp8266len=strlen(Esp_buf);
				 sprintf(Esp8266_buf,"AT+CIPSEND=0,%d\r\n",Esp8266len);//传输数据长度
				 esp8266_set(Esp8266_buf);
				 delay_ms(5);
				 
				 esp8266_set(Esp_buf);//开始传输数据	
			 }
		}
		
		keyvlaue = KEY_Scan(0);
		if(keyvlaue)
		{
			printf("keyvlaue is =%d \r\n",keyvlaue);
			switch(keyvlaue)
			{
				case KEY0_PRES: 
					if(main_param_t.startstate == false)
					main_param_t.prestate++; 
					if(main_param_t.prestate>3)
						main_param_t.prestate = 1;
					
					time_param_t.timestatus = false;//复位定时器
					
					if(main_param_t.prestate == 1)
					{
						GUI_SetColor(GUI_WHITE);
						GUI_DispStringAt("普通pid   ",122,180);
						
						time_param_t.tts = 0;
						GUI_DispDecAt(time_param_t.tts,122,140,3);
					}
					else if(main_param_t.prestate == 2)
					{
						GUI_SetColor(GUI_GREEN);
						GUI_DispStringAt("模糊pid",122,180);
						
						GUI_SetColor(GUI_WHITE);
						time_param_t.tts = 0;
						GUI_DispDecAt(time_param_t.tts,122,140,3);
					}
					else
					{
						GUI_SetColor(GUI_RED);
						GUI_DispStringAt("smith-pid",122,180);
						
						GUI_SetColor(GUI_WHITE);
						time_param_t.tts = 0;
						GUI_DispDecAt(time_param_t.tts,122,140,3);
					}
					
					keyvlaue = 0; 
					break;
				case KEY1_PRES: 
					
				  if(main_param_t.startstate == false)
					{
						time_param_t.tts = 0;
						GUI_DispDecAt(time_param_t.tts,122,140,3);
						time_param_t.timestatus = false;//复位定时器
						
						main_param_t.settemp += 2;
						if(main_param_t.settemp>60)main_param_t.settemp=60;
						GUI_DispDecAt(main_param_t.settemp,122,60,2);
					}
				
					keyvlaue = 0; 
					break;
				case KEY2_PRES:
					
				  if(main_param_t.startstate == false)
					{
						time_param_t.tts = 0;
						GUI_DispDecAt(time_param_t.tts,122,140,3);
						time_param_t.timestatus = false;//复位定时器
						
						main_param_t.settemp -= 2;
						if(main_param_t.settemp<20)main_param_t.settemp=20;
						GUI_DispDecAt(main_param_t.settemp,122,60,2);
					}
				
					keyvlaue = 0; 
					break;
				case KEY3_PRES: 
					
					main_param_t.start_stop++;
					if(main_param_t.start_stop%2==0)
					{
						printf("pid is stop\r\n");
						GUI_SetColor(GUI_WHITE);
						main_param_t.startstate = false;
						GUI_DispStringAt("待机中",122,220);		
            TIM_SetCompare2(TIM5,40);//关闭	
						TIM_Cmd(TIM3,DISABLE);//停止计时		
						time_param_t.timestatus = false;//复位定时器						
					}
					else
					{
						if(main_param_t.temp > main_param_t.settemp)//不符合
						{
							printf("实际温度大于设置温度，不需要调节\r\n");
							break;
						}
						printf("pid is start\r\n");
						GUI_SetColor(GUI_ORANGE);
						main_param_t.startstate = true;
						GUI_DispStringAt("运行中",122,220);			
						TIM_Cmd(TIM3,ENABLE);//开始计时
						
					 //清除图表
					 sprintf(Esp_buf,"##CM=3035;TY=%d;ST=%05.2f;CT=%05.2f;DT=%05.2f;TT=%03d;BT=%05.2f;CL=Y\r\n",main_param_t.prestate,main_param_t.esp_settemp,main_param_t.esp_currenttemp,
							main_param_t.esp_difftemp,main_param_t.adjusttime,main_param_t.pidtemp);
					 Esp8266len=strlen(Esp_buf);
					 sprintf(Esp8266_buf,"AT+CIPSEND=0,%d\r\n",Esp8266len);//传输数据长度
					 esp8266_set(Esp8266_buf);
					 delay_ms(5);
					 
					 esp8266_set(Esp_buf);//开始传输数据			
						
					}
					

					keyvlaue = 0; 
					break;
			}
		}
		
		GUI_Delay(300);
	}
}

//字库更新任务
//KEY_UP键长按2s更新字库
void fontupdata_task(void *pdata)
{
	OS_ERR err;
	while(1)
	{
		if(WK_UP == 1)				//KEY_UP键按下
		{
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);//延时2s
			if(WK_UP == 1)			//还是KEY_UP键
			{
				LCD_Clear(WHITE);
				OSSchedLock(&err);		//调度器上锁
				LCD_ShowString(10,50,250,30,16,"Font Updataing,Please Wait...");
				update_font(10,70,16,"0:");//更新字库
				LCD_Clear(WHITE);
				POINT_COLOR = RED;
				LCD_ShowString(10,50,280,30,16,"Font Updata finshed,Please Restart!");
				OSSchedUnlock(&err);	//调度器解锁
			}
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时10ms
	}
}

//TOUCH任务
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
	}
}

//LED0任务
void led0_task(void *p_arg)
{
	OS_ERR err;
	float max6675temp = 0;

	while(1)
	{
		max6675temp = (MAX6675_Read_data())*0.25;
		MAX6675_Read_data_filter(&max6675_result_t,max6675temp);
		main_param_t.temp = max6675_result_t.u16AdcResultAve;
		
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err);//延时200ms
	}
}
