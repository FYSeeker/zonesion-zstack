/*********************************************************************************************
* 文件：sensor.c
* 作者：Xuzhy 2018.5.16
* 说明：节点工程驱动模版
*       创意产品三种类型设备：采集类（湿度采集）、安防类（门磁报警状态）、控制类（LED）
*       数据包格式：{doorStatus=124.0,doorStatus=0,ledStatus=0}
*       doorStatus表示湿度值，doorStatus为门磁状态（0/1），ledStatus为LED的状态（ON/OFF）
*       查询命令：查询数值参考（比如查询当前湿度：{doorStatus=?}）
*       控制命令：cmd=0为关闭LED，cmd=1为打开LED
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "hal_types.h"
#include "ioCC2541.h"
#include <osal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_uart.h"
#include "led.h"
#include "zxbee.h"
#include "sensor.h"
#include "zxbee-inf.h"
/*********************************************************************************************
* 宏定义
*********************************************************************************************/

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8 ledStatus = 0;                                   // LED状态值
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
  // 初始化LED代码
  led_init();                                                   // LED灯初始化
  
  // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
  osal_start_timerEx(simpleBLEPeripheral_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
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

  // 更新LED状态值
  sprintf(p, "%u", ledStatus); 
  ZXBeeAdd("ledStatus", p); 
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
  }
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
  if(cmd & 0x02){                                               // LED2灯控制位：bit1
    LED2 = ON;                                                  // 开启LED2
  }
  else{
    LED2 = OFF;                                                 // 关闭LED2        
  }
  if(cmd & 0x01){                                               // LED1灯控制位：bit0
    LED1 = ON;                                                  // 开启LED1
  }
  else{
    LED1 = OFF;                                                 // 关闭LED1
  }
  ledStatus = cmd;
}
/*********************************************************************************************
* 名称：ZXBeeUserProcess()
* 功能：解析收到的控制命令
* 参数：*ptag -- 控制命令名称
*       *pval -- 控制命令参数
* 返回：ret -- p字符串长度
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
  if (0 == strcmp("cmd", ptag)){                                // LED的控制命令
    sensorControl(val);
  }
  if (0 == strcmp("ledStatus", ptag)){                        // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", ledStatus);
      ZXBeeAdd("ledStatus", p);
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
    sensorUpdate();                                             // 传感器数据定时上报
    //启动定时器，触发事件：MY_REPORT_EVT 
    osal_start_timerEx(simpleBLEPeripheral_TaskID, MY_REPORT_EVT, 20*1000);
  } 
}