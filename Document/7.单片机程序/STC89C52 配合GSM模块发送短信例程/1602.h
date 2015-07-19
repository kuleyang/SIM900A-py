/*
1602液晶显示函数
RS 	     P2^0
RW	     P2^1
E	     P2^2
D7-D0	 P1

*/									  
#include"reg52.h"
#ifndef _1602_H_
#define _1602_H_
#define uchar unsigned char
#define uint unsigned int


//uchar code table[]="  09-11-19 WEN";
//uchar code table1[]=" 00:00:00";

sbit rs=P3^3;
sbit rw=P3^4;
sbit e=P3^5;

//uint num=100	;

void delay_1602(uchar z)
{
	uchar i,j;
		for(i=z;i>0;i--)
			for(j=120;j>0;j--);
}
void write_com(uchar com)
{
	rs=0;
	rw=0;
	e=0;
	e=1;
	P0=com;

	delay_1602(1);
	e=0;
	rw=1;
	rs=1;
}

void write_data(uchar date)
{	rs=1;
	rw=0;
	e=0;
	e=1;
	P0=date;
	
	delay_1602(1);
	e=0;
	rw=1;
	rs=0;
}
void init_1602()
{
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);

}

void LCD_set_xy( uchar x, uchar y )  //写地址函数
{									//列x=0-15,行y=0,1
    uchar address;
    if (y == 0) 
		address = 0x80 + x;
    else   
		address = 0x80 + 0X40 + x;
    write_com( address);
}
  
void LCD_write_string(uchar X,uchar Y,uchar *s) //列x=0-15,行y=0,1
{
    LCD_set_xy( X, Y ); //写地址    
    while (*s)  // 写显示字符
    {
      write_data( *s );
      s ++;
    }
      
}

void LCD_write_char(uchar X,uchar Y,uchar d )   //列x=0-15,行y=0,1
{
  LCD_set_xy( X, Y ); //写地址
  write_data(d);
  
}
void lcd_disp_str(uchar *s ,uchar line)   //行line=1,2    为了和后面一致 line=1，2
{
  LCD_set_xy( 0, line-1 ); //写地址
  while (*s)  // 写显示字符
    {
      write_data( *s );
      s ++;
    }
  
}

#endif