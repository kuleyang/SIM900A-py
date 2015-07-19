#include"reg52.h"
#include "uart.c"
#include "tc35.c"

//#define RsBuf_N			50

//char RsBuf[RsBuf_N];	//定义串口接收数据缓冲区
//uchar RsPoint=0;


sbit huang=P2^6;
sbit hong=P2^5;
sbit lv=P2^4;
sbit buzz=P2^2;
sbit jk=P2^1;

uchar code PhoneNO[]= "00310038003800340035003100340036003700360031"; //接受号码，这是转码后的
uchar code Text1[]= "4F60731C6211662F8C01FF1F"; //发送内容，这是转码后的




uchar code center[]="+8613800451500";

void main()
{
	Uart_init();
	
	SendString("AT+CMGF=1\r\n");	
	
	delayms_1000ms();
	
	SendString("AT+CSCS=\"UCS2\"\r\n");	
	
	delayms_1000ms();
	
	SendString("AT+CSMP=17,0,2,25\r\n");	
	
	delayms_1000ms();
	
	SendString("AT+CMGS=");	//信息发送指令 AT+CMGS=//
	SendASC('"');
	SendString(PhoneNO);   
	SendASC('"');
	SendASC('\r');			//发送回车指令//
	SendASC('\n');			//发送回车指令//
	
	delayms_1000ms();
	
	SendString(Text1);  

	delayms_1000ms();
	
	SendASC(0x1a);		
	


	while(1);	

 }

 
 


 /*问题：
   每次处理完数据。RsPoint是怎样清零的？？？？？在 准备发一个数据，然后接受一个数据 之前
   RsBuf[RsPoint++]=SBUF;
		RsBuf[RsPoint]=0x00;   //将下一个数据清零

		如果这个发送没问题。加删除，接收函数，参考桌面上的tc35.c

		添加液晶菜单的支持	，类似手机
*/
 
