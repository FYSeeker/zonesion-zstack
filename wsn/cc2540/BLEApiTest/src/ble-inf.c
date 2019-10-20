/*********************************************************************************************
* �ļ���ble-inf.c
* ���ߣ�Xuzhy 2018.5.26
* ˵���������շ��ӿ�
* �޸ģ�
* ע�ͣ�
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
* ���ƣ�bleLinkSet()
* ���ܣ�����ble����״̬
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void bleLinkSet(int s)
{
  bleLinkStatus = s;
}
/*********************************************************************************************
* ���ƣ� GetLinkStatus()
* ���ܣ���ȡble����״̬
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int GetLinkStatus(void)
{
  return bleLinkStatus;
}

/*********************************************************************************************
* ���ƣ�bleRecv()
* ���ܣ�ble���յ����ݵ���
* ������buf�����յ�������
*       len�����յ������ݳ���
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void bleRecv(char *buf, int len)
{
  HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
  HalLedSet(HAL_LED_1, HAL_LED_MODE_BLINK);
  at_notify_data(buf, len);
  ZXBeeInfRecv(buf, len);
}
/*********************************************************************************************
* ���ƣ�bleSend()
* ���ܣ�������Ҫ���͵�����
* ������buf����Ҫ���͵�����
*       len�����ݳ���
* ���أ�
* �޸ģ�
* ע�ͣ�
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
* ���ƣ�bleOnRead
* ���ܣ���Ҫ���͵����ݸ��Ƶ����ͻ��棬���з���
* ������buf���������ݻ���
*       len�����泤��
* ���أ�
* �޸ģ�
* ע�ͣ�
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
* ���ƣ�bleInfPoll
* ���ܣ�����Ƿ���������Ҫ����
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
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
