#include "smithpid.h"
#include "string.h"
#include "math.h"

SMITH_PID smith_pid_t;

#define LAG 36 //滞后10s，LAG = ts/T(滞后时间/采样时间 + 1)  10/0.05+1 = 200+1=201  10/0.25+1=1000/25=40

float Input[LAG] = {0};
float Now_Out = 0, Last_Out = 0;

/*************************************************
			Name：TransFunc_Out
	Describe：传递函数（滞后系统）
		 Param：indata {@ 控制器输出u(t) @}
	  Author：Wangls
		  Date：2020.6.25
		Return：系统输出
*************************************************/
float TransFunc_Out(float indata)
{
	int i;
	
	Input[0] = indata;
	
	for(i = 0; i < LAG - 1; i++)
	{
		Input[LAG - 1 - i] = Input[LAG - 2 - i];
	}
	
	Now_Out = smith_pid_t.a*Last_Out + smith_pid_t.b*Input[LAG-1];
	Last_Out = Now_Out;
	
	if(Now_Out >= 40)
		Now_Out = 40;
	if(Now_Out <= 0)
		Now_Out = 0;	
	
	return Now_Out;
}

/*************************************************
			Name：smith_pid_Init
	Describe：参数初始化	
		 Param：none
	  Author：Wangls
		  Date：2020.6.25
		Return：none
*************************************************/
void smith_pid_Init(void)
{
	smith_pid_t.Err1 = 0;
	
	smith_pid_t.Kp = 0.04;	//0.04 0.006
	smith_pid_t.Ki = 0.006;	//0.01 0.04
	smith_pid_t.Kd = 0.001;

	smith_pid_t.rt = 0;
	smith_pid_t.Yk = 0;
	smith_pid_t.Yk_last = 0;
	
	smith_pid_t.Err2 = 0;
	smith_pid_t.Err2_last = 0;
	smith_pid_t.Err2_pre = 0;
	
	smith_pid_t.Yt = 0;//输出
	memset(smith_pid_t.Uk,0,sizeof(smith_pid_t.Uk));
	
	//参数
	smith_pid_t.T = 0.05; 
	smith_pid_t.T0 = 0.01;
	smith_pid_t.ts = 10;
	smith_pid_t.K = 10;
	
	smith_pid_t.a = exp(-(smith_pid_t.T / smith_pid_t.T0));
	smith_pid_t.b = smith_pid_t.K*(1 - smith_pid_t.a);
}

/*************************************************
	Describe：获取偏差值	
		 Param：In {@ 信号值输入 @}
	  Author：Wangls
		  Date：2020.6.25
		Return：none
*************************************************/
void Get_Err1(float In)
{
	smith_pid_t.rt = In;
	smith_pid_t.Err1 = smith_pid_t.rt - smith_pid_t.Yt;
}

/*************************************************
	Describe：获取上上次偏差值	
		 Param：none
	  Author：Wangls
		  Date：2020.6.25
		Return：none
*************************************************/
void Get_Err2(void)
{
	smith_pid_t.Err2_pre = smith_pid_t.Err2_last;
	smith_pid_t.Err2_last = smith_pid_t.Err2;
	smith_pid_t.Err2 = smith_pid_t.Err1 - smith_pid_t.Yk;
}

/*************************************************
	Describe：预估器补偿	
		 Param：none
	  Author：Wangls
		  Date：2020.6.25
		Return：none
*************************************************/
void Get_Yk(void)
{
	smith_pid_t.Yk_last = smith_pid_t.Yk;
	smith_pid_t.Yk = smith_pid_t.a*smith_pid_t.Yk_last + 
		smith_pid_t.b*(smith_pid_t.Uk[0] - smith_pid_t.Uk[N - 1]);
}

/*************************************************
	Describe：得到控制量
		 Param：none
	  Author：Wangls
		  Date：2020.6.25
		Return：Uk[0] {@ 输出调节信号 @}
*************************************************/
float smith_pid_adjust(void)
{
	int i = 0;
	
	for(i = 0; i< N - 1; i++)
	{
		smith_pid_t.Uk[N-1-i] = smith_pid_t.Uk[N-2-i];
	}
	
	smith_pid_t.Uk[0] = smith_pid_t.Uk[1] 
		+ smith_pid_t.Kp*(smith_pid_t.Err2 - smith_pid_t.Err2_last)
		+ smith_pid_t.Ki*smith_pid_t.Err2 + smith_pid_t.Kd*(smith_pid_t.Err2
		- 2*smith_pid_t.Err2_last + smith_pid_t.Err2_pre);
	
	if(smith_pid_t.Uk[0] >= 40)
		smith_pid_t.Uk[0] = 40;
	if(smith_pid_t.Uk[0] <= 0)
		smith_pid_t.Uk[0] = 0;
 
	return smith_pid_t.Uk[0];
}

/*************************************************
	Describe：算法集合输出信号量
		 Param：targetvlaue {@ 目标值 @} actualvlaue {@ 实时值 @}
	  Author：Wangls
		  Date：2020.6.25
		Return：{@ 输出系统调节信号 @}
*************************************************/
float smith_pid_out(float targetvlaue,float actualvlaue)
{
	float ut;//控制量输出
	
	smith_pid_t.Yt = actualvlaue + 0.12;//实时值  输入信号
	Get_Err1(targetvlaue);
	Get_Yk();
	Get_Err2();
	ut = smith_pid_adjust();
	
	//return ut;
	return TransFunc_Out(ut);
}
