/*********************************************************************************************
* 文件：SensorUart.c
* 作者：
* 说明：xLab Sensor Uart初始化程序
* 修改：
* 注释：
*********************************************************************************************/
#include "SensorUart.h"

void (*UartCallback)(char dat) = NULL;

void UartCallbackSet(void (*func)(char dat))
{
  UartCallback = func;
}

/*********************************************************************************************
* 名称：uart0_init(unsigned char StopBits,unsigned char Parity)
* 功能：串口0初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
****************************************** 
*    CC253O 32M系统时钟波特率参数表      * 
*----------------------------------------* 
*  波特率  UxBAUD         UxGCRM         * 
*  240     59             6              * 
*  4800    59             7              * 
*  9600    59             8              * 
*  14400   216            8              * 
*  19200   59             9              * 
*  28800   216            9              * 
*  38400   59             10             * 
*  57600   216            10             * 
*  76800   59             11             * 
*  115200  216            11             * 
*  23040   216            12             * 
****************************************** 
*********************************************************************************************/
void uart0_init(unsigned char StopBits,unsigned char Parity)
{
  P0SEL |=  0x0C;                                               //初始化UART0端口
  PERCFG&= ~0x01;                                               //选择UART0为可选位置一
  P2DIR &= ~0xC0;                                               //P0优先作为串口0
  U0CSR = 0xC0;                                                 //设置为UART模式,而且使能接受器
  
  U0GCR = 8;                  
  U0BAUD = 59;                                                  //波特率设置为9600
  
  U0UCR = 2;
  URX0IE = 1;                                                   // 使能接收中断
}

/*********************************************************************************************
* 名称：uart_send_char()
* 功能：串口发送字节函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void uart_send_char(char ch)
{
  U0DBUF = ch;                                                  //将要发送的数据填入发送缓存寄存器
  while((U0CSR&0x02) == 0);                                     
  U0CSR &= ~0x02;                                               //发送完成后将数据清零
}



#pragma vector=URX0_VECTOR  
__interrupt void UART0_ISR(void)  
{  
  char ch = 0;
  URX0IF = 0;                                                   // 清除接收中断标志  
  ch = U0DBUF; 
  if(UartCallback != NULL)
    UartCallback(ch);
}