/*********************************************************************************************
* 文件：sensor.c
* 作者：Xuzhy 2018.5.16
* 说明：节点工程驱动模版
*       智慧畜牧系统三种类型设备：采集类（有害气体采集）、安防类（红外光栅报警）、控制类（排风扇）
*       数据包格式：{gas=101.0,gratingStatus=0,fanStatus=0}
*       gas表示有害气体值，gratingStatus为红外光栅报警状态（0/1），fanStatus为排风扇的状态
*       查询命令：查询数值参考（比如查询当前有害气体值：{gas=?}）
*       控制命令：cmd=0为排风扇关闭，cmd=1为排风扇打开
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
#include "grating.h"
#include "zxbee.h"
#include "zxbee-inf.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8_t gratingStatus = 0;                                   // 红外光栅状态值
/*********************************************************************************************
* 名称：updateGrating()
* 功能：更新红外光栅状态值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateGrating(void)
{
  gratingStatus = get_grating_status();                         // 更新光栅传感器的值
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
  printf("sensor->sensorInit(): Sensor init!\r\n");
  // 初始化传感器代码
  grating_init();                                               // 光栅传感器初始化
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
  
  // 更新红外光栅状态
  updateGrating();
  ZXBeeBegin();                                                 // 智云数据帧格式包头
  
  sprintf((char*)p, "%u", gratingStatus); 
  ZXBeeAdd("gratingStatus", p); 
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据上传到智云平台
  }
  printf("sensor->sensorUpdate(): gratingStatus=%u\r\n", gratingStatus); 
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
  static char lastgratingStatus = 0;
  static uint32_t ct0=0;
  char pData[16];
  char *p = pData;
  
  // 更新红外光栅状态
  updateGrating();

  ZXBeeBegin();
  
  if (lastgratingStatus != gratingStatus || (ct0 != 0 && clock_time() > (ct0+3000))) {
    sprintf(p, "%u", gratingStatus);
    ZXBeeAdd("gratingStatus", p);
    ct0 = clock_time();
    if (gratingStatus == 0) {
       ct0 = 0;
    }
    lastgratingStatus = gratingStatus;
  }

  p = ZXBeeEnd();
  if (p != NULL) {
    int len = strlen(p); 
    ZXBeeInfSend(p, len);
	printf("sensor->sensorCheck: Grating alarm!\r\n");
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

  printf("sensor->ZXBeeUserProcess(): Receive LoRa Data!\r\n");
  // 控制命令解析
  if (0 == strcmp("gratingStatus", ptag)){                        // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      updateGrating();
      ret = sprintf(p, "%u", gratingStatus);
      ZXBeeAdd("gratingStatus", p);
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