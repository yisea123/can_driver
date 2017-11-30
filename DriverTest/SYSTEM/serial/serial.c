#include "serial.h"
#include "control.h"
#include "can.h" 
/************************************************
 
 创建者：汪自强
 创建时间：2017/03/20
 文件说明：智乘网络科技公司叉车电机驱动程序
 
 串口控制程序都在此文件中
 
************************************************/

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记
char state = 0;
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);//使能USART1,开启端口B和复用功能时钟
  GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);//使能端口重映射
	
	//USART1_TX   GPIOB.6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PB.6
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.6
   
  //USART1_RX	  GPIOB.7初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}


/*
@函数功能

	1.收指令
	aa; 前进
	bb; 后退
	ll; 左转
	rr; 右转
	ss; 停止
	go10; 遥控前进的速度
	gc10; 自动控制的左轮速度
	gd10; 自动控制的右轮速度
	
	2.解析指令
	  1）主机速度
	  2) 从机速度
	
	3.分发指令
	  1）赋值 ptForkLift->s16speedwant 
	
	*/

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x3b结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		
				
		if(Res==0x3B)//收到结尾符号
		{
			
			if(USART_RX_BUF[0]=='g')
			{
				
				if(USART_RX_BUF[1]=='o')
				{
					ptForkLift->s16speedwant = ptForkLift->s16speedmasterdirect *( ((float)(USART_RX_BUF[2]&0x0f))*10+(float)(USART_RX_BUF[3]&0x0f));
					ptForkLift->Can_Txmessage.Data[0] = 127+ptForkLift->s16speedslavedirect *( ((float)(USART_RX_BUF[2]&0x0f))*10+(float)(USART_RX_BUF[3]&0x0f));;
					Can_Send_Msg();
					state = 1;
					
				}
			}
			else
			{
				if(USART_RX_BUF[0]=='s')//停止
				{
					ptForkLift->s16speedmasterdirect = 0; 
					ptForkLift->s16speedslavedirect =0;
					ptForkLift->s16speedwant = 0; 
					ptForkLift->Can_Txmessage.Data[0] = 127;
					Can_Send_Msg();
				}
				else 
				{
							if(USART_RX_BUF[0]=='a')     //前进
						{
							if(state ==1)
							{
								ptForkLift->s16speedmasterdirect = 1; 
							    ptForkLift->s16speedslavedirect =-1;
								
								ptForkLift->s16speedwant = __fabs(ptForkLift->s16speedwant);
								ptForkLift->Can_Txmessage.Data[0] = 127-__fabs(ptForkLift->Can_Txmessage.Data[0]-127);
								Can_Send_Msg();
								
							}
							else
							{
								ptForkLift->s16speedmasterdirect = 1; 
								ptForkLift->s16speedslavedirect =-1;
							}
				
						}
						else if(USART_RX_BUF[0]=='b')//后退
						{
							if(state ==1)
							{
								ptForkLift->s16speedmasterdirect = -1; 
							    ptForkLift->s16speedslavedirect =1;
								
								ptForkLift->s16speedwant = -__fabs(ptForkLift->s16speedwant);
								ptForkLift->Can_Txmessage.Data[0] = 127+__fabs(ptForkLift->Can_Txmessage.Data[0]-127);
								Can_Send_Msg();
								
							}
							else
							{
								ptForkLift->s16speedmasterdirect = -1; 
								ptForkLift->s16speedslavedirect =1;
							}
						}
						else if(USART_RX_BUF[0]=='l')//左转
						{
							if(state ==1)
							{
								ptForkLift->s16speedmasterdirect = 1; 
							    ptForkLift->s16speedslavedirect =1;
								
								ptForkLift->s16speedwant = __fabs(ptForkLift->s16speedwant);
								ptForkLift->Can_Txmessage.Data[0] = 127+__fabs(ptForkLift->Can_Txmessage.Data[0]-127);
								Can_Send_Msg();
								
							}
							else
							{
								ptForkLift->s16speedmasterdirect = 1; 
								ptForkLift->s16speedslavedirect =1;
							}
						}
						else if(USART_RX_BUF[0]=='r')//右转
						{
							if(state ==1)
							{
								ptForkLift->s16speedmasterdirect = -1; 
							    ptForkLift->s16speedslavedirect =-1;
								
								ptForkLift->s16speedwant = -__fabs(ptForkLift->s16speedwant);
								ptForkLift->Can_Txmessage.Data[0] = 127-__fabs(ptForkLift->Can_Txmessage.Data[0]-127);
								Can_Send_Msg();
								
							}
							else
							{
								ptForkLift->s16speedmasterdirect = -1; 
								ptForkLift->s16speedslavedirect =-1;
							}
						}

				} 
				
			}
			
			
		}
	
		else
		{
			USART_RX_BUF[USART_RX_STA&0X3FFF]=Res;
			USART_RX_STA++;
			if(USART_RX_STA>(USART_REC_LEN-1))
			{
				USART_RX_STA=0;//接收数据错误,重新开始接收	
			}
				  
		} 
	}

} 
#endif	
