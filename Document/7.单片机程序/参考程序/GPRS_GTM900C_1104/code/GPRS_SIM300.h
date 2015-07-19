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
//-------------以下为功能定义------------------------------------------------------------
uchar code  TC_ATF[]={"AT&F\x00D\x00A"};	                //格式化命令。
uchar code  TC_ATE0[]={"ATE0\x00D\x00A"};	                //清除回显命令
uchar code  TC_ATE1[]={"ATE1\x00D\x00A"};	                //回复回显命令
uchar code  TC_WLZT[]={"AT+CREG?\x00D\x00A"};	            //网络状态查询
uchar code  GPRS_QD[]={"AT+CSTT\x00D\x00A"};                //GPRS启动，
uchar code  GPRS_JH[]={"AT+CIICR\x00D\x00A"};               //移动场景开启。
//========模块端口设置================================================================
uchar code  GPRS_DK1[]={"AT+CLPORT="};//端口设置
uchar code  GPRS_DK2[]={"TCP"};//端口设置
uchar code  GPRS_DK3[]={"2022"};//端口设置
//========获得本机IP=========================================================================
uchar code  GPRS_BJ[]={"AT+CIFSR\x00D\x00A"};	            //获得本机IP
//========查询IP状态===============================================================
uchar code  GPRS_CX[]={"AT+CIPSTATUS\x00D\x00A"};	        //查询IP状态
//========解析域名=================================================================
uchar code  GPRS_JX[]={"AT+CDNSGIP\x00D\x00A"};	            //解析域名。
//========DTU连接===============================================================
uchar code GPRS_TCP1[]={"AT+CIPSTART="};
uchar code GPRS_TCP2[]={"TCP"};
uchar code GPRS_TCP3[]={"60.25.74.238"};//IP
uchar code GPRS_TCP4[]={"12345"};
//=================================================================================
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
for(i=0;i<sizeof(GPRS_QD)-1;i++)
{send_uart(GPRS_QD[i]);} 
TC35_CON=GPRS_QD_S;
}
//------------GPRS激活----------------------------------------------
void GPRS_ACTIVE()
{
uchar i;
for(i=0;i<sizeof(GPRS_JH)-1;i++)
{send_uart(GPRS_JH[i]);} 
TC35_CON=GPRS_JH_S;
}
//------------GPRS端口----------------------------------------------
void GPRS_COM()
{//AT+CSCA=+8613010130500
uchar i;

for(i=0;i<sizeof(GPRS_DK1)-1;i++)
{send_uart(GPRS_DK1[i]);} 
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_DK2)-1;i++)
{send_uart(GPRS_DK2[i]);} 
send_uart(0X22);//分号
send_uart(',');
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_DK3)-1;i++)
{send_uart(GPRS_DK3[i]);} 
send_uart(0X22);//分号
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_DK_S;
}
//-----GPRS本机IP--------------------
void GPRS_ADDRESS()
{
uchar i;
for(i=0;i<sizeof(GPRS_BJ)-1;i++)
{send_uart(GPRS_BJ[i]);} 
TC35_CON=GPRS_BJ_S;
}
//-----GPRS查询自己的IP---------------
void GPRS_GETIP()
{
uchar i;
for(i=0;i<sizeof(GPRS_CX)-1;i++)
{send_uart(GPRS_CX[i]);} 
TC35_CON=GPRS_CX_S;
}
//AT接收格式指令
//uchar code GPRS_TCP[]
//-----GPRS解析域名DNS------------------
void GPRS_DNS()
{//AT+CMGF=1
uchar i;
for(i=0;i<sizeof(GPRS_JX)-1;i++)
{send_uart(GPRS_JX[i]);}
TC35_CON=GPRS_JX_S;
}
//-----GPRS数据连接------------------
void GPRS_TCPIP()
{//AT+CMGR=1
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
send_uart(0X22);//分号
for(i=0;i<sizeof(GPRS_TCP4)-1;i++)
{send_uart(GPRS_TCP4[i]);}
send_uart(0X22);//分号
send_uart(0X0D);//回车
send_uart(0X0A);
TC35_CON=GPRS_TCP_S;
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
//------------------------------------------------
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
else if((TC35_RE[2]=='+'))//
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
LCD_12864_DISPLAY(0,2,ATGPRS_QD_12864);
//------------------

TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_ACTIVE();

}
}
//---------------------------------------------------------------
else if(TC35_CON==GPRS_JH_S){
delay_GSM(10);
if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{
//------屏显------------------
LCD_12864_DISPLAY(0,2,ATGPRS_JH_12864);
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_COM();
}
else{

//------屏显------------------
LCD_12864_DISPLAY(0,2,ATGPRS_JH_12864);
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_COM();
}
}
//发送完毕返回继续扫描接收
else if(TC35_CON==GPRS_DK_S){ 
delay_GSM(10);

//uchar code   ATGPRS_QD_12864[17]={"GPRS启动!       "};
//uchar code   ATGPRS_JH_12864[17]={"场景设置!       "};
//uchar code   ATGPRS_DK_12864[17]={"端口设置!       "};
//uchar code   ATGPRS_BJ_12864[17]={"本机IP设置!     "};
//uchar code   ATGPRS_CX_12864[17]={"本机IP查询!     "};
//uchar code   ATGPRS_JX_12864[17]={"解析域名!       "};
//uchar code  ATGPRS_TCP_12864[17]={"短信发送成功!   "};

if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{//------屏显------------------
LCD_12864_DISPLAY(0,2,ATGPRS_DK_12864);
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_ADDRESS();

}
else{GPRS_COM();}
}
else if(TC35_CON==GPRS_BJ_S){ 
delay_GSM(10);

//uchar code   ATGPRS_BJ_12864[17]={"本机IP设置!     "};
//uchar code   ATGPRS_CX_12864[17]={"本机IP查询!     "};
//uchar code   ATGPRS_JX_12864[17]={"解析域名!       "};
//uchar code  ATGPRS_TCP_12864[17]={"短信发送成功!   "};

if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{//------屏显------------------
LCD_12864_DISPLAY(0,2,ATGPRS_BJ_12864);
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_GETIP();

}
else{GPRS_ADDRESS();}
}
else if(TC35_CON==GPRS_CX_S){ 
delay_GSM(10);

//uchar code   ATGPRS_BJ_12864[17]={"本机IP设置!     "};
//uchar code   ATGPRS_CX_12864[17]={"本机IP查询!     "};
//uchar code   ATGPRS_JX_12864[17]={"解析域名!       "};
//uchar code  ATGPRS_TCP_12864[17]={"短信发送成功!   "};

if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{//------屏显------------------
LCD_12864_DISPLAY(0,2,ATGPRS_BJ_12864);
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);
GPRS_GETIP();

}
else{GPRS_ADDRESS();}
}
//复位接收组
for(i_TC35=0;i_TC35<=29;i_TC35++)
{TC35_RE[i_TC35]=0;}
i_TC35_RECE=0;
TC35_MAX=0;
}
}







