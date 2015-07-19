#include<reg52.h>
#define uint unsigned int
#define uchar unsigned char
#define RST      0X01
#define CMGF     0X02
#define CMGD     0X03
#define CREG     0X04
#define CMGR     0X05
#define CMGS     0X06
#define CSCA     0X07
#define ATDAT    0X08
#define TC35RET  0X00
uchar TC35_CON;
uchar read_flag;
uchar i_TC35_RECE;
uchar i_receive,
      TC35_RE[30],
	  TC35_MAX,
	  i_TC35;
//------------TEXT方式-------------------------------
///////需要改动的地方///////////////////////////////////////////////////////////
uchar code TC_MDSJ2[]={"15620323981"};        //目的手机地址，
uchar code TC_ZXH2[]={"+8613010130500"};      //天津联通本地中心号。
//uchar code TC_ZXH2[]={"+8613800220500"};      //天津移动本地中心号。
////////其他地方不要乱动////////////////////////////////////////////////////////
uchar code TC_MDSJ1[]={"AT+CMGS="};             //目的手机地址，
uchar code TC_ZXH1[]={"AT+CSCA="};              //本地中心号。
uchar code TC_FSNR[]={"SOMEBODY WANT GET IN!"};	//发送短信内容。
uchar code TC_WLZT[]={"AT+CREG?"};	            //网络状态查询
uchar code TC_MSXZ[]={"AT+CMGF=1"};	            //发送模式选择。
uchar code TC_DQML[]={"AT+CMGR=1"};	            //短信读取命令。
uchar code TC_SCML[]={"AT+CMGD=1"};	            //短信删除命令。
uchar code TC_ATF[]={"AT&F"};	                //格式化命令。
uchar code TC_ATE0[]={"ATE0"};	                //清除回显命令
uchar code TC_ATE1[]={"ATE1"};	                //回复回显命令
//------------PDU方式-------------------------------
//15620323981F
//5126303289F1
uchar code TC_MSXZ_PDU[]={"AT+CMGF=0"};	            //发送模式选择。
uchar code TC_MDSJ_PDU[]={"AT+CMGS=29"};             //目的手机地址，
uchar code BJ_DATA1[]={"089168"};
//----------------------13010130500F--------------------------
uchar code BJ_DATA2[]={"3110100305F0"};
uchar code BJ_DATA3[]={"11000D9168"};
//----------------------15620323981F--------------------------
uchar code BJ_DATA4[]={"5126303289F1"};
uchar code BJ_DATA5[]={"0008AA"};
//----------------------数据的个数不超过8C--------------------
uchar code BJ_DATA6[]={"0D"};
//----------------------有人闯入！--14------------------
uchar code BJ_DATA7[]={"67094EBA95EF5165FF01000D000A"};







void delay_GSM(uint i)
{unsigned int i_delay,j_delay;
for(i_delay=0;i_delay<i;i_delay++)
{for(j_delay=0;j_delay<100;j_delay++)
{;}}
} 
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

 
void ATE0()
{//I AM OK
uchar i;
for(i=0;i<sizeof(TC_ATE0)-1;i++)
{send_uart(TC_ATE0[i]);} 
send_uart(0X0D);
send_uart(0X0A);
TC35_CON=RST;
//TC35_CON=ATDAT;
}
void ATNAME()
{//I AM OK
uchar i;
for(i=0;i<sizeof(TC_FSNR)-1;i++)
{send_uart(TC_FSNR[i]);} 
delay_GSM(500);
send_uart(0X1A); 
TC35_CON=ATDAT;
}
//写目的手机的
void AT_CMGS()
{//AT+CMGS=13132061066
uchar i;
for(i=0;i<sizeof(TC_MDSJ1)-1;i++)
{send_uart(TC_MDSJ1[i]);} 

send_uart(0X22); 
for(i=0;i<sizeof(TC_MDSJ2)-1;i++)
{send_uart(TC_MDSJ2[i]);} 

send_uart(0X22); 
send_uart(0X0D); 
send_uart(0X0D);
send_uart(0X0A);
TC35_CON=CMGS;
}

void AT_CMGS_PDU()
{//AT+CMGS=13132061066
uchar i;
for(i=0;i<sizeof(TC_MDSJ_PDU)-1;i++)
{send_uart(TC_MDSJ_PDU[i]);} 

send_uart(0X0D); 
send_uart(0X0D);
send_uart(0X0A);
TC35_CON=CMGS;
}

//写信息中心号
void AT_CSCA()
{//AT+CSCA=+8613010130500
uchar i;
for(i=0;i<sizeof(TC_ZXH1)-1;i++)
{send_uart(TC_ZXH1[i]);} 
send_uart(0X22); 
for(i=0;i<sizeof(TC_ZXH2)-1;i++)
{send_uart(TC_ZXH2[i]);} 

send_uart(0X22); 

send_uart(0X0D);
send_uart(0X0A);
TC35_CON=CSCA;
}
//AT复位指令
void AT_RST()
{
uchar i;
for(i=0;i<sizeof(TC_ATF)-1;i++)
{send_uart(TC_ATF[i]);} 
send_uart(0X0D); 
send_uart(0X0D);
send_uart(0X0A);
TC35_CON=RST;
}
//AT网络状态查询指令
void AT_CREG()
{
uchar i;
for(i=0;i<sizeof(TC_WLZT)-1;i++)
{send_uart(TC_WLZT[i]);} 
send_uart(0X0D); 
send_uart(0X0D);
send_uart(0X0A);
TC35_CON=CREG;
}
//AT接收格式指令
void AT_CMGF()
{//AT+CMGF=1
uchar i;
for(i=0;i<sizeof(TC_MSXZ)-1;i++)
{send_uart(TC_MSXZ[i]);}
send_uart(0X0D); 
send_uart(0X0D); 
send_uart(0X0A);
TC35_CON=CMGF;
}

