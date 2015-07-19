/*STM32����SIM900A---��绰��10086����ͨ1���Ӻ�绰�Զ��Ҷ�               */
/*����ƽ̨��STM32F103RBT6                                                 */
/*STM32ʹ�ô���1��SIM900A����ͨ�ţ�����1ʹ�õĲ�����Ϊ9600                */
/*STM32����1��RX��PA10����SIM900A��TXD,STM32����1��TX��PA9����SIM900A��RXD*/
/*STM32��GND��SIM900A�������GND                                          */
/*��SIM900Aģ�鿪�������󣬰���STM32������ĸ�λ��������ͻ���������      */
/*SIM900A��GSM��GPRS���������Ա����۵�ַ��http://shop72125140.taobao.com  */

#include <stm32f10x_lib.h>
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"	 	 

int main(void)
{			
	u16 times=0;  
	Stm32_Clock_Init(9);    /*ϵͳʱ������;72MHz */
	delay_init(72);	        /*��ʱ��ʼ�� */
	uart_init(72,9600);	    /*���ڳ�ʼ��Ϊ9600 */
	LED_Init();		  	    /*��ʼ����LED���ӵ�Ӳ���ӿ� */   

	printf("ati\r");		/*ati��ʼ��sim900ģ��  */
	delay_ms(1000);
	printf("ati\r");
	delay_ms(1000);
	printf("ati\r");
	delay_ms(1000);

	printf("ATD10086;\r");	       /*�������10086*/

	for(times=0;times<60;times++)  /*��ͨ����ʱ1����*/
	{
		LED0=0;			 /*ָʾ����*/
		delay_ms(1000);
	}

	printf("ATH\r");	 /*�Ҷϵ绰*/
	LED0=1;              /*ָʾ����*/

	while(1); 	 
}

























