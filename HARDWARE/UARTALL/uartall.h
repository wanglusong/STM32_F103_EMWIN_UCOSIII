#ifndef	UARTALL_H
#define	UARTALL_H

#include "sys.h"

#define USART_REC_LEN  			200  		//定义最大接收字节数 200


#define EN_USART1_RX 			1			//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			1		
#define EN_USART3_RX 			1	
#define EN_UART4_RX 			1			
#define EN_UART5_RX 			1		

#define RXBUFFERSIZE   1 					//缓存大小

extern u8  USART_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         			//接收状态标记	
extern UART_HandleTypeDef UART1_Handler; 	//UART句柄

extern u8  USART2_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         			//接收状态标记	
extern UART_HandleTypeDef UART2_Handler; 	//UART句柄

extern u8  USART3_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         			//接收状态标记	
extern UART_HandleTypeDef UART3_Handler; 	//UART句柄

extern u8  UART4_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 UART4_RX_STA;         			//接收状态标记	
extern UART_HandleTypeDef UART4_Handler; 	//UART句柄

extern u8  UART5_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 UART5_RX_STA;         			//接收状态标记	
extern UART_HandleTypeDef UART5_Handler; 	//UART句柄


extern u8 aRxBuffer[RXBUFFERSIZE];			//HAL库USART1接收Buffer
extern u8 aRx2Buffer[RXBUFFERSIZE];			//HAL库USART2接收Buffer
extern u8 aRx3Buffer[RXBUFFERSIZE];			//HAL库USART2接收Buffer
extern u8 aRx4Buffer[RXBUFFERSIZE];			//HAL库UART4接收Buffer
extern u8 aRx5Buffer[RXBUFFERSIZE];			//HAL库UART5接收Buffer

//<!---------------  项目添加 ---------------!>
extern u8 flagrun;
extern u8 runstate;
extern u8 flagrunstate;
//<!----------------------------------------!>


void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
void uart4_init(u32 bound);
void uart5_init(u32 bound);
void UART_TransimtByte(UART_HandleTypeDef *huart,uint8_t c);
void UART1_apTrace(char* fmt,...);
void UART2_apTrace(char* fmt,...); 
void UART3_apTrace(char* fmt,...); 
void UART4_apTrace(char* fmt,...);
void UART5_apTrace(char* fmt,...); 

#endif
