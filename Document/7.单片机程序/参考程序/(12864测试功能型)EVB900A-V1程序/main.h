//*********************************************
//功能：汽车导航控制器软件
//设计：DCL,CHM
//2010年08月28日
////////////////////////////////////////////////////////////
#include "C8051F020.H"
#include <string.h>
#include <stdio.h>
#include <intrins.h>

#include "XF.h"
#include "IIC.h"
//#include "SD2203.h"
//#include "SIM300.h"


 
///////////串口1部分不能位操作/////////////////
#define IsTI1 (SCON1&0x02)
#define IsRI1 (SCON1&0x01)
#define ClrTI1() SCON1&=~0x02;
#define ClrRI1() SCON1&=~0x01;
///////////蜂鸣器操作/////////////////
#define spk  SPK=0;	delay_10ms(10); SPK=1;




sbit GPS_BOOT    =   P0^4;//GPS
sbit GPS_EN      =   P0^5;//GPS

sbit FM_SEL1     =   P3^2;//FM24V10--片选
sbit FM_SEL0     =   P3^3;//FM24V10--片选
sbit WP0         =   P3^4;//FM24V10--写保护  WP0=1写保护　0取消保护
sbit FM_SEL3     =   P3^7;//FM24V10--片选


sbit FM_SEL2     =   P2^0;//FM24V10--片选
sbit WP1         =   P2^3;//FM24V10--写保护  WP1=1写保护　0取消保护
sbit XF_ERST     =   P2^6;//讯飞--复位


sbit SPK          =   P1^1;//蜂鸣器
sbit MIC_EN       =   P1^2;//麦克风使能
sbit TPA2005_SD   =   P1^3;//功放使能
sbit SIM_AUDIO_EN =   P1^4;//SIM模块--语音至功放通路
sbit XF_AUDIO_EN  =   P1^5;//讯飞--语音至功放通路
sbit LED0         =   P1^6;//提示LED灯1
sbit LED1         =   P1^7;//提示LED灯2



//************硬件初始化部份**********************************************
//定时器1初始化，作为UART0,UART1的波特率发生器
void Timer_Init()//19200bps UART0:使用time2     UART1:使用time4 
{
    CKCON     = 0x60;
    TCON      = 0x40;
    TMOD      = 0x30;
    T2CON     = 0x34;
    RCAP2L    = 0xDC;
    RCAP2H    = 0xFF;
    T4CON     = 0x34;
    RCAP4L    = 0xDC;
    RCAP4H    = 0xFF;
}
//************************************************************************
//串口初始化，选择UART１
//************************************************************************/
void UART_Init()
{
    SCON0     = 0x50;
    SCON1     = 0x50;
}
void Port_IO_Init()
{
	P0MDOUT   = 0xC0;
    P1MDOUT   = 0x3F;
    P2MDOUT   = 0x07;
    P3MDOUT   = 0xFF;
    P74OUT    = 0xC0;
    XBR0      = 0x04;
    XBR1      = 0x14;
    XBR2      = 0x44;
}
//************************************************************************
//系统时钟初始化，22.1184M 选择外部晶体振荡器
//************************************************************************/
void Oscillator_Init()
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
}
//开串口0，串口1，外部中断0，外部中断1,定时器3,总中断开　
void Interrupts_Init()
{
    IE        = 0x95;
    EIE2      = 0x40;
}
void Init_Device(void)
{
	WDTCN = 0xDE;// 关闭看门狗
	WDTCN = 0xAD;
	Oscillator_Init();
	Port_IO_Init();
    UART_Init();
    Timer_Init();
	Interrupts_Init();
}


/******************************************************************************************************************
//////////////////////函数void sendchar(uchar ch);实现功能:发送一字节数据//////////////////////////////////////////
*****************************************************************************************************************/
void sendchar(unsigned char ch)
{
    unsigned char xx;
	xx=ch;
    SBUF1=xx;
    while(!IsTI1);
	ClrTI1();
}


///////////////////////////////////////////////////////////////////////
// 主程序开始
///////////////////////////////////////////////////////////////////////
void main(void)
{
   
    Init_Device(); //晶振。管脚初始化
	spk;
	

	while(1)
	{	
	   GPS_EN=0;
	   LED0=0;
	   LED1=0;
	}
} 



//////////////////////////////////////////////////////////////////////
//UART0,SP232C上位机串口接收和发送中断处理程序
///////////////////////////////////////////////////////////////////////
void isr_UART(void) interrupt 4 using 3
{
    unsigned char idata test1;
  	unsigned int idata j=0;
  	EA=0;
  	if(RI0)
	{		
		test1=SBUF0;//数据接收过程中，第一个接收到的数据为后面数据字节数
		RI0=0;	
		if(test1==0xAA)
		{
		   

		   	SBUF0=0xBB;//RevBuffer[30]
		 	while(!TI0);
			TI0=0;
		}

	}
 EA=1;

}
///////////////////////////////////////////////////////////////////////
//UART1,MAX485 主副控制串口通讯中断处理程序
///////////////////////////////////////////////////////////////////////
/*
void max485_UART(void) interrupt 20 using 3
{
   unsigned char xor_tmp;
  	EA=0;
  	if(IsRI1)
	{		
	   if(max485_symbol==0xDD)
	   {
	      max485_data[max485_len]=SBUF1;
          max485_len++;
	   }
	   else
	   {
          max485_symbol=0;
	   }
	   if(max485_symbol==0)
	   {
	      max485_symbol=SBUF1;
	   }
       switch (max485_data[1])
       {
         case 0xB0://副控制器上传刷卡信息正常
                  if(max485_len==14)
				  {
				   max485_data[0]=max485_data[0]^max485_data[1]^max485_data[2]^max485_data[3]^max485_data[4]^max485_data[5]^max485_data[6];
				   max485_data[0]=max485_data[0]^max485_data[7]^max485_data[8]^max485_data[9]^max485_data[10]^max485_data[11]^max485_data[12]^0xDD;
                   if(max485_data[0]==max485_data[13])
				   {
                      slave_ok=1;//发送回令数据标志位
			          brush_success_type=0x02;//副控制刷卡成功标志
			          //???副控制器刷卡信息保存入铁电
				   }
		           max485_symbol=0;
	               max485_len=0;
				   }
                   break;
         case 0xB1://副控制器密码开继电器正常
                  if(max485_len==5)
				  {
				     max485_data[0]=max485_data[0]^max485_data[1]^max485_data[2]^max485_data[3]^max485_data[3];
                     if(max485_data[0]==max485_data[4])
					 {
                         //main_ok=1;//主控制器回令正常
					 }
					 max485_symbol=0;
					 max485_len=0;
				  }
                  break;        
         case 0xB2://主控制器同时设置时间
                  if(max485_len==3)
				  {
                      if(max485_data[0]==0xFF&&max485_data[1]==0xB2&&max485_data[2]==0x01)
					  {

						 //send_type=0x03;//时间设置成功
					  }
					  max485_symbol=0;
					  max485_len=0;

				  }
                   break;
         default: 
                   break;
       }
       ClrRI1(); //RI1=0;
	}
    EA=1;
}*/

/******************************************************************************************************************
///////////////////函数void receive(void) interrupt 4 using 1;实现功能:通过串口接收数据////////////////////////////
******************************************************************************************************************/
/*
void receive(void) interrupt 4 using 1
{
	if(RI)
	{
		if(Rx<RxIn)
		{
			SystemBuf[Rx]=SBUF;
			Rx++;
		}
		RI=0;
		
	}
}*/

