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
 创建时间：2017/03/20
 文件说明：智乘网络科技公司叉车电机驱动程序
 
 开发log：
 2017/03/20  CAN通信稳定
 2017/03/21  添加串口，时基系统
 2017/04/24  添加正交编码功能，使用TIM2
 
 

************************************************/


/************************************************

总体概述：

        6个电机 







************************************************/

 int main(void)
 {	 
	 int i=0;
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

	 TIM1_PWM_Init();
	 
	 
	  
	delay_ms(1000);//延时时间不能超过1800，多延时就要多调用
	
		
	TIM2_Encoder_Init();
	TIM3_Int_Init();          //定时器3用作系统时基,1ms进入一次中断  
	 
	ptForkLift->s16speedwant = 0;    //注意想要的速度只能在PID控制函数外部设置，不能再POSITION_PID里面设置
	
	ptForkLift->s16speed_p = 11.89;
	ptForkLift->s16speed_i = 0.3;
	ptForkLift->s16speed_d = 4;
	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN初始化正常模式,波特率250Kbps    
	ptForkLift->Can_Txmessage.StdId=0x01;			   // 标准标识符 
    ptForkLift->Can_Txmessage.ExtId=0x12;			   // 设置扩展标示符 
    ptForkLift->Can_Txmessage.IDE=CAN_Id_Standard;     // 标准帧
    ptForkLift->Can_Txmessage.RTR=CAN_RTR_Data;		   // 数据帧
    ptForkLift->Can_Txmessage.DLC=8;				   // 要发送的数据长度   
	
	printf("system init ok \r\n");

	//ptForkLift->bDrection = BACK;
	ptForkLift->bDrection = FOWARD;
	ptForkLift->u16PWM=0;
		
	
	SetPwmDir(ptForkLift);

 	while(1)
	{
		
		
		if( i>=0 && i < MAX_PWM)
		{
			ptForkLift->bDrection = FOWARD;
			ptForkLift->u16PWM=i;
		}
		else if( i>=MAX_PWM && i < 2*MAX_PWM)
		{
			ptForkLift->bDrection = FOWARD;
			ptForkLift->u16PWM= 2*MAX_PWM - i;
		}
		else if( i>= 2*MAX_PWM && i < 3*MAX_PWM)
		{
			ptForkLift->bDrection = BACK;
			ptForkLift->u16PWM= i - 2*MAX_PWM;
		}
		else if( i>= 3*MAX_PWM && i < 4*MAX_PWM)
		{
			ptForkLift->bDrection = BACK;
			ptForkLift->u16PWM= 4*MAX_PWM - i;
		}
		SetPwmDir(ptForkLift);
		
		i++;
		
		if( i  == 4*MAX_PWM)
		{
			i = 0;
		}
		
		
		printf("EncoderSpeed=%d FilterSpeed=%d\r\n",ptForkLift->s16EncoderSpeed,ptForkLift->s16EncoderFilterSpeed);
        //ptForkLift->Can_Txmessage.Data[0] = 127+5;
		//Can_Send_Msg();
	}
}



