#ifndef _smithpid_H
#define _smithpid_H

#include "sys.h"

#define N 36

typedef struct smith_pid
{
	float Kp;
	float Ki;
	float Kd;//��ʼ��PID����
	
	float rt;//����
	float Err1;//ƫ��err1
	float Yk, Yk_last;//Ԥ��������
	float Err2,Err2_last,Err2_pre;//ƫ��err2

  float Uk[N];//���������
	float Yt;//���
	
	float T;		  //��������
	 int ts;			//���ͺ�ʱ��
	float T0;		  //���ض����ʱ�䳣��
	float K;			//���ض���ķŴ�ϵ��

	float a, b;		//����a,b
	
}SMITH_PID;

/*************************************************
			Name��smith_pid_Init
	Describe��������ʼ��	
		 Param��none
	  Author��Wangls
		  Date��2020.6.25
		Return��none
*************************************************/
void smith_pid_Init(void);

/*************************************************
	Describe���㷨��������ź���
		 Param��targetvlaue {@ Ŀ��ֵ @} actualvlaue {@ ʵʱֵ @}
	  Author��Wangls
		  Date��2020.6.25
		Return��{@ ���ϵͳ�����ź� @}
*************************************************/
float smith_pid_out(float targetvlaue,float actualvlaue);


#endif
