#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "key.h"
#include "time.h"
#include "touch.h"
#define Board_Max 50
#define Input_Max 39

void draw_ui(EVENT *keyboard,int len);
int  RunEvent_Count();
int  RunEvent_English();
void Show_Input();
void Input_Add(EVENT keyboard, int number);
void Input_Sub(int number);
int Key_Touch();
void Disappear_Key();
void Flash_Key();
void KeyBoard_Cap();
void KeyBoard_Sym();
void key_Board(EVENT event);
void Key_Scanf(char *a);