/****1602液晶屏程序区******昆新电子制作*******************
**********************************************************
****可定义区的4个参数可以改动
****
****用于显示两行字母与数字
****
********************************************************/
#include<reg52.h>
#include <intrins.h>
#include <absacc.h>
#define uchar unsigned char 					//定义unsigned char 为 uchar
#define uint  unsigned int
#define BUSY  0x80                               //lcd忙检测标志
//-----------可定义区--------------------------
#define DATAPORT P0                             //定义P0口为LCD通讯端口
sbit LCM_RS=P3^2;								 //数据/命令端
sbit LCM_RW=P3^3;								 //读/写选择端
sbit LCM_EN=P3^4;
//----------------------------------------------
void initLCM( void);               											//LCD初始化子程序
void lcd_wait(void);              											//LCD检测忙子程序
void WriteCommandLCM(uchar WCLCM,uchar BusyC);                              //写指令到ICM子函数
void WriteDataLCM(uchar WDLCM);                                             //写数据到LCM子函数
void DisplayOneChar(uchar X,uchar Y,uchar DData);                           //显示指定坐标的一个字符子函数
void DisplayListChar(uchar X,uchar Y,uchar code *DData);                    //显示指定坐标的一串字符子函数
/***************一微秒延时子程序******************/
void delayus(uint delay){
	while(delay--);
}
/***************一毫秒延时子程序******************/
void delay_LCM(uint delay){
	while(delay--)
		delayus(149);
}
/**********写指令到LCM子函数************/
void WriteCommandLCM(uchar WCLCM,uchar BusyC)
{
    if(BusyC)lcd_wait();
	DATAPORT=WCLCM;
    LCM_RS=0;                   // 选中指令寄存器
    LCM_RW=0;               	// 写模式	
    LCM_EN=1;
	_nop_();
	_nop_();
	_nop_();
    LCM_EN=0;
}
/**********写数据到LCM子函数************/
void WriteDataLCM(uchar WDLCM)
{
    lcd_wait( );                //检测忙信号
	DATAPORT=WDLCM;
    LCM_RS=1;                   // 选中数据寄存器
    LCM_RW=0;           	    // 写模式
    LCM_EN=1;
    _nop_();
	_nop_();
	_nop_();
    LCM_EN=0;
}
/***********lcm内部等待函数*************/
void lcd_wait(void)
{
    DATAPORT=0xff;	
	LCM_EN=1;
    LCM_RS=0;   			
    LCM_RW=1;   			
    _nop_();
    while(DATAPORT&BUSY)
	{  LCM_EN=0;
	   _nop_();
	   _nop_();
	   LCM_EN=1;
	   _nop_();
	   _nop_();
	   }
   	LCM_EN=0;
	
}
/**********LCM初始化子函数***********/
void initLCM( )
{
	DATAPORT=0;	
	delay_LCM(15);
	WriteCommandLCM(0x38,0);    //三次显示模式设置，不检测忙信号
    delay_LCM(5);
    WriteCommandLCM(0x38,0);
    delay_LCM(5);
    WriteCommandLCM(0x38,0);
    delay_LCM(5);

    WriteCommandLCM(0x38,1);    //8bit数据传送，2行显示，5*7字型，检测忙信号
    WriteCommandLCM(0x08,1);    //关闭显示，检测忙信号
    WriteCommandLCM(0x01,1);    //清屏，检测忙信号
    WriteCommandLCM(0x06,1);    //显示光标右移设置，检测忙信号
    WriteCommandLCM(0x0c,1);    //显示屏打开，光标不显示，不闪烁，检测忙信号
}
/****************显示指定坐标的一个字符子函数*************/
void DisplayOneChar(uchar X,uchar Y,uchar DData)
{
    Y&=1;
    X&=15;
    if(Y)X|=0x40;               //若y为1（显示第二行），地址码+0X40
    X|=0x80;                    //指令码为地址码+0X80
    WriteCommandLCM(X,0);
    WriteDataLCM(DData);
}
/***********显示指定坐标的一串字符子函数***********/
void DisplayListChar(uchar X,uchar Y,uchar *DData)
{
    uchar ListLength=0;
    Y&=0x01;
    X&=0x0f;
    while(X<16)
    {
        DisplayOneChar(X,Y,DData[ListLength]);
        ListLength++;
        X++;
    }
}
/**************************主函数********************************************/
void LCD(uchar Frist_line[],uchar Second_line[])
{
EA=0;  //引脚问题需要关中断

  //P1=0xff;                                   				  //初始化p1口，全设为1
	delay_LCM(50);	                                          //延时500ms启动
	initLCM( );                               			  	  //LCD初始化
	DisplayListChar(0,0,Frist_line);	
	DisplayListChar(0,1,Second_line);
EA=1;//开中断
}
void LCD1(uchar Frist_line[])
{
EA=0;  //引脚问题需要关中断

  //P1=0xff;                                   				  //初始化p1口，全设为1
	delay_LCM(50);	                                          //延时500ms启动
	initLCM( );                               			  	  //LCD初始化
	DisplayListChar(0,0,Frist_line);	
//	DisplayListChar(0,1,Second_line);
EA=1;//开中断
}
