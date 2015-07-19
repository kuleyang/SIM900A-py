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
/******����ΪӲ��I/O�ڶ���********/

sbit LCM_RS       =   P0^2; //1602LCD�ܽŶ���
sbit LCM_RW       =   P1^3; //1602LCD�ܽŶ���
sbit LCM_E        =   P1^4; //1602LCD�ܽŶ���


#define  LCD_DATA   P1   //LCD DATA
#define  LCD_BUSY   0x80 // ���ڼ��LCD��BUSY��ʶ�����������õ���ʱ��δ��⣩ 
//��ʱ
void Delay1(void)
{
 unsigned int TempCyc = 100;
 while(TempCyc--);
}

//5ms��ʱ
void Delay5Ms(void)
{
 unsigned int TempCyc = 5552;
 while(TempCyc--);
}

//400ms��ʱ
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

//������
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

//��״̬
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
  
 while (LCD_DATA & LCD_BUSY); //���æ�ź� 
 tmp=LCD_DATA;
 //P4=0x1f;
 LCM_E =0;
 LCM_RW=0;
 LCM_RS=0;
 return(tmp);
 
}


//д����
void WriteDataLCM(unsigned char WDLCM)
{
 ReadStatusLCM(); //���æ
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

//дָ��
void WriteCommandLCM(unsigned char WCLCM,BuysC) //BuysCΪ0ʱ����æ���
{
 if (BuysC) ReadStatusLCM(); //������Ҫ���æ
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

void LCMInit_1602(void) //LCM��ʼ��
{
 LCD_DATA = 0;
 Delay5Ms();
 Delay5Ms();
 Delay5Ms();
 Delay5Ms();
 WriteCommandLCM(0x38,0); //������ʾģʽ���ã������æ�ź�
 Delay5Ms();
 WriteCommandLCM(0x38,0);
 Delay5Ms();
 WriteCommandLCM(0x38,0);
 Delay5Ms();
 WriteCommandLCM(0x38,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
 WriteCommandLCM(0x08,1); //�ر���ʾ
 WriteCommandLCM(0x01,1); //��ʾ����
 WriteCommandLCM(0x06,1); // ��ʾ����ƶ�����
 WriteCommandLCM(0x0C,1); // ��ʾ�����������
}

//��ָ��λ����ʾһ���ַ�
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 Y &= 0x1;
 X &= 0xF; //����X���ܴ���15��Y���ܴ���1
 if (Y) X |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
 X |= 0x80; // ���ָ����
 WriteCommandLCM(X, 0); //���ﲻ���æ�źţ����͵�ַ��
 WriteDataLCM(DData);
}

//��ָ��λ����ʾһ���ַ�
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char len, unsigned char *DData)
{
 unsigned char ListLength;

 Y &= 0x01;
 X &= 0x0F; //����X���ܴ���15��Y���ܴ���1
 for (ListLength=0;ListLength<len;ListLength++) //�������ִ�β���˳�
  {
   if (X <= 0x0F) //X����ӦС��0xF
    {
     DisplayOneChar(X, Y, DData[ListLength]); //��ʾ�����ַ�
     //ListLength++;
     X++;
    }
  }
}



