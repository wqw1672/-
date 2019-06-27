#include "time.h"
int max=0;
struct TIMER_TS timeSoftEvents[MAX_SOFT_TIMER ];
/*******************************************************************************
* �� �� ��         : TIM4_Init
* ��������		   : TIM4��ʼ������
* ��    ��         : per:��װ��ֵ
					 psc:��Ƶϵ��
* ��    ��         : ��
*******************************************************************************/
void TIM4_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//ʹ��TIM4ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period=per;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //������ʱ���ж�
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;//��ʱ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM4,ENABLE); //ʹ�ܶ�ʱ��	
}

/*******************************************************************************
* �� �� ��         : TIM4_IRQHandler
* ��������		   : TIM4�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
	{
       SOFT_TIME_Dispaly();
	}
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	
}


void SOFT_TIME_Init(void)
{
	int i;
	for(i=0; i<MAX_SOFT_TIMER; i++)
	{
	  timeSoftEvents[i].count=0;
		timeSoftEvents[i].curCount=0;
		timeSoftEvents[i].isOpen=OPEN;
		timeSoftEvents[i].pHandle=NULL;
	}
}
//??????
int TIME_Events_Add(int count, void (*pHandle)())
{
	int i = 0;
	if(count<MAX_SOFT_TIMER)
	{
		for(i=0;i<MAX_SOFT_TIMER;i++)
		{
			if(timeSoftEvents[i].isOpen==OPEN)
			{
							timeSoftEvents[i].pHandle=pHandle;
							timeSoftEvents[i].isOpen=CLOSE;
							timeSoftEvents[i].count=count;
							break;
			}

		}
	}
}
//???????
void SOFT_TIME_Dispaly(void)
{
	int i=0;
	for(i=0;i<MAX_SOFT_TIMER;i++)
	{
     if(timeSoftEvents[i].isOpen==CLOSE)
		 {
			 if(timeSoftEvents[i].count==timeSoftEvents[i].curCount)
			  timeSoftEvents[i].pHandle();
			 
   	 }
	 }
	if(max>=2)
	{
	   max=0;
	}
	else
	{ 
		max++;
	}
	
		   for(i=0;i<MAX_SOFT_TIMER;i++)
		{
		   timeSoftEvents[i].curCount=max;
		}
}

