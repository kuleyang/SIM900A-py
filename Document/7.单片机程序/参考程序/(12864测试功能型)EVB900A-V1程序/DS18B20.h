

//适应电压范围更宽，电压范围：3.0～5.5V，在寄生电源方式下可由数据线供电
//独特的单线接口方式，DS18B20在与微处理器连接时仅需要一条口线即可实现微处理器与DS18B20的双向通讯
//S18B20支持多点组网功能，多个DS18B20可以并联在唯一的三线上，实现组网多点测温
//DS18B20在使用中不需要任何外围元件，全部传感元件及转换电路集成在形如一只三极管的集成电路内
//温范围－55℃～＋125℃，在-10～+85℃时精度为±0.5℃
//可编程的分辨率为9～12位，对应的可分辨温度分别为0.5℃、0.25℃、0.125℃和0.0625℃，可实现高精度测温
//在9位分辨率时最多在93.75ms内把温度转换为数字，12位分辨率时最多在750ms内把温度值转换为数字，速度更快
//测量结果直接输出数字温度信号，以"一线总线"串行传送给CPU，同时可传送CRC校验码，具有极强的抗干扰纠错能力

#include "C8051F340.H"

void DS18B20_1us(unsigned int us);  //延时延时
unsigned char Init_DS18B20(void);   //复位
unsigned char read_byte(void);      //从 1-wire 总线上读取一个字节
void write_byte(unsigned char dat); //向 1-WIRE 总线上写一个字节
void Convert_T();                   //启动温度转换
bit Read_Power_Supply();            //读电源工作方式
void Skip_ROM();                    //函数功能：跳过ROM编码指令
//void Read_B20_SN(void);             //读取序列号
unsigned int Read_Temperature(void);//读取温度
