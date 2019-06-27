#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "key.h"
#include "time.h"
#include "touch.h"
#include "rtc.h"
#include "exti.h"
#include "alarmclock.h"
#include "font_show.h"
#include "malloc.h" 
#include "sd.h"
#include "flash.h"
#include "ff.h" 
#include "fatfs_app.h"
#include "mp3player.h"
#include "tftlcd.h"
#include "Led_Control.h"
#include "opencheck.h"
#include "dht11.h"
void Get_Time();
void Set_Time();
void Read_Mem();
void Write_Mem();
void Reset_back();
void Get_Music();
void AlarmAdd();
void Usart_ResetSystem();
//#include "opencheck.h"
#define Desk_APP_Max 8
extern EVENT ALARMCLOCK[Clock_Max];
int current = 0;  //当前任务的数量
int current_Now=0;
u8 keep_Count_Max=0;
void  (* Task[10])() ={0}; //任务的数组
CLOCK_KEEP clockkeep[5]={0};
char *show=0;
extern EVENT clock_ui[Clock_ui_Max];
extern char a[100];
void Led_On();
void Here();
void Led_Off();
EVENT Desk_App[Desk_APP_Max]=
{
{30,45,30,30,"0",0,Alarm_Clock},
{0,75,75,16,"AlarmClock",0,NULL},
{110,45, 30, 30,"0",0,Music_Mo},
{85,75,75,16,"Music",0,NULL},
{190,45, 30, 30,"0",0,Led_Control},
{175,75, 75, 16,"Light",0,NULL},
{15,95,210,50,"nothing",0,NULL},
{90,150,60,20,"Keyboard",0,key_Board},
};
void starcheck();
void TaskPro();
void All_Init();
void homepro();

int main()
{
		All_Init();



	starcheck(); //开机检测
	TOUCH_Adjust();
while(1)
{

		TaskPro();

}


}

void starcheck()
{
  Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"OpenCheck");
   Boot_Up();

	
	Task[current]=homepro;
  current++;
	current_Now++;
	 LCD_Clear(WHITE);

}


