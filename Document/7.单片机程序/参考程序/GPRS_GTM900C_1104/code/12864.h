#include <reg52.h>
#define DATAPORT P0                             //定义P0口为LCD通讯端口
sbit RS=P3^2;								 //数据/命令端
sbit RW=P3^3;								 //读/写选择端
sbit E=P3^4;
sbit RST=P3^7;
sbit PSB=P3^5;

void delay_12864(uint x)
{
  uint i,j;
  for(i=0;i<x;i++)
   for(j=0;j<110;j++);


}
////////////////////////////////////////
void LCM12864_TestBusy()
{
RW=1;
RS=0;
E=1;
while(DATAPORT&0X08);
}


/********************写命令**********************/
//写命令子程序
//
/************************************************/
void send_com(uchar cmd)/*写命令*/
{ LCM12864_TestBusy();
   delay_12864(10);
   RS = 0;
   RW = 0;
   DATAPORT = cmd;
   E = 1;
   E = 0;
}
/********************写数据**********************/
//写数据子程序
//
/************************************************/
void send_data(uchar dat)
{ 
   delay_12864(1);
   RS = 1;
   RW = 0;
   DATAPORT = dat;
   E = 1;
   E = 0;
}
/********************初始化**********************/
//复位、通讯方式选择
/************************************************/
void lcd_init(void)
{ 
   
   RST=1;//复位RST=1
      PSB = 1;       //通讯方式为并口PSB = 1
        //send_com(0x34);//34H--扩充指令操作
   send_com(0x30);//功能设置，一次送8位数据，基本指令集
   send_com(0x0C);//0000,1100 整体显示，游标off，游标位置off
   send_com(0x01);//0000,0001 清DDRAM
   send_com(0x02);//0000,0010 DDRAM地址归位
   send_com(0x80);//1000,0000 设定DDRAM 7位地址000，0000到地址计数器AC
}

/*******************************************************************/
//          设置显示位置    xpos(0~15),tpos(0~3)
/*******************************************************************/
void LCD_Set_xy(uchar xpos,uchar ypos)
 {
    uchar address;
    switch(ypos)
    {
     case 0:
          address = 0x0080 + xpos;   //第一行地址为：0x0080----0x0087
          break;                  
     case 1:
          address = 0x0090 + xpos;   //第二行地址为：0x0090----0x0097
          break;
     case 2:
       address = 0x0088 + xpos;   //第三行地址为：0x0088----0x008F
          break;
     case 3:
       address = 0x0098 + xpos;   //第四行地址为：0x0098----0x009F
          break;
     default:
       address = 0x0080 + xpos;   //默认第一行
      }
       send_com(address);
 } 
void Print_String(char *str)  
 {  
      while( (*str)!='\0' )
      {  
         send_data(*str);
         str++;
      }
 }

 void LCD_12864_DISPLAY(uchar X,uchar Y,uchar *str)
{ EA=0;
 delay_12864(1);
 LCD_Set_xy(X,Y);
    Print_String(str);
EA=1;
}
 

 void LCD_12864_DISPLAY_ZU(uchar x,uchar y,uchar str[])
{ 
   uchar n; 
      LCD_Set_xy(x,y);                                  
   for(n=0;n<sizeof(str)-1;n++)
       { 
        send_data(str[n]);
       }
 }

/********************清屏************************/
//清屏
/************************************************/
void clr_lcd(void)
{
     send_com(0x01);

}









