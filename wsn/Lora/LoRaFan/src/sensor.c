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
#include "FAN.h"
#include "zxbee.h"
#include "zxbee-inf.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8_t fanStatus = 0;                                     // 排风扇状态值
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
  FAN_init();
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
  
  ZXBeeBegin();
  
  sprintf(p, "%u", fanStatus);                                  // 上报控制编码 
  ZXBeeAdd("fanStatus", p);
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据进行打包操作，并发送到LoRa AP
  }
  printf("sensor->sensorUpdate(): fanStatus=%u\r\n", fanStatus); 
}
/*********************************************************************************************
* 名称：sensorControl()
* 功能：传感器控制
* 参数：cmd - 控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorControl(uint8_t cmd)
{
  // 根据cmd参数处理对应的控制程序
  if(cmd & 0x01){                                               // 根据cmd参数处理对应的控制程序
    FAN_on(0x01);                                               // 开启风扇
	printf("sensor->sensorControl(): FAN ON\r\n");
  }
  else{
    FAN_off(0x01);                                              // 关闭风扇     
    printf("sensor->sensorControl(): FAN OFF\r\n");	
  }
  fanStatus = cmd;
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
 
  printf("sensor->ZXBeeUserProcess(): Receive LoRa Data!\r\n"); 
  // 将字符串变量pval解析转换为整型变量赋值
  val = atoi(pval);	
  // 控制命令解析
  if (0 == strcmp("cmd", ptag)){                                // 排风扇的控制命令
    sensorControl(val);
  }
  if (0 == strcmp("fanStatus", ptag)){                        // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", fanStatus);
      ZXBeeAdd("fanStatus", p);
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
  
  PROCESS_BEGIN();
  
  ZXBeeInfInit();
  
  sensorInit();
  
  etimer_set(&et_update, CLOCK_SECOND*20); 
  
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); 
    if (etimer_expired(&et_update)) {
      sensorUpdate();
      etimer_set(&et_update, CLOCK_SECOND*20); 
    } 
  }
  PROCESS_END();
}