///////////////////////////
/****************************************************************************
* Function Name  : TaskPro
* Description    : 任务调度
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void TaskPro()
{
	
		if(Task[current-1]!=NULL)
		{
			Task[current-1]();
					if(current!=1)
					{
							current --;
					  	current_Now--;
					}	
			delay_ms(500);

	}


}


/****************************************************************************
* Function Name  : All_Init
* Description    : 所有的初始化
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void All_Init()
{
	int i=0;
	SysTick_Init(72);
	BEEP_Init();
	TFTLCD_Init();
	LED_Init();
	IRQ_EVENT_INIT();
	TOUCH_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //??????? ?2?
	LED_Init();
	USART1_Init(9600);
	TFTLCD_Init();			//LCD???
	SOFT_TIME_Init();
	KEY_Init();
  TIM4_Init(500,36000-1);	
  SOFT_TIME_Init();
	
	EN25QXX_Init();				//初始化EN25Q128	  
	
	MP3_Init();
	Music_Mo();
	my_mem_init(SRAMIN);		//初始化内部内存池
		//初始化EN25Q128	  



	FRONT_COLOR=RED;//设置字体为红色 
	DHT11_Init();
	uartcmd_init();
	uartcmd_install(CMD_GetTime,"GetTime",Get_Time);
	uartcmd_install(CMD_SetTime,"SetTime",Set_Time);
	uartcmd_install(ReadMem,"ReadMem",Read_Mem);
	uartcmd_install(CMD_Here,"Here",Here);
	uartcmd_install(WriteMem,"WriteMem",Write_Mem);
	uartcmd_install(CMD_OnLed,"LedOn",Led_On);
	uartcmd_install(CMD_OffLed,"LedOff",Led_Off);
	uartcmd_install(CMD_Reset,"Reset",Reset_back);
	uartcmd_install(CMD_GiveTime,"GiveTime",Get_Time);
	uartcmd_install(CMD_Music,"MusicPlay",Get_Music);
	uartcmd_install(CMD_Alarm,"AlarmAdd",AlarmAdd);
	uartcmd_install(CMD_Reset,"Reset",Usart_ResetSystem);

  TIME_Events_Add(0,Flash_Key);
  TIME_Events_Add(1,Disappear_Key);
	show=(char *)malloc(30*sizeof(char *));
	memset(show,0,30);
	
	
//	clockkeep[0].title=(char *)malloc(10*sizeof(char));
//	memset(&clockkeep[0],0,10*sizeof(char));
	for(i=0;i<5;i++)
	{

		
		 	 STM32_FLASH_Read(0x8060000+i*sizeof(CLOCK_KEEP),(u16 *)&clockkeep[i],sizeof(CLOCK_KEEP)/2);
				if(clockkeep[i].flag==0)
			          break;
       clock_ui_Init(i);
		 
//			memset(clock_ui[2+4*i].title,0,10*sizeof(char));   
		  
		
		  sprintf(clock_ui[2+5*keep_Count_Max].title,"%d:%d",clockkeep[keep_Count_Max].hour,clockkeep[keep_Count_Max].mini);
//		sprintf(clock_ui[3+5*keep_Count_Max].title,"title:%s",clockkeep[keep_Count_Max].title);

//			if(&clock_ui[3+4*i]==NULL)
//			{
//				sprintf(clock_ui[3+4*i].title,"%s",p->title);
//			}
//			else
//				{
//						memset(clock_ui[2+4*i].title,0,10);           
//						memset(clock_ui[3+4*i].title,0,20);           

//					sprintf(clock_ui[2+4*i].title,"%d:%d",clockkeep[i].hour,clockkeep[i].mini);
//					sprintf(clock_ui[3+4*i].title,"title:%s",clockkeep[i].title);

					keep_Count_Max++;
////					 }
	 }
	 
	 
	 
	 	 LCD_Clear(WHITE);

	 
}

/****************************************************************************
* Function Name  : homepro
* Description    : 桌面
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void homepro()
{
int i =0,maybe=0;
LCD_Clear(WHITE);
draw_ui(Desk_App,Desk_APP_Max);

LCD_ShowPicture(30,45,30,30,(u8*)RES_timg_BIN);
LCD_ShowPicture(110,45, 30, 30,(u8*)RES_music_BIN);
LCD_ShowPicture(190,45, 30, 30,(u8*)RES_light_BIN);
Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"桌面");

	while(1)
	{ 
		maybe=TOUCH_Scan(1);
		if(maybe==1)
		    return ;
		if(maybe == 0)
		{
			for(i=0;i<Desk_APP_Max;i++)
			{
						 if(TouchData.lcdx>Desk_App[i].xpos&&TouchData.lcdx<(Desk_App[i].xpos+Desk_App[i].wideth)&&TouchData.lcdy>Desk_App[i].ypos&&TouchData.lcdy<(Desk_App[i].ypos+Desk_App[i].height))
						 {
							 if(Desk_App[i].handle==NULL)
							 {
							    continue;
							 }
							Task[current]=Desk_App[i].handle;
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
* Function Name  : Get_Time
* Description    : 获取时间
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Get_Time()
{
	printf("\n%d-%d-%d %d:%d:%d ",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
  sprintf(show,"获取时间 %d-%d-%d %d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
  Desk_App[6].title=show;
	if(current==1)
	Button(15,95,210, 50,YELLOW,show);	
}
void Set_Time()
{
   int i= 0;
	 char s[4]={0};
  int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	 for(i=0;i<40;i++)
	{
     if(i>=8&&i<=11)
		 {
		   s[i-8]=a[i];
			 if(i==11)
			 {
				 year=atoi(s);
				 memset(s,0,4);
			 }
		 }
		 
		 else if(i>=13&&i<=14)
		 {
		   s[i-13]=a[i];
				if(i==14)
			 {
				 mon=atoi(s);
				 memset(s,0,4);
			 }
		 }
		 else if(i>=16&&i<=17)
		 {
		   s[i-16]=a[i];
				if(i==17)
			 {
				 day=atoi(s);
				 memset(s,0,4);
			 }
		 }
		 else if(i>=19&&i<=20)
		 {
		   s[i-19]=a[i];
				if(i==20)
			 {
				 hour=atoi(s);
				 memset(s,0,4);
			 }
		 }
		 else if(i>=22&&i<=23)
		 {
		   s[i-22]=a[i];
				if(i==23)
			 {
				 min=atoi(s);
				 memset(s,0,4);
			 }
		 }
		 else if(i>=25&&i<=26)
		 {
		  s[i-25]=a[i];
				if(i==26)
			 {
				 sec=atoi(s);
				 memset(s,0,4);
			 }
		 }
	}
	
	RTC_Set(year,mon,day,hour,min,sec);
	sprintf(show,"设置时间 %d-%d-%d %d:%d:%d",year,mon,day,hour,min,sec);
	Desk_App[6].title=show;
	if(current==1)
	Button(15,95,210, 50,YELLOW,show);	
}
  char p[100]={0};
	
	/****************************************************************************
* Function Name  : Read_Mem
* Description    : 串口读取内存
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Read_Mem()
{
	   u32 address1=0;
		 char s[9]={0};
		 char len[3]={0};
     int i=0,length=0;
     memset(p,0,100);
		 for(i=8;i<=16;i++)
		 {
		     s[i-8]=a[i];
		 }
		 sscanf(s,"%p",&address1);
		 
		 for(i=19;a[i]!=NULL;i++)
		 {
		     len[i-19]=a[i];
		 }
		 length=atoi(len);
   	 STM32_FLASH_Read(address1,(u16 *)p,length);
		 for(i=0;i<length;i++)
		 {
			  if(i%8==0&&i!=0)
				 printf("\n");
       if(i%8==0&&i!=0)
				 printf(" address: ");
			 if(i==0)
				 printf("\n address: ");

			 printf("%02x ",p[i]);
		 }
		 	sprintf(show,"从%s地址,读取%d个字节",s,length);
		 	Desk_App[6].title=show;
			if(current==1)
			Button(15,95,210, 50,YELLOW,show);
}


	/****************************************************************************
* Function Name  : Write_Mem
* Description    : 串口输入内存
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Write_Mem()
{
	   u32 address=0;
     char add[9]={0};
     u8 keep[50]={0};
     int i=0,length=0,j=0;
     char b[20]={0};
		 for(i=9;i<=17;i++)
		 {
		     add[i-9]=a[i];
		 }
		 sscanf(add,"%p",&address);
		 for(i=19;a[i]!=NULL;i++)
		 {
      if(i==19&&a[i]!='"')
			{
				printf("\n您输入格式有误!\n");
				return ;
			}
			if(a[i+1]==NULL&&a[i]!='"')
			{ 
        printf("\n您输入格式有误!\n");
				return ;
      }
			if((i-19)%3==0&&a[i]!=' '&&i!=19&&a[i+1]!=NULL)
			{ 
				printf("\n您输入格式有误!\n");
				return ;
			}
			if(a[i]!='"'&&a[i]!=' '&&a[i]!=NULL)
			{
           b[j]=a[i];
				   j++;
       }
//			if(a[i+1]==NULL&&j==0)
//			 {
//				 a[i+1]='/0';
//				 j=1;
//			 }
			 


//	   keep[i-19]=GetLow(keep[i-19]);
		 }
		      b[j]='0';
			 j++;
		 for(i=0;i<j;i++)
		 {
			 if((i+1)%2==0)
         sscanf(&b[i-1],"%02x",&(keep[i/2]));
     }
//		 if((i-19)%2==1)
//		 {
//			 a[i+1]='0';
//			 sscanf(&a[i],"%02x",&(keep[(i-19)/2]));
//		 }
		 length=j/2;
//		 keep[0]=0x12;
	   sprintf(show,"从%s地址,写入%d个字节",add,length);
		 	Desk_App[6].title=show;
			if(current==1)
			Button(15,95,210, 50,YELLOW,show);
      STM32_FLASH_Write(address,(u16 *)keep,length);
}
	/****************************************************************************
* Function Name  : Led_On
* Description    : 开灯
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

void Led_On()
{
	int i=0;
	for(i=0;i<7;i++)
    PCout(i)=0;
}

	/****************************************************************************
* Function Name  : Led_Off
* Description    : 关灯
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Led_Off()
{
   	int i=0;
	for(i=0;i<7;i++)
    PCout(i)=1;
}
	/****************************************************************************
* Function Name  : Here
* Description    : 一级指令
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Here()
{
  int i=0;
	while(i<=300)
	{
    beep=1;
    delay_us(100);
		beep=0;
		delay_us(100);
		i++;
   }

}
	/****************************************************************************
* Function Name  : Reset_back
* Description    : 复位
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Reset_back()
{
  LCD_Clear(WHITE);
	__set_FAULTMASK(1);
	 NVIC_SystemReset();
	return;

}

	/****************************************************************************
* Function Name  : Get_Music
* Description    : 跳转到音乐界面
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Get_Music()
{								
   	LCD_Clear(WHITE);
		Task[current]=Music_Mo;;
			   	current+=1;  
			current_Now+=2;
		TIM_Cmd(TIM4,DISABLE);
								 LCD_Clear(WHITE);

}
	/****************************************************************************
* Function Name  : AlarmAdd
* Description    : 跳转到闹钟界面
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void AlarmAdd()
{
	   LCD_Clear(WHITE);
  		Task[current]=Alarm_Clock;;
			   	current+=1;  
			current_Now+=2;
			TIM_Cmd(TIM4,DISABLE);
		 LCD_Clear(WHITE);

}
void Usart_ResetSystem()
{	
	NVIC_SystemReset();
}
