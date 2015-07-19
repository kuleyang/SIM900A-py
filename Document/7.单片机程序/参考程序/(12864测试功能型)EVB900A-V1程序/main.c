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
#include "C8051F340.H"
#include "intrins.h"
#include "SIM900a.h"
#include "lc1602.h"
#include "lc12864.h"
#include "1.h"
#include "DS18B20.h"

sbit SP485_RS     =   P0^3;//485_RS

sbit SPK          =   P2^5;//������
sbit GPRS_EN   	  =   P2^6; //GPRS ʹ��

sbit JDQ_C1       =   P3^3;//�̵���1
sbit JDQ_C2       =   P3^4;//�̵���2
sbit JDQ_C3       =   P3^5;//�̵���3
sbit JDQ_C4       =   P3^6;//�̵���4



sbit KEY_C1       =   P0^6;//KEY1
sbit KEY_C2       =   P0^7;//KEY2
sbit KEY_C3       =   P1^0;//KEY3

sbit LED_C1       =   P1^1;//LED1
sbit LED_C2       =   P1^2;//LED2

///////////////////DTMF����//////////////////////////////
//��Ҫ�����ⲿ�ж�
sbit DTMF_DV      =   P2^4;//�ߵ�ƽ��Ч(���յ�DTMF)
sbit DTMF_OE   	  =   P1^7;//�ߵ�ƽ��Ч(����ɹ�)
//D0-P2.0  D1=P2.1  D2=P2.2  D3=P2.3

///////////����1���ֲ���λ����/////////////////
#define IsTI1 (SCON1&0x02)
#define IsRI1 (SCON1&0x01)
#define ClrTI1() SCON1&=~0x02;
#define ClrRI1() SCON1&=~0x01;
///////////������������Ϣ����/////////////////
unsigned char xdata SIM_NUM[20]; //���SIM������(20λ)
unsigned char xdata Rx=0;
unsigned char xdata SIM900a_BUF[70];//SIM900a������ڽ�������
unsigned char xdata GSM_BUF[300];//SIM900a����������� 
///////////LCDָʾ�Ʋ���/////////////////
//#define LCM_Data  P1   //������
//#define Busy    0x80   //���ڼ��LCM״̬���е�Busy��ʶ

////////////////////////LCD1602Һ����ӭ��ʾ/////////////////////////////////////////////////////////////
unsigned char code uctech[] = {"Welcome!"};
unsigned char code net[] = {"mcuep.taobao.com"};

////////////////////////GPS��λ����ʵ��/////////////////////////////////////////////////////////////////
unsigned char code net2[]         = {"GPS TEST"};
unsigned char code Lat[]          = {"LAT:"};            //γ��ͷ��
unsigned char code Longt[]        = {"LG:"};             //����ͷ��
unsigned char code NO_SIM_LINK[]  = {"NO_MODUL_LINK!!"};  //900aģ���뵥Ƭ������δ���ӣ�����ԭ��:1.����ðδ�Ͻ�(��); 2.ģ��δ����(���Գ�����)

unsigned char code net3[]         = {"���Ƕ�λʵ��"};
unsigned char code gps_tm[]       = {"ʱ��:"}; 
unsigned char code gps_lg[]       = {"����:"};     
unsigned char code gps_long[]     = {"γ��:"};  

