#ifndef __USART2_H
#define __USART2_H 
#include "sys.h" 

#define USART3_MAX_RECV_LEN 400 //最大接收缓存字节数
#define USART3_MAX_SEND_LEN 400 //最大发送缓存字节数
#define USART3_RX_EN 1 //0,不接收;1,接收.

extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 USART3_RX_STA; //接收数据状态
extern u8 AS1[12];
extern u8 AS2[13];
extern u8 AS3[17];
extern u8 flagreceive;
extern u8 buf[17];
extern u8 tureok;
extern u8 flagzhongduan;
extern u8 bufnum;

void USART3_init(u32 bound); //串口2初始化
void USART3_send(u8 value);//发送一个字符
//void u2_printf(char* fmt, ...);

#endif

