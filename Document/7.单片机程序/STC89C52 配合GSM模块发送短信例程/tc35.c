/******************************************************************
MADE BY lyq

my BLOG      http://hi.baidu.com/282280072

*******************************************************************/

#include "uart.c"
#include "1602.h"
#include "delay.h"

#define uchar unsigned char
#define uint unsigned int
#define NULL 0

#define ATwaits			10
#define DWWwaits		10
#define Readwaits		10
#define Sendwaits		10

#define RsBuf_N			50

char RsBuf[RsBuf_N];	//定义串口接收数据缓冲区
uchar RsPoint;
//以标示变量或者函数的定义在别的文件中，提示编译器遇到此变量和函数时在其他模块中寻找其定义。

void Send_AT(void);
char *mystrstr(char *s, char *t);
void Num2char(uchar i);

/***********************TC35模块初始化******************************
*功    能: TC35模块初始化,	   P0.0输出低电平至少100ms
*形    参: 
*返 回 值:
*备    注: 
*****************************************************************/
void TC35_init()
{
	
	P1|=0x01;
	P1&=~0x01;
	delayms_100ms();delayms_100ms();delayms_100ms();delayms_100ms();
	delayms_100ms();delayms_100ms();
	P0|=0x01;
	delayms_10s();delayms_10s();delayms_10s();
}

