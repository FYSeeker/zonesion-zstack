/*********************************************************************************************
* 文件：sensor.c
* 作者：Xuzhy 2018.5.16
* 说明：节点工程驱动模版
*       智慧城市系统三种类型设备：采集类（空气质量采集）、安防类（火灾报警）、控制类（防空警报器）
*       数据包格式：{airGas=101.0,fireStatus=0,beepStatus=0}
*       airGas表示空气质量值，fireStatus为火灾报警状态（0/1），beepStatus为防空警报器的状态
*       查询命令：查询数值参考（比如查询当前空气质量值：{airGas=?}）
*       控制命令：cmd=0为防空警报器关闭，cmd=1为防空警报器打开
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "stm32f10x.h"
#include "flame.h"
#include "zxbee.h"
#include "zxbee-inf.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8_t fireStatus = 0;                                   // 火灾报警状态值
/*********************************************************************************************
* 名称：updateFire()
* 功能：更新火灾报警状态值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateFire(void)
{
  static uint32_t ct = 0;
  
  fireStatus = get_flame_status();                       // 获取火灾报警状态
  if (fireStatus != 0) {
    ct = clock_time();
  } else if (clock_time() > ct+2000) {
    ct = 0;
    fireStatus = 0;
  } else {
    fireStatus = 1;
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
  // 初始化传感器代码
  flame_init();                                             // 火灾报警传感器初始化
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
  
  // 更新火灾报警状态
  updateFire();
  ZXBeeBegin();                                                 // 智云数据帧格式包头
  
  sprintf(p, "%u", fireStatus); 
  ZXBeeAdd("fireStatus", p); 
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据上传到智云平台
  }
  printf("sensor->sensorUpdate(): fireStatus=%u\r\n", fireStatus); 
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
  static char lastfireStatus = 0;
  static uint32_t ct0=0;
  char pData[16];
  char *p = pData;
  
  // 更新火灾报警状态
  updateFire();

  ZXBeeBegin();
  
  if (lastfireStatus != fireStatus || (ct0 != 0 && clock_time() > (ct0+3000))) {
    sprintf(p, "%u", fireStatus);
    ZXBeeAdd("fireStatus", p);
    ct0 = clock_time();
    if (fireStatus == 0) {
       ct0 = 0;
    }
    lastfireStatus = fireStatus;
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
  if (0 == strcmp("fireStatus", ptag)){                        // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      updateFire();
      ret = sprintf(p, "%u", fireStatus);
      ZXBeeAdd("fireStatus", p);
    } 
  }
  
  return ret;
}
/*********************************************************************************************
* 名称：sensor()
* 功能：传感器采集线程
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
PROCESS_THREAD(sensor, ev, data)
{
  static struct etimer et_update;
  static struct etimer et_check;
  
  PROCESS_BEGIN();
  
  ZXBeeInfInit();
  
  sensorInit();
  
  etimer_set(&et_update, CLOCK_SECOND*20); 
  etimer_set(&et_check, CLOCK_SECOND/10); //100 hz
  
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); 
    if (etimer_expired(&et_check)) {
      sensorCheck();
      etimer_set(&et_check, CLOCK_SECOND/10);
    }
    if (etimer_expired(&et_update)) {
      sensorUpdate();
      etimer_set(&et_update, CLOCK_SECOND*20); 
    } 
  }
  PROCESS_END();
}