#include "usart.h"
 QUEUE_T Queue_struct;

char a[100]={0};

int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

/*******************************************************************************
* �� �� ��         : USART1_Init
* ��������		   : USART1��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/ 
void USART1_Init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //��ʱ��
 
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //�������PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //��������PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	

}

/*******************************************************************************
* �� �� ��         : USART1_IRQHandler
* ��������		   : USART1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 r;
  char ch;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		r =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
	  
		queue_datain(&Queue_struct,r);
//	  ch=queue_dataout(&Queue_struct);
		USART_SendData(USART1,r);
	}
	USART_ClearFlag(USART1,USART_FLAG_TC);


} 	

void queue_init(QUEUE_T *pqueue,int len)
{
	pqueue->head=0; 
	pqueue->tail =0; 
	pqueue->queue_len=len;
	pqueue->queue_size =0; 
	pqueue->pdata = (char*)malloc(len);
	memset(pqueue->pdata,0,len);
}
void queue_datain(QUEUE_T *pqueue,char data)
{
	 if(pqueue->queue_len<pqueue->tail)
	{
		pqueue->tail=0;
	}

	pqueue->pdata[(pqueue->tail)]=data;
	pqueue->tail++;
	pqueue->queue_size++;

}
char queue_dataout(QUEUE_T *pqueue)
{
  char temp;
	if(pqueue->head>pqueue->queue_len)
	{
	   pqueue->head=0;
	}
	temp=pqueue->pdata[pqueue->head];
	pqueue->pdata[pqueue->head]=0;
	pqueue->head++;
  if(temp==NULL)
		return NULL;

	return temp;
}
void Analysis()
{
 char ch;
 static int i=0;
 static int flag=0;
 if(Queue_struct.head!=Queue_struct.tail)
 {
	 ch=queue_dataout(&Queue_struct);
   if(ch=='<')
	 {
			 flag=1;
	 }
  else if(ch=='>'&&flag==1)
	 {
	   flag=2;
 	 }
	 else if(flag==1)
	 {
		 a[i]=ch;
		 i++;
	 }
	 
	 if(flag==2)
	 {
		 
	   uartcmd_run(a);
     i=0;
		 memset(a,0,40);
		 flag=0;
	 }
 }
}
CMD_T cmds[10];

void uartcmd_init()
{
  USART1_Init(9600);
	memset(cmds,0,10);
  queue_init(&Queue_struct,40);

}

void uartcmd_install(CMD_TYPE  cmd,char *cmdstr,cmdfun  fun)
{
	cmds[cmd].cmd = cmd;
	cmds[cmd].cmdstr = cmdstr;
	cmds[cmd].fun = fun;
	
}


void uartcmd_unstall(CMD_TYPE  cmd)
{
	cmds[cmd].fun = NULL;
}

//??
void uartcmd_run(char *cmdstr)
{
	int i=0;
	for(i=0;i<CMD_MAX;i++)
	{
		if(cmds[i].fun != NULL)
		{
				if(strstr(cmdstr,cmds[i].cmdstr)!=NULL)
				{
					cmds[i].fun();
				}
		}	
	}
	
}

