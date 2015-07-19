/**************************************************************************
 * EVB900A-V1开发板实验程序                                              *
 * 版本： (C) Copyright 2010,SC.etc,DCL,CHM    All Rights reserved.      *
 * 网站： mcuep.taobao.com                            *
 * 邮箱： dcl0@sina.com                                                  *
 * 作者： DCL,CHM
 * 来源： 思创电子  Xa' STrong ELECTRONICS CO.,LTD      *
 *【版权】Copyright(A) 思创电子 mcuep.taobao.com  All Rights Reserved    *
 *【声明】此程序仅用于学习与参考，引用请注明版权和作者信息！             *
 *************************************************************************/
#include "C8051F340.H"
#include "intrins.h"
#include "SIM900a.h"
#include "lc1602.h"
#include "lc12864.h"
#include "1.h"
#include "DS18B20.h"

sbit SP485_RS     =   P0^3;//485_RS

sbit SPK          =   P2^5;//蜂鸣器
sbit GPRS_EN   	  =   P2^6; //GPRS 使能

sbit JDQ_C1       =   P3^3;//继电器1
sbit JDQ_C2       =   P3^4;//继电器2
sbit JDQ_C3       =   P3^5;//继电器3
sbit JDQ_C4       =   P3^6;//继电器4



sbit KEY_C1       =   P0^6;//KEY1
sbit KEY_C2       =   P0^7;//KEY2
sbit KEY_C3       =   P1^0;//KEY3

sbit LED_C1       =   P1^1;//LED1
sbit LED_C2       =   P1^2;//LED2

///////////////////DTMF设置//////////////////////////////
//需要配置外部中断
sbit DTMF_DV      =   P2^4;//高电平有效(接收到DTMF)
sbit DTMF_OE   	  =   P1^7;//高电平有效(解码成功)
//D0-P2.0  D1=P2.1  D2=P2.2  D3=P2.3

///////////串口1部分不能位操作/////////////////
#define IsTI1 (SCON1&0x02)
#define IsRI1 (SCON1&0x01)
#define ClrTI1() SCON1&=~0x02;
#define ClrRI1() SCON1&=~0x01;
///////////控制器配置信息操作/////////////////
unsigned char xdata SIM_NUM[20]; //存放SIM卡号码(20位)
unsigned char xdata Rx=0;
unsigned char xdata SIM900a_BUF[70];//SIM900a储存出口接收数据
unsigned char xdata GSM_BUF[300];//SIM900a储存短信数据 
///////////LCD指示灯操作/////////////////
//#define LCM_Data  P1   //数据线
//#define Busy    0x80   //用于检测LCM状态字中的Busy标识

////////////////////////LCD1602液晶欢迎显示/////////////////////////////////////////////////////////////
unsigned char code uctech[] = {"Welcome!"};
unsigned char code net[] = {"mcuep.taobao.com"};

////////////////////////GPS定位测试实验/////////////////////////////////////////////////////////////////
unsigned char code net2[]         = {"GPS TEST"};
unsigned char code Lat[]          = {"LAT:"};            //纬度头标
unsigned char code Longt[]        = {"LG:"};             //经度头标
unsigned char code NO_SIM_LINK[]  = {"NO_MODUL_LINK!!"};  //900a模块与单片机串口未联接，可能原因:1.跳线冒未断接(右); 2.模块未启动(调试程序中)

unsigned char code net3[]         = {"卫星定位实验"};
unsigned char code gps_tm[]       = {"时间:"}; 
unsigned char code gps_lg[]       = {"经度:"};     
unsigned char code gps_long[]     = {"纬度:"};  

