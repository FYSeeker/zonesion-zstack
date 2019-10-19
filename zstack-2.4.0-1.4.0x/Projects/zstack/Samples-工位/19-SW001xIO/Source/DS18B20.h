/*********************************************************************************************
* 文件：DS18B30.h
* 作者：yangchh 2016.06.23
* 说明：DS18B20驱动程序头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef __DS18B20_H
#define __DS18B20_H
#include <ioCC2530.h>

#define DS18B20_DQ      P0_5    //水温传感器数据引脚
   	
/*********************************************************************************************
* 说明：函数申明
*********************************************************************************************/
unsigned char DS18B20_Init(void);                                          //初始化DS18B20
void set_io(unsigned char mode);
float DS18B20_Get_Temp(void);                                   //获取温度 
void DS18B20_Start(void);                                       //开始温度转换
void DS18B20_Write_Byte(unsigned char dat);                                //写入一个字节
unsigned char DS18B20_Read_Byte(void);                                     //读出一个字节
unsigned char DS18B20_Read_Bit(void);                                      //读出一个位
unsigned char DS18B20_Check(void);                                         //检测是否存在DS18B20 
void DS18B20_Rst(void);                                         //复位DS18B20


#endif      

