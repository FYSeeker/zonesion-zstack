/*********************************************************************************************
* 文件：sensor.c
* 作者：chennian 2018.6.28
* 说明：通用传感器控制接口程序
*       商用工控IC卡读卡器电磁锁驱动
*       传感器控制参数设置：A0表示RFID采集到的ID卡号
*                           V0表示主动上报时间间隔，默认设置V0=30
*                           D0(Bit0/Bit1)表示RFID采集信息、电磁锁主动上报使能，默认设置D0=3
*                           D1(Bit0)表示电磁锁开关状态，默认设置D1=0
*       Lite节点传感器接线：RFID读卡器接端子A，电磁锁接端子B
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
#include "zxbee.h"
#include "zxbee-inf.h"
#include "OnBoard.h"
#include "RFIDdriver.h"
/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define RELAY1                  P0_6                            // 定义继电器控制引脚
#define RELAY2                  P0_7                            // 定义继电器控制引脚
#define ON                      0                               //宏定义打开状态控制为ON
#define OFF                     1                               //宏定义关闭状态控制为OFF
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint16 V0 = 5;                                          // V0设置为上报时间间隔，默认为30s
static uint8 D0 = 3;                                            // 默认打开主动上报功能
uint8 D1 = 0;                                                   // 电磁锁电源初始状态为关
uint8 A0[16];                                                   // A0存储卡号
uint16 f_usMyReadCardDelay = 1;                                 // 设置1s读卡间隔 
uint16 f_usMyCloseDoorDelay = 3;                                // 开门到关门延时时间
uint8 f_szReadCardCmd[] = {0x09,0x41,0x31,0x46,0x33,0x46,0x0D}; // 读卡指令              

/*********************************************************************************************
* 名称：updateV0()
* 功能：更新V0的值
* 参数：*val -- 待更新的变量
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateV0(char *val)
{
  //将字符串变量val解析转换为整型变量赋值
  V0 = atoi(val);                                               // 获取数据上报时间更改值
}
/*********************************************************************************************
* 名称：updateA0()
* 功能：更新A0的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA0(void)
{

}
/*********************************************************************************************
* 名称：updateA1()
* 功能：更新A1的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA1(void)
{

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
  node_uart_init();                                             // RFID功能初始化
  // 门锁初始化
  ELECLOCK_SEL &= ~ELECLOCK_BV;
  ELECLOCK_DIR |= ELECLOCK_BV;
  
  sensorControl(D1);
  
  // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
  osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
  // 启动定时器，触发传感器监测事件：MY_CHECK_EVT
  osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
  osal_start_timerEx( sapi_TaskID, MY_READCARD_EVT, (uint16)(f_usMyReadCardDelay * 1000));
  
  
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
  
  // 根据D0的位状态判定需要主动上报的数值
  if ((D0 & 0x01) == 0x01){                                     // 若卡号上报允许，则pData的数据包中添加卡号数据
    updateA0();
    sprintf(p, "%s", &A0[0]); 
    ZXBeeAdd("A0", p);
  }
  
  sprintf(p, "%u", D1);                                         // 上报控制编码 
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
  if(cmd == 0x00){                                              //若检测的指令(D1)为0
    ELECLOCK_SBIT = ACTIVE_LOW (0);                             //关闭电磁锁
  }
  if(cmd == 0x01){                                              //若检测的指令(D1)为1
    ELECLOCK_SBIT = ACTIVE_LOW (1);                             //打开电磁锁
    //启动定时器，触发事件：MY_CLOSEDOOR_EVT
    osal_start_timerEx( sapi_TaskID, MY_CLOSEDOOR_EVT, (uint16)(f_usMyCloseDoorDelay * 1000));
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
  int val;
  int ret = 0;	
  char pData[16];
  char *p = pData;
  
  // 将字符串变量pval解析转换为整型变量赋值
  val = atoi(pval);	
  // 控制命令解析
  if (0 == strcmp("CD0", ptag)){                                // 对D0的位进行操作，CD0表示位清零操作
    D0 &= ~val;
  }
  if (0 == strcmp("OD0", ptag)){                                // 对D0的位进行操作，OD0表示位置一操作
    D0 |= val;
  }
  if (0 == strcmp("D0", ptag)){                                 // 查询上报使能编码
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", D0);
      ZXBeeAdd("D0", p);
    } 
  }
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
  if (0 == strcmp("A0", ptag)){ 
    if (0 == strcmp("?", pval)){
      updateA0();
      ret = sprintf(p, "%s", A0);     
      ZXBeeAdd("A0", p);
    } 
  }
  if (0 == strcmp("V0", ptag)){
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", V0);                         	// 上报时间间隔
      ZXBeeAdd("V0", p);
    }else{
      updateV0(pval);
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
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, V0*1000);
  }  
  if (event & MY_CHECK_EVT) { 
    sensorCheck(); 
    // 启动定时器，触发事件：MY_CHECK_EVT 
    osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
  }
  
  if(event & MY_READCARD_EVT){                                  //当协议栈时间轮询到MY_READCARD_EVT事件
    if((D0 & 0x01) == 1){                                       //若允许上报，则进行读卡操作
      sprintf(A0, "%s", "0");                                     //重置A0的值
      uart_485_write(f_szReadCardCmd, sizeof(f_szReadCardCmd)); //发送RFID读取指令
    }
    //启动定时器，触发事件：MY_READCARD_EVT 
    osal_start_timerEx( sapi_TaskID, MY_READCARD_EVT, (uint16)(f_usMyReadCardDelay * 1000));
  }
  if(event & MY_CLOSEDOOR_EVT){                                 //当协议栈时间轮询到MY_CLOSEDOOR_EVT事件
    int len = 0;
    uint16 cmd = 0;
    uint8 pData[128];
    
    D1 &= ~(0x01);                                              //3s后重置D1的值关闭电磁锁
    sensorControl(D1);                                         //刷新电磁锁状态  
    len = sprintf(pData,"{D1=%u}", D1);                         //将D1的值以D1=%u的字符串赋予pData
    //通过zb_SendDataRequest()发送pData到协调器
    zb_SendDataRequest( 0, cmd, len, pData, 0, AF_ACK_REQUEST, AF_DEFAULT_RADIUS );
  }
}