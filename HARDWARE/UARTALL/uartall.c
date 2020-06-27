#include "uartall.h"
#include "esp8266.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif 


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA=0;       //接收状态标记	 
u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART1_Handler; //UART句柄

u8 USART2_RX_BUF[USART_REC_LEN];    
u16 USART2_RX_STA=0;       //接收状态标记	  
u8 aRx2Buffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART2_Handler; //UART句柄

u8 USART3_RX_BUF[USART_REC_LEN];    
u16 USART3_RX_STA=0;       //接收状态标记	  
u8 aRx3Buffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART3_Handler; //UART句柄

u8 UART4_RX_BUF[USART_REC_LEN];    
u16 UART4_RX_STA=0;       //接收状态标记	  
u8 aRx4Buffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART4_Handler; //UART句柄

u8 UART5_RX_BUF[USART_REC_LEN];    
u16 UART5_RX_STA=0;       //接收状态标记	  
u8 aRx5Buffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART5_Handler; //UART句柄

//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}
 
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u32 timeout=0;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>HAL_MAX_DELAY) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>HAL_MAX_DELAY) break;	
	}
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
} 
  
//初始化IO 串口2 
//bound:波特率
void uart2_init(u32 bound)
{	
	//UART 初始化设置
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRx2Buffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//串口2中断服务程序
void USART2_IRQHandler(void)                	
{ 
	u32 timeout=0;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART2_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>HAL_MAX_DELAY) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRx2Buffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>HAL_MAX_DELAY) break;	
	}
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
} 

//初始化IO 串口1 
//bound:波特率
void uart3_init(u32 bound)
{	
	//UART 初始化设置
	UART3_Handler.Instance=USART3;					    //USART3
	UART3_Handler.Init.BaudRate=bound;				    //波特率
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRx3Buffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//串口3中断服务程序
void USART3_IRQHandler(void)                	
{ 
	u32 timeout=0;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART3_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART3_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>HAL_MAX_DELAY) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRx3Buffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>HAL_MAX_DELAY) break;	
	}
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
}

//初始化IO 串口4 
//bound:波特率
void uart4_init(u32 bound)
{	
	//UART 初始化设置
	UART4_Handler.Instance=UART4;					    //UART4
	UART4_Handler.Init.BaudRate=bound;				    //波特率
	UART4_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART4_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART4_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART4_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART4_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART4_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART4_Handler, (u8 *)aRx4Buffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//串口4中断服务程序
void UART4_IRQHandler(void)                	
{ 
	u32 timeout=0;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART4_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART4_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>HAL_MAX_DELAY) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART4_Handler, (u8 *)aRx4Buffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>HAL_MAX_DELAY) break;	
	}
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
}


//初始化IO 串口5
//bound:波特率
void uart5_init(u32 bound)
{	
	//UART 初始化设置
	UART5_Handler.Instance=UART5;					    //UART5
	UART5_Handler.Init.BaudRate=bound;				    //波特率
	UART5_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART5_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART5_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART5_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART5_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART5_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART5_Handler, (u8 *)aRx5Buffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//串口5中断服务程序
void UART5_IRQHandler(void)                	
{ 
	u32 timeout=0;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART5_Handler);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&UART5_Handler) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>HAL_MAX_DELAY) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART5_Handler, (u8 *)aRx5Buffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>HAL_MAX_DELAY) break;	
	}
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
}


void HAL_UART_MspInit(UART_HandleTypeDef *huart) 
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
		
#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,0,3);			//抢占优先级3，子优先级3
#endif	
	}
		
	else if(huart->Instance==USART2)//如果是串口2，进行串口2 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_2;			//PA2
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin=GPIO_PIN_3;			//PA3
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA3
		
#if EN_USART2_RX
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART2中断通道
		HAL_NVIC_SetPriority(USART2_IRQn,2,3);			//抢占优先级3，子优先级3
#endif	
	}

	else if(huart->Instance==USART3)//如果是串口3，进行串口3 MSP初始化
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
		__HAL_RCC_USART3_CLK_ENABLE();			//使能USART3时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_10;			//PB10
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB10

		GPIO_Initure.Pin=GPIO_PIN_11;			//PB11
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB11
		
