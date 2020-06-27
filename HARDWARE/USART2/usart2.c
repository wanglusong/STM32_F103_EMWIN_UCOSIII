//��ʼ��IO ����2 
#include "delay.h"
#include "USART2.h"
#include "stdarg.h"
#include "stdio.h"
#include "led.h"
#include "string.h"
#include "esp8266.h"
#include "sys.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os ʹ��	  
#endif

//���ڷ��ͻ����� 
//__align(8) 
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
#ifdef USART3_RX_EN  //���ʹ���˽��� 
//���ڽ��ջ����� 
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

u16 USART3_RX_STA = 0;
void USART3_IRQHandler(void)
{
	u8 res;

#ifdef SYSTEM_SUPPORT_OS	 	
	OSIntEnter();    
#endif	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
	  res =USART_ReceiveData(USART3);//(USART1->DR); //��ȡ���յ�������
//		esp8266_Receive(res);

  }

#ifdef SYSTEM_SUPPORT_OS	 
	OSIntExit();  											 
#endif
} 
#endif 



//bound:������
void USART3_init(u32 bound)
{ 
	NVIC_InitTypeDef NVIC_INITSTRUCTURE;
	GPIO_InitTypeDef GPIO_INITSTRUCTURE;
	USART_InitTypeDef USART_INITSTRUCTURE;

	//1.����ʱ�Ӻͺ�GPIOʱ��ʹ�ܡ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��

	//USART1_TX   GPIOB.10
  GPIO_INITSTRUCTURE.GPIO_Pin = GPIO_Pin_10; //PA.2
  GPIO_INITSTRUCTURE.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_INITSTRUCTURE.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_INITSTRUCTURE);//��ʼ��GPIOA.2
   
  //USART1_RX	  GPIOB.11��ʼ��
  GPIO_INITSTRUCTURE.GPIO_Pin = GPIO_Pin_11;//PA3
  GPIO_INITSTRUCTURE.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_INITSTRUCTURE);//��ʼ��GPIOA.3
	
	//4.���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ���
	USART_INITSTRUCTURE.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_INITSTRUCTURE.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_INITSTRUCTURE.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_INITSTRUCTURE.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_INITSTRUCTURE.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_INITSTRUCTURE.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
	USART_Init(USART3, &USART_INITSTRUCTURE); //��ʼ������2����
	
	//7.ʹ�ܴ��� 
	USART_Cmd(USART3, ENABLE); //ʹ�ܴ���
	
	//6.�����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж� 
	
	//5.��ʼ��NVIC
	NVIC_INITSTRUCTURE.NVIC_IRQChannel = USART3_IRQn;
	NVIC_INITSTRUCTURE.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�0
	NVIC_INITSTRUCTURE.NVIC_IRQChannelSubPriority = 4; //�����ȼ�3
	NVIC_INITSTRUCTURE.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
	NVIC_Init(&NVIC_INITSTRUCTURE); //����ָ���Ĳ�����ʼ��VIC�Ĵ���

}


void USART3_send(u8 value)//����һ���ַ�
{
   USART_SendData(USART3,value);
	 while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
}