////////////////////////DS18B20温度测试实验//////////////////////////////////////////////////////////////
unsigned char code net4[]     = {"温度控制实验"};
unsigned char code tm_up[]    = {"温度上限:"};
unsigned char code tm_down[]  = {"温度下限:"};
unsigned char tm_up_point=40   ;   //温度上限
unsigned char tm_down_point=25 ;   //温度下限
unsigned char code tm_p[]     = {"当前温度:"};
unsigned char  DS18B20_sn[8];  //4个器件的序列号，先读出单个序列号后填上才可以读取温度
////////////////////////GPS操作/////////////////////////////////////////////
unsigned char xdata GPS_head_buf[5];//GPS命令字存放
unsigned char xdata head__point=0;
unsigned char xdata GPS_data_buf[60];//GPS数据信息存放
unsigned char xdata data__point=0;
unsigned char xdata GPS_Flg1=0;//GPS接收命令标志位
unsigned char xdata GPS_Flg2=0;//GPS接收信息标志位
unsigned char xdata GPS_data_ok_flg=0;//GPS数据接收一次完成标志
unsigned char xdata GPS_inf_state=0; //所接收报文的有效性 A:可用   V:不可用
unsigned char xdata GPS_can_use=0;   //1:主程序可用 0:不可用 
///////////蜂鸣器操作/////////////////
#define spk_1  SPK=0;	delay_10ms(20);      SPK=1; delay_10ms(30);
#define spk_2  SPK=0;	delay_10ms(40);      SPK=1; delay_10ms(100);
#define spk_3  SPK=0;	delay_10ms(50);      SPK=1; delay_10ms(400);
#define spk_4  SPK=0;	delay_10ms(60);      SPK=1; delay_10ms(200);
//************硬件初始化部份**********************************************
void Port_IO_Init()
{    	 
	P0MDOUT   = 0xC0;
	//P1MDOUT   = 0xFF;
	//P2MDOUT   = 0x01;
	P3MDOUT   = 0x00;
	P4MDOUT   = 0x00;//P4口不能按位操作
    XBR0      = 0x01;
    XBR1      = 0x40;
    XBR2      = 0x01;
}
//************************************************************************
//UART0:使用time1 115200BPS; UART1:使用time1 19200BPS
//************************************************************************/
void Timer_Init()
{
    TCON      = 0x40;
    TMOD      = 0x20;
    CKCON     = 0x08;
    TH1       = 0x30;
}
//************************************************************************
//串口初始化，配置：UART0=115200BPS,UART1=9600BPS,使能串口接收
//************************************************************************/
void UART_Init()
{
    SCON0     = 0x10;

    SBRLL1    = 0x1E;  //19200bps     注：需要19200请打开
    SBRLH1    = 0xFB;  //19200bps

	//SBRLL1    = 0x3C;    //9600bps      注：需要9600请打开
    //SBRLH1    = 0xF6;    //9600bps

    SCON1     = 0x10;
    SBCON1    = 0x43;
}

