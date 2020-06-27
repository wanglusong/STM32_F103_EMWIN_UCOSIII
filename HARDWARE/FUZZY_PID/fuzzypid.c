/************************************************************************************
* 模糊自适应控制算法，为了方便测试默认e、ec在[-3,3]区间，
* 如需改变e、ec范围，需引入量化因子(Ke、Kec=N/emax)、缩放因子(Ku=umax/N)。以下代码采
* 用三角隶属函数求隶属度以及加权平均法解模糊，PID采用位置式PID算法，算法仅供参考，欢迎报错。
*************************************************************************************/
#include "fuzzypid.h"


#define NL   -3//尽量不要修改模糊子集，需要时间调试的
#define NM	 -2
#define NS	 -1
#define ZE	 0
#define PS	 1
#define PM	 2
#define PL	 3


float Ee=0.0f,EeC=0.0f;//误差、误差变化率
float SsF=0.0f,TtF=0.0f;
float Pnum=0.0f,Inum=0.0f,Dnum=0.0f;
 
 
static const float fuzzyRuleKp[7][7]={
	PL,	PL,	PM,	PM,	PS,	PS,	ZE,
	PL,	PL,	PM,	PM,	PS,	ZE,	ZE,
	PM,	PM,	PM,	PS,	ZE,	NS,	NM,
	PM,	PS,	PS,	ZE,	NS,	NM,	NM,
	PS,	PS,	ZE,	NS,	NS,	NM,	NM,
	ZE,	ZE,	NS,	NM,	NM,	NM,	NL,
	ZE,	NS,	NS,	NM,	NM,	NL,	NL
};
 
static const float fuzzyRuleKi[7][7]={
	NL,	NL,	NL,	NM,	NM,	ZE,	ZE,
	NL,	NL,	NM,	NM,	NS,	ZE,	ZE,
	NM,	NM,	NS,	NS,	ZE,	PS,	PS,
	NM,	NS,	NS,	ZE,	PS,	PS,	PM,
	NS,	NS,	ZE,	PS,	PS,	PM,	PM,
	ZE,	ZE,	PS,	PM,	PM,	PL,	PL,
	ZE,	ZE,	PS,	PM,	PL,	PL,	PL
};
 
static const float fuzzyRuleKd[7][7]={
	PS,	PS,	ZE,	ZE,	ZE,	PL,	PL,
	NS,	NS,	NS,	NS,	ZE,	NS,	PM,
	NL,	NL,	NM,	NS,	ZE,	PS,	PM,
	NL,	NM,	NM,	NS,	ZE,	PS,	PM,
	NL,	NM,	NS,	NS,	ZE,	PS,	PS,
	NM,	NS,	NS,	NS,	ZE,	PS,	PS,
	PS,	ZE,	ZE,	ZE,	ZE,	PL,	PL
};
 
