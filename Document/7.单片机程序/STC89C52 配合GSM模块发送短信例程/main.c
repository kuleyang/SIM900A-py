#include"reg52.h"
#include "1602.h"
#include "uart.c"
#include "tc35.c"

//#define RsBuf_N			50

//char RsBuf[RsBuf_N];	//���崮�ڽ������ݻ�����
//uchar RsPoint=0;

sbit MQ2=P2^7;
sbit HUOYAN=P2^3;
sbit HONGWAI=P2^0;

sbit huang=P2^6;
sbit hong=P2^5;
sbit lv=P2^4;
sbit buzz=P2^2;
sbit jk=P2^1;

uchar code PhoneNO[]= "+8618011461069";
uchar code Text1[]= "Harmfulgases";
uchar code Text2[]= "Fire";
uchar code Text3[]= "Theft";

uchar code center[]="+8613800451500";

void main()
{
	jk=0;
// 	TC35_init();
	Uart_init();
	init_1602();
	
	Send_AT();
	

	Set_MODE(1);	   //���ö���Ϣģʽ																	
	Set_CENTER(center);  // ���ö���Ϣ����     AT+CSCA=��+8613800531500���س�


	
	
	while(1)
	{
		if(MQ2==0)
		{
			lv=0;
			buzz=0;

			TransmitText(PhoneNO,Text1);
			
			jk=1;	
			while(1);
		}
		if(HUOYAN==0)
		{
			huang=0;
			buzz=0;

			TransmitText(PhoneNO,Text2);	
			jk=1;
			while(1);
		}
		if(HONGWAI==0)
		{
			hong=0;
			buzz=0;
			TransmitText(PhoneNO,Text3);	
			while(1);
		}
	}

		//lcd_disp_str(RsBuf ,2);
		

 }

 
 


 /*���⣺
   ÿ�δ��������ݡ�RsPoint����������ģ����������� ׼����һ�����ݣ�Ȼ�����һ������ ֮ǰ
   RsBuf[RsPoint++]=SBUF;
		RsBuf[RsPoint]=0x00;   //����һ����������

		����������û���⡣��ɾ�������պ������ο������ϵ�tc35.c

		���Һ���˵���֧��	�������ֻ�
*/
 
