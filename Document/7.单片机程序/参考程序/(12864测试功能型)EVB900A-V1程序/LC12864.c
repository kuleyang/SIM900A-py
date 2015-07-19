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
#include "intrins.h"
#include <string.h>


unsigned char code IC_DAT[];
unsigned char code pic1[];
unsigned char code pic2[];
unsigned char code pic3[]; 


sbit LCM_RS       =   P0^2; //1602/12864LCD管脚定义
sbit LCM_RW       =   P1^3; //1602/12864LCD管脚定义
sbit LCM_E        =   P1^4; //1602LCD管脚定义
sbit LCM_PSB      =   P1^5; //串并口选择位
sbit LCM_RES      =   P1^6; //RESET

////////////////////////////LCD12864液晶程序/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void delay(unsigned int ms)       //延时多长时间？？？
{
  unsigned char i;
  while(ms--)
  {
    for(i = 0; i< 250; i++)
    {
      _nop_();_nop_();_nop_();_nop_();
    }
  }
}
/********************************************************************
* 名称 : TransferData()
* 功能 : 向12864液晶写数据或命令
* 输入 : data1,DI=0传送命令,当DI=1,传送数据.
* 输出 : 无
***********************************************************************/
void TransferData(unsigned char data1,bit DI) //传送数据或者命令,当DI=0是,传送命令,当DI=1,传送数据.
{
     LCM_RW=0;
     LCM_RS=DI;
     delay(1);
	 P1=data1;
     LCM_E=1;
     delay(1);
     LCM_E=0;
}
/********************************************************************
* 名称 : initinal()
* 功能 : 字库初始化
* 输入 : 无
* 输出 : 无
***********************************************************************/
void initinal(void) //LCD字库初始化程序
{
           delay(100);            //大于40MS的延时程序
           LCM_PSB=1;             //设置为8BIT并口工作模式
           delay(5);              //延时
           LCM_RES=0;             //复位
           delay(5);              //延时
           LCM_RES=1;             //复位置高
           delay(10);
           TransferData(0x30,0);  //Extended Function Set :8BIT设置,RE=0: basic instruction set, G=0 :graphic display OFF
           delay(100);            //大于100uS的延时程序
           TransferData(0x30,0);  //Function Set
           delay(30);             ////大于37uS的延时程序
           TransferData(0x08,0);  //Display on Control
           delay(100);            //大于100uS的延时程序
           TransferData(0x10,0);  //CuLCM_RSor Display Control光标设置
           delay(100);            //大于100uS的延时程序
           TransferData(0x0C,0);  //Display Control,D=1,显示开
           delay(100);            //大于100uS的延时程序
           TransferData(0x01,0);  //Display Clear
           delay(10);             //大于10mS的延时程序
           TransferData(0x06,0);  //Enry Mode Set,光标从右向左加1位移动
           delay(100);            //大于100uS的延时程序
 }

/********************************************************************
* 名称 : initina2()
* 功能 : 图形显示初始化
* 输入 : 无
* 输出 : 无
***********************************************************************/
void initina2(void) //LCD显示图片(扩展)初始化程序
{
           delay(100);            //大于40MS的延时程序
           LCM_PSB=1;             //设置为8BIT并口工作模式
           delay(5);              //延时
           LCM_RES=0;             //复位
           delay(5);              //延时
           LCM_RES=1;             //复位置高
           delay(5);

           TransferData(0x36,0);  //Extended Function Set RE=1: extended instruction
           delay(100);            //大于100uS的延时程序
           TransferData(0x36,0);  //Extended Function Set:RE=1: extended instruction set
           delay(20);             ////大于37uS的延时程序
           TransferData(0x3E,0);  //EXFUNCTION(DL=8BITS,RE=1,G=1)
           delay(100);            //大于100uS的延时程序
           TransferData(0x01,0);  //CLEAR SCREEN
           delay(100);            //大于100uS的延时程序
}


/********************************************************************
* 名称 : lcd_mesg()
* 功能 : 主函数
* 输入 : 无
* 输出 : 无
***********************************************************************/
void lcd_mesg(unsigned char code *adder1)
{
	unsigned char i;
    
    TransferData(0x80,0);  //Set Graphic Display RAM AddLCM_RESs
    delay(200);
	for(i=0;i<32;i++)
	{
	  	TransferData(*adder1,1);
	   	adder1++;
	}

    TransferData(0x90,0);  //Set Graphic Display RAM AddLCM_RESs
    delay(100);
	for(i=32;i<64;i++)
	{
	  	TransferData(*adder1,1);
	   	adder1++;
	}
}


void LCMInit_12864(void)
{
  delay(100);            //大于40MS的延时程序
  LCM_PSB=1;             //设置为8BIT并口工作模式
  delay(5);              //延时
  LCM_RES=0;             //复位
  delay(5);              //延时
  LCM_RES=1;             //复位置高
  delay(10);
  TransferData(0x30,0);
  delay(100);            //大于100uS的延时程序
  TransferData(0x30,0); //2次显示模式设置，不用判忙
  delay(30);             ////大于37uS的延时程序
  TransferData(0x0C,0); //开显示及光标设置
  delay(100);            //大于100uS的延时程序
  TransferData(0x01,0); //显示清屏
  delay(10);            //大于100uS的延时程序
  TransferData(0x06,1); //显示光标移动设置
  delay(100);            //大于100uS的延时程序

}
/********************************************************************
* 名称 : Main()
* 功能 : 在指定位置显示数据
* 输入 : X:(0-7)横坐标,Y:(0-3)纵坐标,string:要显示的数据,num:数据长度(1个汉字占2个字节)
* 输出 : 无
***********************************************************************/
void Display_String(unsigned char X,unsigned char Y,unsigned char *string,unsigned char num)//显示字符串
{
  unsigned char addr,i;
 
  if(Y==0)
  addr=0x80;
  else if(Y==1)
  addr=0x90;
  else if(Y==2)
  addr=0x88;
  else if(Y==3)
  addr=0x98;
  addr=addr+X;
  TransferData(addr,0);
  for(i=0;i<num;i++)
  {
    TransferData(*string++,1);
  }
}

/********************************************************************
* 名称 : Main()
* 功能 : 主函数
* 输入 : 无
* 输出 : 无
***********************************************************************/
void DisplayGraphic(unsigned char code *adder)
{

   int i,j;
//*******显示上半屏内容设置
   for(i=0;i<32;i++)              //
    { 
      TransferData((0x80 + i),0); //SET  垂直地址 VERTICAL ADD
      TransferData(0x80,0);       //SET  水平地址 HORIZONTAL ADD
      for(j=0;j<16;j++)
       {
         TransferData(*adder,1);
         adder++;
       }
    }	   
//*******显示下半屏内容设置
   for(i=0;i<32;i++)              //
    {
      TransferData((0x80 + i),0); //SET 垂直地址 VERTICAL ADD
      TransferData(0x88,0);       //SET 水平地址 HORIZONTAL ADD
      for(j=0;j<16;j++)
       {
         TransferData(*adder,1);
         adder++;
       }
    }
}




