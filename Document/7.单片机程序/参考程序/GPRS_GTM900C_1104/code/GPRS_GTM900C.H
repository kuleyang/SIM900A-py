#include<reg52.h>
#define uint unsigned int
#define uchar unsigned char
uchar TC35_CON;
uchar read_flag;
uchar i_TC35_RECE;
uchar i_receive,
      TC35_RE[30],
	  TC35_MAX,
	  i_TC35;
////////其他地方不要乱动////////////////////////////////////////////////////////
void delay_GSM(uint i)
{unsigned int i_delay,j_delay;
for(i_delay=0;i_delay<i;i_delay++)
{for(j_delay=0;j_delay<100;j_delay++)
{;}}
} 

//---串口初始化----------------------------------------
void init_serialcom( void ) 
   { 
       SCON = 0x50 ;  //SCON: serail mode 1, 8-bit UART, enable ucvr   
                         //UART为模式1，8位数据，允许接收
          TMOD |= 0x21 ; //TMOD: timer 1, mode 2, 8-bit reload             
                         //定时器1为模式2,8位自动重装
          PCON |= 0x80 ; //SMOD=1; 
          TH1 = 0xFD ;   //Baud:19200 fosc="11".0592MHz 
          IE |= 0x90 ;     //Enable Serial Interrupt 
          TR1 = 1 ;       // timer 1 run 
          TI=1;  
   
       }           

//向串口发送一个字符 
void send_uart( char a_s) 
         { // TI= 0 ; 
          
            while (TI!=1){;} 
  SBUF=a_s; 
               TI= 0 ; 
			   
          } 
//串口接收中断函数 
void serial () interrupt 4 using 3 
{ 
    if (RI) 
          {  
        RI = 0 ; 
        i_receive=SBUF;
        TC35_RE[i_TC35_RECE++]=i_receive;
          // i_TC35_RECE++;
             if(i_TC35_RECE>TC35_MAX){TC35_MAX=i_TC35_RECE;}

              }
} 
#define TC35RET        0X00
#define RST            0X01
#define GPRS_QD_S      0X02
#define GPRS_JH_S      0X03
#define GPRS_DK_S      0X04
#define GPRS_BJ_S      0X05
#define GPRS_CX_S      0X06
#define GPRS_JX_S      0X07
#define GPRS_TCP_S     0X08
#define CREG           0X09
#define GPRS_SEND_OK   0X0A
//AT+CGDCONT=1,"IP","CMNET"
//AT%ETCPIP
//AT%IPOPEN="TCP","60.25.87.72",12345,,5000
//AT%IPSEND="01020304EF" 


//-------------以下为功能定义------------------------------------------------------------
uchar code  TC_ATF[]={"AT&F\x00D\x00A"};	                //格式化命令。
uchar code  TC_ATE0[]={"ATE0\x00D\x00A"};	                //清除回显命令
uchar code  TC_ATE1[]={"ATE1\x00D\x00A"};	                //回复回显命令
uchar code  TC_WLZT[]={"AT+CREG?\x00D\x00A"};	            //网络状态查询
//---------------------------------------------------------------------------
uchar code  GPRS_QD1[]={"AT+CGDCONT=1"};                //GPRS启动，
uchar code  GPRS_QD2[]={"IP"};                //GPRS启动，
uchar code  GPRS_QD3[]={"CMNET"};                //GPRS启动，
//--------------------------
uchar code  GPRS_JH[]={"AT%ETCPIP"};               //移动场景开启。
//========DTU连接===============================================================
uchar code GPRS_TCP1[]={"AT%IPOPEN="};
uchar code GPRS_TCP2[]={"TCP"};
uchar code GPRS_TCP3[]={"60.25.82.191"};//IP
uchar code GPRS_TCP4[]={"12345"};
uchar code GPRS_TCP5[]={"5000"};
//=================================================================================
//========GPRS-SEND===============================================================
uchar code GPRS_SEND1[]={"AT%IPSEND="};
//========================OK===================================================
uchar code GPRS_SEND2[]={"4F4B"};
//========================OPENOK-------------------
uchar code GPRS_SEND3[]={"4F50454E4F4B"};
//========================CLOSEOK-------------------
uchar code GPRS_SEND4[]={"434C4F53454F4B"};
//-------------以下为GPRS的子函数---------------------------------------------------------------
void AT_CREG()
{
uchar i;
for(i=0;i<sizeof(TC_WLZT)-1;i++)
{send_uart(TC_WLZT[i]);} 
TC35_CON=CREG;
}
//------------关闭回显----------------------------------------------
void ATE0()
{//I AM OK
uchar i;
for(i=0;i<sizeof(TC_ATE0)-1;i++)
{send_uart(TC_ATE0[i]);} 
TC35_CON=RST;
//TC35_CON=GPRS_QD_S;
}

//------------GPRS开始----------------------------------------------
void GPRS_START()
{//I AM OK
uchar i;
for(i=0;i<sizeof(GPRS_QD1)-1;i++)
{send_uart(GPRS_QD1[i]);} 
send_uart(',');
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_QD2)-1;i++)
{send_uart(GPRS_QD2[i]);} 
send_uart(0X22);//分号
send_uart(',');
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_QD3)-1;i++)
{send_uart(GPRS_QD3[i]);} 
send_uart(0X22);//分号
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_QD_S;
}
//------------GPRS激活----------------------------------------------
void GPRS_ACTIVE()
{
uchar i;
for(i=0;i<sizeof(GPRS_JH)-1;i++)
{send_uart(GPRS_JH[i]);} 
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_JH_S;
}
//-----GPRS解析域名DNS------------------
void GPRS_DNS()
{//AT+CMGF=1

}
//-----GPRS数据连接------------------
void GPRS_TCPIP()
{//AT%IPOPEN="TCP","60.25.87.72",12345,,5000
uchar i;
for(i=0;i<sizeof(GPRS_TCP1)-1;i++)
{send_uart(GPRS_TCP1[i]);}
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_TCP2)-1;i++)
{send_uart(GPRS_TCP2[i]);}
send_uart(0X22);//分号
send_uart(',');
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_TCP3)-1;i++)
{send_uart(GPRS_TCP3[i]);}
send_uart(0X22);//分号
send_uart(',');
for(i=0;i<sizeof(GPRS_TCP4)-1;i++)
{send_uart(GPRS_TCP4[i]);}
send_uart(',');
send_uart(',');
for(i=0;i<sizeof(GPRS_TCP5)-1;i++)
{send_uart(GPRS_TCP5[i]);}
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_TCP_S;
}

