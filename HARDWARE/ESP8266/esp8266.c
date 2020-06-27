#include "usart2.h"
#include "usart.h"
#include "esp8266.h"	
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "stdio.h"
#include "string.h"

void USART1_send(u8 value)//����һ���ַ�
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
	  esp8266_set("AT+CWSAP=\"С����\",\"xiaofeishu\",11,3\r\n");
	 	delay_ms(1000);
		esp8266_set("AT+CIPMUX=1\r\n");
	 	delay_ms(1000);
		esp8266_set("AT+CIPSERVER=1,5865\r\n");
		delay_ms(1000);
}

u8 Re_buf[255];
unsigned int counter=0;
char com;//ָ�
u16 Esplen=0;
unsigned char sign=0;

void esp8266_Receive(u8 Res)
{
	  char Esp_buf[255];
	  char Esp8266_buf[64];
	
    Re_buf[counter]=Res;
    if(counter==0&&Re_buf[0]!='#') return;      // ���֡ͷ         
    counter++;

    if(Re_buf[counter-4]=='0'&&Re_buf[counter-3]=='D'&&Re_buf[counter-2]=='0'&&Re_buf[counter-1]=='A')//��ǰ�Ѿ��������ˣ���+1�ˣ��������һ����Ҫ��1
    {
      com=Re_buf[8];
			
     if(com=='5')
     {
			 strcpy(Esp_buf,"asong test");
			 Esplen=strlen(Esp_buf);

       printf("##CM=3035;Re=OK;\r\n");
			 
			 sprintf(Esp8266_buf,"AT+CIPSEND=0,%d\r\n",Esplen);//�������ݳ���
			 esp8266_set(Esp8266_buf);
			 delay_ms(5);
			 
			 esp8266_set(Esp_buf);//��ʼ��������
     }
             
      counter=0;                 //���¸�ֵ��׼����һ֡���ݵĽ���
      memset(Re_buf,0,sizeof(Re_buf));
      sign=1; 
    }      	
}

