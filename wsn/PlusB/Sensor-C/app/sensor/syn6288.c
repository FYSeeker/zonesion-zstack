#include "syn6288.h"
#include  "string.h"
#include "usart.h"

void syn6288_io_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                        //定义一个GPIO_InitTypeDef类型的结构体
    RCC_AHB1PeriphClockCmd(SYN6288_RCC, ENABLE);                //开启语音合成相关的GPIO外设时钟
    
    GPIO_InitStructure.GPIO_Pin = SYN6288_PIN;                  //选择要控制的GPIO引脚
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //设置引脚的输出类型为推挽
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                //设置引脚模式为输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;              //设置引脚为下拉模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;            //设置引脚速率为2MHz
    
    GPIO_Init(SYN6288_GPIO, &GPIO_InitStructure);               //初始化GPIO配置
}

void syn6288_init(void)
{
    uart3_init(9600);
    syn6288_io_init();
}

unsigned char syn6288_busy(void)
{
    if(GPIO_ReadInputDataBit(SYN6288_GPIO, SYN6288_PIN))
        return 0;
    else
        return 1;
}

void syn6288_play(char *s)
{
    int i;
    int len = strlen(s);
    unsigned char c = 0;
    unsigned char head[] = {0xFD, 0x00, 0x00, 0x01, 0x00};
    head[1] = (len + 3) >> 8;
    head[2] = (len + 3) & 0xff;
    for(i=0; i<5; i++)
    {
        uart3_putc(head[i]);
        c ^= head[i];
    }
    for(i=0; i<len; i++)
    {
        uart3_putc(s[i]);
        c ^= s[i];
    }
    uart3_putc(0xA5);
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
    uart3_putc(head[i]);
    c ^= head[i];
  }
  for (i=0; i<len; i++){
    uart3_putc(s[i]);
    c ^= s[i];
  }
  uart3_putc(c);
}
