# CAN DRIVER and TRANSFER
## STM32F103C8T6 CAN DC-Motor Driver
    3个电机驱动三个全向轮，每个电机有编号
			 
		  0是接地，即拨到 ON KE字样端(焊接在GND网络)
		
		  2号按键   1号按键
			PB1     PB10
			 0       0     0号轮
			 0       1     1号轮
			 1       0     2号轮	
       //*****电机运行模式选择************//
         在/CTRL/control.h文件中定义电机的工作模式
            1 开环测试
            2 闭环测试
            3 开环运行
            4 闭环运行
            
## STM32F103C8T6 UART-CAN Message transfer
      将按如下所示的一帧数据定义转换为CAN总线消息，并广播到CAN网络中.      
      一帧数据的结构  八个字节
		  帧头       一号轮速度                  二号轮速度        三号轮速度         帧尾
		  0x53  0xFF(高位)    0xFF(低位)       0xFF    0xFF      0xFF   0xFF        0x45
      对应CAN消息 Data[0] Data[1]    Data[2] Data[3]   Data[4] Data[5]
      如，unsigned char instruction_message[8]={0x53,0x7f,0xff,0x7f,0xff,0x7f,0xff,0x45};
      0x53是帧头，0x45是帧尾。
      每个电机的速度用两个字节表示，0x7fff(32767)表示速度的零点，向上加速，向下减速。
      53 8063 8063 8063 45表示每个电机速度都为100.