void AT_CMGF_PDU()
{//AT+CMGF=1
uchar i;
for(i=0;i<sizeof(TC_MSXZ_PDU)-1;i++)
{send_uart(TC_MSXZ_PDU[i]);}
send_uart(0X0D); 
send_uart(0X0D); 
send_uart(0X0A);
TC35_CON=CMGF;
}

//AT短消息读取指令
void AT_CMGR()
{//AT+CMGR=1
uchar i;
for(i=0;i<sizeof(TC_DQML)-1;i++)
{send_uart(TC_DQML[i]);}
send_uart(0X0D); 
send_uart(0X0D);
send_uart(0X0A);
TC35_CON=CMGR;
}
//AT短消息删除指令
void AT_CMGD()
{//AT+CMGD=1
uchar i;
for(i=0;i<sizeof(TC_SCML)-1;i++)
{send_uart(TC_SCML[i]);} 
send_uart(0X0D); 
send_uart(0X0D);
send_uart(0X0A);
TC35_CON=CMGD;
}


void SEND_MESSAGE()
{ 
delay_GSM(10);
if(TC35_MAX>0)
{
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
AT_CMGF();}
//如果不成功则重新关闭回显
else{  delay_GSM(10);
ATE0();
}
}
else if(TC35_CON==CMGF){

if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATCMGF_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCMGF_OK_12864);}
//------------------
TC35_CON=TC35RET; 
 delay_GSM(10);
AT_CREG();}
else{AT_CMGF();;}

}
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
AT_CSCA();}
else{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATCREG2_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCREG2_OK_12864);}
//------------------
AT_CREG();}
}
else if(TC35_CON==CMGD){;}
//填写信息中心号
else if(TC35_CON==CSCA)
{
delay_GSM(10);
if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{ 
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATCSCA_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCSCA_OK_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);AT_CMGS();
}
//如果没出现CALL READY 则重新初始化
else if((TC35_RE[2]=='+'))
{ LCD_12864_DISPLAY(0,2,ATCSCA2_OK_12864);
delay_GSM(100);
AT_CSCA();}
else{AT_CSCA();}
}
//填写发送内容
else if(TC35_CON==CMGS){
delay_GSM(10);
if((TC35_RE[2]==0X3E)&&(TC35_RE[3]==0X20))
{ 
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATCMGS_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCMGS_OK_12864);}
//------------------

TC35_CON=TC35RET;  
delay_GSM(100);ATNAME();}
else{AT_CMGS();}
}
//发送完毕返回继续扫描接收
else if(TC35_CON==ATDAT){ 
delay_GSM(10);
if(TC35_RE[2]=='+')
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATGSM_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATGSM_OK_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(1000);IR_FLAG=0;
}
else{;}
}
//复位接收组
for(i_TC35=0;i_TC35<=29;i_TC35++)
{TC35_RE[i_TC35]=0;}
i_TC35_RECE=0;
TC35_MAX=0;
}
}
//---------------------------PDU---------------------------------

void SEND_MESSAGE_PDU()
{ 
delay_GSM(10);
if(TC35_MAX>0)
{
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
AT_CMGF_PDU();}
//如果不成功则重新关闭回显
else{  delay_GSM(10);
ATE0();
}
}
else if(TC35_CON==CMGF){

if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATCMGF_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCMGF_OK_12864);}
//------------------
TC35_CON=TC35RET; 
 delay_GSM(10);
AT_CREG();}
else{AT_CMGF();;}

}
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
AT_CSCA();}
else{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0, ATCREG2_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCREG2_OK_12864);}
//------------------
AT_CREG();}
}
else if(TC35_CON==CMGD){;}
//填写信息中心号
else if(TC35_CON==CSCA)
{
delay_GSM(10);
if((TC35_RE[2]=='O')&&(TC35_RE[3]=='K'))
{ 
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATCSCA_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCSCA_OK_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(100);AT_CMGS_PDU();
}
//如果没出现CALL READY 则重新初始化
else if((TC35_RE[2]=='+'))
{ LCD_12864_DISPLAY(0,2,ATCSCA2_OK_12864);
delay_GSM(100);
AT_CSCA();}
else{AT_CSCA();}
}
//填写发送内容
else if(TC35_CON==CMGS){
delay_GSM(10);
if((TC35_RE[2]==0X3E)&&(TC35_RE[3]==0X20))
{ 
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATCMGS_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATCMGS_OK_12864);}
//------------------

TC35_CON=TC35RET;  
delay_GSM(100);
ATNAME();

}
else{AT_CMGS();}
}
//发送完毕返回继续扫描接收
else if(TC35_CON==ATDAT){ 
delay_GSM(10);
if(TC35_RE[2]=='+')
{
//------屏显------------------
if(LCD_SELECT==0x01)
{LCD(str0,ATGSM_OK);}
else if(LCD_SELECT==0x02)
{LCD_12864_DISPLAY(0,2,ATGSM_OK_12864);}
//------------------
TC35_CON=TC35RET;  
delay_GSM(1000);IR_FLAG=0;
}
else{;}
}
//复位接收组
for(i_TC35=0;i_TC35<=29;i_TC35++)
{TC35_RE[i_TC35]=0;}
i_TC35_RECE=0;
TC35_MAX=0;
}
}









