/*********************************************************************************************
* 文件：syn6288.c
* 作者：zonesion
* 说明：syn6288驱动程序
* 修改：Chenkm 2017.01.04 增加了注释
* 注释：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "syn6288.h"
#include "string.h"

/*********************************************************************************************
* 名称：uart0_Init(unsigned char StopBits,unsigned char Parity)
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
void uart0_Init(unsigned char StopBits,unsigned char Parity)
{
  P0SEL |=  0x0C;                                               //初始化UART0端口
  PERCFG&= ~0x01;                                               //选择UART0为可选位置一
  P2DIR &= ~0xC0;                                               //P0优先作为串口0
  U0CSR = 0xC0;                                                 //设置为UART模式,而且使能接受器
   
  U0GCR = 0x8;                  
  U0BAUD = 59;                                                  //波特率设置为9600
  
  //U0UCR |= 2|StopBits|Parity;                                     //设置停止位与奇偶校验
  U0UCR = 2;
}

/*********************************************************************************************
* 名称：uart_send_char()
* 功能：串口发送字节函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void uart_send_char(unsigned char ch)
{
  //while(!(U0CSR & 0x02));
  
  U0DBUF = ch;                                                  //将要发送的数据填入发送缓存寄存器
  while(UTX0IF == 0);                                           //等待数据发送完成
  UTX0IF = 0;                                                   //发送完成后将数据清零
  //while(U0CSR & 0x02);

}

/*********************************************************************************************
* 名称：syn6288_init()
* 功能：syn6288初始化
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void syn6288_init()
{
  uart0_Init(0,0); 
  P0SEL &= ~0x02;                                               //配置管脚为通用IO模式
  P0DIR &= ~0x02;                                               //配置控制管脚为输入模式
}

int syn6288_busy(void)
{ 
  if(P0_1)                                                      //忙检测引脚
    return 0;                                                   //没有检测到信号返回 0
  else
    return 1;                                                   //检测到信号返回 1
}
/*********************************************************************************************
* 名称：syn6288_play()
* 功能：
* 参数：s -- 数组名
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void syn6288_play(char *s)
{
  int i;
  int len = strlen(s);
  unsigned char c = 0;  
  unsigned char head[] = {0xFD,0x00,0x00,0x01,0x00};            //数据包头  
  head[1] = (len+3)>>8;
  head[2] = (len+3)&0xff;
  for (i=0; i<5; i++){
    uart_send_char(head[i]);
    c ^= head[i];
  }
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
    c ^= s[i];
  }
  uart_send_char(0xA5);
}

/*******************************************************************************
* 名称: hex2unicode()
* 功能: 将16进制字符串转换成bin格式
* 参数: 
* 返回: 
* 修改:
* 注释: 
*******************************************************************************/
char *hex2unicode(char *str)
{       
  static char uni[64];
  int n = strlen(str)/2;
  if (n > 64) n = 64;
  
  for (int i=0; i<n; i++) {
    unsigned int x = 0;
    for (int j=0; j<2; j++) {
      char c = str[2*i+j];
      char o;
      if (c>='0' && c<='9') o = c - '0';
      else if (c>='A' && c<='F') o = 10+(c-'A');
      else if (c>='a' && c<='f') o = 10+(c-'a');
      else o = 0;
      x = (x<<4) | (o&0x0f);
    }
    uni[i] = x;
  }
  uni[n] = 0;
  return uni;
}


/*******************************************************************************
* syn6288_play_unicode()
* 功能：
* 参数：s -- 数组名
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_play_unicode(char *s, int len)
{
  int i;
  char c = 0;  
  unsigned char head[] = {0xFD,0x00,0x00,0x01,0x03};            //数据包头  
  head[1] = (len+3)>>8;
  head[2] = (len+3)&0xff;
  for (i=0; i<5; i++){
    uart_send_char(head[i]);
    c ^= head[i];
  }
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
    c ^= s[i];
  }
  uart_send_char(c);
}
