#include "key_board.h"
int Input_N=0;
int Input_Now=-1;
extern void Get_Music();
extern void AlarmAdd();
EVENT	keyboard[Board_Max]=
{
	{1,200-20,24,25,"1",0,Input_Add},
	{25,200-20,24,25,"2",0,Input_Add},
	{49,200-20,24,25,"3",0,Input_Add},
	{73,200-20,24,25,"4",0,Input_Add},
	{97,200-20,24,25,"5",0,Input_Add},
	{121,200-20,24,25,"6",0,Input_Add},
	{145,200-20,24,25,"7",0,Input_Add},
	{169,200-20,24,25,"8",0,Input_Add},
	{193,200-20,24,25,"9",0,Input_Add},
	{217,200-20,24,25,"0",0,Input_Add},
	
	{1,225-20,24,25,"q",0,Input_Add},
	{25,225-20,24,25,"w",0,Input_Add},
	{49,225-20,24,25,"e",0,Input_Add},
	{73,225-20,24,25,"r",0,Input_Add},
	{97,225-20,24,25,"t",0,Input_Add},
	{121,225-20,24,25,"y",0,Input_Add},
	{145,225-20,24,25,"u",0,Input_Add},
	{169,225-20,24,25,"i",0,Input_Add},
	{193,225-20,24,25,"o",0,Input_Add},
	{217,225-20,24,25,"p",0,Input_Add},
	
	
	{1+12,250-20,24,25,"a",0,Input_Add},
	{25+12,250-20,24,25,"s",0,Input_Add},
	{49+12,250-20,24,25,"d",0,Input_Add},
	{73+12,250-20,24,25,"f",0,Input_Add},
	{97+12,250-20,24,25,"g",0,Input_Add},
	{121+12,250-20,24,25,"h",0,Input_Add},
	{145+12,250-20,24,25,"j",0,Input_Add},
	{169+12,250-20,24,25,"k",0,Input_Add},
	{193+12,250-20,24,25,"l",0,Input_Add},

	{30,275-20,24,25,"z",0,Input_Add},
	{54,275-20,24,25,"x",0,Input_Add},
	{78,275-20,24,25,"c",0,Input_Add},
	{102,275-20,24,25,"v",0,Input_Add},
	{126,275-20,24,25,"b",0,Input_Add},
	{150,275-20,24,25,"n",0,Input_Add},
	{174,275-20,24,25,"m",0,Input_Add},
  {198,275-20,45,25,"back",0,Input_Sub},
	{1,275-20,29,25,"cap",0,KeyBoard_Cap},
	
	{25,300-20,48,20,"sym",0,KeyBoard_Sym},
  {181,300-20,48,20,"enter",0,NULL},

};

