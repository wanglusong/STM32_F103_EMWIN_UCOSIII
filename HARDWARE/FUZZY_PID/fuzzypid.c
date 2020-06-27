/************************************************************************************
* ģ������Ӧ�����㷨��Ϊ�˷������Ĭ��e��ec��[-3,3]���䣬
* ����ı�e��ec��Χ����������������(Ke��Kec=N/emax)����������(Ku=umax/N)�����´����
* �����������������������Լ���Ȩƽ������ģ����PID����λ��ʽPID�㷨���㷨�����ο�����ӭ����
*************************************************************************************/
#include "fuzzypid.h"


#define NL   -3//������Ҫ�޸�ģ���Ӽ�����Ҫʱ����Ե�
#define NM	 -2
#define NS	 -1
#define ZE	 0
#define PS	 1
#define PM	 2
#define PL	 3


float Ee=0.0f,EeC=0.0f;//�����仯��
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
 
     float etemp,ectemp;//�����仯��
     float eLefttemp,ecLefttemp;//�ϴ����ϴ����仯��
     float eRighttemp ,ecRighttemp;//���ϴ������ϴ����仯��
 
     int eLeftIndex,ecLeftIndex;//תΪ������
     int eRightIndex,ecRightIndex;//תΪ������
     PID fuzzy_PID;//pid �ṹ��
	   //�ȽϺ��������� : ��ȡ�󣬷�ȡ : ���ұ�
     etemp = e > 3.0f ? 0.0f : (e < - 3.0f ? 0.0f : (e >= 0.0f ? (e >= 2.0f ? 2.5f: (e >= 1.0f ? 1.5f : 0.5f)) : (e >= -1.0f ? -0.5f : (e >= -2.0f ? -1.5f : (e >= -3.0f ? -2.5f : 0.0f) ))));
 
     eLeftIndex = (int)e;//�ϴ����
     eRightIndex = eLeftIndex;//���ϴ����
	
	   //���������ĸ�����ֵ  �������ά��������  ����ectemp=2.5  ecLeftIndex=5��ecRightIndex=6
     eLeftIndex = (int)((etemp-0.5f) + 3);//[-3,3] -> [0,6]  +-0.5��Ϊ��ת��������Ϊ����
     eRightIndex = (int)((etemp+0.5f) + 3);
 
     eLefttemp =etemp == 0.0f ? 0.0f:((etemp+0.5f)-e);//�����  ����e=2.15,��etemp=2.5,eLefttemp=2.5+0.5-2.15=0.85 
     eRighttemp=etemp == 0.0f ? 0.0f:( e-(etemp-0.5f));//�����  ����e=2.5,��etemp=2.5,eRighttemp=2.15-(2.5-0.5)=0.15 
     //eRighttemp=1.0f-eLefttemp;//eRighttemp=1-0.85=0.15;
	
	
     ectemp = ec > 3.0f ? 0.0f : (ec < - 3.0f ? 0.0f : (ec >= 0.0f ? (ec >= 2.0f ? 2.5f: (ec >= 1.0f ? 1.5f : 0.5f)) : (ec >= -1.0f ? -0.5f : (ec >= -2.0f ? -1.5f : (ec >= -3.0f ? -2.5f : 0.0f) ))));

     ecLeftIndex = (int)((ectemp-0.5f) + 3);//[-3,3] -> [0,6]  ���������ĸ�����ֵ  �������ά��������  ����ectemp=2.5  ecLeftIndex=5��ecRightIndex=6
     ecRightIndex = (int)((ectemp+0.5f) + 3);
 
     ecLefttemp =ectemp == 0.0f ? 0.0f:((ectemp+0.5f)-ec);//����������
     ecRighttemp=ectemp == 0.0f ? 0.0f:( ec-(ectemp-0.5f));
		 //ecRighttemp=1.0f-ecLefttemp;//��Ϊ1
 
/*************************************��ģ��*************************************/
 
  //https://blog.csdn.net/weixin_36340979/article/details/79168052 �ο���ƪ����
	
	fuzzy_PID.Kp = (eLefttemp * ecLefttemp *  fuzzyRuleKp[eLeftIndex][ecLeftIndex]    //������   ����ֵ     ����������*����ֵ=k��i��d����           
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
 
 
float speed_pid(float tar,float cur)//Ŀ��ֵ , ʵ��ֵ
{
	static PID pid= {0.232, 0.032, 0.02};//�����ֵkp��ki��kd   0.232, 0.032, 0.02//���
	static int sumE = 0; //�ۼ�ƫ��
	static int lastE = 0;//�ϴ�ƫ��
	PID OUT = {0, 0, 0};
	float e = -0.5,ec = -0.12;
	
	e = tar - cur;             //Ŀ��ֵ - ʵ��ֵ      70 190  0  10
	ec = e - lastE;            //���仯��
	sumE += e;
	lastE = e;
	
	OUT = fuzzy(e, ec);      //ģ�����Ƶ���  kp��ki��kd
	
	Pnum=pid.Kp+OUT.Kp;//
	Inum=pid.Ki+OUT.Ki;//
	Dnum=pid.Kd+OUT.Kd;//
 
	return (Pnum*e+Inum*sumE+Dnum*ec);
}
