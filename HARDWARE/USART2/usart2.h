#ifndef __USART2_H
#define __USART2_H 
#include "sys.h" 

#define USART3_MAX_RECV_LEN 400 //�����ջ����ֽ���
#define USART3_MAX_SEND_LEN 400 //����ͻ����ֽ���
#define USART3_RX_EN 1 //0,������;1,����.

extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern u16 USART3_RX_STA; //��������״̬
extern u8 AS1[12];
extern u8 AS2[13];
extern u8 AS3[17];
extern u8 flagreceive;
extern u8 buf[17];
extern u8 tureok;
extern u8 flagzhongduan;
extern u8 bufnum;

void USART3_init(u32 bound); //����2��ʼ��
void USART3_send(u8 value);//����һ���ַ�
//void u2_printf(char* fmt, ...);

#endif

