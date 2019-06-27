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
extern void  (* Task[10])(); //���������
extern int current;  //��ǰ���������
extern int current_Now;
FileNameTypeDef filename[30];
uint8_t misicFile[20][40];
uint8_t misicFile_Show[20][40];
uint8_t Music_Number=0;
int File_First=0;
EVENT music_control[Music_Max]=
{
  {180,240,60,16,"��һ��",NULL},
  {60,260,60,16,"music +",NULL},
  {120,260,60,16,"music -",NULL},
  {20,240,60,16,"��һ��",NULL},
  {20,140,60,16,"����",NULL},
  {100,200,60,16,"STOP",NULL},
  {20,160,60,16,"������",NULL}
};
EVENT music_ui[Music_Ui_Max]=
{
	 {20,60,20,16,"<<",NULL},
   {20,80,20,16,"<<",NULL},
   {20,100,20,16,"<<",NULL},
   {10,130,45,20,"��һҳ",NULL},
   {75,130,20,20,"1",NULL},
   {105,130,20,20,"2",NULL},
   {135,130,20,20,"3",NULL},
   {165,130,20,20,"4",NULL},
   {195,130,45,20,"��һҳ",NULL},
   {10,160,30,20,"ȡ��",NULL},
};
uint8_t MP3_Volume;
void Music_Gui(u8 a);
/****************************************************************************
* Function Name  : MP3_Init
* Description    : ��ʼ��MP3
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

int8_t MP3_Init(void)
{
    uint16_t id;
	VS10XX_Config();        //GPIO��SPI��ʼ��
    id = VS10XX_RAM_Test();	
	if(id != 0x83FF)        //���洢�����ԵĽ��
	{	
		return 0xFF;
	}
	VS10XX_SineTest();     //���Ҳ���
	MP3_BaseSetting(0,0,0,0);               //��������
	MP3_EffectSetting(0);                   //��Ч����

	return 0;
}                                 

/****************************************************************************
* Function Name  : MP3_AudioSetting
* Description    : ���������Ĵ�С������SCI_VOL�Ĵ���
* Input          : vol�������Ĵ�С��0~0xFF��
* Output         : None
* Return         : None
****************************************************************************/

void MP3_AudioSetting(uint8_t vol)
{
	uint16_t volValue = 0;
	
	/* 0�����������0xFE����������8�ֽڿ�����ͨ������8�ֽڿ�����ͨ�� */
	vol = 254 - vol;
	volValue = vol | (vol << 8);

	VS10XX_WriteCmd(SCI_VOL, volValue);
}

/****************************************************************************
* Function Name  : MP3_BaseSetting
* Description    : �������ã�����SCI_BASS�Ĵ���
* Input          : amplitudeH����Ƶ���� 0~15(��λ:1.5dB,С��9��ʱ��Ϊ����)
*                * freqLimitH����Ƶ���� 2~15(��λ:10Hz)
*                * amplitudeL����Ƶ���� 0~15(��λ:1dB)
*                * freqLimitL����Ƶ���� 1~15(��λ:1Khz)
* Output         : None
* Return         : None
****************************************************************************/

void MP3_BaseSetting(
	uint8_t amplitudeH, uint8_t freqLimitH,
	uint8_t amplitudeL, uint8_t freqLimitL
)
{
	uint16_t bassValue = 0;
	
	/* ��Ƶ������12 ��15λ */
	bassValue = amplitudeH & 0x0F;
	bassValue <<= 4;
	
	/* Ƶ�������� 11 ��8λ */
	bassValue |= freqLimitL & 0x0F;
	bassValue <<= 4;
	
	/* ��Ƶ������ 7 ��4 λ */
	bassValue |= amplitudeL & 0x0F;
	bassValue <<= 4;
	
	/* Ƶ�������� 3 �� 0λ */
	bassValue |= freqLimitH & 0x0F;
	
	VS10XX_WriteCmd(SCI_BASS, bassValue); 
		
}

