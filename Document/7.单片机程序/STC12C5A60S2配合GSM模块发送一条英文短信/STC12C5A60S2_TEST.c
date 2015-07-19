#include "STC_NEW_8051.H"
#include "string.h"
#include <intrins.h>	

#define S2RI 0x01	
#define S2TI 0x02	
#define uchar unsigned char
#define uint unsigned int

#define len 52

sbit led=P1^0;
sbit Ukey=P1^1;
sbit Skey=P2^3;

sbit m1=P1^4;
sbit m2=P1^5;
sbit m3=P1^6;
sbit m4=P1^7;

sbit n1=P2^0;
sbit n2=P2^1;
sbit n3=P3^2;
sbit n4=P3^3;
sbit n5=P3^4;
sbit n6=P3^5;
sbit n7=P3^6;
sbit n8=P3^7;

sbit K0=P0^0;
sbit K1=P0^1;
sbit K2=P0^2;
sbit K3=P0^3;
sbit K4=P0^4;
sbit K5=P0^5;
sbit K6=P0^6;
sbit K7=P0^7;
sbit K8=P2^7;
sbit K9=P2^6;
sbit KX=P2^5;
sbit KJ=P2^4;

unsigned int i;//全局用 I
uchar thz=0;	 //通话中标志位

#define y1 1
#define y2 2
#define y3 3
#define y4 4

#define x1 5
#define x2 6
#define x3 7
#define x4 8
#define x5 9
#define x6 10
#define x7 11
#define x8 12			 //系统内存的

#define k0 13	 //0
#define k1 14	 //1
#define k2 15	 //2
#define k3 16	 //3
#define k4 17	 //4
#define k5 18	 //5
#define k6 19	 //6
#define k7 20	 //7
#define k8 21	 //8
#define k9 22	 //9
#define kx 23	 //*
#define kj 24	 //#


unsigned char flag1=0;
unsigned char flag2=0;

void printf1 (uchar *p);
void printf2 (uchar *p);

void UART_1SendOneByte(unsigned char c);
void UART_2SendOneByte(unsigned char c);

uchar  usart1buf[len]={0};
uchar  usart2buf[len]={0};


void delay_ms(unsigned int i)   // -0.018084490741us
{
		while(i--)
		{	
			unsigned char a,b;
			for(b=18;b>0;b--)
					for(a=152;a>0;a--);
			_nop_();  //if Keil,require use intrins.h
		}
}
////////////////////////////////////////////////////


/////////////////GSM模块测试连接函数////////////////
//测试指令：AT                                    //
//软件延时：1000ms                                //
//返回值：1 连接正常                              //
//返回值：0 其他错误                              //
////////////////////////////////////////////////////
uchar GSMinit(void)
{
	flag2=0;
	for(i=0;i<len;i++)
	{
		usart2buf[i]=0;	
	}
	
	printf2("AT&F\r\n");//设置模块波特率115200
	delay_ms(300);

	flag2=0;//复位缓冲
	printf2("AT\r\n");
	delay_ms(1000);
	UART_1SendOneByte(usart2buf[5]);
	UART_1SendOneByte(usart2buf[6]);
	if(usart2buf[5]=='O' && usart2buf[6]=='K')// 如果成功
	{
		flag2=0;
		IE2 &=0xfe;
		return 1;
	}
	else
	{
		flag2=0;
		return 0;
	}
}
/**************** 串口初始化****************/
//串口1 定时器 11.0592  115200
//串口2 独立波特率发生器 11.0592  115200
void InitUART(void)
{
	AUXR = AUXR|0x40 ;
	TMOD = 0x20;
	SCON = 0x50;
	TH1 = 0xFD;
	TL1 = TH1;
	PCON = 0x00;
	EA = 1;
	//   ES = 1;
	TR1 = 1;
	
	AUXR |= 0x08;		//使能波特率倍速位S2SMOD
	S2CON = 0x50;		//8位数据,可变波特率
	BRT = 0xFA;		//设定独立波特率发生器重装值
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	AUXR |= 0x10;		//启动独立波特率发生器
	
	IE2 =0x01;	
}


/*****************主函数******************/
void main(void)
{
	InitUART();	 
	
	while(GSMinit()!=1);//模块连接错误 始终尝试连接，直到连接成功后往下走
	
	P0=0; //板子P0口全部置0 代表初始化通过
	
	printf2("AT+CMGF=1\r\n"); //配置英文模式
	delay_ms(500);
	
	printf2("AT+CSCS=");      //配置字节集
	UART_2SendOneByte(0x22);
	printf2("GSM");
	UART_2SendOneByte(0x22);
	printf2("\r\n");
	delay_ms(500);
	
	printf2("AT+CMGS=");
	UART_2SendOneByte(0x22);
	printf2("18845146761");   //发送号码，各位学生在测试的时候 注意改成自己的号码
	UART_2SendOneByte(0x22);
	printf2("\r\n");
	delay_ms(500);
	
	printf2("nihao");     //发送内容 
	delay_ms(500);
	UART_2SendOneByte(0x1a);
	
	while(1);//发送成功在此等待

}
/************串口中断函数************/
void UART_1Interrupt(void) interrupt 4
{
	if(RI==1)
	{
		RI=0;
		usart1buf[flag1++]=SBUF;
	}
} 
void UART_2Interrupt(void) interrupt 8
{
	if(S2CON&S2RI)
	{
		S2CON&=~S2RI;
		usart2buf[flag2++]=S2BUF;
	} 
}
/****************串口1发送一个字节****************/
void UART_1SendOneByte(unsigned char c)
{
    SBUF = c;
    while(!TI);	  
    TI = 0;	 
}
/****************串口2发送一个字节****************/
void UART_2SendOneByte(unsigned char c)
{
    S2BUF = c;
    while(!(S2CON&S2TI));  
    S2CON&=~S2TI;	  //S2TI=0
}
void printf1 (uchar *p)
{
	while(*p!='\0')
	{
		UART_1SendOneByte(*p++);
	}
}
void printf2 (uchar *p)
{
	while(*p!='\0')
	{
		UART_2SendOneByte(*p++);
	}
}
