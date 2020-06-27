#ifndef __MAX6675_H
#define __MAX6675_H	 

#include "sys.h"

#define 	MAX6675_CS	 		GPIO_Pin_4
#define 	MAX6675_CSL()		GPIOA->BRR = MAX6675_CS;
#define 	MAX6675_CSH()		GPIOA->BSRR = MAX6675_CS;

typedef struct AverageFilter
{
	float u16AdcResult; //输入采样值
	float u16AdcResultAve; //滤波后输出采样值
	float u16AdcSum; //输入采样值累加和
	float u16AdcMax; //输入采样最大值
	float u16AdcMin; //输入采样最小值
	uint8_t u8Cnt; //当前累加计数器
	uint8_t u8TotalCnt; //平均个数 2的u8CntBase次方，避免做除法
	uint8_t u8CntBase; //右移该值 u8TotalCnt = 2<<u8CntBase;
}AVERAGE_FILTER_T;

extern AVERAGE_FILTER_T max6675_result_t;

void MAX6675_Init(void);//初始化
uint16_t MAX6675_Read_data(void);
float MAX6675_Read_data_filter(AVERAGE_FILTER_T *Input, uint8_t AdChannelValue);
		 				    
#endif
