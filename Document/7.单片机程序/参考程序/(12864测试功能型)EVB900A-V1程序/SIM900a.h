//SIM900aģ��
//ATָ���
unsigned char code AT[]			 ="AT";				//�����ź�
unsigned char code AT_CCID[]	 ="AT+CCID";			//��ȡSIM����(SIM����Ϊ20λ�����)89860 0C226 10A74 59948
unsigned char code ATI[]		 ="ATI";				//�豸��ʼ��
unsigned char code ATE0V1[]		 ="ATE0V1";			//�ػ������������OK/V1 0/V0
unsigned char code AT_IPR[]		 ="AT+IPR=9600"; 	//����������
unsigned char code AT_CPIN[]	 ="AT+CPIN?";		//��ѯSIM��״̬
unsigned char code AT_CSQ[] 	 ="AT+CSQ";			//��ѯ�ź�
unsigned char code ATH[]		 ="ATH";				//���й���
unsigned char code AT_COPS[]	 ="AT+COPS?";		//��ѯ������Ӫ��
unsigned char code AT_CDSNORIP[] ="AT+CDNSORIP=0";	//GPRS IP ��ʽ
unsigned char code AT_CIPHEAD[]	 ="AT+CIPHEAD=1"; 	//GPRS ���շ�ʽ
unsigned char code AT_CIPSTART[] ="AT+CIPSTART=\"TCP\",\"123.138.22.72\",\"8080\"";//GPRS���ӷ�ʽ\GPRS���ӵ�ַ(�����û�ʵ��IP�޸�)\GPRS���ӵĶ˿�
unsigned char code AT_CIPSTART_2[] ="AT+CIPSTART=\"TCP\",\"";//GPRS���ӷ�ʽ\GPRS���ӵ�ַ(�����û�ʵ��IP�޸�)\GPRS���ӵĶ˿� //DCL
unsigned char code AT_CIPSEND[]	 ="AT+CIPSEND";		//GPRS������Ϣ����
unsigned char code AT_CIPCLOSE[] ="AT+CIPCLOSE";		//TCP���ӹر�
unsigned char code AT_CIPSHUT[]	 ="AT+CIPSHUT";		//GPRS���ӹر�  ----����һ���ͺ�(�Ͽ��󣬱���IP��ַ�ı�)
unsigned char code AT_CIPSTATUS[]="AT+CIPSTATUS";   //���GPRS��������״̬
unsigned char code ATA[]		="ATA";				//�������

unsigned char code ATD[]		="ATD13259819058;";				//�������

//�������ݿ���

unsigned char code AT_CMGF[]="AT+CMGF=1";     //��ȡ��һ������
unsigned char code AT_CMGS[]="AT+CMGS=\"13259792128\"";	  //������Ϣ
//unsigned char code AT_CMGS[]="AT+CMGS=\"15289453502\"";	  //������Ϣ
unsigned char code AT_CMGR[]="AT+CMGR=1";     //��ȡ��һ������
unsigned char code AT_CMGD[]="AT+CMGD=1,4";   //ɾ�����ж���

//��ʱTIMER0��
unsigned int  xdata timercount;			//50MS��ʱ��,
unsigned char xdata timer_1S_cnt;
unsigned char xdata timer_10S_cnt;
unsigned char xdata timer_S_cnt;

//GSM������Ҫ����绰����
unsigned char xdata AT_NOW[25];    //GSM���յ����ֻ�����
unsigned char xdata VOICE_NOW[100];
unsigned char xdata ALARM_TEL_M  ; //Ӧ���绰����
unsigned char xdata ALARM_TEL[15]; //Ӧ���绰�������

unsigned char xdata ALARM_TEL2_M=0; //Ӧ���绰2���ȡ���---����GPRS����
unsigned char xdata ALARM_TEL2[20]; //Ӧ���绰2�������---����GPRS����

//���½���GPRS��������Ҫ��IP��ַ���˿ں�����
unsigned char xdata IP_PORT_M=0;    //IP��˿ں���ռ�õ����ݳ���(<=24���ֽ�)
unsigned char xdata VAR_IP_PORT[24];//IP��˿ںŴ������
unsigned char xdata IP_PORT_M1=0;    //IP��˿ں���ռ�õ����ݳ���(<=24���ֽ�)
unsigned char xdata VAR_IP_PORT1[24];//IP��˿ںŴ������

//SIM900a��������
void Delay_ms(unsigned int i);
void sendchar(unsigned char ch);
void sendstring(unsigned char *p);
void Start_GSM(void);
void clear_SystemBuf(void);
void GSM_init(void);
unsigned char strsearch(unsigned char *ptr2,unsigned char *ptr1_at);//���ַ���*ptr2��*ptr1�е�λ��






