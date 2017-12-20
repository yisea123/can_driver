#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "timer.h" 
#include "control.h"
 
/************************************************
 创建者：汪自强
 创建时间：2017/03/20
 文件说明：同济大学汪自强SLAM智能车电机驱动程序
 
 开发log：
 2017/03/20  CAN通信稳定
 2017/03/21  添加串口，时基系统
 2017/04/24  添加正交编码功能，使用TIM2
 2017/12/20  使用宏定义决定是电机驱动板工作在那个模式，在"control.h"定义
************************************************/


/************************************************

总体概述：

       3个电机 
			 
		0是接地，即拨到 ON KE字样端(焊接在GND网络)
		
		2号按键   1号按键
			PB1     PB10
			 0       0     0号轮
			 0       1     1号轮
			 1       0     2号轮	



************************************************/
void can_init(void);


 int main(void)
 {	 
	
	 #if OPENLOOPTESTMODE	|| CLOSELOOPTESTMODE
			int i=0;
	 #endif
	 ptForkLift=&TForkLift; 
	 ptmiddle_filter_queue = &middle_filter_queue;
	 create_queue(ptmiddle_filter_queue);   //创建一个编码器滤波队列
	 
	 delay_init();
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	
	 /*           先占优先级         从优先级3级
	  CAN1             	0                  	2
	  
	  TIM1 			   	0				  	3       PWM输出
	  TIM2			   	1					0		正交编码
	  UART1            	3                  	3
	 
	 */
	 LED_Init();
	 uart_init(115200);	 	//串口初始化为115200
	 
	 delay_ms(1000);//延时时间不能超过1800，多延时就要多调用
	 printf("This is wheel slave\r\n");
	 
	 CarID_Select();

	 TIM1_PWM_Init();
	 
	 
	  
	delay_ms(1000);//延时时间不能超过1800，多延时就要多调用
	
		
	TIM2_Encoder_Init();
	TIM3_Int_Init();          //定时器3用作系统时基,1ms进入一次中断  
	 
	ptForkLift->s16speedwant = 0;    //注意想要的速度只能在PID控制函数外部设置，不能再POSITION_PID里面设置
	
//		黄色地板车PID参数	
//		ptForkLift->s16speed_p = 11.89;
//		ptForkLift->s16speed_i = 0.3;
//		ptForkLift->s16speed_d = 4;

	ptForkLift->s16speed_p = 0.7;
	ptForkLift->s16speed_i = 0.2;
	ptForkLift->s16speed_d = 4;
	
	can_init();
		
	


 	while(1)
	{
		#if OPENLOOPTESTMODE		
				if( i>=0 && i < MAX_PWM)
				{
					ptForkLift->s16speedwant= i;
				}
				else if( i>=MAX_PWM && i < 2*MAX_PWM)
				{
					ptForkLift->s16speedwant= 2*MAX_PWM - i;
				}
				else if( i>= 2*MAX_PWM && i < 3*MAX_PWM)
				{
					ptForkLift->s16speedwant = 2*MAX_PWM - i;
				}
				else if( i>= 3*MAX_PWM && i < 4*MAX_PWM)
				{
					ptForkLift->s16speedwant=  i- 4*MAX_PWM ;
				}
								
				i++;
				delay_ms(10);
				
				if( i  == 4*MAX_PWM)
				{
					i = 0;
				}
		#endif
				
		#if CLOSELOOPTESTMODE		
				if( i>=0 && i < MAX_PLUSE)
				{
					ptForkLift->s16speedwant= i;
				}
				else if( i>=MAX_PLUSE && i < 2*MAX_PLUSE)
				{
					ptForkLift->s16speedwant= 2*MAX_PLUSE - i;
				}
				else if( i>= 2*MAX_PLUSE && i < 3*MAX_PLUSE)
				{
					ptForkLift->s16speedwant = 2*MAX_PLUSE - i;
				}
				else if( i>= 3*MAX_PLUSE && i < 4*MAX_PLUSE)
				{
					ptForkLift->s16speedwant=  i- 4*MAX_PLUSE ;
				}
								
				i++;
				delay_ms(10);
				
				if( i  == 4*MAX_PLUSE)
				{
					i = 0;
				}
		#endif	
		
		//printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
    //ptForkLift->Can_Txmessage.Data[0] = 127+5;
		//Can_Send_Msg();
		//printf("ptForkLift->s16speedwant=%d\r\n",i);
    
	}
}

void can_init(void)
{
		CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN初始化正常模式,波特率250Kbps    
		ptForkLift->Can_Txmessage.StdId=0x01;			   // 标准标识符 
    ptForkLift->Can_Txmessage.ExtId=0x12;			   // 设置扩展标示符 
    ptForkLift->Can_Txmessage.IDE=CAN_Id_Standard;     // 标准帧
    ptForkLift->Can_Txmessage.RTR=CAN_RTR_Data;		   // 数据帧
    ptForkLift->Can_Txmessage.DLC=8;				   // 要发送的数据长度   
}

