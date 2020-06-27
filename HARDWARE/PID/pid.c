#include "pid.h"
#include "led.h"
#include "usart.h"
#include "math.h"
#include "stdlib.h"

pid pid1;

u16 indexok=0;

void pid_Init(void)//pid1参数初始化
{
	  pid1.SetSpeed = 0.0;
	  pid1.ActualSpeed = 0.0;
	  pid1.err = 0.0;
	  pid1.err_last = 0.0;
	  pid1.integral = 0.0;
		pid1.lastvoltage = 0.0;
	  pid1.sumvoltage = 0.0;
//	  pid1.Kp=0.032;
//	  pid1.Ki=0.015;
//	  pid1.Kd=0.032;
	  pid1.Kp = 0.5;
	  pid1.Ki = 2.4;
	  pid1.Kd = 0.02;	
		pid1.voltage = 0.0;
}

//	  pid1.Kp=0.45;
//	  pid1.Ki=0.035;
//	  pid1.Kd=0.321;

float pid_realize(float speed)//pid1 反馈 float setvlaue
{
	pid1.SetSpeed=speed;
	pid1.err=pid1.SetSpeed - pid1.ActualSpeed;

//	if(abs((int)pid1.err)>speed)//看情况设定
//	{
//	  indexok=0;
//	}
//	else
//	{
//		indexok=1;
//	  pid1.integral+=pid1.err;	
//	}

	pid1.voltage = pid1.Kp*(pid1.err-pid1.err_last)+pid1.Ki*pid1.err+pid1.Kd*(pid1.err-2*pid1.err_last+pid1.integral);
	pid1.sumvoltage =  pid1.voltage + pid1.lastvoltage;
	
	if(pid1.sumvoltage > 40)pid1.sumvoltage = 40;
	else if(pid1.sumvoltage < 0)pid1.sumvoltage = 0;
	
	pid1.lastvoltage = pid1.sumvoltage;
	
	pid1.integral = pid1.err_last;
	pid1.err_last = pid1.err;
	
	
	return pid1.sumvoltage;
}	

