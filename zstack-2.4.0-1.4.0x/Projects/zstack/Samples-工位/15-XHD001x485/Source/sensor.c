/*********************************************************************************************
* 文件：sensor.c
* 作者：Zhoucj 2018.6.21
* 说明：信号灯传感器
*       定时上报信号灯状态
*       V0表示主动上报时间间隔
*       V1表示闪烁的时间间隔,默认为0
*       D0(Bit0)表示主动上报使能
*       D1(Bit0-Bit5)分别表示信号灯一，信号灯二红黄绿灯状态
*       默认值：D1=0,V0=30,D0=1,V1=0
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
* 宏定义
*********************************************************************************************/
#define RELAY1                  P0_6                            // 定义继电器控制引脚
#define RELAY2                  P0_7                            // 定义继电器控制引脚
#define ON                      0                               // 宏定义打开状态控制为ON
#define OFF                     1                               // 宏定义关闭状态控制为OFF
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8 D0 = 1;                                            // 默认打开主动上报功能
static uint8 D1 = 0;                                            // 继电器初始状态为全关
static uint16 V0 = 30;                                          // V0设置为上报时间间隔，默认为30s
static uint16 V1 = 0;                                           // 三色灯闪烁间隔，默认为0s
unsigned char relay_off[5] = {0xFB,0x00,0xFF,0x02,0x01};
/*********************************************************************************************
* 函数声明
*********************************************************************************************/
void update_relay(void);
void MyUartInit(void);
void MyUartCallBack ( uint8 port, uint8 event );

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
* 名称：updateV0()
* 功能：更新V0的值
* 参数：*val -- 待更新的变量
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateV1(char *val)
{
  //将字符串变量val解析转换为整型变量赋值
  V1 = atoi(val);                                               // 获取数据上报时间更改值
  if(V1 <= 0)
      V1 = 0;
  else if(V1 >= 65535)
      V1 = 65535;
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

void delay_ms(uint16 ms)
{
  for(uint16 i=0; i<ms; i++)
      Onboard_wait(1000);
}

/*********************************************************************************************
* 名称：update_relay()
* 功能：更新继电器控制板状态
* 参数：cmd--继电器编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void update_relay(void)
{
  unsigned char relay_cmd[5] = {0xFB,0x00,0xFF,0x01,0x01};       // 控制指令
  static unsigned char twinkle_flag = 0;                        // 信号灯当前的状态
  static unsigned char num = 0;                                 // 时间标志位
  static int last_D1 = -1;                                       // 上一次D1的值
  if(V1 > 0)
  {
    if(twinkle_flag == 1 && V1 == num){
      HalUARTWrite(HAL_UART_PORT_0, relay_off, 5);                //关闭所有信号灯
      twinkle_flag = 0;
      num = 0;
    }else {
      if(twinkle_flag == 0 && V1 == num) {
        relay_cmd[2] = D1;
        relay_cmd[4] = ((relay_cmd[1] + relay_cmd[2] + relay_cmd[3]) & 0xff);
        HalUARTWrite(HAL_UART_PORT_0, relay_cmd, 5);
        twinkle_flag = 1;
        num = 0;
      }     
    }
    num++;
  }
  else
  {
    if(last_D1 != D1 || twinkle_flag == 0) {                    // D1改变或者信号灯处于关闭状态 重新刷新信号灯状态
      HalUARTWrite(HAL_UART_PORT_0, relay_off, 5);              // 关闭所有信号灯
      delay_ms(200);
      relay_cmd[2] = D1;
      relay_cmd[4] = ((relay_cmd[1] + relay_cmd[2] + relay_cmd[3]) & 0xff);
      HalUARTWrite(HAL_UART_PORT_0, relay_cmd, 5);
      last_D1 = D1;
      twinkle_flag = 1;
    }
  }
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
  MyUartInit();
  
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
  update_relay();
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
    RELAY1 = ON;                                                // 开启继电器1
  }
  else{
    RELAY1 = OFF;                                               // 关闭继电器1
  }
  if(cmd & 0x02){  
    RELAY2 = ON;                                                // 开启继电器2
  }
  else{
    RELAY2 = OFF;                                               // 关闭继电器2        
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
  if (0 == strcmp("CD1", ptag)) {
    D1 &= ~val;
  }
  if (0 == strcmp("OD1", ptag)) {
    D1 |= val;
  }
  if (0 == strcmp("D1", ptag)){                                 // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", D1);
      ZXBeeAdd("D1", p);
    } 
  }
  if (0 == strcmp("V0", ptag)){
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", V0);                         		// 上报时间间隔
      ZXBeeAdd("V0", p);
    }else{
      updateV0(pval);
    }
  }
  if (0 == strcmp("V1", ptag)){
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", V1);
      ZXBeeAdd("V1", p);
    }else{
      updateV1(pval);
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
* 名称：MyUartInit()
* 功能：串口初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void MyUartInit(void)
{
  halUARTCfg_t uartConfig;
  
  //串口配置
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_9600;
  uartConfig.flowControl          = FALSE;
  uartConfig.rx.maxBufSize        = 128;
  uartConfig.tx.maxBufSize        = 128;
  uartConfig.flowControlThreshold = (128 / 2);
  uartConfig.idleTimeout          = 6;
  uartConfig.intEnable            = TRUE;
  uartConfig.callBackFunc         = MyUartCallBack;
  
  //打开串口
  HalUARTOpen (HAL_UART_PORT_0, &uartConfig);
}

/*********************************************************************************************
* 名称：MyUartCallBack()
* 功能：串口回调
* 参数：port - 端口
*       event - 事件，保留未用
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void MyUartCallBack ( uint8 port, uint8 event )
{
  
}
