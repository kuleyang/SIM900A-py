#include<reg52.h>
#include<absacc.h>
#include<stdio.h>
#include"code\ds1302.h"
#include"code\ds18b20.h"
#include"code\1602.h"
#include"code\12864.h"
#define uint unsigned int
#define uchar unsigned char
#define LCD_SELECT 0x01
#define RST      0X01
#define CMGF     0X02
#define CMGD     0X03
#define CREG     0X04
#define CMGR     0X05
#define CMGS     0X06
#define CSCA     0X07
#define ATDAT    0X08
#define TC35RET  0X00
sbit IGT=P1^3;
sbit RELAY=P1^1;
sbit RTIR=P2^0;   //人体感应
bit IR_FLAG,IR_NUM;
uchar a,b,c,d,C_TEMP;
uint temp;

int sum=0,sum1=0;
//uchar code tab[]={0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f,0x01,0x09,0xfd,0xff};//0-9,-,全灭 (共阳字段表)
//---------------1602----------------------
uchar code   GSM_START[17]={"GSM START!      "};
uchar code  GSM_START1[17]={"GSM START!!     "};
uchar code  GSM_START2[17]={"GSM START!!!    "};
uchar code      ATE_OK[17]={"ATE0 OK!        "};
uchar code   ATCMGF_OK[17]={"TEXT MODE OK!   "};
uchar code  ATCREG1_OK[17]={"NET CONNECT OK! "};
uchar code  ATCREG2_OK[17]={"NET CONNECTING! "};
uchar code   ATCSCA_OK[17]={"CENTER NUM OK!  "};
uchar code   ATCMGS_OK[17]={"PHONE NUM OK!   "};
uchar code    ATGSM_OK[17]={"GSM SEND OK!    "};
//----------------------------------------------------------------
uchar code   GSM_START_12864[17]={"模块启动中!     "};
uchar code  GSM_START1_12864[17]={"模块启动中!!    "};
uchar code  GSM_START2_12864[17]={"模块启动完毕!!! "};
uchar code      ATE_OK_12864[17]={"模块启动成功!   "};
uchar code   ATCMGF_OK_12864[17]={"设置文本模式!   "};
uchar code  ATCREG1_OK_12864[17]={"网络连接成功!   "};
uchar code  ATCREG2_OK_12864[17]={"网络连接中。。。"};
uchar code   ATCSCA_OK_12864[17]={"中心号设置完成! "};
uchar code  ATCSCA2_OK_12864[17]={"中心号设置中。。"};
uchar code   ATCMGS_OK_12864[17]={"目标号码完成!   "};
uchar code    ATGSM_OK_12864[17]={"短信发送成功!   "};
//----------------------------------------------------------------
uchar code   ATGPRS_QD_12864[17]={"GPRS启动!       "};
uchar code   ATGPRS_JH_12864[17]={"场景设置!       "};
uchar code   ATGPRS_DK_12864[17]={"端口设置!       "};
uchar code   ATGPRS_BJ_12864[17]={"本机IP设置!     "};
uchar code   ATGPRS_CX_12864[17]={"本机IP查询!     "};
uchar code   ATGPRS_JX_12864[17]={"解析域名!       "};
uchar code  ATGPRS_TCP_12864[17]={"短信发送成功!   "};
//----------------------------------------------------------------
uchar code    ATGPRS_QD_1602[17]={"GPRS START!     "};
uchar code  ATGPRS_QD_1602_1[17]={"GPRS RESTART!   "};
uchar code    ATGPRS_JH_1602[17]={"ACT OK!         "};
uchar code  ATGPRS_JH_1602_1[17]={"ACT RESET!      "};
uchar code    ATGPRS_DK_1602[17]={"COM OK!         "};
uchar code    ATGPRS_BJ_1602[17]={"IP SET!         "};
uchar code    ATGPRS_CX_1602[17]={"IP FOUND!       "};
uchar code    ATGPRS_JX_1602[17]={"IP SEARCH!      "};
uchar code   ATGPRS_TCP_1602[17]={"GPRS OK!        "}; 
uchar code ATGPRS_TCP_1602_1[17]={"GPRS ERROR!     "}; 
//------------------12864------------------//
uchar str0[16]={"TIME:00:00:00   "};
uchar str1[16]={"TEMP:000.0C     "};

