/*STM32控制SIM900A---打电话到10086，打通1分钟后电话自动挂断               */
/*测试平台：STM32F103RBT6                                                 */
/*STM32使用串口1跟SIM900A进行通信，串口1使用的波特率为9600                */
/*STM32串口1的RX（PA10）接SIM900A的TXD,STM32串口1的TX（PA9）接SIM900A的RXD*/
/*STM32的GND接SIM900A开发板的GND                                          */
/*当SIM900A模块开机正常后，按下STM32开发板的复位键，程序就会正常运行      */
/*SIM900A（GSM、GPRS）开发板淘宝销售店址：http://shop72125140.taobao.com  */

#include <stm32f10x_lib.h>
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"	 	 

int main(void)
{			
	u16 times=0;  
	Stm32_Clock_Init(9);    /*系统时钟设置;72MHz */
	delay_init(72);	        /*延时初始化 */
	uart_init(72,9600);	    /*串口初始化为9600 */
	LED_Init();		  	    /*初始化与LED连接的硬件接口 */   

	printf("ati\r");		/*ati初始化sim900模块  */
	delay_ms(1000);
	printf("ati\r");
	delay_ms(1000);
	printf("ati\r");
	delay_ms(1000);

	printf("ATD10086;\r");	       /*拨打号码10086*/

	for(times=0;times<60;times++)  /*打通后延时1分钟*/
	{
		LED0=0;			 /*指示灯亮*/
		delay_ms(1000);
	}

	printf("ATH\r");	 /*挂断电话*/
	LED0=1;              /*指示灯灭*/

	while(1); 	 
}

























