#include "STC_NEW_8051.H"
#include "string.h"
#include <intrins.h>	

#define S2RI 0x01	
#define S2TI 0x02	
#define uchar unsigned char
#define uint unsigned int

#define len 60


unsigned int i;//全局用 I


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
		//IE2 &=0xfe;
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
	
	printf2("AT+CNMI=2,2,0,1\r\n"); //配置短信显示模式
	delay_ms(500);
	
	while(1)//开始检测短信
	{
		if(flag2!=0)
		{
			delay_ms(200);
			flag2=0;
			
			UART_1SendOneByte(usart2buf[50]);
			UART_1SendOneByte(usart2buf[51]);
			UART_1SendOneByte(usart2buf[52]);
			UART_1SendOneByte(usart2buf[53]);
			UART_1SendOneByte(usart2buf[54]);
			UART_1SendOneByte(usart2buf[55]);
			
			if((usart2buf[50]=='L')&&(usart2buf[51]=='E')&&(usart2buf[52]=='D'))
			{
				if((usart2buf[53]=='O')&&(usart2buf[54]=='N'))
				{
					printf1("打开\r\n");
					P0=0XFF;
				}
				else	if((usart2buf[53]=='O')&&(usart2buf[54]=='F'))
				{
					printf1("关闭\r\n");
					P0=0;
				}
			}

			
			//printf1(usart2buf);
		}
		else
		{
			for(i=0;i<len;i++)
			{
				usart2buf[i]=0;	
			}
		}
	}

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