//************************************************************************
//系统时钟初始化，48M 选择外部晶体振荡器
//************************************************************************/
void Oscillator_Init()
{
    int i = 0;
    OSCICN    = 0x83;
    CLKMUL    = 0x80;
    for (i = 0; i < 20; i++);    // Wait 5us for initialization
    CLKMUL    |= 0xC0;
    while ((CLKMUL & 0x20) == 0);
    CLKSEL    = 0x03;

}
//************************************************************************
//开串口0/串口1/总中断
//************************************************************************/
void Interrupts_Init()
{
    EIE2      = 0x02;
    IE        = 0x90;
}
void Init_Device(void)
{
    PCA0MD &= ~0x40;//关闭看门狗定时器 
    Timer_Init();
    UART_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
void Delay_ms(unsigned int i)
{
    unsigned char j;
	unsigned char k;
	for (k=0;k<12;k++)
	{
	    for(;i>0;i--)
		{
	    	for(j=0;j<125;j++){;}
	    	
		}
	}
}
void delay_1ms(unsigned char _1ms)// Delay 1ms
{
    unsigned char i;
    unsigned int  j;

	for (i=0;i<_1ms;i++)
	{
	   for(j=0;j<2400;j++)
	   ;
	}
}
void delay_10ms(unsigned int _10ms)// Delay 10ms
{
  unsigned int  i;
  for (i=0;i<_10ms;i++)
  {
    delay_1ms(10);
  }

}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void sendchar(unsigned char ch)
{ 
    EA=0;
	SBUF0=ch;
	while(TI0==0);
	TI0=0;
	EA=1;
}
/****************************************************************************************************************/
///////////////////////////////////////单片机启动SIMCOM模块/////////////////////////////////
/****************************************************************************************************************/
void Start_GSM(void)
{

	GPRS_EN=0;
    Delay_ms(60000);Delay_ms(60000);
    //GPRS_EN=1;
	Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
}
/****************************************************************************************************************/
///////////////////////////////////////void clear_SIM900a_BUF();清除串口接收缓冲区/////////////////////////////////
/****************************************************************************************************************/
void clear_SIM900a_BUF()
{
    unsigned char i;
	for(i=0;i<70;i++)
	{
		SIM900a_BUF[i]=0;
	}
	Rx=0;
}
void clear_GSM_BUF()
{
    unsigned char i;
	for(i=0;i<100;i++)
	{
		GSM_BUF[i]=0;
	}
	Rx=0;
}
//////////////////////////////////////////////////////////////////////////////
//检查SIM900a模块工作状态，以及读取SIM卡号放入-->SIM_NUM
//////////////////////////////////////////////////////////////////////////////
void GSM_check(void)
{ 
    unsigned char  i,j;
	while(1)
	{
	    clear_SIM900a_BUF();
	    sendstring(AT);
	  	Delay_ms(10000);
		j++;
	    if((SIM900a_BUF[6]=='O')&&(SIM900a_BUF[7]=='K'))//判断是否模块初始化成功,成功的话模块会回复"OK"给单片机------>用户根据仿真器监控接收数组，改动回令OK数据存放位置
        {												//如果单片机没有收到OK,就继续发送初始化指令/ 
           clear_SIM900a_BUF();
           sendstring(AT_CCID);Delay_ms(10000);	//查SIM卡号  89860 0C226 10A74 59948(20位ASCII码表示)        
		   for(i=11;i<31;i++)
	       {
			 SIM_NUM[i-11]=SIM900a_BUF[i];
	       }
		   spk_1;//上电蜂鸣器叫
		   break;	
        }
		if(j>15)//提示:SIM900B模块与单片机通讯未联接
		{      
	        //LCMInit_1602(); //LCD1602液晶初始化
            //DisplayListChar(0, 0,13, NO_SIM_LINK);   //在第1行显示"NO-NO_SIM_LINK-!!"

			LCMInit_12864(); //LCD12864液晶初始化
            Display_String(0,1,NO_SIM_LINK,15);
            Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
            break;
		}
	}
}
//*************************SIM300程序******************************************************/
//*****************************************************************************************/
/*********************************************************************
//本函数是用来检查字符串*STRING_much是否完全包含在"@:"字符
//返回:  0  没有找到
//1-255 从第N个字符开始相同
 *********************************************************************/
unsigned char strsearch_head(unsigned char *STRING_much)//查字符串*STRING_little在*STRING_much中的位置
{
	unsigned char i;
	for(i=0;i<300;i++)
	{
        	if(STRING_much[i]==0x40)//第一个字符相同
       		{	
				if(STRING_much[i+1]==0x3A) //比较正确
				{
       			   return(i+2); //返回值是整数
				}
        	}
	}
	return(0);
}
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
void send_string(unsigned char *p,unsigned char num)
{
   unsigned char xdata i;
   for(i=0;i<num;i++)
   {
	  sendchar(*p);
      p++;
   } 
  	sendchar(0x0D);
  	sendchar(0x0A);  
}
void send_matrix(unsigned char *p,unsigned char num)
{
   unsigned char xdata i;
   for(i=0;i<num;i++)
   {
	  sendchar(*p);
      p++;
   }   
}
//控制器485发送程序
//串口1 19200bps/或者9600bps：   根据UART_Init()说明进行第二串口波特率选择
/////////////////////////////////////////////////////////////////////////
void max485_send(unsigned char XXX)
{
    EA=0;
	SP485_RS=1;
    SBUF1=XXX;
	while(!IsTI1);
	ClrTI1();
	EA=1;
	SP485_RS=0;
}
///////////////DS18B20/////////////////////////////
void Read_B20_SN(void) //读取序列号
{ 
  unsigned char i;

  Init_DS18B20(); //复位 
  DS18B20_1us(100); //发读取电源工作方式指令 
  write_byte(0x33); //发送读序列号子程序 
  for(i=0;i<8;i++) //因为是8个字节，所以要读取8次
  {
    DS18B20_sn[i]=read_byte(); //读取序列号
  }
}
///////////////////////////////////////////////////////////////////////
// 主程序开始
///////////////////////////////////////////////////////////////////////
void main(void)
{
    unsigned char code table[]={"0123456789ABCDEF"}; //用于1602显示

	unsigned char GPS_hour;  //GPS定位实验:中间变量
	
	unsigned int TT;         //DS18B20实验:温度值	
	unsigned char table_temp[6];   //DS18B20实验:中间变量
	unsigned char table_temp2[4];  //DS18B20实验:中间变量
	unsigned char table_temp3[4];  //DS18B20实验:中间变量
	unsigned char tab1,tab2,tab3;  //DS18B20实验:中间变量
	unsigned char tab_C[2]="℃";   //DS18B20实验:℃符号

	unsigned char DTMF_data;  //DTMF解码值--------->双音多频实验相关变量

    Init_Device(); //晶振。管脚初始化
	Start_GSM();//GSM上电
	GSM_check();///检测单片机与模块是否连接正常。 注：跳线选择单片机与模块连接---,如果选择模块与计算机通讯功能，请将些语句屏蔽
	  //////////////////////LCD1602液晶显示测试程序---START//////////////////////////
	  /////////A套件程序
	  
	  LCMInit_1602(); //LCM1602初始化
	  Delay5Ms();Delay5Ms();Delay5Ms();//LCM初始化延时
      DisplayListChar(3, 0,8, uctech);   //在第一行显示"Welcome!"
      DisplayListChar(0, 1,16, net);     //在第二行显示"mcuep.taobao.com"
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  DisplayListChar(0, 1,16, table);     //在第二行显示"0123456789"
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  //////////////////////LCD1602液晶显示测试程序---END////////////////////////////
    while(1)
	{ 
	     //////////////////////LCD12864液晶显示测试程序---START//////////////////////
		 ////////////////1:图型显示实验//////////////////////////
		  initina2();            //调用LCD显示图片(扩展)初始化程序
          DisplayGraphic(pic2);  //显示图片2
		  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
          
		  ///////////////2:汉字显示实验1//////////////////////////
          initinal();   		 //调用LCD字库初始化程序
          delay(200);            //大于100uS的延时程序
          lcd_mesg(IC_DAT);      //显示中文汉字1
          Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);

		  ///////////////3:汉字显示实验2//////////	  
          LCMInit_12864();   	 //调用LCD字库初始化程序
          delay(200);            //大于100uS的延时程序 
          lcd_mesg(IC_DAT2);     //显示中文汉字2
          Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
		  


          ///////////////4:测温显示实验1--<指定位置显示>//////////////
		  LCMInit_12864();//初始化液晶且清屏
		  Read_B20_SN();//读DS18B20序列号
		  while(1)
		  {

		  Display_String(1,0,net4,12);     //"温度控制实验"
		  Display_String(0,1,tm_up,10);    //"温度上限:"
		  Display_String(0,2,tm_down,10);  //"温度下限:"
          Display_String(0,3,tm_p,10);     //"当前温度:"


          table_temp2[0]=(tm_up_point/10)+0x30;       //温度上限十位
          table_temp2[1]=(tm_up_point%10)+0x30;       //温度上限个位
		  table_temp2[2]=tab_C[0];                    //显示温度符号"℃"
		  table_temp2[3]=tab_C[1];                    //显示温度符号"℃"
          table_temp3[0]=(tm_down_point/10)+0x30;     //温度下限十位
          table_temp3[1]=(tm_down_point%10)+0x30;     //温度下限个位
          table_temp3[2]=tab_C[0];                    //显示温度符号"℃"
          table_temp3[3]=tab_C[1];                    //显示温度符号"℃"
          Display_String(5,1,table_temp2,4);           //LCD12864液晶上显示设定温度上限
          Display_String(5,2,table_temp3,4);           //LCD12864液晶上显示设定温度下限

		  TT=Read_Temperature();//读温度
		  ////////////以下为拆分温度值且变为ASCII码进行液晶显示//////
          tab1 = TT / 100;   
          table_temp[0] = tab1 + 0x30;     // 十位   
          tab2= TT / 10 - tab1*10;   
          table_temp[1] = tab2 + 0x30;     //个位   
          table_temp[2] = '.';  
          tab3 = TT - tab1*100 - tab2*10;   
          table_temp[3] = tab3 + 0x30;     //小数点一位   

		  table_temp[4] =tab_C[0];//显示温度符号"℃"
		  table_temp[5] =tab_C[1];//显示温度符号"℃"         
		  Display_String(5,3,table_temp,6);//LCD12864液晶上显示温度值
		    
		  }
          ///////////////5:汉字显示实验3--<GPS定位实验且数据12864液晶显示>//////////////
          LCMInit_12864();//初始化液晶且清屏
		  while(1)
		  {
            Display_String(1,0,net3,12);    //"卫星定位实验"
		    Display_String(0,1,gps_tm,6);   //"时间:"
		    Display_String(0,2,gps_long,6); //"纬度:"
			Display_String(0,3,gps_lg,6);   //"经度:"
	     
			if(GPS_data_ok_flg==1)//接收到GPS信息$GPRMC指令
	        {
              GPS_data_ok_flg=0;
              GPS_inf_state=GPS_data_buf[12];//'A'=0x41-->卫星报文可用  'V'=0x56-->卫星报文不可用
		      if(GPS_inf_state==0x41)GPS_can_use=1;
              ////////////////////定位有效后：解析GPS数据///////////////////////////////////////
		      if(GPS_can_use==1)//GPS数据定位有效
		      {
			     GPS_can_use=0; //清除GPS定位状态
		   	    //////////////////////GPS数据接收显示/////////////////////////////////////////////
				//////////////////////时间显示/////////////////////////////////////
                GPS_hour=(GPS_data_buf[1]-0x30)*10+(GPS_data_buf[2]-0x30)+8;//＋8区时间计算,且加入":"
		        if(GPS_hour>23)GPS_hour=GPS_hour-24;
                GPS_data_buf[0]=(GPS_hour/10)+0x30;
			    GPS_data_buf[1]=(GPS_hour%10)+0x30;
				GPS_data_buf[2]=0x3a;//:
				GPS_data_buf[7]=GPS_data_buf[6];
				GPS_data_buf[6]=GPS_data_buf[5];
				GPS_data_buf[5]=0x3a;//:	
		        Display_String(3,1,GPS_data_buf,8);   //卫星时间:--应该+8转成东八区时(北京时间)
                //////////////////////纬度显示/////////////////////////////////////////////
		        Display_String(3,2,GPS_data_buf+14,9); //纬度
                //////////////////////经度显示///////////////////////////////////////////// 
		        Display_String(3,3,GPS_data_buf+26,9); //经度
			  }
			}
		  }
 

		  

      //////////////////////LCD12864液晶显示测试程序---END/////////////////////////
      ///////////////////////////程序运行过程中测试其它硬件是否正常工作//////////////
	  //1。LED灯是否正常 2.四路继电器吸合是否正常 3.四路继电器指示灯是否正常  
	  //4.蜂鸣器工作     5.485总线是否成功输出(端子座定义--->5:485_A , 6:485_B)
	  LED_C1=0;spk_1; max485_send(0x0A);JDQ_C1=0;
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  LED_C1=1;Delay_ms(60000);
	  JDQ_C1=1;Delay_ms(60000);Delay_ms(60000);

	  LED_C2=0;spk_1; max485_send(0x0B);JDQ_C2=0;
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  LED_C2=1;Delay_ms(60000);JDQ_C2=1;Delay_ms(60000);Delay_ms(60000);

	  P4=0xFE; spk_1; max485_send(0x0C);JDQ_C3=0;
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  P4=0xFF;Delay_ms(60000);JDQ_C3=1;Delay_ms(60000);Delay_ms(60000);

	  P4=0xFD;spk_1; max485_send(0x0D);JDQ_C4=0;
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  P4=0xFF;Delay_ms(60000);JDQ_C4=1;Delay_ms(60000);Delay_ms(60000);
	}
			
}
/*
          //////////////5:DTMF实验//////////////
		  if(DTMF_DV==1)//接收到成功的DTMF信号输出高电平
          {
		    DTMF_OE =1;spk_1;
			DTMF_data=P2&0x0F;
		  }
          if(DTMF_data==1)//接收到成功的DTMF信号输出高电平
          {
           LED_C1=0;
		   spk_1;
          }
          if(DTMF_data==8)//解码成功输出高电平
		  {
            LED_C2=0;
			spk_1;
		  }
           if(DTMF_data==5)//解码成功输出高电平
		  {
            LED_C1=1;
		    LED_C2=1;

		  }*/	