/****************************************************************************
* Function Name  : MP3_EffectSetting
* Description    : ��Ч���á�����SCI_MODE�Ĵ���
* Input          : effect��0,�ر�;1,��С;2,�е�;3,���
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
* Description    : ��ʾ����
* Input          : value����ʾ������
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

  LCD_ShowFont12Char(100,140,"��������Ϊ:");
  LCD_Fill(180,140,200,160, WHITE); //�����ʾλ��
	LCD_ShowString(180,140,tftlcd_data.width,tftlcd_data.height,16,showData);
}
/****************************************************************************
* Function Name  : MP3_PlaySong
* Description    : ����һ�׸���
* Input          : addr�����ŵ�ַ�͸������������ǵü�.mp3��׺��
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
LCD_ShowFont12Char(0,40,"���ڲ��ŵĸ���Ϊ:");
LCD_ShowFont12Char(40,70,misicFile_Show[judge]);		
Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"���ֲ���");
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
											/* ���͸�����Ϣ */
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
												LCD_Fill(0, 60,245, 100, WHITE); //�����ʾλ��
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
												LCD_Fill(0, 60,245, 100, WHITE); //�����ʾλ��
										  	LCD_ShowFont12Char(40,70,misicFile_Show[judge]);		
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,RED,music_control[i].title);
												delay_ms(100);
												Button(music_control[i].xpos,music_control[i].ypos,music_control[i].wideth, music_control[i].height,YELLOW,music_control[i].title);
												
											}
											else if(i==4)
											{
											 LCD_Clear(WHITE);	
												f_close(&file);  //�����Ǵ򿪣������½��ļ���һ���ǵùر�
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
												LCD_ShowFont12Char(0,40,"���ڲ��ŵĸ���Ϊ:");
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
* Description    : ��SD����������
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void Music_Mo()
{

	FATFS fs;
	uint32_t sdCapacity, free;
  uint8_t dat[9] = {"0:"}, i=0, j=0, k=0,k1=0,judge=0; //Ҫ��ʾ��ͼƬ���ļ���ַ
    
  LCD_Clear(WHITE);
  Lable(0,300,CYAN,245, 20);
	LCD_ShowFont12Char(100,302,"�ֿ�ѡ��");
  while(SD_Init()!=0)
	{	
		LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"SD Card Error!");
	}
			FATFS_Init();							//Ϊfatfs��ر��������ڴ�				 
			f_mount(&fs,"0:",1); 					//����SD�� 
	
	while(FATFS_GetFree("0:", &sdCapacity, &free) != 0)
    {
        LCD_ShowFont12Char(0, 21, "FATfs error!");   
    }
	
	/* �������� */
    MP3_Volume = 230;
    MP3_AudioSetting(MP3_Volume);
    /* ��ʼ����ʾ */
	
	/*  ɨ���ļ���ַ�������е��ļ� */
			if(File_First==0)	
			{
				FATFS_ScanFiles(dat, filename);
					printf("%s%s\r\n",filename[0].name,filename[0].type);
					printf("%s%s\r\n",filename[1].name,filename[1].type);
					while(i<15)
					{	
						
						/* �ж��Ƿ���PM3ͼƬ�ļ� */

									if((filename[i].type[1] == 'm') && (filename[i].type[2] == 'p') &&
												 (filename[i].type[3] == '3')) 
									{
										/* �����ļ�·��,������ļ�·�� */
										k = 0;
										k1=0;
										while(*(dat + k) != '\0')
										{
											*(misicFile[Music_Number] + k) = *(dat + k);
											k++;
										}
										
													/* ·��֮�����һб�� */
										*(misicFile[Music_Number] + k) = '/';
										k++;

													/* ����ļ��������� */
										j = 0;
										while(filename[i].name[j] != '\0')
										{
											*(misicFile[Music_Number] + k) = filename[i].name[j];
											*(misicFile_Show[Music_Number] + k1) = filename[i].name[j];
												
											k1++;
											k++;
											j++;	
										}

													/* ����ļ���׺ */
										j = 0;
										while(filename[i].type[j] != '\0')
										{
											*(misicFile[Music_Number] + k) = filename[i].type[j];
											*(misicFile_Show[Music_Number] + k1) = filename[i].type[j];
											k1++;
											k++;
											j++;	
										}

													/* �ļ�������һ���������� */
													*(misicFile[Music_Number] + k) = '\0';
													*(misicFile_Show[Music_Number] + k1) = '\0';

										/* ��ʾ���ŵĸ��������Ÿ��� */
												
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

								LCD_Fill(0, 40, tftlcd_data.height, 165, WHITE); //�����ʾλ��
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
																			LCD_Fill(0, 40, tftlcd_data.height, 165, WHITE); //�����ʾλ��
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
	FRONT_COLOR=RED;//��������Ϊ��ɫ
	LCD_Clear(WHITE);
  LCD_ShowFont12Char(10,40,"���");
  LCD_ShowFont12Char(100,40,"�ֿ�");
	a++;
	LCD_ShowxNum(0,60,a,2,16,1);
	a++;
	LCD_ShowxNum(0,80,a,2,16,1);
	a++;
	LCD_ShowxNum(0,100,a,2,16,1);
	
  Button(10,130,50,20,CYAN,"��һҳ");
  Button(80,130,20,20,CYAN,"1");
  Button(10,130,20,20,CYAN,"2");
  Button(10,130,20,20,CYAN,"");
  Button(10,130,50,20,CYAN,"��һҳ");

}
