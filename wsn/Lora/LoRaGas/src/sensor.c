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
#include "delay.h"
#include "MP-503.h"
#include "math.h"
#include "zxbee.h"
#include "zxbee-inf.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static float gas = 0.0;                              // 有害气体值
/*********************************************************************************************
* 名称：updateGas()
* 功能：更新有害气体值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateGas(void)
{
  // 读取有害气体值
  gas = get_airgas_data(); 
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
  airgas_init();                                                // 空气质量传感器初始化
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
  
  // 有害气体采集
  updateGas();
  
  ZXBeeBegin();                                                 // 智云数据帧格式包头

  // 上报有害气体值
  sprintf((char*)p, "%.1f", gas); 
  ZXBeeAdd("gas", p);

  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据发送到智云平台
  }
  printf("sensor->sensorUpdate(): gas=%.1f\r\n", gas);
}
/*********************************************************************************************
* 名称：ZXBeeUserProcess()
* 功能：解析收到的控制命令
* 参数：*ptag -- 控制命令名称
*       *pval -- 控制命令参数
* 返回：<0:不支持指令，>=0 指令已处理
* 修改：
* 注释：
*********************************************************************************************/
int ZXBeeUserProcess(char *ptag, char *pval)
{ 
  int ret = 0;	
  char pout[16];
  
  printf("sensor->ZXBeeUserProcess(): Receive LoRa Data!\r\n");
  // 控制命令解析
  if (0 == strcmp("gas", ptag)){                                 // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      updateGas();
      ret = sprintf(pout, "%.1f", gas);
      ZXBeeAdd("gas", pout);
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
  
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); 
    if (etimer_expired(&et_update)) {
      printf("sensor->PROCESS_EVENT_TIMER: PROCESS_EVENT_TIMER trigger!\r\n");
      sensorUpdate();
      etimer_set(&et_update, CLOCK_SECOND*20); 
    } 
  }
  PROCESS_END();
}