#include "control.h"
#include "stdlib.h"
#include "delay.h"
#include "usart.h"
/************************************************
 
 创建者：汪自强
 创建时间：2017/03/20
 文件说明：智乘网络科技公司叉车电机驱动程序
 
 电机上层控制程序都在此文件中
 
************************************************/

//全局变量声明
TCtrl            TForkLift;     //定义叉车结构体
ptCtrl           ptForkLift;   //定义叉车结构体指针
linkqueue        message_queue;
linkqueue*       ptmessage_queue;



void Bubble_Sort()
{
	int a[10] = {1,23,45,67,89,11,33,57,11,45};
	int temp,i,j;
	for(i = 0; i< 9;i++)
	{
		for(j=i;j<9;j++)
		{
			if(a[j] < a[i])
			{
				temp = a[j];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
	
	
}

void Quick_Sort(linkqueue *  Fliter_q,int left,int right)
{
	int i = left;
	int j = right;

	
	int threshold = get_queue_value(Fliter_q,left);

	if (left > right)
	{
		return;
	}
	
	while (i < j)
	{
		while (get_queue_value(Fliter_q,j) >= threshold && i < j)
		{
			j--;
		}
		
		swap_data(Fliter_q,i,j);
		
		while (get_queue_value(Fliter_q,i) <= threshold && i < j)
		{
			i++;
		}
		
		swap_data(Fliter_q,i,j);
		

	}

	set_data(Fliter_q,i,threshold);

	
	Quick_Sort(Fliter_q, left, i - 1);

	Quick_Sort(Fliter_q, i + 1, right);
}

/*

创建一个队列
*/

void create_queue(linkqueue *  Fliter_q)
{
		
	Fliter_q->rear = Fliter_q->front = (Filter_Queue*)malloc(sizeof (Filter_Queue));
	
	if(!Fliter_q->front){

		gotoerror();
	}
	Fliter_q->front->next = NULL;
			
}

/*
   入队 
*/
void en_queue(linkqueue *  Fliter_q, u8 data)
{
	Filter_Queue * node;
	node = (Filter_Queue*)malloc(sizeof (Filter_Queue));
	
	if(!node){
		gotoerror();
	}
	
	node->date=data;
	node->next=NULL;
	
	Fliter_q->size ++;
	
	Fliter_q->rear->next = node; //先把两个节点链起来
	Fliter_q->rear = node;       //再将尾指针向后移动一位，使其指向队尾
	
	//free(node);这里不能free，如果在这里去free，那刚刚开辟的内存空间就被释放了，这个节点随着消逝

	
}

/*

 判断一个队列是否为空，空返回1，不空返回0

*/
int is_empty(linkqueue *  Fliter_q)
{
	return Fliter_q->front == Fliter_q->rear ? 1 : 0 ;
	
}


/*
 出队，失败返回0，成功返回1
*/
u8 de_queue(linkqueue *  Fliter_q){
	
	Filter_Queue*  node;
	
	u8 data;
	
	if(is_empty(Fliter_q))
	{
		return 'N';
	}
	node = Fliter_q->front;
	
	data = node->date;
	
	Fliter_q->front = node->next;
	
	Fliter_q->size --;
	
	free(node);//一定要释放掉头结点指向的内存空间
	
	return data;
	

	
}

/*
打印队列的所有值

*/
void printf_queue(linkqueue *  Fliter_q)
{
	Filter_Queue * node;
	
	node = Fliter_q->front;
	
	while(node)
	{
		printf("%c \r\n",node->date);
		
		node = node->next;
		
	}
}


/*
返回队列的第num个节点的data成员变量
*/
uint16_t get_queue_value(linkqueue *  Fliter_q,int num)
{
	Filter_Queue * node;
	int i=0;
	
	node = Fliter_q->front;
	
	for(i=0;i<num;i++)
	{
		node = node->next;
	}
	
	return node->date;

}








/*
   交换队列中第i个和第j个节点的data成员变量
*/
void swap_data(linkqueue *  Fliter_q,int i,int j)
{
	Filter_Queue * node1;
	
	Filter_Queue * node2;
	int k;
	uint16_t temp;
	
	node1 = Fliter_q->front;
	node2 = Fliter_q->front;
	
	for(k=0;k<i;k++)
	{
		node1 = node1->next;
	}
	
	for(k=0;k<j;k++)
	{
		node2 = node2->next;
	}
	
	temp = node1->date;
	
	node1->date = node2->date;
	node2->date = temp;
		
}


/*
   重新设置队列中第i个节点的data成员变量的值
*/
void set_data(linkqueue *  Fliter_q,int i,s16 value)
{
	Filter_Queue * node1;
	int k=0;
	node1 = Fliter_q->front;
	
	for(k=0;k<i;k++)
	{
		node1 = node1->next;
	}
	node1->date = value;
	
}


/*
   求队列数据的均值
*/
int average_data(linkqueue *  Fliter_q)
{
	Filter_Queue * node;
	int avg = 0;
	node = Fliter_q->front;
	
	while(node)
	{
		avg += node->date;		
		node = node->next;
		//printf("%d ",avg);
	}
	//printf("\r\n");
	return avg/10;
}


void gotoerror(void)
{
	while(1){
		
		printf("error\r\n");
		delay_ms(500);//延时时间不能超过1800，多延时就要多调用
	
	}
	
}










