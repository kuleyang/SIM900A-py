#include "C8051F020.h"
#include "SIM300.h"

//AT指令表
unsigned char code AT[]			="AT";				//握手信号
unsigned char code ATI[]		="ATI";				//设备初始化
unsigned char code ATE0V1[]		="ATE0V1";			//关回显设置命令返回OK/V1 0/V0
unsigned char code AT_IPR[]		="AT+IPR=9600"; 	//波特率设置
unsigned char code AT_CPIN[]	="AT+CPIN?";		//查询SIM卡状态
unsigned char code AT_CSQ[] 	="AT+CSQ";			//查询信号
unsigned char code ATH[]		="ATH";				//呼叫挂起
unsigned char code AT_COPS[]	="AT+COPS?";		//查询网络运营商
unsigned char code AT_CDSNORIP[]="AT+CDNSORIP=0";	//GPRS IP 方式
unsigned char code AT_CIPHEAD[]	="AT+CIPHEAD=1"; 	//GPRS 接收方式
unsigned char code AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"122.234.240.62\",\"1001\"";//GPRS连接方式\GPRS连接地址(根据用户实际IP修改)\GPRS连接的端口
unsigned char code AT_CIPSEND[]	="AT+CIPSEND";		//GPRS发送信息命令
unsigned char code AT_CIPCLOSE[]="AT+CIPCLOSE";		//TCP连接关闭
unsigned char code AT_CIPSHUT[]	="AT+CIPSHUT";		//GPRS连接关闭   
unsigned char code ATA[]		="ATA";				//来电接听

//定时TIMER0用
unsigned int   timercount;			//50MS定时器,
unsigned char  timer_1S_cnt;
unsigned char  timer_10S_cnt;
unsigned char  timer_S_cnt;

unsigned char idata SystemBuf[RxIn];  //储存出口接收数据 
unsigned char Rx=0;
unsigned char jump=0;




//延时函数
void delayms(unsigned  int ii)//1ms延时函数
{
	unsigned int i,x;
	for (x=0;x<ii;x++)
	{
		for (i=0;i<100;i++);
	}
}
void delay(unsigned int i)
{
	while(i!=0)
	{
	    i--;
	}
}
//信号强度
void Signal()
{
	unsigned char k=0,q=0;

	Rx=0;
	sendstring(AT_CSQ);
	q=((SystemBuf[8]-48)*10+(SystemBuf[9]-48));
	//信号范围
	if		((q>18)&&(q<=31)) 	k=4;
	else if ((q>14)&&(q<=18)) 	k=3;
	else if	((q>10)&&(q<=14))	k=2;
	else if	((q>7 )&&(q<=10))	k=1;
	else if ((q>31)||(q<=7))	k=0;
	//信号条显示
}
/*********************************************************************
 ** 函数名称: strsearch ()
 ** 函数功能: 在指定的数组里连续找到相同的内容
 ** 入口参数: ptr2要查找的内容, ptr1当前数组
 ** 出口参数: 0-没有 找到   >1 查找到
 *********************************************************************/
unsigned char strsearch(unsigned char *ptr2,unsigned char *ptr1_at)//查字符串*ptr2在*ptr1中的位置
//本函数是用来检查字符串*ptr2是否完全包含在*ptr1中
//返回:  0  没有找到
//1-255 从第N个字符开始相同
{
	unsigned char i,j,k;
	unsigned char flag;
	if(ptr2[0]==0) return(0);
	flag=0;
	for(i=0,j=0;i<RxIn-2;i++)
	{
        	if(ptr1_at[i]==ptr2[j])
       		{	//第一个字符相同
        		for(k=i;k<RxIn-2;k++,j++)
        		{
        			if(ptr2[j]==0)//比较正确
        				return(i+1);               //返回值是整数，不含0
        			if(ptr1_at[k]!=ptr2[j]) break;
        		}
        		j=0;
        	}
	}
	return(0);
}

/*****************************************************************************************************************
//////////////////////函数void sendstring(uchar *p);实现功能:通过串口发送字符串//////////////////////////////////
*****************************************************************************************************************/
void sendstring(unsigned char *p)
{
  	while(*p)
  	{
  		sendchar(*p);
  		p++;
	}
  	sendchar(0x0D);
  	sendchar(0x0A);
}
/*****************************************************************************************************************
////////////////////////////////函数void Delay_ms(unsigned int i);实现功能:进行毫秒延时///////////////////////////////////
*****************************************************************************************************************/
void Delay_ms(unsigned int i)
{
    unsigned int j,k;
	for (k=0;k<12;k++)
	{
	    for(;i>0;i--)
		{
	    	for(j=0;j<125;j++)
	    	{;}
		}
	}
}
//---延时S------------------------------
void dmsec (unsigned int count)
{
	unsigned int i;
	while (count)
	{
        i =300;
		while (i>0) i--;
		count--;
    }
}
/************************************************************************************************
********************************系统初始化*******************************************************
************************************************************************************************/

/*****************************************************************************************************************
实现功能:对TC35进行启动,开启TC35功能详述:单片机上是跟TC35的管脚(/GPRS_EN)加时长至少为100ms的低电平信号
*****************************************************************************************************************/
void Start_GSM(void)
{
	unsigned char i;
	GPRS_EN=0;
	for(i=0;i<=200;i++) dmsec(30);
    GPRS_EN=1;
}
/******************************************************************************************************************
///////////////////函数void GSM_INIT(void);实现功能:初始化TC35模块///////////////////////////////
******************************************************************************************************************/
void GSM_init(void)
{ 
	Delay_ms(5000);
	Delay_ms(5000);
	while(1)
	{
		clear_SystemBuf();
		sendstring(AT);
		Delay_ms(5000);	
		if((SystemBuf[5]=='O')&&(SystemBuf[6]=='K'))	//判断是否模块初始化成功,成功的话模块会回复"OK"给单片机
        {												//如果单片机没有收到OK,就继续发送初始化指令/                 
			//LED1=~LED1;	break; 
        }
		Delay_ms(5000);
	}
	sendstring(ATI);//初始化
	Delay_ms(1000);
	sendstring(AT_IPR);//9600
	Delay_ms(1000);
	sendstring(ATE0V1);//关闭回显设置DCE为OK方式
	Delay_ms(1000);
	while(1)
	{
		clear_SystemBuf();
		sendstring(AT_CPIN);//查询SIM卡状态
		Delay_ms(5000);
		if((SystemBuf[18]=='O')&&(SystemBuf[19]=='K'))
		{ 
		//	LED1=~LED1;break;
		}
		Delay_ms(5000);//READY??
	}
	//GPRS初始化
	sendstring(AT_CDSNORIP);Delay_ms(5000);	
	sendstring(AT_CIPHEAD); Delay_ms(5000);		
}
/****************************************************************************************************************/
///////////////////////////////////////void clear_SystemBuf();清除串口接收缓冲区/////////////////////////////////
/****************************************************************************************************************/
void clear_SystemBuf()
{
	for(Rx=0;Rx<RxIn;Rx++)
	{
		SystemBuf[Rx]=0x00;
	}
	Rx=0;
}

