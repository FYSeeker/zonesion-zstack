/*********************************************************************************************
* �ļ���sensor.c
* ���ߣ�Xuzhy 2018.5.16
* ˵�����ڵ㹤������ģ��
*       �ǻ۳���ϵͳ���������豸���ɼ��ࣨ���������ɼ����������ࣨ���ֱ������������ࣨ���վ�������
*       ���ݰ���ʽ��{airGas=101.0,fireStatus=0,beepStatus=0}
*       airGas��ʾ��������ֵ��fireStatusΪ���ֱ���״̬��0/1����beepStatusΪ���վ�������״̬
*       ��ѯ�����ѯ��ֵ�ο��������ѯ��ǰ��������ֵ��{airGas=?}��
*       �������cmd=0Ϊ���վ������رգ�cmd=1Ϊ���վ�������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "stm32f10x.h"
#include "flame.h"
#include "zxbee.h"
#include "zxbee-inf.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static uint8_t fireStatus = 0;                                   // ���ֱ���״ֵ̬
/*********************************************************************************************
* ���ƣ�updateFire()
* ���ܣ����»��ֱ���״ֵ̬
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateFire(void)
{
  static uint32_t ct = 0;
  
  fireStatus = get_flame_status();                       // ��ȡ���ֱ���״̬
  if (fireStatus != 0) {
    ct = clock_time();
  } else if (clock_time() > ct+2000) {
    ct = 0;
    fireStatus = 0;
  } else {
    fireStatus = 1;
  }
}
/*********************************************************************************************
* ���ƣ�sensorInit()
* ���ܣ�������Ӳ����ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorInit(void)
{
  // ��ʼ������������
  flame_init();                                             // ���ֱ�����������ʼ��
}
/*********************************************************************************************
* ���ƣ�sensorUpdate()
* ���ܣ����������ϱ�������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorUpdate(void)
{ 
  char pData[16];
  char *p = pData;
  
  // ���»��ֱ���״̬
  updateFire();
  ZXBeeBegin();                                                 // ��������֡��ʽ��ͷ
  
  sprintf(p, "%u", fireStatus); 
  ZXBeeAdd("fireStatus", p); 
  
  p = ZXBeeEnd();                                               // ��������֡��ʽ��β
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // ����Ҫ�ϴ��������ϴ�������ƽ̨
  }
  printf("sensor->sensorUpdate(): fireStatus=%u\r\n", fireStatus); 
}
/*********************************************************************************************
* ���ƣ�sensorCheck()
* ���ܣ����������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorCheck(void)
{
  static char lastfireStatus = 0;
  static uint32_t ct0=0;
  char pData[16];
  char *p = pData;
  
  // ���»��ֱ���״̬
  updateFire();

  ZXBeeBegin();
  
  if (lastfireStatus != fireStatus || (ct0 != 0 && clock_time() > (ct0+3000))) {
    sprintf(p, "%u", fireStatus);
    ZXBeeAdd("fireStatus", p);
    ct0 = clock_time();
    if (fireStatus == 0) {
       ct0 = 0;
    }
    lastfireStatus = fireStatus;
  }

  p = ZXBeeEnd();
  if (p != NULL) {
    int len = strlen(p); 
    ZXBeeInfSend(p, len);
  }
}
/*********************************************************************************************
* ���ƣ�ZXBeeUserProcess()
* ���ܣ������յ��Ŀ�������
* ������*ptag -- ������������
*       *pval -- �����������
* ���أ�ret -- pout�ַ�������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int ZXBeeUserProcess(char *ptag, char *pval)
{ 
  int ret = 0;	
  char pData[16];
  char *p = pData;

  // �����������
  if (0 == strcmp("fireStatus", ptag)){                        // ��ѯִ�����������
    if (0 == strcmp("?", pval)){
      updateFire();
      ret = sprintf(p, "%u", fireStatus);
      ZXBeeAdd("fireStatus", p);
    } 
  }
  
  return ret;
}
/*********************************************************************************************
* ���ƣ�sensor()
* ���ܣ��������ɼ��߳�
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
PROCESS_THREAD(sensor, ev, data)
{
  static struct etimer et_update;
  static struct etimer et_check;
  
  PROCESS_BEGIN();
  
  ZXBeeInfInit();
  
  sensorInit();
  
  etimer_set(&et_update, CLOCK_SECOND*20); 
  etimer_set(&et_check, CLOCK_SECOND/10); //100 hz
  
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); 
    if (etimer_expired(&et_check)) {
      sensorCheck();
      etimer_set(&et_check, CLOCK_SECOND/10);
    }
    if (etimer_expired(&et_update)) {
      sensorUpdate();
      etimer_set(&et_update, CLOCK_SECOND*20); 
    } 
  }
  PROCESS_END();
}