#include"code\GPRS_GTM900C.h"
void initLCM( void);               											//LCD初始化子程序
void lcd_wait(void);              											//LCD检测忙子程序
void WriteCommandLCM(uchar WCLCM,uchar BusyC);                              //写指令到ICM子函数
void WriteDataLCM(uchar WDLCM);                                             //写数据到LCM子函数
void DisplayOneChar(uchar X,uchar Y,uchar DData);                           //显示指定坐标的一个字符子函数
void DisplayListChar(uchar X,uchar Y,uchar *DData);                    //显示指定坐标的一串字符子函数

void ATE0(void);
void delay_GSM(uint);
void init_serialcom( void );
void send_uart( unsigned char);
/*
void ATNAME(void);
void AT_CMGS(void);
void AT_CSCA(void);
void AT_RST(void);
void AT_CREG(void);
void AT_CMGF(void);
void AT_CMGR(void);
void AT_CMGD(void);*/
unsigned int ReadTemperature(void);

void Init_T0 (void)   
{           
    TMOD = 0x21;   
    TH0 = (65536-10000) /256; 
    TL0 = (65536-10000) %256;
    EA = 1;      
    ET0 = 1;
    TR0 = 1;      
}
void Time0_ISR(void)  interrupt 1    using 0
{  
 TH0 = (65536-10000) /256; //初始值
 TL0 = (65536-10000) %256; 
C_TEMP++;

}

void IR_TEST()
{
if((RTIR==1)&&(IR_FLAG==0))
{
LCD_12864_DISPLAY(0,3,"有人闯入禁区!");
delay_GSM(1000);//时间自己调整
if((RTIR==1)&&(IR_FLAG==0))
{IR_FLAG=1;IR_NUM=1;
LCD_12864_DISPLAY(0,3,"确认闯入报警!");


}
}
else if((RTIR==0)&&(IR_FLAG==0))
{//IR_FLAG=0;
LCD_12864_DISPLAY(0,3,"无人闯入禁区!");
}

}
//显示时间和温度
void LCD_SCREEN()
{if(C_TEMP>=30){

temp=readtemperature();
b=ReadTimeValue(0X81);
c=ReadTimeValue(0X83);
d=ReadTimeValue(0X85);
str0[5]=d/0x10+0x30;
str0[6]=d%0x10+0x30;
str0[8]=c/0x10+0x30;
str0[9]=c%0x10+0x30;
str0[11]=b/0x10+0x30;
str0[12]=b%0x10+0x30;
a=(temp-(temp&10))/10;
str1[6]=(a/100)+0x30;
str1[7]=(a/10)+0x30;
str1[8]=(a%10)+0x30;
str1[10]=(temp%10)+0x30;
C_TEMP=0;
//IR_TEST();

//LCD1(str0);

//LCD_12864_DISPLAY(0,0,str0);
//LCD_12864_DISPLAY(0,1,str1);
}
}

main()
{//P1=0X00;
//--------初始化-------------------
RELAY=0;
init_serialcom(); //设定串口工作方式
Init_T0();        //设定定时器工作方式
InitDS1302();     //设定DS1302工作方式
lcd_init();       //设定液晶工作方式

//---------------------------------
LCD(str0, str1);
//LCD_12864_DISPLAY(0,0,str0);
//LCD_12864_DISPLAY(0,1,str1);
//-------启动GSM/GPRS模块--------------
LCD(str0, GSM_START);
//LCD_12864_DISPLAY(0,2,GSM_START_12864);
delay_GSM(100);
IGT=0;
delay_GSM(100);
IGT=1; 
LCD(str0, GSM_START1);
//LCD_12864_DISPLAY(0,2,GSM_START1_12864);
delay_GSM(100);
TC35_MAX=0;
ATE0();
delay_GSM(100);
LCD(str0, GSM_START2);
//LCD_12864_DISPLAY(0,2,GSM_START2_12864);
for(i_TC35=0;i_TC35<=29;i_TC35++)
{TC35_RE[i_TC35]=0;}
i_TC35_RECE=0;
TC35_MAX=0;
ATE0();
delay_GSM(100);
for(i_TC35=0;i_TC35<=29;i_TC35++)
{TC35_RE[i_TC35]=0;}
i_TC35_RECE=0;
TC35_MAX=0;

ATE0();
//-------主程序--------------
while(1)
{
delay_GSM(20);

//LCD_12864_DISPLAY(0,2,ATCREG2_OK);

LCD_SCREEN(); 
SEND_MESSAGE();
/*
if(IR_FLAG==1)
{
if(IR_NUM==1)
{ATE0();
delay_GSM(100);
IR_NUM=0;
}
}
*/

}
}