////////////////////////DS18B20�¶Ȳ���ʵ��//////////////////////////////////////////////////////////////
unsigned char code net4[]     = {"�¶ȿ���ʵ��"};
unsigned char code tm_up[]    = {"�¶�����:"};
unsigned char code tm_down[]  = {"�¶�����:"};
unsigned char tm_up_point=40   ;   //�¶�����
unsigned char tm_down_point=25 ;   //�¶�����
unsigned char code tm_p[]     = {"��ǰ�¶�:"};
unsigned char  DS18B20_sn[8];  //4�����������кţ��ȶ����������кź����ϲſ��Զ�ȡ�¶�
////////////////////////GPS����/////////////////////////////////////////////
unsigned char xdata GPS_head_buf[5];//GPS�����ִ��
unsigned char xdata head__point=0;
unsigned char xdata GPS_data_buf[60];//GPS������Ϣ���
unsigned char xdata data__point=0;
unsigned char xdata GPS_Flg1=0;//GPS���������־λ
unsigned char xdata GPS_Flg2=0;//GPS������Ϣ��־λ
unsigned char xdata GPS_data_ok_flg=0;//GPS���ݽ���һ����ɱ�־
unsigned char xdata GPS_inf_state=0; //�����ձ��ĵ���Ч�� A:����   V:������
unsigned char xdata GPS_can_use=0;   //1:��������� 0:������ 
///////////����������/////////////////
#define spk_1  SPK=0;	delay_10ms(20);      SPK=1; delay_10ms(30);
#define spk_2  SPK=0;	delay_10ms(40);      SPK=1; delay_10ms(100);
#define spk_3  SPK=0;	delay_10ms(50);      SPK=1; delay_10ms(400);
#define spk_4  SPK=0;	delay_10ms(60);      SPK=1; delay_10ms(200);
//************Ӳ����ʼ������**********************************************
void Port_IO_Init()
{    	 
	P0MDOUT   = 0xC0;
	//P1MDOUT   = 0xFF;
	//P2MDOUT   = 0x01;
	P3MDOUT   = 0x00;
	P4MDOUT   = 0x00;//P4�ڲ��ܰ�λ����
    XBR0      = 0x01;
    XBR1      = 0x40;
    XBR2      = 0x01;
}
//************************************************************************
//UART0:ʹ��time1 115200BPS; UART1:ʹ��time1 19200BPS
//************************************************************************/
void Timer_Init()
{
    TCON      = 0x40;
    TMOD      = 0x20;
    CKCON     = 0x08;
    TH1       = 0x30;
}
//************************************************************************
//���ڳ�ʼ�������ã�UART0=115200BPS,UART1=9600BPS,ʹ�ܴ��ڽ���
//************************************************************************/
void UART_Init()
{
    SCON0     = 0x10;

    SBRLL1    = 0x1E;  //19200bps     ע����Ҫ19200���
    SBRLH1    = 0xFB;  //19200bps

	//SBRLL1    = 0x3C;    //9600bps      ע����Ҫ9600���
    //SBRLH1    = 0xF6;    //9600bps

    SCON1     = 0x10;
    SBCON1    = 0x43;
}

//************************************************************************
//ϵͳʱ�ӳ�ʼ����48M ѡ���ⲿ��������
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
//������0/����1/���ж�
//************************************************************************/
void Interrupts_Init()
{
    EIE2      = 0x02;
    IE        = 0x90;
}
void Init_Device(void)
{
    PCA0MD &= ~0x40;//�رտ��Ź���ʱ�� 
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
///////////////////////////////////////��Ƭ������SIMCOMģ��/////////////////////////////////
/****************************************************************************************************************/
void Start_GSM(void)
{

	GPRS_EN=0;
    Delay_ms(60000);Delay_ms(60000);
    //GPRS_EN=1;
	Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
}
/****************************************************************************************************************/
///////////////////////////////////////void clear_SIM900a_BUF();������ڽ��ջ�����/////////////////////////////////
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
//���SIM900aģ�鹤��״̬���Լ���ȡSIM���ŷ���-->SIM_NUM
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
	    if((SIM900a_BUF[6]=='O')&&(SIM900a_BUF[7]=='K'))//�ж��Ƿ�ģ���ʼ���ɹ�,�ɹ��Ļ�ģ���ظ�"OK"����Ƭ��------>�û����ݷ�������ؽ������飬�Ķ�����OK���ݴ��λ��
        {												//�����Ƭ��û���յ�OK,�ͼ������ͳ�ʼ��ָ��/ 
           clear_SIM900a_BUF();
           sendstring(AT_CCID);Delay_ms(10000);	//��SIM����  89860 0C226 10A74 59948(20λASCII���ʾ)        
		   for(i=11;i<31;i++)
	       {
			 SIM_NUM[i-11]=SIM900a_BUF[i];
	       }
		   spk_1;//�ϵ��������
		   break;	
        }
		if(j>15)//��ʾ:SIM900Bģ���뵥Ƭ��ͨѶδ����
		{      
	        //LCMInit_1602(); //LCD1602Һ����ʼ��
            //DisplayListChar(0, 0,13, NO_SIM_LINK);   //�ڵ�1����ʾ"NO-NO_SIM_LINK-!!"

			LCMInit_12864(); //LCD12864Һ����ʼ��
            Display_String(0,1,NO_SIM_LINK,15);
            Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
            break;
		}
	}
}
//*************************SIM300����******************************************************/
//*****************************************************************************************/
/*********************************************************************
//����������������ַ���*STRING_much�Ƿ���ȫ������"@:"�ַ�
//����:  0  û���ҵ�
//1-255 �ӵ�N���ַ���ʼ��ͬ
 *********************************************************************/
