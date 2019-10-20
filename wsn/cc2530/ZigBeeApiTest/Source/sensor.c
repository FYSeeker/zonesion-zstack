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
/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define RELAY                   P0_6                            // 定义继电器控制引脚
#define ON                      0                               // 宏定义打开状态控制为ON
#define OFF                     1                               // 宏定义关闭状态控制为OFF
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static float lightIntensity = 0.0;                              // 光照强度值
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
  // 光强传感器初始化
  // 初始化遮阳电机代码
  // 初始化继电器代码
  P0SEL &= ~0xC0;                                               // 配置管脚为通用IO模式
  P0DIR |= 0xC0;                                                // 配置控制管脚为输入模式

  // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
  osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
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
  printf("sensor->sensorLinkOn(): Sensor Link on!\r\n"); 
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
  
  // 光强度采集集（0~1000随机数）
  lightIntensity = (uint16)(osal_rand()%1000);
  
  // 更新gas的值
  sprintf(p, "lightIntensity=%.1f", lightIntensity);
  zb_SendDataRequest( 0, 0, strlen(p), (uint8*)p, 0, 0, AF_DEFAULT_RADIUS );
  HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
  HalLedSet( HAL_LED_1, HAL_LED_MODE_BLINK );
  
  printf("sensor->sensorUpdate(): lightIntensity=%.1f\r\n", lightIntensity);
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
  if(cmd == 1){ 
    RELAY = ON;                                                 // 开启继电器1，模拟遮阳电机开
    printf("sensor->sensorControl(): Motor ON\r\n");
  }
  else if(cmd == 0){
    RELAY = OFF;                                                // 关闭继电器1，模拟遮阳电机关
    printf("sensor->sensorControl(): Motor OFF\r\n");
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
void ZXBeeInfRecv(char *pkg, int len)
{
  uint8 val;
  char pData[16];
  char *p = pData;
  char *ptag = NULL;
  char *pval = NULL;

  printf("sensor->ZXBeeInfRecv(): Receive ZigBee Data!\r\n");
  
  ptag = pkg;
  p = strchr(pkg, '=');
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
    printf("sensor->MyEventProcess(): MY_REPORT_EVT trigger!\r\n");
    sensorUpdate();                                             // 传感器数据定时上报
    //启动定时器，触发事件：MY_REPORT_EVT 
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, 20*1000);
  }
}