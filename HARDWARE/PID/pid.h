#ifndef __pid_H
#define __pid_H

#include "sys.h"

//pid结构体
typedef struct {
		float SetSpeed;//定义设定值
		float ActualSpeed;//定义实际值
		float err;//定义偏差值
		float err_last;//定义上一次偏差值
		float Kp,Kd,Ki;//比例，积分，微分系数
		float voltage;//本次电压值（控制执行器的变量）
	  float lastvoltage;//上次电压
		float sumvoltage;//总电压值
	  float integral;//定义积分值
}pid;

extern pid pid1;

float pid_realize(float speed);//pid 反馈
void pid_Init(void);//pid参数初始化


#endif