/***********************发送联机指令******************************
*功    能: 串口发送数组命令到TC35，"AT",
*形    参: 
*返 回 值:
*备    注: 测试GSM模块是否连接正确
*****************************************************************/
void Send_AT(void)
{
  uchar *p,i=ATwaits;  //ATwaits=10
  //lcd_disp_str(RsBuf,1);

	
	while(i--)	//测试10次，在某一次成功就退出
	{
    	//RsBuf[0]='\0';	 //有下一句就不需要这句 //清空接收缓冲区
		RsPoint=0;
		// 本来只有\r  我添加的\n	后来证明 可以不加的
		SendString("AT\r");
		//****************************等待应答"OK"
    	ES=1;    //必须中断  // 串口中断应许 
    	delayms_100ms();//等待接受数据完成//delayms_1000ms();delayms_1000ms();
		
    	p=mystrstr(RsBuf,"OK");   //接收到的数据存在RsBuf
    	if(p!=NULL)	 //接收到"OK"
		
		{
			lcd_disp_str("GSM module is OK",1);
			lcd_disp_str("Will contimue!  ",2);
			delayms_1000ms();delayms_1000ms();//delayms_1000ms();delayms_1000ms();
			lcd_disp_str("                ",1);
			lcd_disp_str("                ",2);
			break;
		}
		lcd_disp_str("No GSM connected",1);delayms_1000ms();
		lcd_disp_str("                ",1);
	}

}
/***********************设置短消息模式******************************
*功    能: 串口发送数组命令到TC35，AT+CMGF=1
*形    参: 	  uchar m =1 text模式      m=0 PDU模式
*返 回 值:
*备    注: 1 TEXT
*****************************************************************/
void Set_MODE(uchar m)
{
  uchar *p,i=ATwaits;  //ATwaits=10
  //lcd_disp_str(RsBuf,1);

	
	while(i--)	//测试10次，在某一次成功就退出
	{
    	
		RsPoint=0;
		if(m)
			SendString("AT+CMGF=1");		 //设置短消息模式  1 TEXT
		else
			SendString("AT+CMGF=0");		 //设置短消息模式  PDU模式
		//****************************等待应答"OK"
    	ES=1;    //必须中断  // 串口中断应许 
    	delayms_100ms();//等待接受数据完成//delayms_1000ms();delayms_1000ms();
		
    	p=mystrstr(RsBuf,"OK");   //接收到的数据存在RsBuf
    	if(p!=NULL)	 //接收到"OK"
		
		{
			lcd_disp_str("Set_MODE is OK",1);
			lcd_disp_str("contimue!  ",2);
			delayms_1000ms();delayms_1000ms();//delayms_1000ms();delayms_1000ms();
			lcd_disp_str("                ",1);
			lcd_disp_str("                ",2);
			break;
		}
		lcd_disp_str("Set_MODE failed",1);delayms_1000ms();
		lcd_disp_str("                ",1);
	}

}
/***********************设置短消息中心******************************
*功    能: 串口发送数组命令到TC35， AT+CSCA=“+8613800531500”回车
*形    参: 		uchar *canter 短消息中心	   “+8613800531500”
*返 回 值:
*备    注: 
*****************************************************************/
void Set_CENTER(uchar *center)
{
  uchar *p,i=ATwaits;  //ATwaits=10
  //lcd_disp_str(RsBuf,1);

	
	while(i--)	//测试10次，在某一次成功就退出
	{
    	
		RsPoint=0;
		
		SendString("AT+CSCA=");	   //设置短消息中心
		SendASC('"');
		SendString(center);   //短消息中心号码
		SendASC('"');
		SendASC('\r');			//发送回车指令//
		//****************************等待应答"OK"
    	ES=1;    //必须中断  // 串口中断应许 
    	delayms_100ms();//等待接受数据完成//delayms_1000ms();delayms_1000ms();
		
    	p=mystrstr(RsBuf,"OK");   //接收到的数据存在RsBuf
    	if(p!=NULL)	 //接收到"OK"
		
		{
			lcd_disp_str("Set_CENTER is OK",1);
			lcd_disp_str("CENTER contimue!  ",2);
			delayms_1000ms();delayms_1000ms();//delayms_1000ms();delayms_1000ms();
			lcd_disp_str("                ",1);
			lcd_disp_str("                ",2);
			break;
		}
		lcd_disp_str("Set_CENTER failed",1);delayms_1000ms();
		lcd_disp_str("                ",1);
	}

}
/***********************发送TEXT短信息********************************
*功    能: TEXT模式发送短信息
*形    参: char *dialnum 目的号码+8613333333333    char *text发送内容
*返 回 值: 1 发送成功  0 发送失败
*备    注:发送TEXT短信息  之前应该先  1，设置短消息模式   AT+CMGF=? (0)=PDU (1)=TEXT
                                  2，设置短消息中心     AT+CSCA=“+8613800531500”回车                 

*****************************************************************/
char TransmitText(char *dialnum,char *text)   //发送号码	发送内容，字母或数字
{
	uchar i=Sendwaits,j=Sendwaits;
	uchar *p;

	


	SendString("AT+CMGS=");	//信息发送指令 AT+CMGS=//
	
///////////////////////////////
	SendASC('"');
	SendString(dialnum);   //手机号码的引号丢了！！！！！！！！！ 已经加上
	SendASC('"');
////////////////////////////////////
	SendASC('\r');			//发送回车指令//
	RsPoint=0;	  //清空接收缓冲区
	//delayms_1000ms();
	while(i--)
	{
		
		delayms_100ms();
		p=mystrstr(RsBuf,">");//">"
		if(p!=NULL)	//如果接受到 >  发送text
		{	
    		///////Get">"////////////////////////////
			
			SendString(text);
			RsPoint=0;	  //清空接收缓冲区
			SendString("\x1a\r");//'\x1a'结束符(相当CTRL+Z) '\r'回车符
			//delayms_100ms();
			while(j--)
			{
				delayms_100ms();
				p=mystrstr(RsBuf,"OK");
				if(p!= NULL )
				{
					//发送成功
					lcd_disp_str("Transmit already",1);
					
					return 1;
				}
				lcd_disp_str("wait Transmit!  ",1);
				delayms_1000ms();
				lcd_disp_str("                ",1);
				}		
		}
		//else lcd_disp_str("Transmit failure",1);
	}
	return 0;
}
/***********************字符串查找********************************
*功    能: 查找字符串
*形    参: char *s,  char *t  ;在s中查找t
*返 回 值: s_temp(t在s中的位置)成功     0 （失败 ）
*备    注: 
*****************************************************************/
char *mystrstr(char *s, char *t)
{
  char    *s_temp;        /*the s_temp point to the s*/
  char    *m_temp;        /*the mv_tmp used to move in the loop*/
  char    *t_temp;        /*point to the pattern string*/

	if ((s == NULL )|| (t == NULL)) return NULL;

	/*s_temp point to the s string*/
	for (s_temp = s; *s_temp != '\0'; s_temp++)
	{
   		/*the move_tmp used for pattern loop*/
		m_temp = s_temp;
    	/*the pattern string loop from head every time*/
		for (t_temp = t; *t_temp == *m_temp; t_temp++, m_temp++);
        /*if at the tail of the pattern string return s_tmp*/
		if (*t_temp == '\0') return s_temp;
     
	}
	return NULL;
 }



//  通讯中断接收程序   中断函数无返回值
  void uart_rx(void)  interrupt 4	using 3	  //放在这里 和放在main（）里面是一样的
 {
 	EA=0; 
   	if((RsPoint<RsBuf_N)&&RI) //必须判断RI是否为1
	//if(RI)
    {
    	RI=0;
    	RsBuf[RsPoint++]=SBUF;
		//RsBuf[RsPoint]=0x00;   //将下一个数据清零
		//SendASC(RsBuf[RsPoint-1]);
		//lcd_disp_str(RsBuf ,2);
    }
	EA=1;
 }
