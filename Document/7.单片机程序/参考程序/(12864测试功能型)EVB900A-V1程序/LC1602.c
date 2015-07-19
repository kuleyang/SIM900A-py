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

#include "C8051F340.H"     //调用头文件（单片机内部的寄存器定义）
/******本段为硬件I/O口定义********/

sbit LCM_RS       =   P0^2; //1602LCD管脚定义
sbit LCM_RW       =   P1^3; //1602LCD管脚定义
sbit LCM_E        =   P1^4; //1602LCD管脚定义


#define  LCD_DATA   P1   //LCD DATA
#define  LCD_BUSY   0x80 // 用于检测LCD的BUSY标识（本程序中用的延时，未检测） 
//延时
void Delay1(void)
{
 unsigned int TempCyc = 100;
 while(TempCyc--);
}

//5ms延时
void Delay5Ms(void)
{
 unsigned int TempCyc = 5552;
 while(TempCyc--);
}

//400ms延时
void Delay400Ms(void)
{
 unsigned char TempCycA = 5;
 unsigned int TempCycB;
 while(TempCycA--)
 {
  TempCycB=7269;
  while(TempCycB--);
 };
}

//读数据
unsigned char ReadDataLCM(void)
{unsigned char tmp;
 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
// P4=0x3f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=1;

 //P4=0x7f;
 LCM_E =0;
 LCM_RW=1;
 LCM_RS=1;
 //P4=0xff;
 LCM_E =1;
 LCM_RW=1;
 LCM_RS=1;
 Delay1(); Delay1();
 //LCM_E = 0;
 //LCM_E = 1;
 tmp=LCD_DATA;
 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
 return(tmp);
 
}

//读状态
unsigned char ReadStatusLCM(void)
{unsigned char tmp;
 LCD_DATA = 0xFF;

 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
 //P4=0x5f;
 LCM_E =0;
 LCM_RW=1;
 LCM_RS=0;
 //P4=0xdf;
 LCM_E =1;
 LCM_RW=1;
 LCM_RS=0;
 Delay1(); Delay1();
  
 while (LCD_DATA & LCD_BUSY); //检测忙信号 
 tmp=LCD_DATA;
 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
 return(tmp);
 
}


//写数据
void WriteDataLCM(unsigned char WDLCM)
{
 ReadStatusLCM(); //检测忙
 LCD_DATA = WDLCM;
 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
 //P4=0x3f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=1;
 //P4=0xbf;
 LCM_E =1;
 LCM_RW=0;
 LCM_RS=1;
 Delay1(); Delay1();

 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
}

//写指令
void WriteCommandLCM(unsigned char WCLCM,BuysC) //BuysC为0时忽略忙检测
{
 if (BuysC) ReadStatusLCM(); //根据需要检测忙
 LCD_DATA = WCLCM;
 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
 //P4=0x9f;
 LCM_E =1;
 LCM_RW=0;
 LCM_RS=0;
 Delay1(); Delay1();
 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
} 

void LCMInit_1602(void) //LCM初始化
{
 LCD_DATA = 0;
 Delay5Ms();
 Delay5Ms();
 Delay5Ms();
 Delay5Ms();
 WriteCommandLCM(0x38,0); //三次显示模式设置，不检测忙信号
 Delay5Ms();
 WriteCommandLCM(0x38,0);
 Delay5Ms();
 WriteCommandLCM(0x38,0);
 Delay5Ms();
 WriteCommandLCM(0x38,1); //显示模式设置,开始要求每次检测忙信号
 WriteCommandLCM(0x08,1); //关闭显示
 WriteCommandLCM(0x01,1); //显示清屏
 WriteCommandLCM(0x06,1); // 显示光标移动设置
 WriteCommandLCM(0x0C,1); // 显示开及光标设置
}

//按指定位置显示一个字符
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 Y &= 0x1;
 X &= 0xF; //限制X不能大于15，Y不能大于1
 if (Y) X |= 0x40; //当要显示第二行时地址码+0x40;
 X |= 0x80; // 算出指令码
 WriteCommandLCM(X, 0); //这里不检测忙信号，发送地址码
 WriteDataLCM(DData);
}

//按指定位置显示一串字符
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char len, unsigned char *DData)
{
 unsigned char ListLength;

 Y &= 0x01;
 X &= 0x0F; //限制X不能大于15，Y不能大于1
 for (ListLength=0;ListLength<len;ListLength++) //若到达字串尾则退出
  {
   if (X <= 0x0F) //X坐标应小于0xF
    {
     DisplayOneChar(X, Y, DData[ListLength]); //显示单个字符
     //ListLength++;
     X++;
    }
  }
}



