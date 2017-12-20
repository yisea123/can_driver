#include "led.h"
#include "control.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED0-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	 GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //PB.5 输出高


}


void CarID_Select(void)
{
     GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTE时钟	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_1;				 //LED0-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
	
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	 
	/*  0是接地，即拨到 ON KE字样端
			PB1   PB10
			 0     0     1号轮
			 0     1     2号轮
			 1     0     3号轮	
	*/
	 
	 //ptForkLift->u8CarID=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10);
	
	 if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==0 && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
	 {
			ptForkLift->u8CarID = 0x00;
	 }
	 else if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==1 && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
	 {
			ptForkLift->u8CarID = 0x01;
	 }
	 else if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==0 && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 1)
	 {
			ptForkLift->u8CarID = 0x02;
	 }
	 
	 printf("this is wheel %d\r\n",ptForkLift->u8CarID);
	 if(Mode == 1)
		 printf("开环测试模式\r\n");
	 else if(Mode == 2)
		 printf("闭环测试模式\r\n");
	 else if(Mode == 3)
		 printf("开环运行模式\r\n");
	 else if(Mode == 4)
		 printf("闭环运行模式\r\n");
	 
	 
}










 