//-----------GPRS数据发送-------------------
void GPRS_SENDDATA()
{//I AM OK
uchar i;
for(i=0;i<sizeof(GPRS_SEND1)-1;i++)
{send_uart(GPRS_SEND1[i]);} 
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_SEND2)-1;i++)
{send_uart(GPRS_SEND2[i]);} 
send_uart(0X22);//分号
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_SEND_OK;
}

void GPRS_SENDDATA_OPEN()
{//I AM OK
uchar i;
for(i=0;i<sizeof(GPRS_SEND1)-1;i++)
{send_uart(GPRS_SEND1[i]);} 
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_SEND3)-1;i++)
{send_uart(GPRS_SEND3[i]);} 
send_uart(0X22);//分号
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_SEND_OK;
}

void GPRS_SENDDATA_CLOSE()
{//I AM OK
uchar i;
for(i=0;i<sizeof(GPRS_SEND1)-1;i++)
{send_uart(GPRS_SEND1[i]);} 
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_SEND4)-1;i++)
{send_uart(GPRS_SEND4[i]);} 
send_uart(0X22);//分号
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_SEND_OK;
}

//-------------以上为GPRS的子函数-------------------------------------------------------
//000000000000000000000000000000000000000000000000000000000000000000000
//---------------------------------------------------------------------
void SEND_MESSAGE()
{ delay_GSM(10);
if(TC35_MAX>0)
{
//--------关闭回显-----------------------------------------------
if(TC35_CON==RST){
if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATE_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATE_OK_12864);}
//------------------
TC35_CON=TC35RET; 
delay_GSM(10);
AT_CREG();}
//如果不成功则重新关闭回显
else{
delay_GSM(10);
ATE0();
}
}
//---------注册是否成功--------------------------------------
else if(TC35_CON==CREG){
if(((TC35_RE[9]=='0')&&(TC35_RE[11]=='1'))||((TC35_RE[9]=='0')&&(TC35_RE[11]=='5')))
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATCREG1_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCREG1_OK_12864);}
//------------------
TC35_CON=TC35RET;
//----------------
GPRS_START();
//----------------
}
else{LCD(str0, ATCREG2_OK);AT_CREG();}
}
//----------GTM900C启动---------------------------
else if(TC35_CON==GPRS_QD_S)
{delay_GSM(10);
if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATGPRS_QD_1602);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATGPRS_QD_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_ACTIVE();
}
else{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATGPRS_QD_1602_1);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATGPRS_QD_12864);}
//------------------

TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_START();

}
}
//uchar code   ATGPRS_QD_1602[17]={"GPRS START!     "};
//uchar code ATGPRS_QD_1602_1[17]={"GPRS RESTART!   "};
//uchar code   ATGPRS_JH_1602[17]={"ACT OK!         "};
//uchar code ATGPRS_JH_1602_1[17]={"ACT RESET!      "};
//uchar code   ATGPRS_DK_1602[17]={"COM OK!         "};
//uchar code   ATGPRS_BJ_1602[17]={"IP SET!         "};
//uchar code   ATGPRS_CX_1602[17]={"IP FOUND!       "};
//uchar code   ATGPRS_JX_1602[17]={"IP SEARCH!      "};
//uchar code  ATGPRS_TCP_1602[17]={"GPRS OK!        "}; 
//---------------------------------------------------------------
else if(TC35_CON==GPRS_JH_S){
delay_GSM(10);
if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATGPRS_JH_1602);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATGPRS_JH_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_TCPIP();
}
else{

//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATGPRS_JH_1602_1);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATGPRS_JH_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_ACTIVE();
}
}
//发送完毕返回继续扫描接收
else if(TC35_CON==GPRS_TCP_S){ 
delay_GSM(10);

if((TC35_RE[2]=='C')&&(TC35_RE[3]=='O'))
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATGPRS_TCP_1602);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATGPRS_TCP_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_SENDDATA();

}
else{LCD(str0, ATGPRS_TCP_1602_1);
TC35_CON=TC35RET;
delay_GSM(100);
GPRS_TCPIP();
}
}
//GPRS_SEND_OK
else if(TC35_CON==GPRS_SEND_OK){ 
delay_GSM(10);
if((TC35_RE[0]==0X25)&&(TC35_RE[1]==0X49))
{
 if((TC35_RE[11]==0X33)&&(TC35_RE[12]==0X31))
{
RELAY=1;
//GPRS_SENDDATA();
GPRS_SENDDATA_OPEN();
}
else if((TC35_RE[11]==0X33)&&(TC35_RE[12]==0X32))
{
RELAY=0;
//GPRS_SENDDATA();
GPRS_SENDDATA_CLOSE();
}

delay_GSM(100);

}

}
//复位接收组
for(i_TC35=0;i_TC35<=29;i_TC35++)
{TC35_RE[i_TC35]=0;}
i_TC35_RECE=0;
TC35_MAX=0;
}
}






