#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

#define MAX_PWM            300   //PWM1000速度对应4523的脉冲
#define MIN_PWM            0
#define FOWARD             0
#define BACK               1
#define CanBoxPost         1
#define CanBoxPend         0
#define MotorPeriodIn      1
#define MotorPeriodOut     0

typedef struct TControl//与控制相关的参数全部在此结构体中
{
	u16      u16PWM;
	u8       bDrection;
	u8		 u8CarID;
	
	u16	     u16RunPIDControl;
	u16	     u16RunLED;
	float	 s16error[3];
	float	 s16ErrorSum;
	float      s16speedwant;
	float      s16speedmasterdirect;
	float      s16speedslavedirect;
	float      s16speedout;
	
	float      s16speed_p;
	float      s16speed_i;
	float      s16speed_d;
	
	u16	     u16EncoderPeriod;
	s16      s16EncoderSpeed;
	s16		 s16EncoderFilterSpeed;
	u16      u32ADCResult;


//*************CAN总线相关*********************//	
	u8       bCanComBox;
	u16	     u16CanComCount[2];
	u16      u16CanComPeriod;
	
	u16	     u16RunRecvPeriodMotor;
	u16	     u16RunSendPeriodMotor;
	CanTxMsg Can_Txmessage;
  
	u16	     u16time_ms;

//*************CAN总线相关*********************//	
	
	u16      u16MotorControlFlag;
	
}TCtrl,*ptCtrl;


typedef struct Filter_Queue_node{
	
	s16 date;
	struct Filter_Queue_node* next;
	
}Filter_Queue, *queue_ptr;
  

typedef struct Linkqueue{

	Filter_Queue*  front;//前指针
	Filter_Queue*  rear;//后指针
	
}linkqueue;

/*
class Ctrl1
{
	public:
  
    
	protected:
  
};
*/




extern TCtrl TForkLift;     //定义叉车结构体
extern ptCtrl ptForkLift;   //定义叉车结构体指针
extern linkqueue  middle_filter_queue;
extern linkqueue* ptmiddle_filter_queue;

void SetPwmDir(ptCtrl ptCtrlCar);
void POSITION_PID(ptCtrl ptCtrlCar,int EncoderSpeed);
void uart_init(u32 bound);

void create_queue(linkqueue *  Fliter_q);
void en_queue(linkqueue *  Fliter_q, uint16_t data);
int de_queue(linkqueue *  Fliter_q);
void printf_queue(linkqueue *  Fliter_q);
uint16_t get_queue_value(linkqueue *  Fliter_q,int num);
void swap_data(linkqueue *  Fliter_q,int i,int j);
void set_data(linkqueue *  Fliter_q,int i,uint16_t value);
void Quick_Sort(linkqueue *  Fliter_q,int left,int right);
int average_data(linkqueue *  Fliter_q);

void gotoerror(void);
#endif
