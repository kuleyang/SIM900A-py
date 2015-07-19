/**************************************************************************
 * EVB900A-V1¿ª·¢°åÊµÑé³ÌĞò                                              *
 * °æ±¾£º (C) Copyright 2010,SC.etc,DCL,CHM    All Rights reserved.      *
 * ÍøÕ¾£º mcuep.taobao.com                            *
 * ÓÊÏä£º dcl0@sina.com                                                  *
 * ×÷Õß£º DCL,CHM
 * À´Ô´£º Ë¼´´µç×Ó  Xa' STrong ELECTRONICS CO.,LTD      *
 *¡¾°æÈ¨¡¿Copyright(A) Ë¼´´µç×Ó mcuep.taobao.com  All Rights Reserved    *
 *¡¾ÉùÃ÷¡¿´Ë³ÌĞò½öÓÃÓÚÑ§Ï°Óë²Î¿¼£¬ÒıÓÃÇë×¢Ã÷°æÈ¨ºÍ×÷ÕßĞÅÏ¢£¡             *
 *************************************************************************/
//DS18B20¶ÁÎÂ¶È³ÌĞò£¬¹Òµ¥¸öĞ¾Æ¬¡£
//ÕâÀïÒÔ11.0592M¾§ÌåÎªÀı£¬²»Í¬µÄ¾§ÌåËÙ¶È¿ÉÄÜĞèÒªµ÷ÕûÑÓÊ±µÄÊ±¼ä
//sbit DQ =P1^0;         ¸ù¾İÊµ¼ÊÇé¿ö¶¨Òå¶Ë¿Ú
#include "C8051F340.H"     //µ÷ÓÃÍ·ÎÄ¼ş£¨µ¥Æ¬»úÄÚ²¿µÄ¼Ä´æÆ÷¶¨Òå£
#include "intrins.h"
#include "math.h"

sbit DQ           =   P3^7;//18B20-1
bit isExternPower;

//sbit B20-DQ1      =   P4^7;//18B20-1
//sbit B20-DQ2      =   P4^6;//18B20-1
/*****************ÑÓÊ±**********/
void DS18B20_1us(unsigned int us)  //Ô¼1us  ---¾§Õñ48MHZ
{  
    while (us)
 {
   _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_();_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();_nop_();
   --us;
 }
 
}
/*****************¸´Î»**********/
unsigned char Init_DS18B20(void)
{
  unsigned char i,presence;

  Clr_DQ();                    // pull DQ line low
  DS18B20_1us(467);
  Set_DQ();                    // allow line to return high
  DS18B20_1us(15);
    DS18B20_1us(200);
	presence = DQ;             // Èç¹û=0Ôò³õÊ¼»¯³É¹¦ =1Ôò³õÊ¼»¯Ê§°Ü 
  for (i=0;i<30;i++)           //240us
  DS18B20_1us(15);
  return(presence);            // 0=presence, 1 = no part
}                 
/*******Ïò 1-WIRE ×ÜÏßÉÏĞ´Ò»¸ö×Ö½Ú**********/
void write_byte(unsigned char dat)
{
  unsigned char i;
  EA=0;
  Set_DQ();           
  DS18B20_1us(1);
  DS18B20_1us(10);
  for (i=8; i>0; i--)   // writes byte, one bit at a time
  {
    
    Clr_DQ();           // pull DQ low to start timeslot
	DS18B20_1us(15);
    if((dat&0x01)==1){Set_DQ();}
    DS18B20_1us(50);    // hold value for remainder of timeslot
    Set_DQ();
    dat>>=1;
	DS18B20_1us(1);
  }
  EA=1;
}
/*******´Ó 1-wire ×ÜÏßÉÏ¶ÁÈ¡Ò»¸ö×Ö½Ú**********/
unsigned char read_byte(void)
{
  unsigned char i;
  unsigned char value = 0;
  EA=0;
  Set_DQ();           
  DS18B20_1us(1);
  for (i=8;i>0;i--)
  {   
    Clr_DQ();           // ¸øÂö³åĞÅºÅ
	DS18B20_1us(5);
    Set_DQ();           // ¸øÂö³åĞÅºÅ
	 
	DS18B20_1us(1);
    value>>=1;
    if((DQ)==0x80)value|=0x80;
    DS18B20_1us(60);    // wait for rest of timeslot

  }
  EA=1;
  return(value);
}
/******************************************************
º¯Êı¹¦ÄÜ£ºÆô¶¯ÎÂ¶È×ª»»
*******************************************************/
void Convert_T()
{
  unsigned char pre=1;

  while(pre==1)
  {
    pre=Init_DS18B20(); //DS18B20¸´Î»
  }
  write_byte(0xCC);  //Ìø¹ı¶ÁĞòºÅÁĞºÅµÄ²Ù×÷
  write_byte(0x44);  //Ğ´Èë44HÃüÁî£¬Æô¶¯ÎÂ¶È×ª»» 
}
/******************************************************
º¯Êı¹¦ÄÜ£ºµçÔ´¹¤×÷·½Ê½
*******************************************************/
bit Read_Power_Supply()
{
  write_byte(0xB4);
  DS18B20_1us(100); //·¢¶ÁÈ¡µçÔ´¹¤×÷·½Ê½Ö¸Áî
  if(DQ){
    isExternPower=1; //ÊÇÍâ²¿µçÔ´¹©µç
  
  }else{
  
   isExternPower=0; //ÊÇ¼ÄÉúµçÔ´
  }
  return isExternPower;

}
/******************************************************
º¯Êı¹¦ÄÜ£ºÌø¹ıROM±àÂëÖ¸Áî
*******************************************************/
void Skip_ROM()
{  
  unsigned char pre=1;

  while(pre==1)
  {
    pre=Init_DS18B20(); //DS18B20¸´Î»
  }
  write_byte(0xCC); //ÓÉÓÚÔÚ±¾ÊµÑéÖĞÖ»ÓÃÒ»¸ö18B20,ËùÒÔ²»ĞèÒª¹ØÓÚROMµÄÖ¸Áî£¬µ÷ÓÃ´ËÖ¸Áî£¬Ìø¹ıROMµÄÏà¹Ø²Ù×÷
}
/*******¶ÁÈ¡ÎÂ¶È**********/
unsigned int Read_Temperature(void)
{

   unsigned char a=0;
   unsigned char b=0;      
   float temper=0;

   Convert_T();      // Skip ROM and Start Conversion
   Skip_ROM();       //Skip ROM-->Ìø¹ı¶ÁĞòºÅÁĞºÅµÄ²Ù×÷ÇÒÆô¶¯ÎÂ¶È×ª»»
   write_byte(0xBE); // Read Scratch Pad-->¶ÁÈ¡ÎÂ¶È¼Ä´æÆ÷µÈ£¨¹²¿É¶Á9¸ö¼Ä´æÆ÷£© Ç°Á½¸ö¾ÍÊÇÎÂ¶È
   a = read_byte();  //¶ÁµÍÎ»
   b = read_byte();  //¶Á¸ßÎ»
   
   temper=a+b*256;
   
   if (temper==0xFFFF) return 0xFFFF;
   if (temper>0x8000) 
   {
     temper=-temper;
     return (0x8000+temper*5/8);
   }
   else
   return (temper*5/8);
}


