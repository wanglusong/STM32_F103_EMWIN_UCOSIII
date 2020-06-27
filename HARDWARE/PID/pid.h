#ifndef __pid_H
#define __pid_H

#include "sys.h"

//pid�ṹ��
typedef struct {
		float SetSpeed;//�����趨ֵ
		float ActualSpeed;//����ʵ��ֵ
		float err;//����ƫ��ֵ
		float err_last;//������һ��ƫ��ֵ
		float Kp,Kd,Ki;//���������֣�΢��ϵ��
		float voltage;//���ε�ѹֵ������ִ�����ı�����
	  float lastvoltage;//�ϴε�ѹ
		float sumvoltage;//�ܵ�ѹֵ
	  float integral;//�������ֵ
}pid;

extern pid pid1;

float pid_realize(float speed);//pid ����
void pid_Init(void);//pid������ʼ��


#endif
