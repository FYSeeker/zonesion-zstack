/*********************************************************************************************
* 文件：sensor.c
* 作者：Zhoucj 2018.6.27
* 说明：多合一传感器(ZPHS01)
*       变量A0表示CO2，A1表示VOC等级/甲醛，A2表示湿度，A3表示温度，A4表示PM2.5
*       V0表示主动上报时间间隔，D0(Bit0/Bit1/Bit2/Bit3/Bit4/Bit5)表示主动上报A0/A1/A2/A3/A4使能
*       V1=0表示CO2零点校准(零点指的是 400ppm，发送零点校准命令前请确保传感器在 400ppm 浓度下稳定运行 20 分钟)
*       默认值：A0=0,A1=0,A2=0.0,A3=0.0,A4=0,V0=30,D1=1,D0=0xFF
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sapi.h"
#include "osal_nv.h"
#include "addrmgr.h"
#include "mt.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "hal_uart.h"
#include "sensor.h"
#include "zxbee.h"
#include "zxbee-inf.h"
#include "OnBoard.h"
/*********************************************************************************************
* 宏定义---
*********************************************************************************************/
#define RELAY1                  P0_6                            // 定义继电器控制引脚
#define RELAY2                  P0_7                            // 定义继电器控制引脚
#define ON                      0                               // 宏定义打开状态控制为ON
#define OFF                     1                               // 宏定义关闭状态控制为OFF
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8 D0 = 0xFF;                                         // 默认打开主动上报功能
static uint8  D1 = 1;                                           // 粉尘测量初始状态为开启
static uint16 A0 = 0;                                           //CO2
static uint16 A1 = 0;                                           //VOC等级/甲醛
static float A2 = 0;                                            //湿度
static float A3 = 0;                                            //温度
static uint16 A4 = 0;                                           //PM2.5
static uint16 V0 = 30;                                          // V0设置为上报时间间隔，默认为30s

static unsigned char read_data[5] = {0x11,0x02,0x01,0x00,0xEC}; //获取当前传感器的值
static unsigned char co2_correct[6] = {0x11,0x03,0x03,0x01,0x90,0x58};//CO2零点校准
static unsigned char pm25_open[6] = {0x11,0x03,0x0C,0x02,0x1E,0xC0};//开启粉尘测量
static unsigned char pm25_stop[6] = {0x11,0x03,0x0C,0x01,0x1E,0xC1};//停止粉尘测量
/*********************************************************************************************
* UART RS485相关接口函数
*********************************************************************************************/
static void node_uart_init(void);
static void uart_callback_func(uint8 port, uint8 event);
static void uart_485_write(uint8 *pbuf, uint16 len);
static void node_uart_callback ( uint8 port, uint8 event );

