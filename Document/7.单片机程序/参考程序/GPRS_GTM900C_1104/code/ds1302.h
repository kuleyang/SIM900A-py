/****DS1302程序区******昆新电子制作*******************
**********************************************************
****可定义区的3个参数可以改动
****
****用于存储和读取当前时间
****
********************************************************/
#include<reg52.h>
#include <intrins.h>
#include <absacc.h>
#define uint unsigned int
#define uchar unsigned char
#define DS1302_SECOND 0x80
#define DS1302_MINUTE 0x82
#define DS1302_HOUR 0x84 
#define DS1302_WEEK 0x8A
#define DS1302_DAY 0x86
#define DS1302_MONTH 0x88
#define DS1302_YEAR 0x8C
//-----------可定义区--------------------------
sbit DS_REST=P1^6;
sbit DS_SCLK=P1^4;
sbit DS_SDATA=P1^5;
//---------------------------------------------
/*
1、秒寄存器的最高位要先清0
2、可能时钟在走数据没读出来，检查程序
3、单片机系统是否正常？晶振，复位、能不能正常跑其他程序
4、DS1302时钟数据线上最好加10k左右的上拉电阻
5、读写操作的时候注意那个REST脚要先设置成低然后再设置成高电平
能想到的也就这么多了，关键还是要抱住数据手册不放，看仔细了你就有所启发。再给你两个写和读的函数做参考，都是能运行起来的。祝你好运 
*/
void delay_1302(unsigned int cnt)//延时
{
 while(--cnt);
}
void Write_DS_Byte(unsigned char DS_Byte)
{
    unsigned char i=0;
 for(i=0;i<8;i++)
 {
     if((DS_Byte&0x01)==0x01)
         DS_SDATA=1;
  else
         DS_SDATA=0; 
    delay_1302(10);
        DS_SCLK=1;
       delay_1302(10);
        DS_SCLK=0;
        DS_Byte=(DS_Byte>>1);
 }
}
void Wirte_DS1302(unsigned char DS_Addr,unsigned char DS_Data)
{
    DS_REST=0;
    DS_SCLK=0;
    DS_REST=1;
    Write_DS_Byte(DS_Addr);
    Write_DS_Byte(DS_Data);
    DS_SCLK=1;
    DS_REST=0;    
}
unsigned char Read_DS_Byte()    //只负责8位数据的位移读取
{
    unsigned char i=0;
    unsigned char ClockValue=0;

    for(i=0;i<7;i++)
    {
        if(DS_SDATA==1)
        {
            ClockValue=(ClockValue|0x80);
        }
        else
        {
            ClockValue=(ClockValue&0x7f);
        }
        ClockValue=(ClockValue>>1);
        DS_SCLK=1;
        delay_1302(2);
        DS_SCLK=0;
    }
    return (ClockValue);
}
unsigned char ReadTimeValue(unsigned char DS_Addr) //读指定地址的寄存器数据
{
    unsigned char Value;
    DS_REST=0;
    DS_SCLK=0;
    DS_REST=1;
    Write_DS_Byte(DS_Addr);
    Value=Read_DS_Byte();
    DS_SCLK=1; 
    DS_REST=0;
    return(Value); 
} 

void InitDS1302(void)
{
Wirte_DS1302(0x8E,0x00);
Wirte_DS1302(DS1302_YEAR,0x09);
Wirte_DS1302(DS1302_MONTH,0x04);
Wirte_DS1302(DS1302_DAY,0x06);
Wirte_DS1302(DS1302_WEEK,0x01);
Wirte_DS1302(DS1302_HOUR,0x01);
Wirte_DS1302(DS1302_MINUTE,0x02);
Wirte_DS1302(DS1302_SECOND,0x20);
Wirte_DS1302(0x8E,0x80);
}

