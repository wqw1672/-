#include "mp3player.h"
#include "ff.h"
#include "vs10xx.h"
#include "spi.h"
#include "key.h"
#include "led.h"
#include "touch.h"
#include "sd.h"
#include "fatfs_app.h"
#define Music_Max 7
#define Music_Ui_Max 10
extern INPUT KeyBoard_Input;
extern void  (* Task[10])(); //任务的数组
extern int current;  //当前任务的数量
extern int current_Now;
FileNameTypeDef filename[30];
uint8_t misicFile[20][40];
uint8_t misicFile_Show[20][40];
uint8_t Music_Number=0;
int File_First=0;
EVENT music_control[Music_Max]=
{
  {180,240,60,16,"下一首",NULL},
  {60,260,60,16,"music +",NULL},
  {120,260,60,16,"music -",NULL},
  {20,240,60,16,"上一首",NULL},
  {20,140,60,16,"返回",NULL},
  {100,200,60,16,"STOP",NULL},
  {20,160,60,16,"重命名",NULL}
};
EVENT music_ui[Music_Ui_Max]=
{
	 {20,60,20,16,"<<",NULL},
   {20,80,20,16,"<<",NULL},
   {20,100,20,16,"<<",NULL},
   {10,130,45,20,"上一页",NULL},
   {75,130,20,20,"1",NULL},
   {105,130,20,20,"2",NULL},
   {135,130,20,20,"3",NULL},
   {165,130,20,20,"4",NULL},
   {195,130,45,20,"下一页",NULL},
   {10,160,30,20,"取消",NULL},
};
uint8_t MP3_Volume;
void Music_Gui(u8 a);
/****************************************************************************
* Function Name  : MP3_Init
* Description    : 初始化MP3
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

int8_t MP3_Init(void)
{
    uint16_t id;
	VS10XX_Config();        //GPIO和SPI初始化
    id = VS10XX_RAM_Test();	
	if(id != 0x83FF)        //检测存储器测试的结果
	{	
		return 0xFF;
	}
	VS10XX_SineTest();     //正弦测试
	MP3_BaseSetting(0,0,0,0);               //基本设置
	MP3_EffectSetting(0);                   //音效设置

	return 0;
}                                 

/****************************************************************************
* Function Name  : MP3_AudioSetting
* Description    : 设置声音的大小，设置SCI_VOL寄存器
* Input          : vol：声音的大小（0~0xFF）
* Output         : None
* Return         : None
****************************************************************************/

void MP3_AudioSetting(uint8_t vol)
{
	uint16_t volValue = 0;
	
	/* 0是最大音量，0xFE是无声，低8字节控制右通道，高8字节控制左通道 */
	vol = 254 - vol;
	volValue = vol | (vol << 8);

	VS10XX_WriteCmd(SCI_VOL, volValue);
}

/****************************************************************************
* Function Name  : MP3_BaseSetting
* Description    : 基本设置：设置SCI_BASS寄存器
* Input          : amplitudeH：高频增益 0~15(单位:1.5dB,小于9的时候为负数)
*                * freqLimitH：高频上限 2~15(单位:10Hz)
*                * amplitudeL：低频增益 0~15(单位:1dB)
*                * freqLimitL：低频下限 1~15(单位:1Khz)
* Output         : None
* Return         : None
****************************************************************************/

void MP3_BaseSetting(
	uint8_t amplitudeH, uint8_t freqLimitH,
	uint8_t amplitudeL, uint8_t freqLimitL
)
{
	uint16_t bassValue = 0;
	
	/* 高频增益是12 ：15位 */
	bassValue = amplitudeH & 0x0F;
	bassValue <<= 4;
	
	/* 频率下限是 11 ：8位 */
	bassValue |= freqLimitL & 0x0F;
	bassValue <<= 4;
	
	/* 低频增益是 7 ：4 位 */
	bassValue |= amplitudeL & 0x0F;
	bassValue <<= 4;
	
	/* 频率上限是 3 ： 0位 */
	bassValue |= freqLimitH & 0x0F;
	
	VS10XX_WriteCmd(SCI_BASS, bassValue); 
		
}

