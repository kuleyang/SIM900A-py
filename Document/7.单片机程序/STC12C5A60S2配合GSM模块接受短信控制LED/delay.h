#include <intrins.h>
void delay_ms(unsigned int i)   //?? -0.018084490741us
{
		while(i--)
		{	
			unsigned char a,b;
			for(b=18;b>0;b--)
					for(a=152;a>0;a--);
			_nop_();  //if Keil,require use intrins.h
		}
}