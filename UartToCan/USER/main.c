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
 ALIENTEK战舰STM32开发板实验26
 CAN通信实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
 
 二次开发：
 创建者：汪自强
 创建时间：2017/11/21
 文件说明：硕士毕业设计串口转CAN程序
 
 开发log：

 
 

************************************************/


/************************************************

总体概述：

        6个电机 


************************************************/

void can_init(void);

 int main(void)
 {	 
	 ptForkLift=&TForkLift; 
	 ptmessage_queue = &message_queue;
	 create_queue(ptmessage_queue);   //创建一个消息队列
	 ptmessage_queue->size = 0;
	 
	 delay_init();
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	
	 /*           先占优先级         从优先级3级
	  CAN1             	0                  	2
	  
	  UART1            	3                  	3
	 
	 */
	 LED_Init();
	 uart_init(115200);	 	//串口初始化为115200
	 TIM1_PWM_Init();
	  
	 delay_ms(1000);//延时时间不能超过1800，多延时就要多调用
		
	 TIM3_Int_Init();          //定时器3用作系统时基,1ms进入一次中断  
	 
	 can_init();	//can总线初始化
		
	 printf("This UART to CAN node\r\n");
	

 	while(1)
	{
//		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
//        printf("speedout=%f ErrorSum=%f PWM=%d\r\n",ptForkLift->s16speedout,ptForkLift->s16ErrorSum,ptForkLift->u16PWM);
// 		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
//			
		
//		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
        		
		
//		printf_queue(ptmessage_queue);
//		printf("queue size = %d\r\n",ptmessage_queue->size);
//		
//		printf("\r\n");
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