INPUT KeyBoard_Input[Input_Max]={0};
	/****************************************************************************
* Function Name  : key_Board
* Description    : º¸≈ÃΩÁ√Ê
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void key_Board(EVENT event)
{
		u8 key;
    int i=0;
		TIM_Cmd(TIM4,ENABLE);
	memset(KeyBoard_Input,0,39*sizeof(INPUT));
 draw_ui(keyboard,Board_Max);
	Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"º¸≈Ã");
		while(1)
		{
					key=KEY_Scan(0);
					if(key==KEY_UP)
					{
						TOUCH_Adjust(); //??
						draw_ui(keyboard,Board_Max);
					}
					i=Key_Touch();
					if(i==1)
					{
						TIM_Cmd(TIM4,DISABLE);
						return ;
					}
		}
}
	
/****************************************************************************
* Function Name  : draw_ui
* Description    : ªÊª≠BUTTON
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void draw_ui(EVENT *keyboard,int len)
{
	int i=0;
	for(i=0;i<len;i++)
	{
	   Button(keyboard[i].xpos,keyboard[i].ypos,keyboard[i].wideth, keyboard[i].height,YELLOW,keyboard[i].title);
	}
}
int RunEvent_Count()
{
	int i=0,x=0,y=0;
	if(TOUCH_Scan()==1)
		return 1;
   if(TOUCH_Scan() == 0)
	 {
		  
			 for(i=0;i<Board_Max;i++)
			 {
       
						 if(TouchData.lcdx>keyboard[i].xpos&&TouchData.lcdx<(keyboard[i].xpos+keyboard[i].wideth)&&TouchData.lcdy>keyboard[i].ypos&&TouchData.lcdy<(keyboard[i].ypos+keyboard[i].height))
						 {
							Button(keyboard[i].xpos,keyboard[i].ypos,keyboard[i].wideth, keyboard[i].height,RED,keyboard[i].title);
							 delay_ms(100);
							Button(keyboard[i].xpos,keyboard[i].ypos,keyboard[i].wideth, keyboard[i].height,YELLOW,keyboard[i].title);
							 if(i==39)
								 return 1;
							 keyboard[i].handle(keyboard[i]);
								 break;
							}
			 }
			for(i=0;i<Input_Max;i++)
			 {
					if(TouchData.lcdx>KeyBoard_Input[i].xpos&&TouchData.lcdx<(KeyBoard_Input[i].xpos+KeyBoard_Input[i].wideth)&&TouchData.lcdy>KeyBoard_Input[i].ypos&&TouchData.lcdy<(KeyBoard_Input[i].ypos+KeyBoard_Input[i].height))
					{
						Input_Now=i;
							Show_Input();
						break;
					} 
			 }
	 }
	 return 0;

}

/****************************************************************************
* Function Name  : draw_ui
* Description    : ªÊª≠BUTTON
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void KeyBoard_Cap()
{
	char *a[40]={"Q","W","E","R","T","Y","U","I","O","P","A","S","D","F","G","H","J","K","L","Z","X","C","V","B","N","M"};
  char  *b[40]={"q","w","e","r","t","y","u","i","o","p","a","s","d","f","g","h","j","k","l","z","x","c","v","b","n","m"};
	int i=0;
  if(keyboard[36].flag==0)
	{
	   keyboard[36].flag=1;

	 for(i=0;i<26;i++)
		{
		 keyboard[i+10].title=a[i];
	   Button(keyboard[i+10].xpos,keyboard[i+10].ypos,keyboard[i+10].wideth, keyboard[i+10].height,CYAN,keyboard[i+10].title);
		}
		 
	}
	else if(keyboard[36].flag==1)
	{
		keyboard[36].flag=0;
		 for(i=0;i<26;i++)
		{
		 keyboard[i+10].title=b[i];
	   Button(keyboard[i+10].xpos,keyboard[i+10].ypos,keyboard[i+10].wideth, keyboard[i+10].height,CYAN,keyboard[i+10].title);
		}	
	}
}


/****************************************************************************
* Function Name  : Show_Input
* Description    :  ‰≥ˆ◊÷∑˚¥Æ
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Show_Input()
{
	int xpos=5,ypos=184-20,bettwen=10;
	int i=0,x=xpos,y=ypos;

	
	for(i=0;i<=Input_N/20;i++)
 		{
			LCD_Fill(x,y,240-20,y+16,YELLOW);
			y=y-16;
		}
		if(Input_N<=20)
	  LCD_Fill(x,y,240-20,y+16,WHITE);
	for(i=0;i<Input_N;i++)
	{
      if(i%20==0)
			{
				x=xpos;
				y=y+16;
			}
		  LCD_ShowChar1(x+1,y+1,KeyBoard_Input[i].title,16,1);
			KeyBoard_Input[i].xpos=x;
			KeyBoard_Input[i].ypos=y;
			KeyBoard_Input[i].wideth=bettwen;
			KeyBoard_Input[i].height=16;
			x=x+bettwen;
	}
}
/****************************************************************************
* Function Name  : Input_Add
* Description    : ‘ˆº”¥Ú”°µƒ◊÷∑˚
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Input_Add(EVENT keyboard,int number)
{
	 int i=0;
	 INPUT t1={0};
	 if(Input_N<Input_Max)
	 {  Input_N++;
		 	Input_Now++;
				for(i=Input_N;i>Input_Now;i--)
				{
					 KeyBoard_Input[i]=KeyBoard_Input[i-1];
				}
		
	     	KeyBoard_Input[Input_Now].title=*keyboard.title;

	 }

	 	Show_Input();
}

/****************************************************************************
* Function Name  : Input_Sub
* Description    : ºı…Ÿ¥Ú”°µƒ◊÷∑˚
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Input_Sub(int number)
{
	int i =0;
	
  INPUT t1={0};
	if(Input_N>0&&Input_Now>-1)
	{
	for(i=Input_Now;i<Input_N;i++)
	{
	  KeyBoard_Input[i]=KeyBoard_Input[i+1];
	}
	 KeyBoard_Input[Input_N-1]=t1;
   Input_N--;
		Input_Now--;
	
	Show_Input();
	}
}
/****************************************************************************
* Function Name  : KeyBoard_Sym
* Description    : º¸≈Ã«–ªª ˝◊÷”Î◊÷∑˚
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void KeyBoard_Sym()
{
  char *a[20]={"F1","F2","F3","F4",",",".","?","!",";","/"};
  char *b[20]={"1","2","3","4","5","6","7","8","9","0"};
	int i=0;
  if(keyboard[37].flag==0)
	{
	   keyboard[37].flag=1;

	 for(i=0;i<10;i++)
		{
		 keyboard[i].title=a[i];
		if(i==0)
		{
	  keyboard[i].handle=AlarmAdd;
		}
		if(i==1)
		{
	  keyboard[i].handle=Get_Music;
		}
	   Button(keyboard[i].xpos,keyboard[i].ypos,keyboard[i].wideth, keyboard[i].height,CYAN,keyboard[i].title);
		}
		 
	}
	else if(keyboard[37].flag==1)
	{
		keyboard[37].flag=0;
		 for(i=0;i<10;i++)
		{
				if(i==0)
				{
				keyboard[i].handle=Input_Add;
				}
				if(i==1)
				{
				keyboard[i].handle=Input_Add;
				}
		 keyboard[i].title=b[i];
	   Button(keyboard[i].xpos,keyboard[i].ypos,keyboard[i].wideth, keyboard[i].height,CYAN,keyboard[i].title);
		}	
	}
}
/****************************************************************************
* Function Name  : Key_Scanf
* Description    : ¥´ ‰º¸≈Ã…œ ‰»Îµƒƒ⁄»›
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Key_Scanf(char *a)
{
	int k=0;
	for(k=0;KeyBoard_Input[k].title!=0x00;k++)
	{ 
		*(a+k)=KeyBoard_Input[k].title;			
	}

}
int Key_Touch()
{
	int i=0;
	while(1)
	{
		i=RunEvent_Count();
		if(i==1)
			return 1;
	}
}
void Flash_Key()
{
   LCD_ShowChar1(KeyBoard_Input[Input_Now].xpos+5,KeyBoard_Input[Input_Now].ypos,'|',16,1);
}
void Disappear_Key()
{
  	LCD_Fill(KeyBoard_Input[Input_Now].xpos+8,KeyBoard_Input[Input_Now].ypos,KeyBoard_Input[Input_Now].xpos+9,KeyBoard_Input[Input_Now].ypos+16,YELLOW);
}
