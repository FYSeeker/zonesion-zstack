/*********************************************************************************************
* 文件：sensor.c
* 作者：Xuzhy 2018.5.16
* 说明：xLab Sensor-D传感器程序
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "hal_types.h"
#include "ioCC2541.h"
#include <osal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_adc.h"
#include "hal_uart.h"
#include "sensor.h"
#include "lcd.h"
#include "zlg7290.h"
#include "iic2.h"
#include "oled.h"
#include "delay.h"
#include "zxbee.h"
#include "zxbee-inf.h"
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8  D1 = 1;                                           // 电视初始状态为开
static uint8  V1 = 0;                                           // 电视频道
static uint8  V2 = 0;                                           // 电视音量
static unsigned char key_val = 0;
/*********************************************************************************************
* 名称：P0_ISR
* 功能：P0口外部中断
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
#pragma vector = P0INT_VECTOR
__interrupt void P0_ISR(void)
{ 
    if(P0IFG&(1<<4))
    {                     
        key_val = zlg7290_get_keyval();
        P0IFG &= ~(1<<4); 
    }
    P0IF = 0;   //中断标志清0
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
  OLED_Init();
  oled_turn_on();
  char buff[64];
  sprintf(buff,"%d ",V1);
  OLED_ShowString(8,1,(unsigned char*)buff,16);
  sprintf(buff,"%2d",V2);
  OLED_ShowString(104,5,(unsigned char*)buff,16);OLED_ShowString(56,3,"TV",16);
  zlg7290_init();
  segment_display(V1*100+V2);
  key_val = zlg7290_read_reg(SYETEM_REG);

  // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
  osal_start_timerEx(simpleBLEPeripheral_TaskID, MY_REPORT_EVT, (osal_rand()%10) * 1000);
  // 启动定时器，触发传感器监测事件：MY_CHECK_EVT
  osal_start_timerEx(simpleBLEPeripheral_TaskID, MY_CHECK_EVT, 100);
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
  
  ZXBeeBegin();                                                 // 智云数据帧格式包头
  
  sprintf(p, "%d", V1);
  ZXBeeAdd("V1", p);
  
  sprintf(p, "%d", V2);
  ZXBeeAdd("V2", p);
  
  sprintf(p, "%u", D1);                                  // 上报控制编码 
  ZXBeeAdd("D1", p);
  
  p = ZXBeeEnd();                                               // 智云数据帧格式包尾
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
  }
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
  char buf[32];
  if (key_val != 0) {
    ZXBeeBegin();                                               //开始将传感器的状态拷贝到发送缓冲区
    sprintf(buf, "%d", key_val);
    ZXBeeAdd("A0", buf);
    if (D1 & 0x01) {
      if(key_val == RIGHT){
        V2++;
        if(V2 > 99)
          V2 = 0;
      }
      if(key_val == LEFT){
        if(V2 == 0)
          V2 = 99;
        else
          V2--;
      }
      if(key_val == UP){
        V1++;
        if(V1 > 19)
          V1 = 0;
      }
      if(key_val == DOWN){
        if(V1 == 0)
          V1 = 19;
        else
          V1--;
      }
    }
    if(key_val == CENTER){
      if(D1 & 0X01){
        D1 &= ~0X01;
      }
      else
      {
        D1 |= 0X01;
      }
      sprintf(buf, "%u", D1);
      ZXBeeAdd("D1", buf);
    }
    if(D1 & 0X01){
      sprintf(buf, "%d", V1);
      ZXBeeAdd("V1", buf);
      sprintf(buf, "%d", V2);
      ZXBeeAdd("V2", buf);
    }
    char *p = ZXBeeEnd();                                       // 智云数据帧格式包尾
    if (p != NULL) {												
      ZXBeeInfSend(p, strlen(p));	                         // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
    }
    key_val = 0;
  }
  static uint8 lastV1 = 0, lastV2 = 0, lastDis = 1;
  if (lastDis != (D1&0x01)) {
    lastDis = D1 & 0x01;
    if (lastDis == 0) {
       OLED_Display_Off();
       display_off();
    } else {
   
      OLED_Display_On();
      
      char buff[64];
      sprintf(buff,"%d ",V1);
      OLED_ShowString(8,1,(unsigned char*)buff,16);
      sprintf(buff,"%2d",V2);
      OLED_ShowString(104,5,(unsigned char*)buff,16);OLED_ShowString(56,3,"TV",16);
      segment_display(V1*100+V2);
    }
  }
  if (V1 != lastV1 ||  V2 != lastV2) {
    char buff[64];  
    segment_display(V1*100+V2);
      
    sprintf(buff,"%d ",V1);
    OLED_ShowString(8,1,(unsigned char*)buff,16);
    sprintf(buff,"%2d",V2);
    OLED_ShowString(104,5,(unsigned char*)buff,16);OLED_ShowString(56,3,"TV",16);
    lastV1 = V1;
    lastV2 = V2;
  }   
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
	//由于LED刷屏会占用大量cpu直接，此处刷屏会导致后面读取命令超时，
	//因此D1控制统一放到sensorCheck中处理
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
  int val;
  int ret = 0;	
  char pData[16];
  char *p = pData;
  
  // 将字符串变量pval解析转换为整型变量赋值
  val = atoi(pval);	
  // 控制命令解析
  if (0 == strcmp("CD1", ptag)){                                // 对D1的位进行操作，CD1表示位清零操作
    D1 &= ~val;
    sensorControl(D1);                                          // 处理执行命令
  }
  if (0 == strcmp("OD1", ptag)){                                // 对D1的位进行操作，OD1表示位置一操作
    D1 |= val;
    sensorControl(D1);                                          // 处理执行命令
  }
  if (0 == strcmp("D1", ptag)){                                 // 查询执行器命令编码
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", D1);
      ZXBeeAdd("D1", p);
    } 
  }
  
  if (0 == strcmp("V1", ptag)){ 
    if (0 == strcmp("?", pval)){   
      sprintf(p, "%d", V1);
      ZXBeeAdd("V1", p);
    }
    else {   
    if (val >= 0 && val <= 19) {
      V1 = val;
      }
    } 
  }  
  if (0 == strcmp("V2", ptag)){ 
    if (0 == strcmp("?", pval)){   
      sprintf(p, "%d", V2); 
      ZXBeeAdd("V2", p);
    } 
    else {   
      if (val >= 0 && val <= 99) {
        V2 = val;
      }
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
    sensorUpdate();
    //启动定时器，触发事件：MY_REPORT_EVT 
    osal_start_timerEx(simpleBLEPeripheral_TaskID, MY_REPORT_EVT, 30*1000);
  }  
  if (event & MY_CHECK_EVT) { 
    sensorCheck(); 
    // 启动定时器，触发事件：MY_CHECK_EVT 
    osal_start_timerEx(simpleBLEPeripheral_TaskID, MY_CHECK_EVT, 100);
  } 
}