#ifndef __RTC_H
#define __RTC_H	    
//////////////////////////////////////////////////////////////////////////////////	 
//?????????,??????,??????????
//ALIENTEK??STM32???
//RTC????	   
//????@ALIENTEK
//????:www.openedv.com
//????:2012/9/7
//??:V1.0
//????,?????
//Copyright(C) ????????????? 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//?????
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//??????
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;					 
extern _calendar_obj calendar;	//?????
												    
void Disp_Time(u8 x,u8 y,u8 size);			//???????????
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);	//?????????
u8 RTC_Init(void);        //???RTC,??0,??;1,??;
u8 Is_Leap_Year(u16 year);//??,????
u8 RTC_Get(void);         //????   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//????			 
#endif




























 
