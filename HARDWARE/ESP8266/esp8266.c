#include "usart2.h"
#include "usart.h"
#include "esp8266.h"	
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "stdio.h"
#include "string.h"

void USART1_send(u8 value)//发送一个字符
{
   USART_SendData(USART1,value);
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

void usart_send(char value)
{
  USART3_send(value);
}

void esp8266_set(char *puf)
{
   	while(*puf!='\0')    
	{
		usart_send(*puf); 
		puf++;		
	}
}

void esp8266_init(void)
{	
	  esp8266_set("AT+RST\r\n");
	 	delay_ms(2000);		
	  esp8266_set("AT+CWMODE=2\r\n");
	 	delay_ms(1000);	
	  esp8266_set("AT+CIPAP=\"192.168.10.5\"\r\n");
	 	delay_ms(2000);	
	  esp8266_set("AT+CWSAP=\"小飞鼠\",\"xiaofeishu\",11,3\r\n");
	 	delay_ms(1000);
		esp8266_set("AT+CIPMUX=1\r\n");
	 	delay_ms(1000);
		esp8266_set("AT+CIPSERVER=1,5865\r\n");
		delay_ms(1000);
}

u8 Re_buf[255];
unsigned int counter=0;
char com;//指令集
u16 Esplen=0;
unsigned char sign=0;

void esp8266_Receive(u8 Res)
{
	  char Esp_buf[255];
	  char Esp8266_buf[64];
	
    Re_buf[counter]=Res;
    if(counter==0&&Re_buf[0]!='#') return;      // 检查帧头         
    counter++;

    if(Re_buf[counter-4]=='0'&&Re_buf[counter-3]=='D'&&Re_buf[counter-2]=='0'&&Re_buf[counter-1]=='A')//当前已经接收完了，还+1了，所以最后一个需要减1
    {
      com=Re_buf[8];
			
     if(com=='5')
     {
			 strcpy(Esp_buf,"asong test");
			 Esplen=strlen(Esp_buf);

       printf("##CM=3035;Re=OK;\r\n");
			 
			 sprintf(Esp8266_buf,"AT+CIPSEND=0,%d\r\n",Esplen);//传输数据长度
			 esp8266_set(Esp8266_buf);
			 delay_ms(5);
			 
			 esp8266_set(Esp_buf);//开始传输数据
     }
             
      counter=0;                 //重新赋值，准备下一帧数据的接收
      memset(Re_buf,0,sizeof(Re_buf));
      sign=1; 
    }      	
}

