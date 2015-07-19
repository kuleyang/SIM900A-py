//*********************************************
//���ܣ������������������
//��ƣ�DCL,CHM
//2010��08��28��
////////////////////////////////////////////////////////////
#include "C8051F020.H"
#include <string.h>
#include <stdio.h>
#include <intrins.h>

#include "XF.h"
#include "IIC.h"
//#include "SD2203.h"
//#include "SIM300.h"


 
///////////����1���ֲ���λ����/////////////////
#define IsTI1 (SCON1&0x02)
#define IsRI1 (SCON1&0x01)
#define ClrTI1() SCON1&=~0x02;
#define ClrRI1() SCON1&=~0x01;
///////////����������/////////////////
#define spk  SPK=0;	delay_10ms(10); SPK=1;




sbit GPS_BOOT    =   P0^4;//GPS
sbit GPS_EN      =   P0^5;//GPS

sbit FM_SEL1     =   P3^2;//FM24V10--Ƭѡ
sbit FM_SEL0     =   P3^3;//FM24V10--Ƭѡ
sbit WP0         =   P3^4;//FM24V10--д����  WP0=1д������0ȡ������
sbit FM_SEL3     =   P3^7;//FM24V10--Ƭѡ


sbit FM_SEL2     =   P2^0;//FM24V10--Ƭѡ
sbit WP1         =   P2^3;//FM24V10--д����  WP1=1д������0ȡ������
sbit XF_ERST     =   P2^6;//Ѷ��--��λ


sbit SPK          =   P1^1;//������
sbit MIC_EN       =   P1^2;//��˷�ʹ��
sbit TPA2005_SD   =   P1^3;//����ʹ��
sbit SIM_AUDIO_EN =   P1^4;//SIMģ��--����������ͨ·
sbit XF_AUDIO_EN  =   P1^5;//Ѷ��--����������ͨ·
sbit LED0         =   P1^6;//��ʾLED��1
sbit LED1         =   P1^7;//��ʾLED��2



//************Ӳ����ʼ������**********************************************
//��ʱ��1��ʼ������ΪUART0,UART1�Ĳ����ʷ�����
void Timer_Init()//19200bps UART0:ʹ��time2     UART1:ʹ��time4 
{
    CKCON     = 0x60;
    TCON      = 0x40;
    TMOD      = 0x30;
    T2CON     = 0x34;
    RCAP2L    = 0xDC;
    RCAP2H    = 0xFF;
    T4CON     = 0x34;
    RCAP4L    = 0xDC;
    RCAP4H    = 0xFF;
}
//************************************************************************
//���ڳ�ʼ����ѡ��UART��
//************************************************************************/
void UART_Init()
{
    SCON0     = 0x50;
    SCON1     = 0x50;
}
void Port_IO_Init()
{
	P0MDOUT   = 0xC0;
    P1MDOUT   = 0x3F;
    P2MDOUT   = 0x07;
    P3MDOUT   = 0xFF;
    P74OUT    = 0xC0;
    XBR0      = 0x04;
    XBR1      = 0x14;
    XBR2      = 0x44;
}
//************************************************************************
//ϵͳʱ�ӳ�ʼ����22.1184M ѡ���ⲿ��������
//************************************************************************/
void Oscillator_Init()
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
}
//������0������1���ⲿ�ж�0���ⲿ�ж�1,��ʱ��3,���жϿ���
void Interrupts_Init()
{
    IE        = 0x95;
    EIE2      = 0x40;
}
void Init_Device(void)
{
	WDTCN = 0xDE;// �رտ��Ź�
	WDTCN = 0xAD;
	Oscillator_Init();
	Port_IO_Init();
    UART_Init();
    Timer_Init();
	Interrupts_Init();
}


/******************************************************************************************************************
//////////////////////����void sendchar(uchar ch);ʵ�ֹ���:����һ�ֽ�����//////////////////////////////////////////
*****************************************************************************************************************/
void sendchar(unsigned char ch)
{
    unsigned char xx;
	xx=ch;
    SBUF1=xx;
    while(!IsTI1);
	ClrTI1();
}


