#ifndef _smithpid_H
#define _smithpid_H

#include "sys.h"

#define N 36

typedef struct smith_pid
{
	float Kp;
	float Ki;
	float Kd;//初始的PID参数
	
	float rt;//输入
	float Err1;//偏差err1
	float Yk, Yk_last;//预估器补偿
	float Err2,Err2_last,Err2_pre;//偏差err2

  float Uk[N];//输出控制量
	float Yt;//输出
	
	float T;		  //采样周期
	 int ts;			//纯滞后时间
	float T0;		  //被控对象的时间常数
	float K;			//被控对象的放大系数

	float a, b;		//参数a,b
	
}SMITH_PID;

/*************************************************
			Name：smith_pid_Init
	Describe：参数初始化	
		 Param：none
	  Author：Wangls
		  Date：2020.6.25
		Return：none
*************************************************/
void smith_pid_Init(void);

/*************************************************
	Describe：算法集合输出信号量
		 Param：targetvlaue {@ 目标值 @} actualvlaue {@ 实时值 @}
	  Author：Wangls
		  Date：2020.6.25
		Return：{@ 输出系统调节信号 @}
*************************************************/
float smith_pid_out(float targetvlaue,float actualvlaue);


#endif
