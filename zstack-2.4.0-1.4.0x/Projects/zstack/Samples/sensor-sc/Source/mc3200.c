/*********************************************************************************************
* 文件：SensorUart.c
* 作者：
* 说明：xLab Sensor Uart初始化程序
* 修改：
* 注释：
*********************************************************************************************/
#include "mc3200.h"

#define MC_RBUFSIZE 256

static char MC_RBUF[MC_RBUFSIZE];
static char Mc3200UartReady = 0;
static short Mc3200Result = 0;
void (*Mc3200SenByte)(char dat) = NULL;

void Mc3200SendByteSet(void (*func)(char dat))
{
  Mc3200SenByte = func;
}

char Mc3200SendData(char* dat,short len)
{
  if(Mc3200SenByte != NULL)
  {
    for(short i=0;i<len;i++)
    {
      Mc3200SenByte(dat[i]);
    }
    return 0;
  }
  return 1; 
}

char Mc3200GetUartReady()
{
  if(Mc3200UartReady == 1)
    return 1;
  return 0;
}

void Mc3200CheckUart()
{
  char ReadDevice[] = {"$010300-C980"};
  Mc3200SendData(ReadDevice,strlen(ReadDevice));
}

void Mc3200FactoryReset()
{
  char ReadDevice[] = {"$010200-BF34"};
  Mc3200SendData(ReadDevice,strlen(ReadDevice));
}

void Mc3200SetReadInterval()
{
  char ReadDevice[] = {"$1003F2-388B"};//相邻条码 2 秒内不重复输出
  Mc3200SendData(ReadDevice,strlen(ReadDevice));
}

void Mc3200SetEndMark()
{
  char ReadDevice[] = {"$201003-881D"};//增加结束符 CRLF(0x0D,0x0A)
  Mc3200SendData(ReadDevice,strlen(ReadDevice));
}


void Mc3200SetResponseMode()
{
  char ReadDevice[] = {"$020B01-0090"};//应答模式1
  Mc3200SendData(ReadDevice,strlen(ReadDevice));
}

void Mc3200Set()
{
  Mc3200SetEndMark();
  Mc3200SetReadInterval();
}

char Mc3200GetResult()
{
  if(Mc3200Result > 2)
  {
    return 1;
  }
  return 0;
}

char Mc3200GetResultData(char* buf)
{
  if(Mc3200Result > 2)
  {
    memcpy(buf,(const char*)MC_RBUF,Mc3200Result);
    Mc3200Result = 0;
    return 1;
  }
  return 0;
}

void Mc3200UartIrq(char dat)
{
  static short offset = 0;
  
  if(Mc3200Result == 0)
  {
    if(offset < MC_RBUFSIZE)
    {
      MC_RBUF[offset++] = dat;
      if(memcmp(&MC_RBUF[offset-2],"\r\n",2) == 0)
      {
        if(Mc3200UartReady == 1)
        {
          MC_RBUF[offset-2] = 0;
          Mc3200Result = offset-2;
        }
        else
        {
          if(memcmp(MC_RBUF,"FW",2) == 0)
          {
            Mc3200UartReady = 1;
          }
        }
        offset = 0;
      }
    }
    else
    {
      offset = 0;
    }
  }
}

void Mc3200Init()
{
  uart0_init(0,0);
  UartCallbackSet(Mc3200UartIrq);
  Mc3200SendByteSet(uart_send_char);
}



