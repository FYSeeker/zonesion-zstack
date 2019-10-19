/*********************************************************************************************
* 文件：relay.c
* 作者：
* 说明：
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "relay.h"
#include "hal_uart.h"
#include "string.h"
#include "sapi.h"
#include "OnBoard.h"

/*********************************************************************************************
* 变量定义
*********************************************************************************************/
uint8 rx_len=0;
uint8 rx_buff[50] = {0};
unsigned char relay_status = 0;

/*********************************************************************************************
* 函数声明
*********************************************************************************************/
void node_uart_init(void);
void uart_callback_func(uint8 port, uint8 event);

/*********************************************************************************************
* 名称：uart_callback_func()
* 功能：RS485通讯回调函数，使用延时
* 参数：port -- 输入参数，数据接收端口
*       event -- 输入参数，接收事件
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void uart_callback_func(uint8 port, uint8 event) 
{
  uint8 ch=0;
  static uint8 rx_status = 0;
  static uint8 check_data = 0;
  //接收通过串口传来的数据
  while (Hal_UART_RxBufLen(port))                             //获取数据
  {                            
    HalUARTRead (port,&ch,1);                               //提取数据赋予ch
    
    if(rx_len >= 3) {
      if (check_data == ch){
      if(rx_buff[2] == 0x01){
        relay_status |= rx_buff[1];
      }
      else{
        relay_status &= ~rx_buff[1];
      }
    }
      memset(rx_buff,0,50);
      rx_status = 0;
      rx_len = 0;
      check_data = 0;
    }
    else{
    if(ch == 0XFD){
      rx_status = 1;
    }
    else if(rx_status == 1){
      rx_buff[rx_len++] = ch; 
      check_data = ((check_data + ch) & 0xff);
    }
  }
  }
}


/*********************************************************************************************
* 名称：node_uart_init()
* 功能：RS485串口0初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void node_uart_init(void)
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
* 名称：relay_init
* 功能：噪声传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void relay_init(void)
{
  node_uart_init();
  relay_off(0XFF);
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
* 名称：relay_on
* 功能：继电器控制
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void relay_on(unsigned char cmd)
{
  uint8 relay_on_cmd[5] = {0xFB,0x00,0x00,0x01,0x01};
  relay_on_cmd[2] = cmd;
  relay_on_cmd[4] = (relay_on_cmd[1] + relay_on_cmd[2] + relay_on_cmd[3]) & 0xff;
  uart_485_write(relay_on_cmd, 5);
}

/*********************************************************************************************
* 名称：relay_off
* 功能：继电器控制
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void relay_off(unsigned char cmd)
{
  uint8 relay_on_cmd[5] = {0xFB,0x00,0x00,0x00,0x00};
  relay_on_cmd[2] = cmd;
  relay_on_cmd[4] = (relay_on_cmd[1] + relay_on_cmd[2] + relay_on_cmd[3]) & 0xff;
  uart_485_write(relay_on_cmd, 5);  
}

/*********************************************************************************************
* 名称：get_relay_status
* 功能：继电器状态获取
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_relay_status(void)
{
  return relay_status;
}
