/*********************************************************************************************
* 文件：ble-inf.c
* 作者：Xuzhy 2018.5.26
* 说明：蓝牙收发接口
* 修改：
* 注释：
*********************************************************************************************/
#include <osal.h>
#include "hal_led.h"
#include "ble-inf.h"
#include "zxbee-inf.h"


#define min(a, b) ((a)<(b)?(a):(b))



static char pbleNotifyData[96];
static int sendLength = 0;
static int dataLength = 0;

void at_notify_data(char *buf, int len);
void simpleNotify(void);

static int bleLinkStatus;
/*********************************************************************************************
* 名称：bleLinkSet()
* 功能：设置ble连接状态
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void bleLinkSet(int s)
{
  bleLinkStatus = s;
}
/*********************************************************************************************
* 名称： GetLinkStatus()
* 功能：获取ble连接状态
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
int GetLinkStatus(void)
{
  return bleLinkStatus;
}

/*********************************************************************************************
* 名称：bleRecv()
* 功能：ble接收到数据调用
* 参数：buf：接收到的数据
*       len：接收到的数据长度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void bleRecv(char *buf, int len)
{
  HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
  HalLedSet(HAL_LED_1, HAL_LED_MODE_BLINK);
  at_notify_data(buf, len);
  ZXBeeInfRecv(buf, len);
}
/*********************************************************************************************
* 名称：bleSend()
* 功能：设置需要发送的数据
* 参数：buf：需要发送的数据
*       len：数据长度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void bleSend(char *buf, int len)
{
  if (dataLength == 0) {
    if (len <= sizeof pbleNotifyData) {
      osal_memcpy(pbleNotifyData, buf, len);
      dataLength = len;
      sendLength = 0;
    }
  }
}
/*********************************************************************************************
* 名称：bleOnRead
* 功能：将要发送的数据复制到发送缓存，进行发送
* 参数：buf：发送数据缓存
*       len：缓存长度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
int bleOnRead(char *buf, int len)
{
  int s ;
  if (sendLength < dataLength) {
    HalLedSet(HAL_LED_1, HAL_LED_MODE_BLINK);
    int left = dataLength - sendLength;
    s = min(len, left);
    osal_memcpy(buf, pbleNotifyData+sendLength, s);
    sendLength += s;
    return s;
  } else {
    return 0;
  }
}
/*********************************************************************************************
* 名称：bleInfPoll
* 功能：检查是否有数据需要发送
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void bleInfPoll(void)
{
  if (sendLength < dataLength) {
    simpleNotify();
  } else {
    sendLength = 0;
    dataLength = 0;
  }
}
