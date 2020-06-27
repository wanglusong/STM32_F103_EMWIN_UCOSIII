#ifndef _pid_H
#define _pid_H

#include "sys.h"

extern float Ee,EeC;
extern float SsF,TtF;
extern float Pnum,Inum,Dnum;

float speed_pid(float tar,float cur);


#endif