///////////////////////////////////////////////////////////////////////
//UART0,115200BPS 单片机中断接收SIM900a串口通讯中断处理程序
///////////////////////////////////////////////////////////////////////
void SIM900a_UART_REC(void) interrupt INTERRUPT_UART0 using 3
{
  	EA=0;
  	if(RI0)
	{	
          SIM900a_BUF[Rx]=SBUF0;  //短信专用BUF
	      if(Rx==70)Rx=0;
	      else
	      Rx++;
	}
    RI0=0;
    EA=1;
}
//////////////////////////////////////////////////////////////////////
//UART1,配置为：19200BPS 单片机中断方式接收GPS定位报文程序 
//解析：$GPRMC电文
///////////////////////////////////////////////////////////////////////
void isr_UART_1(void) interrupt INTERRUPT_UART1 using 3
{
	unsigned char tmp;
	EA=0;
  	if(IsRI1)
	{	
	  tmp=SBUF1;
	  if(tmp=='$')//判断报文针头"$"
	  {
	     GPS_Flg1=1;
		 goto  over_1;
		 
	  }
	  if(GPS_Flg2==1)
	  {
	     if(GPS_head_buf[0]=='G'&&GPS_head_buf[1]=='P'&&GPS_head_buf[2]=='R'&&GPS_head_buf[3]=='M'&&GPS_head_buf[4]=='C')//接收报文信息GPRMC
		 {
		   GPS_data_buf[data__point]=tmp;
		   data__point++;
		   if(data__point==60)
		   {
		     data__point=0;
		     GPS_Flg2=0;//清除接收信息
			 GPS_data_ok_flg=1;//GPS数据接收一次完成标志
		   }
		 }
		 else
         {
		   GPS_Flg2=0;//清除接收信息
		 }
	  }
	  if(GPS_Flg1==1)//接收报文命令字
	  {
	     
         GPS_head_buf[head__point]=tmp;
		 head__point++;
		 if(head__point==5)
		 {
		   head__point=0;
		   GPS_Flg1=0;//清除接收报文命令字
		   GPS_Flg2=1;
		 }
	  }
over_1:
       ClrRI1(); //RI1=0;	
	}
    EA=1;
}

		  
