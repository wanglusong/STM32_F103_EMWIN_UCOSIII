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
	
	float esp_settemp;//�趨�¶�
	float esp_currenttemp;//ʵʱ�¶�
	float esp_difftemp;//��ֵ�¶�
	int adjusttime;//����ʱ��
	
}MAIN_PARAM;

MAIN_PARAM main_param_t;

//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				4
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO 				5
//�����ջ��С
#define LED0_STK_SIZE				128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0����
void led0_task(void *p_arg);

//�ֿ��������
//�����������ȼ�
#define FONTUPDATA_TASK_PRIO		6
//�����ջ��С
#define FONTUPDATA_STK_SIZE			128
//������ƿ�
OS_TCB FontupdataTaskTCB;
//�����ջ
CPU_STK FONTUPDATA_TASK_STK[FONTUPDATA_STK_SIZE];
//�ֿ��������
void fontupdata_task(void *p_arg);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO			7
//�����ջ��С
#define EMWINDEMO_STK_SIZE			1024
//������ƿ�
OS_TCB EmwindemoTaskTCB;
//�����ջ
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task����
void emwindemo_task(void *p_arg);

int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();	    	//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			    //LED�˿ڳ�ʼ��
	TFTLCD_Init();			//LCD��ʼ��	
	KEY_Init();	 			//������ʼ��
	BEEP_Init();			//��ʼ��������
	MAX6675_Init();
	FSMC_SRAM_Init();		//��ʼ��SRAM
	my_mem_init(SRAMIN); 	//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);  	//��ʼ���ⲿ�ڴ��
	memset(&main_param_t,0,sizeof(MAIN_PARAM));
	main_param_t.prestate = 1;
	main_param_t.settemp = 20;
	
	TIM3_Int_Init(999,7199);//1000*7200 / 72 000 000 = 1/10 = 100ms
	
	pid_Init();//��ͨpid
	smith_pid_Init();//smith_pid
	
	TIM5_PWM_Init(39,719);//40*720 / 72 000 000 = 1/100=100hz
	TIM_SetCompare2(TIM5,40);//�ر�
	
	USART3_init(115200);
	
	exfuns_init();			//Ϊfatfs�ļ�ϵͳ�����ڴ�
	f_mount(fs[0],"0:",1);	//����SD��
	f_mount(fs[1],"1:",1);	//����FLASH
	while(font_init())		//��ʼ���ֿ�
	{
		LCD_ShowString(30,70,200,16,16,"Font Error!");
		while(SD_Init())	//���SD��
		{
			LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,90,200+30,70+16,WHITE);
			delay_ms(200);		    
		}
		update_font(30,90,16,"0:");	//����ֿⲻ���ھ͸����ֿ�
		delay_ms(2000);
		LCD_Clear(WHITE);	//����
		break;
	}
	TP_Init();				//��������ʼ��
	
	OSInit(&err);			//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//����CRCʱ��
	GUI_Init();  			//STemWin��ʼ��
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//STemWin Demo����	
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
	//����������
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
	//LED0����
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
	//�ֿ��������
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
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

//EMWINDEMO����
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
	GUI_DispStringAt("����ģ���ʼ����...",50,140);	
	esp8266_init();//����init
	
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_FontHZ24);
	GUI_DispStringAt("PID�㷨�¶ȿ���ϵͳ",8,10);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt("Ŀ���¶�:00.00��",50,60);
	GUI_DispStringAt("ʵʱ�¶�:00.00��",50,100);
	GUI_DispStringAt("ʵʱʱ��:000s",50,140);
	GUI_DispStringAt("��ǰ����:��ͨpid",50,180);  
	GUI_DispStringAt("��ǰ״̬:������",50,220);

  //�ϵ��ȡ�����¶�Ϊ��ʼ�����¶�	
	main_param_t.firstsettemp = (uint16_t)(MAX6675_Read_data()*0.25);
	main_param_t.settemp = main_param_t.firstsettemp;//�Ի����¶�Ϊ��ʼ�¶�
	
	GUI_DispDecAt(main_param_t.firstsettemp%100,122,60,2);	//��ʾĿ���¶�
	GUI_DispCharAt('.',138,60);
	GUI_DispDecAt(00,146,60,2);	//��ʾĿ���¶�С����
	
  GUI_DispDecAt(00,122,100,2);	//��ʾʵʱ�¶�
	GUI_DispCharAt('.',138,100);
	GUI_DispDecAt(00,146,100,2);	//��ʾʵʱ�¶�С����
	
	GUI_DispDecAt(000,122,140,3);	//��ʾʵʱʱ��
	
	GUI_SetFont(&GUI_FontHZ16);
