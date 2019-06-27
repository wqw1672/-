#ifndef __usart_H
#define __usart_H

#include "system.h" 
#include "stdio.h" 
#include "string.h"
#include "stdlib.h"
#include "led.h"
typedef struct QUEUE			
{	
	unsigned int head;
	unsigned int tail;
	unsigned int queue_len;
	
	unsigned int queue_size;
	
  char *pdata;	 	
}QUEUE_T;
typedef enum
{
	CMD_Here,
	CMD_OnLed,
	CMD_OffLed,
	CMD_GetTime,  //????
	CMD_SetTime,	//???
	ReadMem     ,     //????
	WriteMem,
	CMD_Reset,
	CMD_GETTIME,
	CMD_Alarm,
	CMD_Music,
	CMD_GiveTime,
	CMD_MAX

}CMD_TYPE;

typedef void (*cmdfun)(void);

typedef struct  uartcmd
{
	CMD_TYPE  cmd;
	char *cmdstr; 
	cmdfun  fun;
}CMD_T;


void USART1_Init(u32 bound);
void queue_init(QUEUE_T *pqueue,int len);
void queue_datain(QUEUE_T *pqueue,char data);
char queue_dataout(QUEUE_T *pqueue);
void choice_power(char *a);

void Analysis();

void uartcmd_init();

//?? ?? ??fun     ??? cmdstr
void uartcmd_install(CMD_TYPE  cmd,char *cmdstr,cmdfun  fun);


void uartcmd_unstall(CMD_TYPE  cmd);

//??
void uartcmd_run(char *cmdstr);

#endif


