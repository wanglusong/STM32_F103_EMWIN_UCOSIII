//初始化IO 串口2 
#include "delay.h"
#include "USART2.h"
#include "stdarg.h"
#include "stdio.h"
#include "led.h"
#include "string.h"
#include "esp8266.h"
#include "sys.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os 使用	  
#endif

//串口发送缓存区 
//__align(8) 
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
#ifdef USART3_RX_EN  //如果使能了接收 
//串口接收缓存区 
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //接收缓冲,最大USART3_MAX_RECV_LEN个字节.

u16 USART3_RX_STA = 0;
void USART3_IRQHandler(void)
{
	u8 res;

#ifdef SYSTEM_SUPPORT_OS	 	
	OSIntEnter();    
#endif	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
	  res =USART_ReceiveData(USART3);//(USART1->DR); //读取接收到的数据
//		esp8266_Receive(res);

  }

#ifdef SYSTEM_SUPPORT_OS	 
	OSIntExit();  											 
#endif
} 
#endif 



//bound:波特率
void USART3_init(u32 bound)
{ 
	NVIC_InitTypeDef NVIC_INITSTRUCTURE;
	GPIO_InitTypeDef GPIO_INITSTRUCTURE;
	USART_InitTypeDef USART_INITSTRUCTURE;

	//1.串口时钟和和GPIO时钟使能。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟

	//USART1_TX   GPIOB.10
  GPIO_INITSTRUCTURE.GPIO_Pin = GPIO_Pin_10; //PA.2
  GPIO_INITSTRUCTURE.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_INITSTRUCTURE.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_INITSTRUCTURE);//初始化GPIOA.2
   
  //USART1_RX	  GPIOB.11初始化
  GPIO_INITSTRUCTURE.GPIO_Pin = GPIO_Pin_11;//PA3
  GPIO_INITSTRUCTURE.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_INITSTRUCTURE);//初始化GPIOA.3
	
	//4.串口参数初始化：设置波特率，字长，奇偶校验等参数
	USART_INITSTRUCTURE.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_INITSTRUCTURE.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_INITSTRUCTURE.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_INITSTRUCTURE.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_INITSTRUCTURE.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_INITSTRUCTURE.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
	USART_Init(USART3, &USART_INITSTRUCTURE); //初始化串口2参数
	
	//7.使能串口 
	USART_Cmd(USART3, ENABLE); //使能串口
	
	//6.开启中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断 
	
	//5.初始化NVIC
	NVIC_INITSTRUCTURE.NVIC_IRQChannel = USART3_IRQn;
	NVIC_INITSTRUCTURE.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级0
	NVIC_INITSTRUCTURE.NVIC_IRQChannelSubPriority = 4; //子优先级3
	NVIC_INITSTRUCTURE.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
	NVIC_Init(&NVIC_INITSTRUCTURE); //根据指定的参数初始化VIC寄存器

}


void USART3_send(u8 value)//发送一个字符
{
   USART_SendData(USART3,value);
	 while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
}