/****************************************************************************
* Function Name  : MP3_EffectSetting
* Description    : 音效设置。设置SCI_MODE寄存器
* Input          : effect：0,关闭;1,最小;2,中等;3,最大。
* Output         : None
* Return         : None
****************************************************************************/

void MP3_EffectSetting(uint8_t effect)
{
	uint16_t effectValue;

	effectValue = VS10XX_ReadData(SCI_MODE);
	if(effect & 0x01)
	{
		effectValue |= 1 << 4;	
	}
	else
	{
		effectValue &= ~(1 << 5);
	}
	if(effect & 0x02)
	{
		effectValue |= 1 << 7;
	}
	else
	{
		effectValue &= ~(1 << 7);
	}

	VS10XX_WriteCmd(SCI_MODE, effectValue);
}

/****************************************************************************
* Function Name  : MP3_ShowVolume
* Description    : 显示音量
* Input          : value：显示的音量
* Output         : None
* Return         : None
****************************************************************************/

void MP3_ShowVolume(uint8_t value)
{
    uint8_t showData[4] = {0, 0, 0, 0};

    showData[0] = value % 1000 / 100 + '0';
    showData[1] = value % 100 / 10 + '0';
    showData[2] = value % 10 + '0';

    FRONT_COLOR=RED;

  LCD_ShowFont12Char(100,140,"现在音量为:");
  LCD_Fill(180,140,200,160, WHITE); //清除显示位置
	LCD_ShowString(180,140,tftlcd_data.width,tftlcd_data.height,16,showData);
}
/****************************************************************************
* Function Name  : MP3_PlaySong
* Description    : 播放一首歌曲
* Input          : addr：播放地址和歌名（歌曲名记得加.mp3后缀）
* Output         : None
* Return         : None
****************************************************************************/
void MP3_PlaySong(u8 Number)
{
	FIL file;
	UINT br;
	int i=0;
	int judge=Number;
	int stop=0;
	char keep[50];
	FRESULT res;
	uint8_t musicBuff[512];
	uint16_t k;

    /*open file*/
	res = f_open(&file, (const TCHAR*)misicFile[judge], FA_READ);
 	led1=0;
	VS10XX_SoftReset();
	led2=0;
  LCD_Clear(WHITE);
LCD_ShowFont12Char(0,40,"现在播放的歌曲为:");
LCD_ShowFont12Char(40,70,misicFile_Show[judge]);		
Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"音乐播放");
  draw_ui(music_control,Music_Max);
	
	if(res == FR_OK)
	{
		
		while(1)
		{
			if(stop==0)
			{
						res = f_read(&file, musicBuff, sizeof(musicBuff), &br);
						k = 0;

						do
						{	
											/* 发送歌曲信息 */
											if(VS10XX_SendMusicData(musicBuff+k) == 0)
											{
													k += 32;
											}
					
											 
						}
						while(k < br);
		 }
			 if(TOUCH_Scan(1)==1)
			 return ;
			 if(TOUCH_Scan() == 0)
					{
		
							for(i=0;i<Music_Max;i++)
								{
										 if(TouchData.lcdx>music_control[i].xpos&&TouchData.lcdx<(music_control[i].xpos+music_control[i].wideth)
													&&TouchData.lcdy>music_control[i].ypos&&TouchData.lcdy<(music_control[i].ypos+music_control[i].height))
										 {
												if(i==1)
												{
													MP3_Volume+=10;
													MP3_AudioSetting(MP3_Volume);
													MP3_ShowVolume(MP3_Volume);
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,RED,music_control[i].title);
												delay_ms(100);
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,YELLOW,music_control[i].title);
                      

													
												}
			                 else if(i==2)
												{												
													MP3_Volume-=10;
													MP3_AudioSetting(MP3_Volume);
													MP3_ShowVolume(MP3_Volume);
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,RED,music_control[i].title);
												delay_ms(100);
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,YELLOW,music_control[i].title);
                      
												}
											 else if(i==3&&judge>0)
											 {
												judge--;
												res = f_open(&file, (const TCHAR*)misicFile[judge], FA_READ);
												 memset(musicBuff,0,512);
												led1=0;
												VS10XX_SoftReset();
												led2=0;
												LCD_Fill(0, 60,245, 100, WHITE); //清除显示位置
												LCD_ShowFont12Char(40,70,misicFile_Show[judge]);		
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,RED,music_control[i].title);
												delay_ms(100);
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,YELLOW,music_control[i].title);
                       }
											else if(i==0&&misicFile_Show[judge+1][1]!=0x00)
											{									
												judge++;
												res = f_open(&file, (const TCHAR*)misicFile[judge], FA_READ);
												 memset(musicBuff,0,512);
												led1=0;
												VS10XX_SoftReset();
												led2=0;
												LCD_Fill(0, 60,245, 100, WHITE); //清除显示位置
										  	LCD_ShowFont12Char(40,70,misicFile_Show[judge]);		
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,RED,music_control[i].title);
												delay_ms(100);
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,YELLOW,music_control[i].title);
												
											}
											else if(i==4)
											{
											 LCD_Clear(WHITE);	
												f_close(&file);  //不论是打开，还是新建文件，一定记得关闭
												return ;
											}
											else if(i==5)
											{
												if(stop==0)
												{
													stop=1;
													Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,RED,music_control[i].title);
												}
												else
												{
													stop=0;
												  Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,YELLOW,music_control[i].title);

												}
											
												delay_ms(100);
												
                      }
											else if(i==6)
											{
                        key_Board();
	                      memset(misicFile_Show[judge],0,40);
                        Key_Scanf(misicFile_Show[judge],KeyBoard_Input);
											  LCD_Clear(WHITE);
												LCD_ShowFont12Char(0,40,"现在播放的歌曲为:");
												LCD_ShowFont12Char(40,70,misicFile_Show[judge]);		

												draw_ui(music_control,Music_Max);	
                      }
										}
								}
				}
	
				
		}
		
	}
}