/*********************************************************************************************
* 名称：updateV0()
* 功能：更新V0的值
* 参数：*val -- 待更新的变量
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateV0(char *val)
{
  //将字符串变量val解析转换为整型变量赋值
  V0 = atoi(val);                                               // 获取数据上报时间更改值
}
/*********************************************************************************************
* 名称：updateA0()
* 功能：更新A0的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA0(void)
{
  // 发送读取空气质量传感器值指令
  uart_485_write(read_data, 5);
}
/*********************************************************************************************
* 名称：updateA1()
* 功能：更新A1的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA1(void)
{

}
/*********************************************************************************************
* 名称：sensorInit()
* 功能：传感器硬件初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorInit(void)
{
  // 初始化传感器代码
  node_uart_init();

  // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
  osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
  // 启动定时器，触发传感器监测事件：MY_CHECK_EVT
  osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
}
/*********************************************************************************************
* 名称：sensorLinkOn()
* 功能：传感器节点入网成功调用函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorLinkOn(void)
{
  sensorUpdate();
  uart_485_write(read_data, 5);
}
/*********************************************************************************************
* 名称：sensorUpdate()
* 功能：处理主动上报的数据
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorUpdate(void)
{ 
  char pData[16];
  char *p = pData;
  
  ZXBeeBegin();                                                 // 智云数据帧格式包头
  
  // 根据D0的位状态判定需要主动上报的数值
  if ((D0 & 0x01) == 0x01){                                     // 若上报允许，则pData的数据包中添加数据
    updateA0();
    sprintf(p, "%d", A0); 
    ZXBeeAdd("A0", p);
  }
  if ((D0 & 0x02) == 0x02){                                     // 若上报允许，则pData的数据包中添加数据
    updateA0();
    sprintf(p, "%d", A1); 
    ZXBeeAdd("A1", p);
  }
  if ((D0 & 0x04) == 0x04){                                     // 若上报允许，则pData的数据包中添加数据
    updateA0();
    sprintf(p, "%.1f", A2); 
    ZXBeeAdd("A2", p);
  }
  if ((D0 & 0x08) == 0x08){                                     // 若上报允许，则pData的数据包中添加数据
    updateA0();
    sprintf(p, "%.1f", A3); 
    ZXBeeAdd("A3", p);
  }
  if ((D0 & 0x10) == 0x10){                                     // 若上报允许，则pData的数据包中添加数据
    updateA0();
    sprintf(p, "%d", A4); 
    ZXBeeAdd("A4", p);
  }
  
  sprintf(p, "%u", D1);                                         // 上报控制编码 
  ZXBeeAdd("D1", p);
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
  }
}
/*********************************************************************************************
* 名称：sensorCheck()
* 功能：传感器监测
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorCheck(void)
{
  updateA0();
}
/*********************************************************************************************
* 名称：sensorControl()
* 功能：传感器控制
* 参数：cmd - 控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorControl(uint8 cmd)
{
  // 根据cmd参数处理对应的控制程序
   if(cmd & 0x01){ 
    uart_485_write((unsigned char*)pm25_open, 6);                     //开启粉尘测量
  }
  else{
    uart_485_write((unsigned char*)pm25_stop, 6);                     //关闭粉尘测量
  }
}
/*********************************************************************************************
* 名称：ZXBeeUserProcess()
* 功能：解析收到的控制命令
* 参数：*ptag -- 控制命令名称
*       *pval -- 控制命令参数
* 返回：ret -- pout字符串长度
* 修改：
* 注释：
*********************************************************************************************/
int ZXBeeUserProcess(char *ptag, char *pval)
{ 
  int val;
  int ret = 0;	
  char pData[16];
  char *p = pData;
  
  // 将字符串变量pval解析转换为整型变量赋值
  val = atoi(pval);	
  // 控制命令解析
  if (0 == strcmp("CD0", ptag)){                                // 对D0的位进行操作，CD0表示位清零操作
    D0 &= ~val;
  }
  if (0 == strcmp("OD0", ptag)){                                // 对D0的位进行操作，OD0表示位置一操作
    D0 |= val;
  }
  if (0 == strcmp("D0", ptag)){                                 // 查询上报使能编码
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", D0);
      ZXBeeAdd("D0", p);
    } 
  }
  if (0 == strcmp("CD1", ptag)){                                // 对D1的位进行操作，CD1表示位清零操作
    D1 &= ~val;
    sensorControl(D1);                                          // 处理执行命令
  }
  if (0 == strcmp("OD1", ptag)){                                // 对D1的位进行操作，OD1表示位置一操作
    D1 |= val;
    sensorControl(D1);                                          // 处理执行命令
  }
  if (0 == strcmp("D1", ptag)){                                 // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", D1);
      ZXBeeAdd("D1", p);
    } 
  }
  if (0 == strcmp("A0", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%d", A0);     
      ZXBeeAdd("A0", p);
    } 
  }
  if (0 == strcmp("A1", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%d", A1);  
      ZXBeeAdd("A1", p);
    } 
  }
  if (0 == strcmp("A2", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%.1f", A2);  
      ZXBeeAdd("A2", p);
    } 
  }
  if (0 == strcmp("A3", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%.f", A3);  
      ZXBeeAdd("A3", p);
    } 
  }
  if (0 == strcmp("A4", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%d", A4);  
      ZXBeeAdd("A4", p);
    } 
  }
  if (0 == strcmp("V0", ptag)){
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", V0);                         	// 上报时间间隔
      ZXBeeAdd("V0", p);
    }else{
      updateV0(pval);
    }
  }
  return ret;
}
/*********************************************************************************************
* 名称：MyEventProcess()
* 功能：自定义事件处理
* 参数：event -- 事件编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void MyEventProcess( uint16 event )
{
  if (event & MY_REPORT_EVT) { 
    sensorUpdate();
    //启动定时器，触发事件：MY_REPORT_EVT 
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, V0*1000);
  }  
  if (event & MY_CHECK_EVT) { 
    sensorCheck(); 
    // 启动定时器，触发事件：MY_CHECK_EVT 
    osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 1000);
  } 
}
/*********************************************************************************************
* 名称：uart0_init()
* 功能：串口0初始化函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void node_uart_init(void)
{
  halUARTCfg_t _UartConfigure;
  
  // UART 配置信息
  _UartConfigure.configured           = TRUE;
  _UartConfigure.baudRate             = HAL_UART_BR_9600;
  _UartConfigure.flowControl          = FALSE;
  _UartConfigure.rx.maxBufSize        = 128;
  _UartConfigure.tx.maxBufSize        = 128;
  _UartConfigure.flowControlThreshold = (128 / 2);
  _UartConfigure.idleTimeout          = 6;
  _UartConfigure.intEnable            = TRUE;
  _UartConfigure.callBackFunc         = uart_callback_func;
  
  HalUARTOpen (HAL_UART_PORT_0, &_UartConfigure);               //启动UART
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
  P2_0 = 1;
  
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
  if (event & HAL_UART_TX_EMPTY) 
  {
    uint16 szDelay[] = {2000,1000,500,100,10,10000};
    MicroWait(szDelay[HAL_UART_BR_9600]);                       // 波特率越慢延时时间需越长,延时太长将影响数据接收
    P2_0 = 0;
  }
  node_uart_callback(port, event);
}

/*********************************************************************************************
* 名称：node_uart_callback()
* 功能：节点串口通讯回调函数
* 参数：port -- 输入参数，接收端口
*       event -- 输入参数，接收事件
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void node_uart_callback( uint8 port ,uint8 event)
{
#define RBUFSIZE 32
  (void)event;
  uint8  ch;
  static uint8 szBuf[RBUFSIZE];
  static uint8 rlen = 0;
  static uint8 data_len = 0;
  static uint8 flag_ = 0x00;
  static uint8 data_mode = 0xff;
  
  // 接收通过串口传来的数据
  while (Hal_UART_RxBufLen(port))
  {
    HalUARTRead (port, &ch, 1);
    if((ch == 0x16) &&(flag_ == 0x00))
    {
      flag_ = 0x01;
    }
    else if(flag_ == 0x01){
      data_len = ch;
      flag_ = 0x02;
    }
    else if(flag_ == 0x02)
    {
      data_mode = ch;
      flag_ = 0x03;
    }
    else if((flag_ == 0x03)){
      szBuf[rlen++] = ch;
      if(rlen>=data_len)
      {
        flag_ = 0x00;
        switch (data_mode){
        case 0x01:
          A0 = szBuf[0]*256+szBuf[1];
          A1 = szBuf[2]*256+szBuf[3];
          A2 = (szBuf[4]*256+szBuf[5])*0.1;
          A3 = (szBuf[6]*256+szBuf[7]-500)*0.1;
          A4 = szBuf[8]*256+szBuf[9];
          break;
        case 0x03:
          break;
        case 0x0c:
          if(szBuf[0] == 0x01)
            D1 = 0;
          else
            D1 = 1;
          break;
        default:
          rlen = 0;
          break;
        }
        rlen = 0;
      }
    }
  }
}