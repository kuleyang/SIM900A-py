//SIM900a模块
//AT指令表
unsigned char code AT[]			 ="AT";				//握手信号
unsigned char code AT_CCID[]	 ="AT+CCID";			//获取SIM卡号(SIM卡号为20位号组成)89860 0C226 10A74 59948
unsigned char code ATI[]		 ="ATI";				//设备初始化
unsigned char code ATE0V1[]		 ="ATE0V1";			//关回显设置命令返回OK/V1 0/V0
unsigned char code AT_IPR[]		 ="AT+IPR=9600"; 	//波特率设置
unsigned char code AT_CPIN[]	 ="AT+CPIN?";		//查询SIM卡状态
unsigned char code AT_CSQ[] 	 ="AT+CSQ";			//查询信号
unsigned char code ATH[]		 ="ATH";				//呼叫挂起
unsigned char code AT_COPS[]	 ="AT+COPS?";		//查询网络运营商
unsigned char code AT_CDSNORIP[] ="AT+CDNSORIP=0";	//GPRS IP 方式
unsigned char code AT_CIPHEAD[]	 ="AT+CIPHEAD=1"; 	//GPRS 接收方式
unsigned char code AT_CIPSTART[] ="AT+CIPSTART=\"TCP\",\"123.138.22.72\",\"8080\"";//GPRS连接方式\GPRS连接地址(根据用户实际IP修改)\GPRS连接的端口
unsigned char code AT_CIPSTART_2[] ="AT+CIPSTART=\"TCP\",\"";//GPRS连接方式\GPRS连接地址(根据用户实际IP修改)\GPRS连接的端口 //DCL
unsigned char code AT_CIPSEND[]	 ="AT+CIPSEND";		//GPRS发送信息命令
unsigned char code AT_CIPCLOSE[] ="AT+CIPCLOSE";		//TCP连接关闭
unsigned char code AT_CIPSHUT[]	 ="AT+CIPSHUT";		//GPRS连接关闭  ----就用一个就好(断开后，本地IP地址改变)
unsigned char code AT_CIPSTATUS[]="AT+CIPSTATUS";   //检测GPRS网络连接状态
unsigned char code ATA[]		="ATA";				//来电接听

unsigned char code ATD[]		="ATD13259819058;";				//来电接听

//短信内容控制

unsigned char code AT_CMGF[]="AT+CMGF=1";     //读取第一条短信
unsigned char code AT_CMGS[]="AT+CMGS=\"13259792128\"";	  //发短信息
//unsigned char code AT_CMGS[]="AT+CMGS=\"15289453502\"";	  //发短信息
unsigned char code AT_CMGR[]="AT+CMGR=1";     //读取第一条短信
unsigned char code AT_CMGD[]="AT+CMGD=1,4";   //删除所有短信

//定时TIMER0用
unsigned int  xdata timercount;			//50MS定时器,
unsigned char xdata timer_1S_cnt;
unsigned char xdata timer_10S_cnt;
unsigned char xdata timer_S_cnt;

//GSM接收需要拨打电话号码
unsigned char xdata AT_NOW[25];    //GSM接收到的手机号码
unsigned char xdata VOICE_NOW[100];
unsigned char xdata ALARM_TEL_M  ; //应急电话长度
unsigned char xdata ALARM_TEL[15]; //应急电话存放数组

unsigned char xdata ALARM_TEL2_M=0; //应急电话2长度　　---用在GPRS拨号
unsigned char xdata ALARM_TEL2[20]; //应急电话2存放数组---用在GPRS拨号

//重新建立GPRS联接所需要的IP地址及端口号数组
unsigned char xdata IP_PORT_M=0;    //IP与端口号所占用的数据长度(<=24个字节)
unsigned char xdata VAR_IP_PORT[24];//IP与端口号存放数组
unsigned char xdata IP_PORT_M1=0;    //IP与端口号所占用的数据长度(<=24个字节)
unsigned char xdata VAR_IP_PORT1[24];//IP与端口号存放数组

//SIM900a函数声明
void Delay_ms(unsigned int i);
void sendchar(unsigned char ch);
void sendstring(unsigned char *p);
void Start_GSM(void);
void clear_SystemBuf(void);
void GSM_init(void);
unsigned char strsearch(unsigned char *ptr2,unsigned char *ptr1_at);//查字符串*ptr2在*ptr1中的位置






