#ifndef _UART_C_
#define	 _UART_C_

#include"reg52.h"

#define uchar unsigned char
#define uint unsigned int

/*
void delay_uart(uint ms)
{
	uint i,j;
	for(i=0;i<ms;i++)
		for(j=0;j<112;j++);
}    
*/
void Uart_init()
{
	
	TMOD= 0X20;		//T1  方式2  ，8位   自动重装   
	TH1=0Xfd;
	TL1=0Xfd;	    //9600
	TR1=1; 		    // 定时器1启动  


	SM0=0;	    // 设置串口的工作模式
	SM1=1; 	    //方式1


	REN=1; 		 // 允许串口接收数据 	 


	ES=1; 	     // 串口中断应许 
	EA=1;		 // 开启中断 
	
}

void SendASC(uchar d)
{
	
	SBUF=d;
	while(!TI);

	TI=0;
}
void SendString(uchar *str)	
{
	while(*str)
	{
		SendASC(*str) ;
		str++;
		//delay_uart(1);
	}
}
#endif
												       