#if EN_USART3_RX
		HAL_NVIC_EnableIRQ(USART3_IRQn);				//使能USART2中断通道
		HAL_NVIC_SetPriority(USART3_IRQn,1,3);			//抢占优先级3，子优先级3
#endif		
	}
	
	else if(huart->Instance==UART4)//如果是串口4，进行串口4 MSP初始化
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();			//使能GPIOC时钟
		__HAL_RCC_UART4_CLK_ENABLE();			//使能UART4时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_10;			//PC10
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//初始化

		GPIO_Initure.Pin=GPIO_PIN_11;			//PC11
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//初始化
		
#if EN_UART4_RX
		HAL_NVIC_EnableIRQ(UART4_IRQn);				//使能UART4中断通道
		HAL_NVIC_SetPriority(UART4_IRQn,1,4);			//抢占优先级3，子优先级3
#endif		
	}

	else if(huart->Instance==UART5)//如果是串口5，进行串口5 MSP初始化
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();			//使能GPIOC时钟
		__HAL_RCC_GPIOD_CLK_ENABLE();			//使能GPIOD时钟
		__HAL_RCC_UART5_CLK_ENABLE();			//使能UART5时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_12;			//PC12
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);	   	//初始化

		GPIO_Initure.Pin=GPIO_PIN_2;			//PD2
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOD,&GPIO_Initure);	   	//初始化
		
#if EN_UART5_RX
		HAL_NVIC_EnableIRQ(UART5_IRQn);				//使能USART2中断通道
		HAL_NVIC_SetPriority(UART5_IRQn,1,5);			//抢占优先级3，子优先级3
#endif		
	}	
}

void UART_TransimtByte(UART_HandleTypeDef *huart,uint8_t c)
{
	HAL_UART_Transmit(huart,&c,1,1);
}

void UART1_apTrace(char* fmt,...)  
{
	  char buffer[200];
	  uint16_t i;
	  va_list ap;
	  va_start(ap,fmt);          			//ap指向fmt的地址
	  i = vsprintf(buffer,fmt,ap);	                //vsprintf返回数组的长度
	  va_end(ap);

	  HAL_UART_Transmit(&UART1_Handler,(uint8_t *)buffer,i,0X00FF);
}

void UART2_apTrace(char* fmt,...)  
{
	  char buffer[200];
	  uint16_t i;
	  va_list ap;
	  va_start(ap,fmt);          			//ap指向fmt的地址
	  i = vsprintf(buffer,fmt,ap);	                //vsprintf返回数组的长度
	  va_end(ap);

	  HAL_UART_Transmit(&UART2_Handler,(uint8_t *)buffer,i,0X00FF);
}

void UART3_apTrace(char* fmt,...)  
{
	  char buffer[200];
	  uint16_t i;
	  va_list ap;
	  va_start(ap,fmt);          			//ap指向fmt的地址
	  i = vsprintf(buffer,fmt,ap);	                //vsprintf返回数组的长度
	  va_end(ap);

	  HAL_UART_Transmit(&UART3_Handler,(uint8_t *)buffer,i,0X00FF);
}

void UART4_apTrace(char* fmt,...)  
{
	  char buffer[200];
	  uint16_t i;
	  va_list ap;
	  va_start(ap,fmt);          			//ap指向fmt的地址
	  i = vsprintf(buffer,fmt,ap);	                //vsprintf返回数组的长度
	  va_end(ap);

	  HAL_UART_Transmit(&UART4_Handler,(uint8_t *)buffer,i,0X00FF);
}

void UART5_apTrace(char* fmt,...)  
{
	  char buffer[200];
	  uint16_t i;
	  va_list ap;
	  va_start(ap,fmt);          			//ap指向fmt的地址
	  i = vsprintf(buffer,fmt,ap);	                //vsprintf返回数组的长度
	  va_end(ap);

	  HAL_UART_Transmit(&UART5_Handler,(uint8_t *)buffer,i,0X00FF);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance==USART1)//如果是串口1
	{
	  esp8266_Receive(aRxBuffer[0]);
	}
	else if(huart->Instance==USART2)//如果是串口2
	{

	}	
	else if(huart->Instance==USART3)//如果是串口3
	{
		
	}		
	else if(huart->Instance==UART4)//如果是串口4
	{

	}	
	else if(huart->Instance==UART5)//如果是串口5
  {

	}		
	
}