/****************************************************************************
* Function Name  : Music_Mo
* Description    : 将SD卡歌曲传入
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Music_Mo()
{

	FATFS fs;
	uint32_t sdCapacity, free;
  uint8_t dat[9] = {"0:"}, i=0, j=0, k=0,k1=0,judge=0; //要显示的图片的文件地址
    
  LCD_Clear(WHITE);
  Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"乐库选择");
  while(SD_Init()!=0)
	{	
		LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"SD Card Error!");
	}
			FATFS_Init();							//为fatfs相关变量申请内存				 
			f_mount(&fs,"0:",1); 					//挂载SD卡 
	
	while(FATFS_GetFree("0:", &sdCapacity, &free) != 0)
    {
        LCD_ShowFont12Char(0, 21, "FATfs error!");   
    }
	
	/* 设置音量 */
    MP3_Volume = 230;
    MP3_AudioSetting(MP3_Volume);
    /* 初始化显示 */
	
	/*  扫描文件地址里面所有的文件 */
			if(File_First==0)	
			{
				FATFS_ScanFiles(dat, filename);
					printf("%s%s\r\n",filename[0].name,filename[0].type);
					printf("%s%s\r\n",filename[1].name,filename[1].type);
					while(i<15)
					{	
						
						/* 判断是否是PM3图片文件 */

									if((filename[i].type[1] == 'm') && (filename[i].type[2] == 'p') &&
												 (filename[i].type[3] == '3')) 
									{
										/* 处理文件路径,先添加文件路径 */
										k = 0;
										k1=0;
										while(*(dat + k) != '\0')
										{
											*(misicFile[Music_Number] + k) = *(dat + k);
											k++;
										}
										
													/* 路径之后加上一斜杠 */
										*(misicFile[Music_Number] + k) = '/';
										k++;

													/* 添加文件名字名字 */
										j = 0;
										while(filename[i].name[j] != '\0')
										{
											*(misicFile[Music_Number] + k) = filename[i].name[j];
											*(misicFile_Show[Music_Number] + k1) = filename[i].name[j];
												
											k1++;
											k++;
											j++;	
										}

													/* 添加文件后缀 */
										j = 0;
										while(filename[i].type[j] != '\0')
										{
											*(misicFile[Music_Number] + k) = filename[i].type[j];
											*(misicFile_Show[Music_Number] + k1) = filename[i].type[j];
											k1++;
											k++;
											j++;	
										}

													/* 文件最后添加一个结束符号 */
													*(misicFile[Music_Number] + k) = '\0';
													*(misicFile_Show[Music_Number] + k1) = '\0';

										/* 显示播放的歌曲并播放歌曲 */
												
									}
								if(misicFile_Show[Music_Number][1]!=0x00)
											 Music_Number++;
								i++; 
								
							}
//							if(i > 10)
//							{
//									i = 0;
//							}	
							File_First=1;
							return ;
	           }
					
							j=0;

								LCD_Fill(0, 40, tftlcd_data.height, 165, WHITE); //清除显示位置
								Music_Gui(j/3);
								draw_ui(music_ui,Music_Ui_Max);
								for(i=0;i<3;j++,i++)
									{

									LCD_ShowFont12Char(40,music_ui[i].ypos, misicFile_Show[j]);			
			//					MP3_PlaySong(misicFile[music_number]);
									}
									while(1)
									{
                         if(TOUCH_Scan() == 0)
												{

	
														for(i=0;i<Music_Ui_Max;i++)
																{
																	 if(TouchData.lcdx>music_ui[i].xpos&&TouchData.lcdx<(music_ui[i].xpos+music_ui[i].wideth)
																				&&TouchData.lcdy>music_ui[i].ypos&&TouchData.lcdy<(music_ui[i].ypos+music_ui[i].height))
																	 {
																		 
																		 if(i==0||i==1|i==2)
																		 {
																			Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,RED,music_ui[i].title);
																			delay_ms(100);
																			Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,YELLOW,music_ui[i].title);
							                       MP3_PlaySong(j+i-3);

																		 LCD_Clear(WHITE);
                                     }
                                      else if(i==3)  
																			{
																				if(j>4)
                                         j-=3;
																			 Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,RED,music_ui[i].title);
                                         delay_ms(100);
																			   Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,YELLOW,music_ui[i].title);
                                      }
																			else if(i==4||i==5||i==6||i==7)
																			{
                                         j=(i-3)*3;
																			   Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,RED,music_ui[i].title);
                                         delay_ms(100);
																			   Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,YELLOW,music_ui[i].title);

																			}
																			else if(i==8)
																			{
																				if(j<12)
                                         j+=3;
																		     Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,RED,music_ui[i].title);
                                         delay_ms(100);
																			   Button(music_ui[i].xpos,music_ui[i].ypos,music_ui[i].wideth, music_ui[i].height,YELLOW,music_ui[i].title);
																			}
																			else if(i==9)
																			{
																				  LCD_Clear(WHITE);	
                                         return ;
                                      }
																		}
																	 
																 }
																 j-=3;
																			LCD_Fill(0, 40, tftlcd_data.height, 165, WHITE); //清除显示位置
																			Music_Gui(j/3);
																			draw_ui(music_ui,Music_Ui_Max);
																			for(i=0;i<3;j++,i++)
																				{

																				LCD_ShowFont12Char(40,music_ui[i].ypos, misicFile_Show[j]);			
														//					MP3_PlaySong(misicFile[music_number]);
																				}
													}
						       }
				}


void Music_Gui(u8 a)
{
	a*=3;
	FRONT_COLOR=RED;//设置字体为红色
	LCD_Clear(WHITE);
  LCD_ShowFont12Char(10,40,"序号");
  LCD_ShowFont12Char(100,40,"乐库");
	a++;
	LCD_ShowxNum(0,60,a,2,16,1);
	a++;
	LCD_ShowxNum(0,80,a,2,16,1);
	a++;
	LCD_ShowxNum(0,100,a,2,16,1);
	
  Button(10,130,50,20,CYAN,"上一页");
  Button(80,130,20,20,CYAN,"1");
  Button(10,130,20,20,CYAN,"2");
  Button(10,130,20,20,CYAN,"");
  Button(10,130,50,20,CYAN,"上一页");

}
