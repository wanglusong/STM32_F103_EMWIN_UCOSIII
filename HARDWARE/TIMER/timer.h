#ifndef _TIMER_H
#define _TIMER_H

#include "sys.h"
#include "stdbool.h"

typedef struct time_param
{
	uint16_t ttms;
	uint16_t tts;
	bool timestatus;
	
}TIME_PARAM;

extern TIME_PARAM time_param_t;

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);
#endif
