#ifndef __esp8266_H
#define	__esp8266_H


#include "sys.h"

void esp8266_init(void);
void esp8266_set(char *puf);
void usart_send(char value);
void esp8266_Receive(u8 Res);

#endif 

