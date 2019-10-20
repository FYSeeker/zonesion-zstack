/*********************************************************************************************
* 文件：sensor.c
* 作者：Xuzhy 2018.5.16
* 说明：节点工程驱动模版
*       智慧农业系统三种类型设备：采集类（光照度采集）、安防类（光照强度阀值报警状态）、控制类（遮阳电机）
*       数据包格式：{lightIntensity=124.0,lightIntensityStatus=0,mortorStatus=0}
*       lightIntensity表示光强值，lightIntensityStatus为光强报警阀值（0/1），motorStatus为遮阳电机的状态（ON/OFF）
*       查询命令：查询数值参考（比如查询当前光强度：{lightIntensity=?}）
*       控制命令：cmd=0为关闭遮阳电机，cmd=1为打开电机
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
#include "BH1750.h"
#include "iic.h"
#include "zxbee.h"
#include "zxbee-inf.h"
/*********************************************************************************************
* 宏定义
*********************************************************************************************/

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static float lightIntensity = 0.0;                              // 光照强度值
static uint8 lightStatus = 0;                                   // 光照阀值报警状态值
/*********************************************************************************************
* 名称：updatelightIntensity()
* 功能：更新光强值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateLightIntensity(void)
{  
  // 读取光照强度值
  lightIntensity = bh1750_get_data();
  
  // 监测光强阀值报警状态值
  if(lightIntensity <= 800)
    lightStatus = 0;
  else
    lightStatus = 1;
  // printf("sensor->updateLightIntensity(): lightIntensity=%.1f,lightStatus=%u\r\n", lightIntensity,lightStatus); 
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
  bh1750_init();                                                // 光强传感器初始化

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
  
  // 光强度采集
  updateLightIntensity();
  
  ZXBeeBegin();                                                 // 智云数据帧格式包头

  // 更新光强值
  sprintf(p, "%.1f", lightIntensity); 
  ZXBeeAdd("lightIntensity", p);
  
  // 更新lightStatus的值
  sprintf(p, "%u", lightStatus); 
  ZXBeeAdd("lightStatus", p); 
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
  }
  printf("sensor->sensorUpdate(): lightIntensity=%.1f,lightStatus=%u\r\n", lightIntensity,lightStatus); 
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
  static char lastlightStatus=0;
  static uint32 ct0=0;
  char pData[16];
  char *p = pData;
  
  // 光强阀值报警状态采集
  updateLightIntensity();

  ZXBeeBegin();

  if (lastlightStatus != lightStatus || (ct0 != 0 && osal_GetSystemClock() > (ct0+3000))) {  // 光强阀值报警状态监测
    sprintf(p, "%u", lightStatus);
    ZXBeeAdd("lightStatus", p);
	ct0 = osal_GetSystemClock();
    if (lightStatus == 0) {
      ct0 = 0;
    }
    lastlightStatus = lightStatus;
  }

  p = ZXBeeEnd();
  if (p != NULL) {
    int len = strlen(p); 
    ZXBeeInfSend(p, len);
  }
}
/*********************************************************************************************
* 名称：ZXBeeUserProcess()
* 功能：解析收到的控制命令
* 参数：*ptag -- 控制命令名称
*       *pval -- 控制命令参数
*       *pout -- 控制响应数据，将数据返回给上级调用，通过zb_SendDataRequest{}发送给协调器
* 返回：ret -- pout字符串长度
* 修改：
* 注释：
*********************************************************************************************/
int ZXBeeUserProcess(char *ptag, char *pval)
{ 
  int ret = 0;	
  char pData[16];
  char *p = pData;

  // 控制命令解析
  if (0 == strcmp("lightIntensity", ptag)){                     // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      updateLightIntensity();
      ret = sprintf(p, "%.1f", lightIntensity);
      ZXBeeAdd("lightIntensity", p);
    } 
  }
  if (0 == strcmp("lightStatus", ptag)){                        // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
	  updateLightIntensity();
      ret = sprintf(p, "%u", lightStatus);
      ZXBeeAdd("lightStatus", p);
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
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, 20*1000);
  }  
  if (event & MY_CHECK_EVT) { 
    sensorCheck();                                              // 传感器状态实时监测
    // 启动定时器，触发事件：MY_CHECK_EVT 
    osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
  } 
}