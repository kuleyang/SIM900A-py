#ifndef _DELAY_H_
#define	_DELAY_H_
#define uchar unsigned char
void delay_ms(uchar z)
{
	uchar i,j;
		for(i=z;i>0;i--)
			for(j=120;j>0;j--);
}
void delayms_100ms()
{
	
	delay_ms(100);
		
}
void delayms_1000ms()
{
	  uchar i;
	  for(i=0;i<30;i++)
	         delayms_100ms();
}

#endif