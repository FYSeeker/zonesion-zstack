/*********************************************************************************************
* 文件：photoresistance_sensor.c
* 作者：shench 2015.3.27
* 说明: 商业光照传感器，采用RS485通讯方式
* 修改：
* 注释：
*********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ioCC2530.h>
#include "sapi.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "OnBoard.h"
#include "hal_types.h"
#include "hal_uart.h"

#include "NH3.h"

/*********************************************************************************************
* 本地全局变量
*********************************************************************************************/
static uint8 f_szGetNH3[8]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};              //获取氨气命令,采用RS485通讯方式，
                                                                //通讯协议采用ModBus通讯协议
static float NH3_VAL = 0.0;                                     //缓存氨气值

/*********************************************************************************************
* 本地函数原型
*********************************************************************************************/
static void node_uart_init(void);
static void uart_callback_func(uint8 port, uint8 event);
static void uart_485_write(uint8 *pbuf, uint16 len);
static void node_uart_callback ( uint8 port, uint8 event );
static uint16 calc_crc(uint8 *buf, uint8 len);

/*********************************************************************************************
* 名称：phs_init()
* 功能：初始化商业光照传感器控制端口
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void NH3_init(void)
{
  // 初始化传感器代码 
  node_uart_init();
  
  NH3_update();
}

/*********************************************************************************************
* 名称：phs_update()
* 功能：更新一次光照度值，保存到全局静态变量(NH3_VAL)中
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void NH3_update(void)
{ 
  uart_485_write(f_szGetNH3, sizeof(f_szGetNH3));
}

/*********************************************************************************************
* 名称：get_lightness_val()
* 功能：获取光照度值的外部接口函数
* 参数：无
* 返回：f_fLightnessVal -- 返回静态全局变量光照度值
* 修改：
* 注释：
*********************************************************************************************/
float get_NH3_val(void)
{
  return (NH3_VAL/100);
}

/*********************************************************************************************
* 名称：uart_485_write()
* 功能：写485通讯
* 参数：pbuf -- 输入参数，发送命令指针
*       len -- 输入参数,发送命令长度
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void uart_485_write(uint8 *pbuf, uint16 len)
{
  HalUARTWrite(HAL_UART_PORT_0, pbuf, len);
}

/*********************************************************************************************
* 名称：uart_callback_func()
* 功能：RS485通讯回调函数，使用延时
* 参数：port -- 输入参数，数据接收端口
*       event -- 输入参数，接收事件
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void uart_callback_func(uint8 port, uint8 event) 
{
  if (event & HAL_UART_TX_EMPTY){
    uint16 szDelay[] = {2000,1000,500,100,10,10000};
    MicroWait(szDelay[HAL_UART_BR_9600]);                       // 波特率越慢延时时间需越长,延时太长将影响数据接收
  }
  node_uart_callback(port, event);
}

/*********************************************************************************************
* 名称：node_uart__callback()
* 功能：节点串口通讯回调函数
* 参数：port -- 输入参数，接收端口
*       event -- 输入参数，接收事件
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void node_uart_callback ( uint8 port, uint8 event )
{
#define RBUFSIZE 128
  (void)event;                                                // 此处暂不定义参数
  uint8  ch;
  static uint8 szBuf[RBUFSIZE];
  static uint8  len = 0;
  
  // 接收通过串口传来的数据
  while (Hal_UART_RxBufLen(port)){                              //获取数据
    HalUARTRead (port, &ch, 1);                                 //提取数据赋予ch
    if (len > 0){                                               //如果长度大于0
      szBuf[len++] = ch;                                        //将ch值赋予szBuf并将地址加一
      if (len == 7){                                            //如果长度为7，数据接收完毕
        uint16 crc;
        crc = calc_crc(szBuf, 5);                               //循环冗余检验，检验最后两个字节
        if (crc == ((szBuf[6]<<8) | szBuf[5])){                 //如果正确
          NH3_VAL = (float)((szBuf[3]<<8) | szBuf[4]);          //提取传感器采集光照强度值
        }
        len = 0;                                                //数据提取成功，len置零
      }
    }else{
      if (len == 0 && ch == 0x01){                              //当检测到数据0x01且len为0
        szBuf[len++] = ch;                                      //将ch值赋予szBuf并将地址加一
      }else{
        len = 0;                                                //否则数据错误，len置零
      }
    }
  }
}

/*********************************************************************************************
* 名称：node_uart_init()
* 功能：RS485串口初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void node_uart_init(void)
{
  halUARTCfg_t _UartConfigure;
  
  // UART 配置信息
  _UartConfigure.configured           = TRUE;                   //启用串口        
  _UartConfigure.baudRate             = HAL_UART_BR_9600;       //波特率率设置为9600
  _UartConfigure.flowControl          = FALSE;                  //关闭硬件数据流控制
  _UartConfigure.rx.maxBufSize        = 128;                    //最大接收数组128
  _UartConfigure.tx.maxBufSize        = 128;                    //最大发送数据128
  _UartConfigure.flowControlThreshold = (128 / 2);
  _UartConfigure.idleTimeout          = 6;
  _UartConfigure.intEnable            = TRUE;                   //使能串口        
  _UartConfigure.callBackFunc         = uart_callback_func;     //配置数据提取回调函数
  
  // 启动 UART 功能
  HalUARTOpen (HAL_UART_PORT_0, &_UartConfigure);
}

/*********************************************************************************************
* 名称：calc_crc()
* 功能：crc校验
* 参数：pbuf -- 输入参数，校验指针
*       len -- 输入参数，校验数组长度
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static uint16 calc_crc(uint8 *pbuf, uint8 len)
{
  uint16 crc = 0xffff;
  uint8 k, i;
  
  for (k = 0; k < len; k++){
    crc = crc ^ pbuf[k];                                      
    for(i = 0; i < 8; i++){
      uint8 tmp;
      tmp = crc&1;
      crc = crc>>1;
      crc &= 0x7fff;
      if (tmp == 1){
        crc ^= 0xa001;
      }
      crc &= 0xffff;
    }   
  }
  return crc;
}