///////////////////////////////////////////////////////////////////////
// ������ʼ
///////////////////////////////////////////////////////////////////////
void main(void)
{
   
    Init_Device(); //���񡣹ܽų�ʼ��
	spk;
	

	while(1)
	{	
	   GPS_EN=0;
	   LED0=0;
	   LED1=0;
	}
} 



//////////////////////////////////////////////////////////////////////
//UART0,SP232C��λ�����ڽ��պͷ����жϴ������
///////////////////////////////////////////////////////////////////////
void isr_UART(void) interrupt 4 using 3
{
    unsigned char idata test1;
  	unsigned int idata j=0;
  	EA=0;
  	if(RI0)
	{		
		test1=SBUF0;//���ݽ��չ����У���һ�����յ�������Ϊ���������ֽ���
		RI0=0;	
		if(test1==0xAA)
		{
		   

		   	SBUF0=0xBB;//RevBuffer[30]
		 	while(!TI0);
			TI0=0;
		}

	}
 EA=1;

}
///////////////////////////////////////////////////////////////////////
//UART1,MAX485 �������ƴ���ͨѶ�жϴ������
///////////////////////////////////////////////////////////////////////
/*
void max485_UART(void) interrupt 20 using 3
{
   unsigned char xor_tmp;
  	EA=0;
  	if(IsRI1)
	{		
	   if(max485_symbol==0xDD)
	   {
	      max485_data[max485_len]=SBUF1;
          max485_len++;
	   }
	   else
	   {
          max485_symbol=0;
	   }
	   if(max485_symbol==0)
	   {
	      max485_symbol=SBUF1;
	   }
       switch (max485_data[1])
       {
         case 0xB0://���������ϴ�ˢ����Ϣ����
                  if(max485_len==14)
				  {
				   max485_data[0]=max485_data[0]^max485_data[1]^max485_data[2]^max485_data[3]^max485_data[4]^max485_data[5]^max485_data[6];
				   max485_data[0]=max485_data[0]^max485_data[7]^max485_data[8]^max485_data[9]^max485_data[10]^max485_data[11]^max485_data[12]^0xDD;
                   if(max485_data[0]==max485_data[13])
				   {
                      slave_ok=1;//���ͻ������ݱ�־λ
			          brush_success_type=0x02;//������ˢ���ɹ���־
			          //???��������ˢ����Ϣ����������
				   }
		           max485_symbol=0;
	               max485_len=0;
				   }
                   break;
         case 0xB1://�����������뿪�̵�������
                  if(max485_len==5)
				  {
				     max485_data[0]=max485_data[0]^max485_data[1]^max485_data[2]^max485_data[3]^max485_data[3];
                     if(max485_data[0]==max485_data[4])
					 {
                         //main_ok=1;//����������������
					 }
					 max485_symbol=0;
					 max485_len=0;
				  }
                  break;        
         case 0xB2://��������ͬʱ����ʱ��
                  if(max485_len==3)
				  {
                      if(max485_data[0]==0xFF&&max485_data[1]==0xB2&&max485_data[2]==0x01)
					  {

						 //send_type=0x03;//ʱ�����óɹ�
					  }
					  max485_symbol=0;
					  max485_len=0;

				  }
                   break;
         default: 
                   break;
       }
       ClrRI1(); //RI1=0;
	}
    EA=1;
}*/

/******************************************************************************************************************
///////////////////����void receive(void) interrupt 4 using 1;ʵ�ֹ���:ͨ�����ڽ�������////////////////////////////
******************************************************************************************************************/
/*
void receive(void) interrupt 4 using 1
{
	if(RI)
	{
		if(Rx<RxIn)
		{
			SystemBuf[Rx]=SBUF;
			Rx++;
		}
		RI=0;
		
	}
}*/

