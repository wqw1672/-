#ifndef _time_H
#define _time_H

#include "system.h"
#define MAX_SOFT_TIMER 2  //????????
#define OPEN   1
#define CLOSE  0
#define NULL   0
#define ZERO   0
#define TIMER_TS_FULL 1
#define TIMER_TS_OK   2

struct TIMER_TS
{
	int count;
	int curCount;
	void (*pHandle)();
	int isOpen;
};

void TIM4_Init(u16 per,u16 psc);
void SOFT_TIME_Init(void);
void SOFT_TIME_Dispaly(void);
int TIME_Events_Add(int count, void (*pHandle)());



#endif
