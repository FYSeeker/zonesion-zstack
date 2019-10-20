/*********************************************************************************************
* 文件：sensor.c
* 作者：Xuzhy 2018.5.16
* 说明：节点工程驱动模版
*       智慧交通系统三种类型设备：采集类（大气压力采集）、安防类（山体滑坡振动报警）、控制类（RGB交通红绿灯）
*       数据包格式：{airPressure=101.0,shockStatus=0,rgbStatus=0}
*       airPressure表示大气压力值，shockStatus为振动状态（0/1），rgbStatus为rgb灯的状态（R/G/Y）
*       查询命令：查询数值参考（比如查询当前大气压力值：{airPressure=?}）
*       控制命令：cmd=0为rgb灯灭，cmd=1为红灯，cmd=2为绿灯，cmd=3为黄灯
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

#include "vibration.h"
#include "zxbee.h"
#include "zxbee-inf.h"
#include "sensor.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8_t shockStatus = 0;                                   // 震动状态值
/*********************************************************************************************
* 名称：updateShock()
* 功能：更新震动状态值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateShock(void)
{
  static uint32_t ct = 0;
  
  shockStatus = get_vibration_status();                       // 获取震动状态
  if (shockStatus != 0) {
    ct = clock_time();
  } else if (clock_time() > ct+2000) {
    ct = 0;
    shockStatus = 0;
  } else {
    shockStatus = 1;
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
  vibration_init();                                             // 振动传感器初始化
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
  
  // 更新震动状态
  updateShock();
  ZXBeeBegin();                                                 // 智云数据帧格式包头
  
  sprintf(p, "%u", shockStatus); 
  ZXBeeAdd("shockStatus", p); 
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据上传到智云平台
  }
  printf("sensor->sensorUpdate(): shockStatus=%u\r\n", shockStatus); 
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
  static char lastShockStatus = 0;
  static uint32_t ct0=0;
  char pData[16];
  char *p = pData;
  
  // 更新震动状态
  updateShock();

  ZXBeeBegin();
  
  if (lastShockStatus != shockStatus || (ct0 != 0 && clock_time() > (ct0+3000))) {
    sprintf(p, "%u", shockStatus);
    ZXBeeAdd("shockStatus", p);
    ct0 = clock_time();
    if (shockStatus == 0) {
       ct0 = 0;
    }
    lastShockStatus = shockStatus;
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
  if (0 == strcmp("shockStatus", ptag)){                        // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      updateShock();
      ret = sprintf(p, "%u", shockStatus);
      ZXBeeAdd("shockStatus", p);
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