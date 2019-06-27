#include "alarmclock.h"
extern void  (* Task[10])(); //任务的数组
extern int current;  //当前任务的数量
extern int current_Now;
u8 keep_Count=0;
extern u8 keep_Count_Max;
extern CLOCK_KEEP clockkeep[10];
void Warn_Ui();
EVENT ALARMCLOCK[Clock_Max]=
{
  {20,40,60,30,"取消",0,NULL},
	{165,40,60,30,"确定",0,NULL},
	{20,120,30,30,"0",0,Clock_Move},
	{195,120,30,30,"0",0,Clock_Move},
	{20,90,30,30," ",0,NULL},
	{195,90,30,30," ",0,NULL},
	{20,150,30,30,"1",0,NULL},
	{195,150,30,30,"1",0,NULL},
	{110,120,30,30,":",0,NULL},
	{5,190,30,30,"一",0,NULL},
	{40,190,30,30,"二",0,NULL},
	{75,190,30,30,"三",0,NULL},
	{110,190,30,30,"四",0,NULL},
	{145,190,30,30,"五",0,NULL},
	{180,190,30,30,"六",0,NULL},
	{215,190,30,30,"日",0,NULL},
};
EVENT clock_ui[Clock_ui_Max]=
{
  {20,40,60,30,"返回",0,NULL},
	{165,40,60,30,"添加",0,Clock_Move},
	
//	{5,80,180,32,"00:00",0,Clock_Move},
//	{5,96,180,16,"title",0,NULL},
//	{70,80,115,16,"1234567",0,NULL},
//	{185,80,25,32,"关",0,NULL},
//	{210，80,25,32,"X",0,NULL},

//	{5,112+5,195,32,"00:00",0,Clock_Move},
//	{5,128+5,180,16,"title",0,NULL},
//	{70,112+5,115,16,"1234567",0,NULL},
//	{185,112+5,50,32,"关",0,NULL},
//	{210，80,25,32,"X",0,NULL},

//	{5,144+10,195,32,"00:00",0,Clock_Move},
//	{5,160+10,180,16,"title",0,NULL},
//	{70,144+10,115,16,"1234567",0,NULL},
//	{185,144+10,50,32,"关",0,NULL},

};
/****************************************************************************
* Function Name  : Alarm_Clock
* Description    : 闹钟主界面
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Alarm_Clock()
{
	int i=0,maybe=0,j=0;
  LCD_Clear(WHITE);
  Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"闹钟选择界面");
 draw_ui(clock_ui,Clock_ui_Max);
 while(1)
	{ 
		maybe=TOUCH_Scan(1);
		if(maybe==1)
		    return ;
		if(maybe == 0)
		{
			for(i=0;i<Clock_ui_Max;i++)
			{
						 if(TouchData.lcdx>clock_ui[i].xpos&&TouchData.lcdx<(clock_ui[i].xpos+clock_ui[i].wideth)&&TouchData.lcdy>clock_ui[i].ypos&&TouchData.lcdy<(clock_ui[i].ypos+clock_ui[i].height))
						 {
							 
								if(i==0)
									{
									 LCD_Clear(WHITE);
									    return ;
									}

               if(i%5==0)
							 {
							  if(clock_ui[i].flag==1)
									 {
											Button(clock_ui[i].xpos,clock_ui[i].ypos,clock_ui[i].wideth, clock_ui[i].height,RED,clock_ui[i].title);
											delay_ms(100);
											clock_ui[i].title="开";
											Button(clock_ui[i].xpos,clock_ui[i].ypos,clock_ui[i].wideth, clock_ui[i].height,YELLOW,clock_ui[i].title);
											clock_ui[i].flag=1;
											keep_Count=(i-2)/5;
										  clockkeep[keep_Count].flag=1;
                      STM32_FLASH_Write(0x8060000+keep_Count*sizeof(CLOCK_KEEP),(u16 *)&clockkeep[keep_Count],sizeof(CLOCK_KEEP)/2);
									 }
										else if(clock_ui[i].flag==2)
									 {
											Button(clock_ui[i].xpos,clock_ui[i].ypos,clock_ui[i].wideth, clock_ui[i].height,RED,clock_ui[i].title);
											delay_ms(100);
											clock_ui[i].title="关";
											Button(clock_ui[i].xpos,clock_ui[i].ypos,clock_ui[i].wideth, clock_ui[i].height,YELLOW,clock_ui[i].title);
									 	  clock_ui[i].flag=1;
										  keep_Count=(i-2)/5;
										  clockkeep[keep_Count].flag=0;
                      STM32_FLASH_Write(0x8060000+keep_Count*sizeof(CLOCK_KEEP),(u16 *)&clockkeep[keep_Count],sizeof(CLOCK_KEEP)/2);
                      
									 }
							 
							 }
//							 if((i-1)%5==0&&i!=1)
//							 {
//							   keep_Count=(i-2)/5;
//								 clockkeep[keep_Count].flag=0;
//								 STM32_FLASH_Write(0x8060000+keep_Count*sizeof(CLOCK_KEEP),(u16 *)&clockkeep[keep_Count],sizeof(CLOCK_KEEP)/2);
//                 for(j=keep_Count;j<keep_Count_Max-1;j++)
//								 {
//								 STM32_FLASH_Write(0x8060000+keep_Count*sizeof(CLOCK_KEEP),(u16 *)&clockkeep[keep_Count+1],sizeof(CLOCK_KEEP)/2);
//                 }
//								 clockkeep[keep_Count_Max-1].flag=0;
//								 STM32_FLASH_Write(0x8060000+(keep_Count_Max-1)*sizeof(CLOCK_KEEP),(u16 *)&clockkeep[keep_Count_Max-1],sizeof(CLOCK_KEEP)/2);
//              
//							 Task[current]=Alarm_Clock;
//							 current+=2;
//							 current_Now+=2;
//								 return ;
//               }
							 
							 if(clock_ui[i].handle==NULL)
							 {
							    continue;
							 }
							 keep_Count=(i-2)/5;
							 if(i==1)
								keep_Count=keep_Count_Max; 

							Task[current]=clock_ui[i].handle;
							 current+=2;
							 current_Now+=2;
							 if(i%2==0)
								 LCD_Clear(WHITE);
							 return ;
						 }
			}
		}
	}

}
/****************************************************************************
* Function Name  : Clock_Move
* Description    : 闹钟设置
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Clock_Move()
{
	 int i=0,y=0,y1=0,y2=0,hour_or_mini=0,hm_max=0,k=0;
	 LCD_Clear(WHITE);

	if(keep_Count>=5)
	 {
			Task[current-1]=Warn_Ui;
			 current_Now+=2;
			 return ;
	 }
	
	Lable(0,120,CYAN,245, 30);
	 Lable(0,190,CYAN,245, 30);
	  Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"闹钟设置");
	for(i=2;i<8;i++)
	{
	ALARMCLOCK[i].title=(char *)malloc(2*sizeof(char));
	memset(ALARMCLOCK[i].title,0,2*sizeof(char));
	}
		sprintf(ALARMCLOCK[2].title,"%d",clockkeep[keep_Count].hour);
		sprintf(ALARMCLOCK[3].title,"%d",clockkeep[keep_Count].mini);
    

	draw_ui(ALARMCLOCK,Clock_Max);
  for(i=9;i<=15;i++)
	{
   if( clockkeep[keep_Count].week[i-9]==1)
		Button(ALARMCLOCK[i].xpos,ALARMCLOCK[i].ypos,ALARMCLOCK[i].wideth, ALARMCLOCK[i].height,RED,ALARMCLOCK[i].title);

  }

	while(1)
	{
		if(TOUCH_Scan(1)==1)	
		return ;
		
		if(TOUCH_Scan(1)==0xFF)
		   ALARMCLOCK[hour_or_mini].flag=0;
		
		 if(hour_or_mini==3)
			 hm_max=60;
		 if(hour_or_mini==2)
			 hm_max=24;
		 if(TOUCH_Scan(1) == 0)
		 {
			 if(ALARMCLOCK[hour_or_mini].flag==1)
				 {
								 if(TouchData.lcdy<y1)
								 y =atoi(ALARMCLOCK[hour_or_mini].title)+(y1-TouchData.lcdy)/10;
                 else if(TouchData.lcdy>y1)
								 y =atoi(ALARMCLOCK[hour_or_mini].title)-(TouchData.lcdy-y1)/10;
									
								 if(y<0)
										y=y+hm_max;						
									y=y%hm_max;

									
									sprintf(ALARMCLOCK[hour_or_mini].title,"%d",y);
								Button(ALARMCLOCK[hour_or_mini].xpos,ALARMCLOCK[hour_or_mini].ypos,ALARMCLOCK[hour_or_mini].wideth, ALARMCLOCK[hour_or_mini].height,CYAN,ALARMCLOCK[hour_or_mini].title);
			
									y2=y-1;
							   if(y2<0)
										y2=y2+hm_max;						
									y2=y2%hm_max;
									sprintf(ALARMCLOCK[hour_or_mini+2].title,"%d",y2);
								Button(ALARMCLOCK[hour_or_mini+2].xpos,ALARMCLOCK[hour_or_mini+2].ypos,ALARMCLOCK[hour_or_mini+2].wideth, ALARMCLOCK[hour_or_mini+2].height,CYAN,ALARMCLOCK[hour_or_mini+2].title);
							    	y2=y+1;
							   if(y2<0)
										y2=y2+hm_max;						
									y2=y2%hm_max;
									
									sprintf(ALARMCLOCK[hour_or_mini+4].title,"%d",y2);
										Button(ALARMCLOCK[hour_or_mini+4].xpos,ALARMCLOCK[hour_or_mini+4].ypos,ALARMCLOCK[hour_or_mini+4].wideth, ALARMCLOCK[hour_or_mini+4].height,CYAN,ALARMCLOCK[hour_or_mini+4].title);
								   y1=TouchData.lcdy;

				 }
				
				 for(i=0;i<Clock_Max;i++)
				 {
				 
							 if(ALARMCLOCK[i].flag==0&&TouchData.lcdx>ALARMCLOCK[i].xpos&&TouchData.lcdx<(ALARMCLOCK[i].xpos+ALARMCLOCK[i].wideth)&&TouchData.lcdy>ALARMCLOCK[i].ypos&&TouchData.lcdy<(ALARMCLOCK[i].ypos+ALARMCLOCK[i].height))
							 {
								 if(i==2||i==3)
									 {
												ALARMCLOCK[i].flag=1;
												 hour_or_mini=i;
											 y1=TouchData.lcdy;
												break;
									 }
									if(i==0)
									{
										 LCD_Clear(WHITE);
												return ;
									}
									if(i==1)
									{
											clockkeep[keep_Count].flag=2;
											clockkeep[keep_Count].hour=atoi(ALARMCLOCK[2].title);
											clockkeep[keep_Count].mini=atoi(ALARMCLOCK[3].title);
										  clockkeep[keep_Count].title="title";
												if(keep_Count==keep_Count_Max)
												   clock_ui_Init(keep_Count);
											for(i=1;i<=7;i++)
												{
													if(clockkeep[keep_Count].week[i-1]==1)
													{
														 clock_ui[4+5*keep_Count].title[k]=i+'0';
															k++;
													}
												}
										  STM32_FLASH_Write(0x8060000+keep_Count*sizeof(CLOCK_KEEP),(u16 *)&clockkeep[keep_Count],sizeof(CLOCK_KEEP)/2);

						   				sprintf(clock_ui[2+5*keep_Count].title,"%d:%d",clockkeep[keep_Count].hour,clockkeep[keep_Count].mini);
											if(keep_Count_Max==keep_Count)
												 keep_Count_Max++;
										  return ;
										
									}
									if(i>=9&&i<=15)
									{
											if(clockkeep[keep_Count].week[i-9]!=1)
											{
											clockkeep[keep_Count].week[i-9]=1;
											Button(ALARMCLOCK[i].xpos,ALARMCLOCK[i].ypos,ALARMCLOCK[i].wideth, ALARMCLOCK[i].height,RED,ALARMCLOCK[i].title);

											}
											else if(clockkeep[keep_Count].week[i-9]==1)
											{
											clockkeep[keep_Count].week[i-9]=0;
											Button(ALARMCLOCK[i].xpos,ALARMCLOCK[i].ypos,ALARMCLOCK[i].wideth, ALARMCLOCK[i].height,YELLOW,ALARMCLOCK[i].title);
											}
									}
							 }
				 }
				 	delay_ms(100);
		}	
	}

}
void clock_ui_Init(int N)
{
  int i =0,k=0;
  clock_ui[2+N*5].xpos=5;
	clock_ui[2+N*5].ypos=80+37*N;
  clock_ui[2+N*5].wideth=180;
	clock_ui[2+N*5].height=32;
	clock_ui[2+N*5].flag=0;
  clock_ui[2+N*5].handle=Clock_Move;
				clock_ui[2+5*N].title=(char *)malloc(10*sizeof(char));		
//			memset(clock_ui[3+4*i].title,0,10*sizeof(char));
			clock_ui[3+5*N].title=(char *)malloc(15*sizeof(char));

  clock_ui[3+N*5].xpos=5;
	clock_ui[3+N*5].ypos=96+37*N;
  clock_ui[3+N*5].wideth=180;
	clock_ui[3+N*5].height=16;
	clock_ui[3+N*5].flag=0;
	clock_ui[3+N*5].handle=NULL;
 
	clock_ui[4+5*N].title=(char *)malloc(8*sizeof(char));			
	clock_ui[4+N*5].xpos=70;
	clock_ui[4+N*5].ypos=80+37*N;
  clock_ui[4+N*5].wideth=115;
	clock_ui[4+N*5].height=16;
	clock_ui[4+N*5].flag=0;
	clock_ui[4+N*5].handle=NULL;
	
	clock_ui[5+N*5].xpos=185;
	clock_ui[5+N*5].ypos=80+37*N;
  clock_ui[5+N*5].wideth=25;
	clock_ui[5+N*5].height=32;
	clock_ui[5+N*5].handle=NULL;
	clock_ui[5+5*N].flag=clockkeep[N].flag;
						
	if(clockkeep[N].flag==1)
			clock_ui[5+5*N].title="关";
	else if(clockkeep[N].flag==2)
			clock_ui[5+5*N].title="开";
	//	{210，80,25,32,"X",0,NULL},
	clock_ui[6+N*5].xpos=210;
	clock_ui[6+N*5].ypos=80+37*N;
  clock_ui[6+N*5].wideth=25;
	clock_ui[6+N*5].height=32;
	clock_ui[6+N*5].flag=0;
	clock_ui[6+N*5].handle=NULL;
	for(i=1;i<=7;i++)
	{
		if(clockkeep[N].week[i-1]==1)
		{
       clock_ui[4+5*N].title[k]=i+'0';
		  	k++;
		}
  }
//	{5,80,195,32,"00:00",0,Clock_Move},
//	{5,96,180,16,"title",0,NULL},
//	{70,80,115,16,"1234567",0,NULL},
//	{185,80,50,32,"关",0,NULL},


}

void Warn_Ui()
{
 Lable(20,100,GRAY,200,120);
 
 List_Input(90,120,"READY_MAX",16);
 
 Button(90,150,60, 50,GREEN,"OK");
 while(1)
 {
    if(TOUCH_Scan(1) == 0)
		{
				 if(TouchData.lcdx>90&&TouchData.lcdx<150&&TouchData.lcdy>150&&TouchData.lcdy<200)
				 {					
					 LCD_Clear(WHITE);
					 return ;
         }
		
	  }
 }


}