typedef struct{
	float Kp;
	float Ki;
	float Kd;
}PID;
 
 
static PID fuzzy(float e,float ec)
{
 
     float etemp,ectemp;//误差，误差变化率
     float eLefttemp,ecLefttemp;//上次误差，上次误差变化率
     float eRighttemp ,ecRighttemp;//上上次误差，上上次误差变化率
 
     int eLeftIndex,ecLeftIndex;//转为正整数
     int eRightIndex,ecRightIndex;//转为正整数
     PID fuzzy_PID;//pid 结构体
	   //比较函数，是则 : 的取左，否取 : 的右边
     etemp = e > 3.0f ? 0.0f : (e < - 3.0f ? 0.0f : (e >= 0.0f ? (e >= 2.0f ? 2.5f: (e >= 1.0f ? 1.5f : 0.5f)) : (e >= -1.0f ? -0.5f : (e >= -2.0f ? -1.5f : (e >= -3.0f ? -2.5f : 0.0f) ))));
 
     eLeftIndex = (int)e;//上次误差
     eRightIndex = eLeftIndex;//上上次误差
	
	   //计算属于哪个隶属值  即计算二维数组坐标  假如ectemp=2.5  ecLeftIndex=5，ecRightIndex=6
     eLeftIndex = (int)((etemp-0.5f) + 3);//[-3,3] -> [0,6]  +-0.5是为了转化浮点数为整数
     eRightIndex = (int)((etemp+0.5f) + 3);
 
     eLefttemp =etemp == 0.0f ? 0.0f:((etemp+0.5f)-e);//正最大  假设e=2.15,则etemp=2.5,eLefttemp=2.5+0.5-2.15=0.85 
     eRighttemp=etemp == 0.0f ? 0.0f:( e-(etemp-0.5f));//负最大  假设e=2.5,则etemp=2.5,eRighttemp=2.15-(2.5-0.5)=0.15 
     //eRighttemp=1.0f-eLefttemp;//eRighttemp=1-0.85=0.15;
	
	
     ectemp = ec > 3.0f ? 0.0f : (ec < - 3.0f ? 0.0f : (ec >= 0.0f ? (ec >= 2.0f ? 2.5f: (ec >= 1.0f ? 1.5f : 0.5f)) : (ec >= -1.0f ? -0.5f : (ec >= -2.0f ? -1.5f : (ec >= -3.0f ? -2.5f : 0.0f) ))));

     ecLeftIndex = (int)((ectemp-0.5f) + 3);//[-3,3] -> [0,6]  计算属于哪个隶属值  即计算二维数组坐标  假如ectemp=2.5  ecLeftIndex=5，ecRightIndex=6
     ecRightIndex = (int)((ectemp+0.5f) + 3);
 
     ecLefttemp =ectemp == 0.0f ? 0.0f:((ectemp+0.5f)-ec);//计算隶属率
     ecRighttemp=ectemp == 0.0f ? 0.0f:( ec-(ectemp-0.5f));
		 //ecRighttemp=1.0f-ecLefttemp;//和为1
 
/*************************************反模糊*************************************/
 
  //https://blog.csdn.net/weixin_36340979/article/details/79168052 参考本篇文章
	
	fuzzy_PID.Kp = (eLefttemp * ecLefttemp *  fuzzyRuleKp[eLeftIndex][ecLeftIndex]    //隶属率   隶属值     计算隶属率*隶属值=k、i、d参数           
					+ eLefttemp * ecRighttemp * fuzzyRuleKp[eLeftIndex][ecRightIndex]
					+ eRighttemp * ecLefttemp * fuzzyRuleKp[eRightIndex][ecLeftIndex]
					+ eRighttemp * ecRighttemp * fuzzyRuleKp[eRightIndex][ecRightIndex]);
 
	fuzzy_PID.Ki = (eLefttemp * ecLefttemp *  fuzzyRuleKi[eLeftIndex][ecLeftIndex]
					+ eLefttemp * ecRighttemp * fuzzyRuleKi[eLeftIndex][ecRightIndex]
					+ eRighttemp * ecLefttemp * fuzzyRuleKi[eRightIndex][ecLeftIndex]
					+ eRighttemp * ecRighttemp * fuzzyRuleKi[eRightIndex][ecRightIndex]);
 
	fuzzy_PID.Kd = (eLefttemp * ecLefttemp *  fuzzyRuleKd[eLeftIndex][ecLeftIndex]
					+ eLefttemp * ecRighttemp * fuzzyRuleKd[eLeftIndex][ecRightIndex]
					+ eRighttemp * ecLefttemp * fuzzyRuleKd[eRightIndex][ecLeftIndex]
					+ eRighttemp * ecRighttemp * fuzzyRuleKd[eRightIndex][ecRightIndex]);
					
	Ee=etemp;
	EeC=ectemp;
					
  return fuzzy_PID;
 
}
 
 
float speed_pid(float tar,float cur)//目标值 , 实际值
{
	static PID pid= {0.232, 0.032, 0.02};//赋予初值kp，ki，kd   0.232, 0.032, 0.02//最佳
	static int sumE = 0; //累加偏差
	static int lastE = 0;//上次偏差
	PID OUT = {0, 0, 0};
	float e = -0.5,ec = -0.12;
	
	e = tar - cur;             //目标值 - 实际值      70 190  0  10
	ec = e - lastE;            //误差变化率
	sumE += e;
	lastE = e;
	
	OUT = fuzzy(e, ec);      //模糊控制调整  kp，ki，kd
	
	Pnum=pid.Kp+OUT.Kp;//
	Inum=pid.Ki+OUT.Ki;//
	Dnum=pid.Kd+OUT.Kd;//
 
	return (Pnum*e+Inum*sumE+Dnum*ec);
}
