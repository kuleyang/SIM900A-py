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
#include "C8051F340.H"     //����ͷ�ļ�����Ƭ���ڲ��ļĴ������壩
#include "intrins.h"
#include <string.h>


unsigned char code IC_DAT[];
unsigned char code pic1[];
unsigned char code pic2[];
unsigned char code pic3[]; 


sbit LCM_RS       =   P0^2; //1602/12864LCD�ܽŶ���
sbit LCM_RW       =   P1^3; //1602/12864LCD�ܽŶ���
sbit LCM_E        =   P1^4; //1602LCD�ܽŶ���
sbit LCM_PSB      =   P1^5; //������ѡ��λ
sbit LCM_RES      =   P1^6; //RESET

////////////////////////////LCD12864Һ������/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void delay(unsigned int ms)       //��ʱ�೤ʱ�䣿����
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
* ���� : TransferData()
* ���� : ��12864Һ��д���ݻ�����
* ���� : data1,DI=0��������,��DI=1,��������.
* ��� : ��
***********************************************************************/
void TransferData(unsigned char data1,bit DI) //�������ݻ�������,��DI=0��,��������,��DI=1,��������.
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
* ���� : initinal()
* ���� : �ֿ��ʼ��
* ���� : ��
* ��� : ��
***********************************************************************/
void initinal(void) //LCD�ֿ��ʼ������
{
           delay(100);            //����40MS����ʱ����
           LCM_PSB=1;             //����Ϊ8BIT���ڹ���ģʽ
           delay(5);              //��ʱ
           LCM_RES=0;             //��λ
           delay(5);              //��ʱ
           LCM_RES=1;             //��λ�ø�
           delay(10);
           TransferData(0x30,0);  //Extended Function Set :8BIT����,RE=0: basic instruction set, G=0 :graphic display OFF
           delay(100);            //����100uS����ʱ����
           TransferData(0x30,0);  //Function Set
           delay(30);             ////����37uS����ʱ����
           TransferData(0x08,0);  //Display on Control
           delay(100);            //����100uS����ʱ����
           TransferData(0x10,0);  //CuLCM_RSor Display Control�������
           delay(100);            //����100uS����ʱ����
           TransferData(0x0C,0);  //Display Control,D=1,��ʾ��
           delay(100);            //����100uS����ʱ����
           TransferData(0x01,0);  //Display Clear
           delay(10);             //����10mS����ʱ����
           TransferData(0x06,0);  //Enry Mode Set,�����������1λ�ƶ�
           delay(100);            //����100uS����ʱ����
 }

/********************************************************************
* ���� : initina2()
* ���� : ͼ����ʾ��ʼ��
* ���� : ��
* ��� : ��
***********************************************************************/
void initina2(void) //LCD��ʾͼƬ(��չ)��ʼ������
{
           delay(100);            //����40MS����ʱ����
           LCM_PSB=1;             //����Ϊ8BIT���ڹ���ģʽ
           delay(5);              //��ʱ
           LCM_RES=0;             //��λ
           delay(5);              //��ʱ
           LCM_RES=1;             //��λ�ø�
           delay(5);

           TransferData(0x36,0);  //Extended Function Set RE=1: extended instruction
           delay(100);            //����100uS����ʱ����
           TransferData(0x36,0);  //Extended Function Set:RE=1: extended instruction set
           delay(20);             ////����37uS����ʱ����
           TransferData(0x3E,0);  //EXFUNCTION(DL=8BITS,RE=1,G=1)
           delay(100);            //����100uS����ʱ����
           TransferData(0x01,0);  //CLEAR SCREEN
           delay(100);            //����100uS����ʱ����
}


/********************************************************************
* ���� : lcd_mesg()
* ���� : ������
* ���� : ��
* ��� : ��
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
  delay(100);            //����40MS����ʱ����
  LCM_PSB=1;             //����Ϊ8BIT���ڹ���ģʽ
  delay(5);              //��ʱ
  LCM_RES=0;             //��λ
  delay(5);              //��ʱ
  LCM_RES=1;             //��λ�ø�
  delay(10);
  TransferData(0x30,0);
  delay(100);            //����100uS����ʱ����
  TransferData(0x30,0); //2����ʾģʽ���ã�������æ
  delay(30);             ////����37uS����ʱ����
  TransferData(0x0C,0); //����ʾ���������
  delay(100);            //����100uS����ʱ����
  TransferData(0x01,0); //��ʾ����
  delay(10);            //����100uS����ʱ����
  TransferData(0x06,1); //��ʾ����ƶ�����
  delay(100);            //����100uS����ʱ����

}
/********************************************************************
* ���� : Main()
* ���� : ��ָ��λ����ʾ����
* ���� : X:(0-7)������,Y:(0-3)������,string:Ҫ��ʾ������,num:���ݳ���(1������ռ2���ֽ�)
* ��� : ��
***********************************************************************/
void Display_String(unsigned char X,unsigned char Y,unsigned char *string,unsigned char num)//��ʾ�ַ���
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
* ���� : Main()
* ���� : ������
* ���� : ��
* ��� : ��
***********************************************************************/
void DisplayGraphic(unsigned char code *adder)
{

   int i,j;
//*******��ʾ�ϰ�����������
   for(i=0;i<32;i++)              //
    { 
      TransferData((0x80 + i),0); //SET  ��ֱ��ַ VERTICAL ADD
      TransferData(0x80,0);       //SET  ˮƽ��ַ HORIZONTAL ADD
      for(j=0;j<16;j++)
       {
         TransferData(*adder,1);
         adder++;
       }
    }	   
//*******��ʾ�°�����������
   for(i=0;i<32;i++)              //
    {
      TransferData((0x80 + i),0); //SET ��ֱ��ַ VERTICAL ADD
      TransferData(0x88,0);       //SET ˮƽ��ַ HORIZONTAL ADD
      for(j=0;j<16;j++)
       {
         TransferData(*adder,1);
         adder++;
       }
    }
}