unsigned char strsearch_head(unsigned char *STRING_much)//���ַ���*STRING_little��*STRING_much�е�λ��
{
	unsigned char i;
	for(i=0;i<300;i++)
	{
        	if(STRING_much[i]==0x40)//��һ���ַ���ͬ
       		{	
				if(STRING_much[i+1]==0x3A) //�Ƚ���ȷ
				{
       			   return(i+2); //����ֵ������
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
//������485���ͳ���
//����1 19200bps/����9600bps��   ����UART_Init()˵�����еڶ����ڲ�����ѡ��
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
void Read_B20_SN(void) //��ȡ���к�
{ 
  unsigned char i;

  Init_DS18B20(); //��λ 
  DS18B20_1us(100); //����ȡ��Դ������ʽָ�� 
  write_byte(0x33); //���Ͷ����к��ӳ��� 
  for(i=0;i<8;i++) //��Ϊ��8���ֽڣ�����Ҫ��ȡ8��
  {
    DS18B20_sn[i]=read_byte(); //��ȡ���к�
  }
}
///////////////////////////////////////////////////////////////////////
// ������ʼ
///////////////////////////////////////////////////////////////////////
void main(void)
{
    unsigned char code table[]={"0123456789ABCDEF"}; //����1602��ʾ

	unsigned char GPS_hour;  //GPS��λʵ��:�м����
	
	unsigned int TT;         //DS18B20ʵ��:�¶�ֵ	
	unsigned char table_temp[6];   //DS18B20ʵ��:�м����
	unsigned char table_temp2[4];  //DS18B20ʵ��:�м����
	unsigned char table_temp3[4];  //DS18B20ʵ��:�м����
	unsigned char tab1,tab2,tab3;  //DS18B20ʵ��:�м����
	unsigned char tab_C[2]="��";   //DS18B20ʵ��:�����

	unsigned char DTMF_data;  //DTMF����ֵ--------->˫����Ƶʵ����ر���

    Init_Device(); //���񡣹ܽų�ʼ��
	Start_GSM();//GSM�ϵ�
	GSM_check();///��ⵥƬ����ģ���Ƿ����������� ע������ѡ��Ƭ����ģ������---,���ѡ��ģ��������ͨѶ���ܣ��뽫Щ�������
	  //////////////////////LCD1602Һ����ʾ���Գ���---START//////////////////////////
	  /////////A�׼�����
	  
	  LCMInit_1602(); //LCM1602��ʼ��
	  Delay5Ms();Delay5Ms();Delay5Ms();//LCM��ʼ����ʱ
      DisplayListChar(3, 0,8, uctech);   //�ڵ�һ����ʾ"Welcome!"
      DisplayListChar(0, 1,16, net);     //�ڵڶ�����ʾ"mcuep.taobao.com"
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  DisplayListChar(0, 1,16, table);     //�ڵڶ�����ʾ"0123456789"
	  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
	  //////////////////////LCD1602Һ����ʾ���Գ���---END////////////////////////////
    while(1)
	{ 
	     //////////////////////LCD12864Һ����ʾ���Գ���---START//////////////////////
		 ////////////////1:ͼ����ʾʵ��//////////////////////////
		  initina2();            //����LCD��ʾͼƬ(��չ)��ʼ������
          DisplayGraphic(pic2);  //��ʾͼƬ2
		  Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
          
		  ///////////////2:������ʾʵ��1//////////////////////////
          initinal();   		 //����LCD�ֿ��ʼ������
          delay(200);            //����100uS����ʱ����
          lcd_mesg(IC_DAT);      //��ʾ���ĺ���1
          Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);

		  ///////////////3:������ʾʵ��2//////////	  
          LCMInit_12864();   	 //����LCD�ֿ��ʼ������
          delay(200);            //����100uS����ʱ���� 
          lcd_mesg(IC_DAT2);     //��ʾ���ĺ���2
          Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);Delay_ms(60000);
		  


          ///////////////4:������ʾʵ��1--<ָ��λ����ʾ>//////////////
		  LCMInit_12864();//��ʼ��Һ��������
		  Read_B20_SN();//��DS18B20���к�
		  while(1)
		  {

		  Display_String(1,0,net4,12);     //"�¶ȿ���ʵ��"
		  Display_String(0,1,tm_up,10);    //"�¶�����:"
		  Display_String(0,2,tm_down,10);  //"�¶�����:"
          Display_String(0,3,tm_p,10);     //"��ǰ�¶�:"


          table_temp2[0]=(tm_up_point/10)+0x30;       //�¶�����ʮλ
          table_temp2[1]=(tm_up_point%10)+0x30;       //�¶����޸�λ
		  table_temp2[2]=tab_C[0];                    //��ʾ�¶ȷ���"��"
		  table_temp2[3]=tab_C[1];                    //��ʾ�¶ȷ���"��"
          table_temp3[0]=(tm_down_point/10)+0x30;     //�¶�����ʮλ
          table_temp3[1]=(tm_down_point%10)+0x30;     //�¶����޸�λ
          table_temp3[2]=tab_C[0];                    //��ʾ�¶ȷ���"��"
          table_temp3[3]=tab_C[1];                    //��ʾ�¶ȷ���"��"
          Display_String(5,1,table_temp2,4);           //LCD12864Һ������ʾ�趨�¶�����
          Display_String(5,2,table_temp3,4);           //LCD12864Һ������ʾ�趨�¶�����

		  TT=Read_Temperature();//���¶�
		  ////////////����Ϊ����¶�ֵ�ұ�ΪASCII�����Һ����ʾ//////
          tab1 = TT / 100;   
          table_temp[0] = tab1 + 0x30;     // ʮλ   
          tab2= TT / 10 - tab1*10;   
          table_temp[1] = tab2 + 0x30;     //��λ   
          table_temp[2] = '.';  
          tab3 = TT - tab1*100 - tab2*10;   
          table_temp[3] = tab3 + 0x30;     //С����һλ   

		  table_temp[4] =tab_C[0];//��ʾ�¶ȷ���"��"
		  table_temp[5] =tab_C[1];//��ʾ�¶ȷ���"��"         
		  Display_String(5,3,table_temp,6);//LCD12864Һ������ʾ�¶�ֵ
		    
		  }
          ///////////////5:������ʾʵ��3--<GPS��λʵ��������12864Һ����ʾ>//////////////
          LCMInit_12864();//��ʼ��Һ��������
		  while(1)
		  {
            Display_String(1,0,net3,12);    //"���Ƕ�λʵ��"
		    Display_String(0,1,gps_tm,6);   //"ʱ��:"
		    Display_String(0,2,gps_long,6); //"γ��:"
			Display_String(0,3,gps_lg,6);   //"����:"
	     
			if(GPS_data_ok_flg==1)//���յ�GPS��Ϣ$GPRMCָ��
	        {
              GPS_data_ok_flg=0;
              GPS_inf_state=GPS_data_buf[12];//'A'=0x41-->���Ǳ��Ŀ���  'V'=0x56-->���Ǳ��Ĳ�����
		      if(GPS_inf_state==0x41)GPS_can_use=1;
              ////////////////////��λ��Ч�󣺽���GPS����///////////////////////////////////////
		      if(GPS_can_use==1)//GPS���ݶ�λ��Ч
		      {
			     GPS_can_use=0; //���GPS��λ״̬
		   	    //////////////////////GPS���ݽ�����ʾ/////////////////////////////////////////////
				//////////////////////ʱ����ʾ/////////////////////////////////////
                GPS_hour=(GPS_data_buf[1]-0x30)*10+(GPS_data_buf[2]-0x30)+8;//��8��ʱ�����,�Ҽ���":"
		        if(GPS_hour>23)GPS_hour=GPS_hour-24;
                GPS_data_buf[0]=(GPS_hour/10)+0x30;
			    GPS_data_buf[1]=(GPS_hour%10)+0x30;
				GPS_data_buf[2]=0x3a;//:
				GPS_data_buf[7]=GPS_data_buf[6];
				GPS_data_buf[6]=GPS_data_buf[5];
				GPS_data_buf[5]=0x3a;//:	
		        Display_String(3,1,GPS_data_buf,8);   //����ʱ��:--Ӧ��+8ת�ɶ�����ʱ(����ʱ��)
                //////////////////////γ����ʾ/////////////////////////////////////////////
		        Display_String(3,2,GPS_data_buf+14,9); //γ��
                //////////////////////������ʾ///////////////////////////////////////////// 
		        Display_String(3,3,GPS_data_buf+26,9); //����
			  }
			}
		  }
 

		  

      //////////////////////LCD12864Һ����ʾ���Գ���---END/////////////////////////
      ///////////////////////////�������й����в�������Ӳ���Ƿ���������//////////////
	  //1��LED���Ƿ����� 2.��·�̵��������Ƿ����� 3.��·�̵���ָʾ���Ƿ�����  
	  //4.����������     5.485�����Ƿ�ɹ����(����������--->5:485_A , 6:485_B)
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
          //////////////5:DTMFʵ��//////////////
		  if(DTMF_DV==1)//���յ��ɹ���DTMF�ź�����ߵ�ƽ
          {
		    DTMF_OE =1;spk_1;
			DTMF_data=P2&0x0F;
		  }
          if(DTMF_data==1)//���յ��ɹ���DTMF�ź�����ߵ�ƽ
          {
           LED_C1=0;
		   spk_1;
          }
          if(DTMF_data==8)//����ɹ�����ߵ�ƽ
		  {
            LED_C2=0;
			spk_1;
		  }
           if(DTMF_data==5)//����ɹ�����ߵ�ƽ
		  {
            LED_C1=1;
		    LED_C2=1;

		  }*/	
///////////////////////////////////////////////////////////////////////
//UART0,115200BPS ��Ƭ���жϽ���SIM900a����ͨѶ�жϴ������
///////////////////////////////////////////////////////////////////////
void SIM900a_UART_REC(void) interrupt INTERRUPT_UART0 using 3
{
  	EA=0;
  	if(RI0)
	{	
          SIM900a_BUF[Rx]=SBUF0;  //����ר��BUF
	      if(Rx==70)Rx=0;
	      else
	      Rx++;
	}
    RI0=0;
    EA=1;
}
//////////////////////////////////////////////////////////////////////
//UART1,����Ϊ��19200BPS ��Ƭ���жϷ�ʽ����GPS��λ���ĳ��� 
//������$GPRMC����
///////////////////////////////////////////////////////////////////////
void isr_UART_1(void) interrupt INTERRUPT_UART1 using 3
{
	unsigned char tmp;
	EA=0;
  	if(IsRI1)
	{	
	  tmp=SBUF1;
	  if(tmp=='$')//�жϱ�����ͷ"$"
	  {
	     GPS_Flg1=1;
		 goto  over_1;
		 
	  }
	  if(GPS_Flg2==1)
	  {
	     if(GPS_head_buf[0]=='G'&&GPS_head_buf[1]=='P'&&GPS_head_buf[2]=='R'&&GPS_head_buf[3]=='M'&&GPS_head_buf[4]=='C')//���ձ�����ϢGPRMC
		 {
		   GPS_data_buf[data__point]=tmp;
		   data__point++;
		   if(data__point==60)
		   {
		     data__point=0;
		     GPS_Flg2=0;//���������Ϣ
			 GPS_data_ok_flg=1;//GPS���ݽ���һ����ɱ�־
		   }
		 }
		 else
         {
		   GPS_Flg2=0;//���������Ϣ
		 }
	  }
	  if(GPS_Flg1==1)//���ձ���������
	  {
	     
         GPS_head_buf[head__point]=tmp;
		 head__point++;
		 if(head__point==5)
		 {
		   head__point=0;
		   GPS_Flg1=0;//������ձ���������
		   GPS_Flg2=1;
		 }
	  }
over_1:
       ClrRI1(); //RI1=0;	
	}
    EA=1;
}

		  
