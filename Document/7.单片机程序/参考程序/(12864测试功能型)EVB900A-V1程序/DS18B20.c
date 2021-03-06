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
//DS18B20读温度程序，挂单个芯片。
//这里以11.0592M晶体为例，不同的晶体速度可能需要调整延时的时间
//sbit DQ =P1^0;         根据实际情况定义端口
#include "C8051F340.H"     //调用头文件（单片机内部的寄存器定义�
#include "intrins.h"
#include "math.h"

sbit DQ           =   P3^7;//18B20-1
bit isExternPower;

//sbit B20-DQ1      =   P4^7;//18B20-1
//sbit B20-DQ2      =   P4^6;//18B20-1
/*****************延时**********/
void DS18B20_1us(unsigned int us)  //约1us  ---晶振48MHZ
{  
    while (us)
 {
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_();_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_();
   --us;
 }
 
}
/*****************复位**********/
unsigned char Init_DS18B20(void)
{
  unsigned char i,presence;

  Clr_DQ();                    // pull DQ line low
  DS18B20_1us(467);
  Set_DQ();                    // allow line to return high
  DS18B20_1us(15);
    DS18B20_1us(200);
	presence = DQ;             // 如果=0则初始化成功 =1则初始化失败 
  for (i=0;i<30;i++)           //240us
  DS18B20_1us(15);
  return(presence);            // 0=presence, 1 = no part
}                 
/*******向 1-WIRE 总线上写一个字节**********/
void write_byte(unsigned char dat)
{
  unsigned char i;
  EA=0;
  Set_DQ();           
  DS18B20_1us(1);
  DS18B20_1us(10);
  for (i=8; i>0; i--)   // writes byte, one bit at a time
  {
    
    Clr_DQ();           // pull DQ low to start timeslot
	DS18B20_1us(15);
    if((dat&0x01)==1){Set_DQ();}
    DS18B20_1us(50);    // hold value for remainder of timeslot
    Set_DQ();
    dat>>=1;
	DS18B20_1us(1);
  }
  EA=1;
}
/*******从 1-wire 总线上读取一个字节**********/
unsigned char read_byte(void)
{
  unsigned char i;
  unsigned char value = 0;
  EA=0;
  Set_DQ();           
  DS18B20_1us(1);
  for (i=8;i>0;i--)
  {   
    Clr_DQ();           // 给脉冲信号
	DS18B20_1us(5);
    Set_DQ();           // 给脉冲信号
	 
	DS18B20_1us(1);
    value>>=1;
    if((DQ)==0x80)value|=0x80;
    DS18B20_1us(60);    // wait for rest of timeslot

  }
  EA=1;
  return(value);
}
/******************************************************
函数功能：启动温度转换
*******************************************************/
void Convert_T()
{
  unsigned char pre=1;

  while(pre==1)
  {
    pre=Init_DS18B20(); //DS18B20复位
  }
  write_byte(0xCC);  //跳过读序号列号的操作
  write_byte(0x44);  //写入44H命令，启动温度转换 
}
/******************************************************
函数功能：电源工作方式
*******************************************************/
bit Read_Power_Supply()
{
  write_byte(0xB4);
  DS18B20_1us(100); //发读取电源工作方式指令
  if(DQ){
    isExternPower=1; //是外部电源供电
  
  }else{
  
   isExternPower=0; //是寄生电源
  }
  return isExternPower;

}
/******************************************************
函数功能：跳过ROM编码指令
*******************************************************/
void Skip_ROM()
{  
  unsigned char pre=1;

  while(pre==1)
  {
    pre=Init_DS18B20(); //DS18B20复位
  }
  write_byte(0xCC); //由于在本实验中只用一个18B20,所以不需要关于ROM的指令，调用此指令，跳过ROM的相关操作
}
/*******读取温度**********/
unsigned int Read_Temperature(void)
{

   unsigned char a=0;
   unsigned char b=0;      
   float temper=0;

   Convert_T();      // Skip ROM and Start Conversion
   Skip_ROM();       //Skip ROM-->跳过读序号列号的操作且启动温度转换
   write_byte(0xBE); // Read Scratch Pad-->读取温度寄存器等（共可读9个寄存器） 前两个就是温度
   a = read_byte();  //读低位
   b = read_byte();  //读高位
   
   temper=a+b*256;
   
   if (temper==0xFFFF) return 0xFFFF;
   if (temper>0x8000) 
   {
     temper=-temper;
     return (0x8000+temper*5/8);
   }
   else
   return (temper*5/8);
}


