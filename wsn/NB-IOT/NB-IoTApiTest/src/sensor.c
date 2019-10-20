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
#include "zxbee.h"
#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"
#include "math.h"
#include "sensor.h"
#include "config.h"

int zhiyun_send_coap(char *pkg);
int zhiyun_send_udp(char *pkg);

#define COAP    0
#define UDP     1

PROCESS(sensor, "sensor");
PROCESS_NAME(u_zhiyun_coap);
PROCESS_NAME(u_zhiyun_udp);
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static float airGas = 0.0;                              // 空气质量值

/*********************************************************************************************
* 名称：ZXBeeInfInit
* 功能：ZXBee接口底层初始化
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void ZXBeeInfInit(void)
{
  config_init();

  if (nbConfig.mode == 0) {
    process_start(&u_zhiyun_coap, NULL);
  } else {
     process_start(&u_zhiyun_udp, NULL);
  }
}

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

  printf("sensor->ZXBeeInfRecv(): Receive NB-IoT Data!\r\n");
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
  if (nbConfig.mode == COAP) {
    zhiyun_send_coap(p);
  } else {
    zhiyun_send_udp(p);
  }
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
  // 空气质量传感器初始化
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
  
  // 空气质量采集集（100~110随机数）
  airGas = 100 + (uint16_t)(rand()%10);
  
  // 更新空气质量的值并上报
  sprintf(p, "airGas=%.1f", airGas);
  if (pData != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 上传数据到智云平台
  }
  
  printf("sensor->sensorUpdate(): airGas=%.1f\r\n", airGas);
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
    printf("sensor->sensorControl(): Beep OFF\r\n");
	relay_control(cmd);
  }
  else if(cmd == 1){                                              
    printf("sensor->sensorControl(): Beep ON\r\n");
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
  
  ZXBeeInfInit();
  
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