#ifndef _ALARMCLOCK_H
#define _ALARMCLOCK_H	
#include "system.h"
#include "key_board.h"
#include "sram.h"
#include "malloc.h" 
#include "stm32_flash.h"
#define Clock_Max 16
#define Clock_ui_Max 40
typedef struct clock_keep
{
	u8 number;
  u8 hour;
	u8 mini;
	u8 week[7];
	u8 flag;//1¹Ø2¿ª3É¾³ý
  char *title;
}CLOCK_KEEP;
void clock_ui_Init(int N);
void Clock_Move();
void Alarm_Clock();
#endif