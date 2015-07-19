
//º¯ÊýÉùÃ÷
void Delay1(void);
void Delay5Ms(void);
void Delay400Ms(void);
unsigned char ReadDataLCM(void);
unsigned char ReadStatusLCM(void);
void WriteDataLCM(unsigned char WDLCM);
void WriteCommandLCM(unsigned char WCLCM,BuysC);  
void LCMInit_1602(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char len, unsigned char *DData); 