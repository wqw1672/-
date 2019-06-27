#include "usart.h"
 QUEUE_T Queue_struct;

char a[100]={0};

int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

/*******************************************************************************
* 函 数 名         : USART1_Init
* 函数功能		   : USART1初始化函数
* 输    入         : bound:波特率
* 输    出         : 无
*******************************************************************************/ 
void USART1_Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //打开时钟
 
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //串口输出PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //串口输入PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */
	

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	

}

/*******************************************************************************
* 函 数 名         : USART1_IRQHandler
* 函数功能		   : USART1中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 r;
  char ch;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		r =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
	  
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

