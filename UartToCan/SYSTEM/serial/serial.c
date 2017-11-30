#include "serial.h"
#include "control.h"
#include "can.h" 
#include "led.h"

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
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}


/*
@函数功能

	一帧数据的结构  八个字节

		帧头  一号轮速度       二号轮速度        三号轮速度       帧尾
		0x53  0xFF    0xFF     0xFF    0xFF      0xFF 0xFF        0x45
			  高位    低位
对应CAN消息 Data[0] Data[1]    Data[2] Data[3]   Data[4] Data[5]
	
	*/
bool start_flag = 0;
#define MESSAGE_SIZE 6

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	
	u8 Res;
	u8 i;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x3b结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
						
		
		
		if(start_flag)
		{
			en_queue(ptmessage_queue,Res);	
		}
		
		if(Res==0x53)//收到开始“S”符号
		{ 
			start_flag = 1;
			
		}
		if(Res==0x45)//收到结束“E”符号
		{ 
			start_flag = 0;
			de_queue(ptmessage_queue);
			
			for( i = 0 ; i < MESSAGE_SIZE ; i++ )
			{
				ptForkLift->Can_Txmessage.Data[i] = de_queue(ptmessage_queue);
			}
			Can_Send_Msg();
			
			ptForkLift->u16CanComCount[0]++;
			
		}
		
		
	
		
	}

} 
#endif	
