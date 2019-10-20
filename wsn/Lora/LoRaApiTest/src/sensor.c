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
#include "zxbee.h"
#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"
#include "math.h"
#include "sensor.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static float gas = 0.0;                              // 有害气体值
/*********************************************************************************************
* 名称：ZXBeeInfRecv()
* 功能：节点收到无线数据包
* 参数：*pkg -- 收到的无线数据包
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void ZXBeeInfRecv(char *buf, int len)
{
  uint8_t val;
  char pData[16];
  char *p = pData;
  char *ptag = NULL;
  char *pval = NULL;

  printf("sensor->ZXBeeInfRecv(): Receive LoRa Data!\r\n");
  
  ptag = buf;
  p = strchr(buf, '=');
  if (p != NULL) {
    *p++ = 0;
    pval = p;
  }
  val = atoi(pval);	
	
  // 控制命令解析
  if (0 == strcmp("cmd", ptag)){                                // 对D0的位进行操作，CD0表示位清零操作
    sensorControl(val);
  }
}
/*********************************************************************************************
* 名称：ZXBeeInfSend()
* 功能：节点发送数据包到智云平台
* 参数：*p -- 要发送的数据包
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void ZXBeeInfSend(char *p, int len)
{
  LoraNetSend(0, p, len);
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
  // 有害气体传感器初始化
  // 继电器初始化
  relay_init();                                                 
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
  char pData[32];
  char *p = pData;
  
  // 有害气体采集集（100~110随机数）
  gas = 100 + (uint16_t)(rand()%10);
  
  // 更新有害气体的值并上报
  sprintf(p, "gas=%.1f", gas);
  if (pData != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 上传数据到智云平台
  }
  
  printf("sensor->sensorUpdate(): gas=%.1f\r\n", gas);
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
  if(cmd == 0){                                                 
    printf("sensor->sensorControl(): Fan OFF\r\n");
	relay_control(cmd);
  }
  else if(cmd == 1){                                              
    printf("sensor->sensorControl(): Fan ON\r\n");
	relay_control(cmd);
  }
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
  
  LoraNetInit();
  LoraNetSetOnRecv(ZXBeeInfRecv);
  
  sensorInit();
  
  etimer_set(&et_update, CLOCK_SECOND*10); 
  
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); 
    if (etimer_expired(&et_update)) {
	  printf("sensor->PROCESS_EVENT_TIMER: PROCESS_EVENT_TIMER trigger!\r\n");
      sensorUpdate();
      etimer_set(&et_update, CLOCK_SECOND*10); 
    } 
  }
  PROCESS_END();
}