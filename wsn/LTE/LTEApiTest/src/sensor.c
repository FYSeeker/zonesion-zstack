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
static float airPressure = 0.0;                              // 大气压力值
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

  printf("sensor->ZXBeeInfRecv(): Receive LTE Data!\r\n");
  
  leds_on(1);
  clock_delay_ms(50);
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

  leds_off(1);
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
  leds_on(1);
  clock_delay_ms(50);
  zhiyun_send(p);
  leds_off(1);
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
  // 大气压力传感器初始化
  // 继电器初始化
  relay_init();                                                 
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
  printf("sensor->sensorLinkOn(): Sensor LinkOn!\r\n"); 
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
  char pData[32];
  char *p = pData;
  
  // 大气压力采集集（100~110随机数）
  airPressure = 100 + (uint16_t)(rand()%10);
  
  // 更新大气压力的值并上报
  sprintf(p, "airPressure=%.1f", airPressure);
  if (pData != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 上传数据到智云平台
  }
  
  printf("sensor->sensorUpdate(): airPressure=%.1f\r\n", airPressure);
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
    printf("sensor->sensorControl(): Light OFF\r\n");
	relay_control(cmd);
  }
  else if(cmd == 1){                                              
    printf("sensor->sensorControl(): Light ON\r\n");
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
  
  // ZXBeeInfInit();
  
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