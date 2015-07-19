/**************************************************************************
 * EVB900A-V1������ʵ�����                                              *
 * �汾�� (C) Copyright 2010,SC.etc,DCL,CHM    All Rights reserved.      *
 * ��վ�� mcuep.taobao.com                            *
 * ���䣺 dcl0@sina.com                                                  *
 * ���ߣ� DCL,CHM
 * ��Դ�� ˼������  Xa' STrong ELECTRONICS CO.,LTD      *
 *����Ȩ��Copyright(A) ˼������ mcuep.taobao.com  All Rights Reserved    *
 *���������˳��������ѧϰ��ο���������ע����Ȩ��������Ϣ��             *
 *************************************************************************/
//DS18B20���¶ȳ��򣬹ҵ���оƬ��
//������11.0592M����Ϊ������ͬ�ľ����ٶȿ�����Ҫ������ʱ��ʱ��
//sbit DQ =P1^0;         ����ʵ���������˿�
#include "C8051F340.H"     //����ͷ�ļ�����Ƭ���ڲ��ļĴ�������
#include "intrins.h"
#include "math.h"

sbit DQ           =   P3^7;//18B20-1
bit isExternPower;

//sbit B20-DQ1      =   P4^7;//18B20-1
//sbit B20-DQ2      =   P4^6;//18B20-1
/*****************��ʱ**********/
void DS18B20_1us(unsigned int us)  //Լ1us  ---����48MHZ
{  
    while (us)
 {
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_();_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_();
   --us;
 }
 
}
/*****************��λ**********/
unsigned char Init_DS18B20(void)
{
  unsigned char i,presence;

  Clr_DQ();                    // pull DQ line low
  DS18B20_1us(467);
  Set_DQ();                    // allow line to return high
  DS18B20_1us(15);
    DS18B20_1us(200);
	presence = DQ;             // ���=0���ʼ���ɹ� =1���ʼ��ʧ�� 
  for (i=0;i<30;i++)           //240us
  DS18B20_1us(15);
  return(presence);            // 0=presence, 1 = no part
}                 
/*******�� 1-WIRE ������дһ���ֽ�**********/
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
/*******�� 1-wire �����϶�ȡһ���ֽ�**********/
unsigned char read_byte(void)
{
  unsigned char i;
  unsigned char value = 0;
  EA=0;
  Set_DQ();           
  DS18B20_1us(1);
  for (i=8;i>0;i--)
  {   
    Clr_DQ();           // �������ź�
	DS18B20_1us(5);
    Set_DQ();           // �������ź�
	 
	DS18B20_1us(1);
    value>>=1;
    if((DQ)==0x80)value|=0x80;
    DS18B20_1us(60);    // wait for rest of timeslot

  }
  EA=1;
  return(value);
}
/******************************************************
�������ܣ������¶�ת��
*******************************************************/
void Convert_T()
{
  unsigned char pre=1;

  while(pre==1)
  {
    pre=Init_DS18B20(); //DS18B20��λ
  }
  write_byte(0xCC);  //����������кŵĲ���
  write_byte(0x44);  //д��44H��������¶�ת�� 
}
/******************************************************
�������ܣ���Դ������ʽ
*******************************************************/
bit Read_Power_Supply()
{
  write_byte(0xB4);
  DS18B20_1us(100); //����ȡ��Դ������ʽָ��
  if(DQ){
    isExternPower=1; //���ⲿ��Դ����
  
  }else{
  
   isExternPower=0; //�Ǽ�����Դ
  }
  return isExternPower;

}
/******************************************************
�������ܣ�����ROM����ָ��
*******************************************************/
void Skip_ROM()
{  
  unsigned char pre=1;

  while(pre==1)
  {
    pre=Init_DS18B20(); //DS18B20��λ
  }
  write_byte(0xCC); //�����ڱ�ʵ����ֻ��һ��18B20,���Բ���Ҫ����ROM��ָ����ô�ָ�����ROM����ز���
}
/*******��ȡ�¶�**********/
unsigned int Read_Temperature(void)
{

   unsigned char a=0;
   unsigned char b=0;      
   float temper=0;

   Convert_T();      // Skip ROM and Start Conversion
   Skip_ROM();       //Skip ROM-->����������кŵĲ����������¶�ת��
   write_byte(0xBE); // Read Scratch Pad-->��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
   a = read_byte();  //����λ
   b = read_byte();  //����λ
   
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