//	GUI_DispStringAt("������...",90,130);	
	
	//����Ƥ��
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
	
	while(1)
	{
		if(time_param_t.timestatus == false)//δ��ƽ��
		{
			GUI_SetColor(GUI_WHITE);
			GUI_DispDecAt(time_param_t.tts,122,140,3);//��ʾ��ʱ
		}
		
		tempdot = main_param_t.temp * 100;
		printf("�ɼ��¶��ǣ�%04.2f,��ǰ�¶��ǣ�%d,С�����¶ȣ�%d\r\n",main_param_t.temp,((uint16_t)main_param_t.temp)%100,tempdot%100);
		
		GUI_SetColor(GUI_WHITE);
		GUI_DispDecAt((((uint16_t)main_param_t.temp))%100,122,100,2);	//��ʾʵʱ�¶�
		GUI_DispCharAt('.',138,100);
		GUI_DispDecAt(tempdot % 100,146,100,2);	//��ʾʵʱ�¶�С����		
		
		//����pid�¼�
		//��ͨpid
		if(main_param_t.prestate == 1 && main_param_t.startstate == true)
		{
			pid1.ActualSpeed = main_param_t.temp;
			main_param_t.pidtemp = pid_realize(main_param_t.settemp);
			printf("��ͨpid is starting= %f\r\n",main_param_t.pidtemp);
			
			if(fabs(main_param_t.temp - main_param_t.settemp) < 0.3 && time_param_t.timestatus == false)
			{
					
				  balancenum++;
				  //�Ѵﵽƽ��
					if(balancenum >= 300)
					{
						 balancenum = 0;
						 time_param_t.timestatus = true;
						 GUI_SetColor(GUI_GREEN);
						 TIM_Cmd(TIM3,DISABLE);//ֹͣ��ʱ		
						 GUI_DispDecAt(time_param_t.tts,122,140,3);//��ʾ��ʱ
						 BEEP_ok();
					}
			}
			
			TIM_SetCompare2(TIM5,40 - main_param_t.pidtemp);//����
		}
		//ģ��pid
		else if(main_param_t.prestate == 2 && main_param_t.startstate == true)
		{
			TtF = main_param_t.temp;//ʵʱ�¶�
			SsF = main_param_t.settemp;//�趨�¶�
			
			main_param_t.pidtemp = speed_pid(SsF,TtF);
			printf("ģ��pid is starting= %f\r\n",main_param_t.pidtemp);
			
			if(fabs(main_param_t.temp - main_param_t.settemp) < 0.3 && time_param_t.timestatus == false)
			{
					
				  balancenum++;
				  //�Ѵﵽƽ��
					if(balancenum >= 10)
					{
						 time_param_t.timestatus = true;
						 GUI_SetColor(GUI_GREEN);
						 TIM_Cmd(TIM3,DISABLE);//ֹͣ��ʱ		
						 GUI_DispDecAt(time_param_t.tts,122,140,3);//��ʾ��ʱ
						 BEEP_ok();
					}
			}			
			
			if(main_param_t.pidtemp > 40)main_param_t.pidtemp = 40;
			else if(main_param_t.pidtemp < 0)main_param_t.pidtemp = 0;
			
			printf("�������ģ��pid is starting= %f\r\n",main_param_t.pidtemp);
			
			TIM_SetCompare2(TIM5,40 - main_param_t.pidtemp);//����
		}
		//smith-pid
		else if(main_param_t.prestate == 3 && main_param_t.startstate == true)
		{
			
			main_param_t.pidtemp = smith_pid_out(main_param_t.settemp,main_param_t.temp);
			printf("smith-pid is starting= %f\r\n",main_param_t.pidtemp);
			
			if(fabs(main_param_t.temp - main_param_t.settemp) < 0.3 && time_param_t.timestatus == false)
			{
					
				  balancenum++;
				  //�Ѵﵽƽ��
					if(balancenum >= 10)
					{
						 time_param_t.timestatus = true;
						 GUI_SetColor(GUI_GREEN);
						 TIM_Cmd(TIM3,DISABLE);//ֹͣ��ʱ		
						 GUI_DispDecAt(time_param_t.tts,122,140,3);//��ʾ��ʱ
						 BEEP_ok();
					}
			}			
			
			TIM_SetCompare2(TIM5,40 - main_param_t.pidtemp);//����			
		}
		
		if(main_param_t.startstate == true)//������λ���鿴
		{
			 Esp8266num++;//����һ��
			
			 // ��ȡ����
			 main_param_t.esp_settemp = main_param_t.settemp;
			 main_param_t.esp_currenttemp = main_param_t.temp;
			 main_param_t.esp_difftemp = main_param_t.settemp - main_param_t.temp;
			 main_param_t.adjusttime = time_param_t.tts;
			
			 if(Esp8266num >= 3)//1s
			 {
				 Esp8266num = 0;
			   //����
				 sprintf(Esp_buf,"##CM=3035;TY=%d;ST=%05.2f;CT=%05.2f;DT=%05.2f;TT=%03d;BT=%05.2f;CL=N\r\n",main_param_t.prestate,main_param_t.esp_settemp,main_param_t.esp_currenttemp,
						main_param_t.esp_difftemp,main_param_t.adjusttime,main_param_t.pidtemp);
				 Esp8266len=strlen(Esp_buf);
				 sprintf(Esp8266_buf,"AT+CIPSEND=0,%d\r\n",Esp8266len);//�������ݳ���
				 esp8266_set(Esp8266_buf);
				 delay_ms(5);
				 
				 esp8266_set(Esp_buf);//��ʼ��������	
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
					
					time_param_t.timestatus = false;//��λ��ʱ��
					
					if(main_param_t.prestate == 1)
					{
						GUI_SetColor(GUI_WHITE);
						GUI_DispStringAt("��ͨpid   ",122,180);
						
						time_param_t.tts = 0;
						GUI_DispDecAt(time_param_t.tts,122,140,3);
					}
					else if(main_param_t.prestate == 2)
					{
						GUI_SetColor(GUI_GREEN);
						GUI_DispStringAt("ģ��pid",122,180);
						
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
						time_param_t.timestatus = false;//��λ��ʱ��
						
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
						time_param_t.timestatus = false;//��λ��ʱ��
						
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
						GUI_DispStringAt("������",122,220);		
            TIM_SetCompare2(TIM5,40);//�ر�	
						TIM_Cmd(TIM3,DISABLE);//ֹͣ��ʱ		
						time_param_t.timestatus = false;//��λ��ʱ��						
					}
					else
					{
						if(main_param_t.temp > main_param_t.settemp)//������
						{
							printf("ʵ���¶ȴ��������¶ȣ�����Ҫ����\r\n");
							break;
						}
						printf("pid is start\r\n");
						GUI_SetColor(GUI_ORANGE);
						main_param_t.startstate = true;
						GUI_DispStringAt("������",122,220);			
						TIM_Cmd(TIM3,ENABLE);//��ʼ��ʱ
						
					 //���ͼ��
					 sprintf(Esp_buf,"##CM=3035;TY=%d;ST=%05.2f;CT=%05.2f;DT=%05.2f;TT=%03d;BT=%05.2f;CL=Y\r\n",main_param_t.prestate,main_param_t.esp_settemp,main_param_t.esp_currenttemp,
							main_param_t.esp_difftemp,main_param_t.adjusttime,main_param_t.pidtemp);
					 Esp8266len=strlen(Esp_buf);
					 sprintf(Esp8266_buf,"AT+CIPSEND=0,%d\r\n",Esp8266len);//�������ݳ���
					 esp8266_set(Esp8266_buf);
					 delay_ms(5);
					 
					 esp8266_set(Esp_buf);//��ʼ��������			
						
					}
					

					keyvlaue = 0; 
					break;
			}
		}
		
		GUI_Delay(300);
	}
}

//�ֿ��������
//KEY_UP������2s�����ֿ�
void fontupdata_task(void *pdata)
{
	OS_ERR err;
	while(1)
	{
		if(WK_UP == 1)				//KEY_UP������
		{
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);//��ʱ2s
			if(WK_UP == 1)			//����KEY_UP��
			{
				LCD_Clear(WHITE);
				OSSchedLock(&err);		//����������
				LCD_ShowString(10,50,250,30,16,"Font Updataing,Please Wait...");
				update_font(10,70,16,"0:");//�����ֿ�
				LCD_Clear(WHITE);
				POINT_COLOR = RED;
				LCD_ShowString(10,50,280,30,16,"Font Updata finshed,Please Restart!");
				OSSchedUnlock(&err);	//����������
			}
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ10ms
	}
}

//TOUCH����
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
	}
}

//LED0����
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
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err);//��ʱ200ms
	}
}
