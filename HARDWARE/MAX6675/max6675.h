#ifndef __MAX6675_H
#define __MAX6675_H	 

#include "sys.h"

#define 	MAX6675_CS	 		GPIO_Pin_4
#define 	MAX6675_CSL()		GPIOA->BRR = MAX6675_CS;
#define 	MAX6675_CSH()		GPIOA->BSRR = MAX6675_CS;

typedef struct AverageFilter
{
	float u16AdcResult; //�������ֵ
	float u16AdcResultAve; //�˲����������ֵ
	float u16AdcSum; //�������ֵ�ۼӺ�
	float u16AdcMax; //����������ֵ
	float u16AdcMin; //���������Сֵ
	uint8_t u8Cnt; //��ǰ�ۼӼ�����
	uint8_t u8TotalCnt; //ƽ������ 2��u8CntBase�η�������������
	uint8_t u8CntBase; //���Ƹ�ֵ u8TotalCnt = 2<<u8CntBase;
}AVERAGE_FILTER_T;

extern AVERAGE_FILTER_T max6675_result_t;

void MAX6675_Init(void);//��ʼ��
uint16_t MAX6675_Read_data(void);
float MAX6675_Read_data_filter(AVERAGE_FILTER_T *Input, uint8_t AdChannelValue);
		 				    
#endif
