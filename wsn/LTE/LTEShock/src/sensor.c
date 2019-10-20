/*********************************************************************************************
* �ļ���sensor.c
* ���ߣ�Xuzhy 2018.5.16
* ˵�����ڵ㹤������ģ��
*       �ǻ۽�ͨϵͳ���������豸���ɼ��ࣨ����ѹ���ɼ����������ࣨɽ�廬���񶯱������������ࣨRGB��ͨ���̵ƣ�
*       ���ݰ���ʽ��{airPressure=101.0,shockStatus=0,rgbStatus=0}
*       airPressure��ʾ����ѹ��ֵ��shockStatusΪ��״̬��0/1����rgbStatusΪrgb�Ƶ�״̬��R/G/Y��
*       ��ѯ�����ѯ��ֵ�ο��������ѯ��ǰ����ѹ��ֵ��{airPressure=?}��
*       �������cmd=0Ϊrgb����cmd=1Ϊ��ƣ�cmd=2Ϊ�̵ƣ�cmd=3Ϊ�Ƶ�
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

#include "vibration.h"
#include "zxbee.h"
#include "zxbee-inf.h"
#include "sensor.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static uint8_t shockStatus = 0;                                   // ��״ֵ̬
/*********************************************************************************************
* ���ƣ�updateShock()
* ���ܣ�������״ֵ̬
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateShock(void)
{
  static uint32_t ct = 0;
  
  shockStatus = get_vibration_status();                       // ��ȡ��״̬
  if (shockStatus != 0) {
    ct = clock_time();
  } else if (clock_time() > ct+2000) {
    ct = 0;
    shockStatus = 0;
  } else {
    shockStatus = 1;
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
  vibration_init();                                             // �񶯴�������ʼ��
}
/*********************************************************************************************
* ���ƣ�sensorLinkOn()
* ���ܣ��������ڵ������ɹ����ú���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorLinkOn(void)
{
  sensorUpdate();
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
  
  // ������״̬
  updateShock();
  ZXBeeBegin();                                                 // ��������֡��ʽ��ͷ
  
  sprintf(p, "%u", shockStatus); 
  ZXBeeAdd("shockStatus", p); 
  
  p = ZXBeeEnd();                                               // ��������֡��ʽ��β
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // ����Ҫ�ϴ��������ϴ�������ƽ̨
  }
  printf("sensor->sensorUpdate(): shockStatus=%u\r\n", shockStatus); 
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
  static char lastShockStatus = 0;
  static uint32_t ct0=0;
  char pData[16];
  char *p = pData;
  
  // ������״̬
  updateShock();

  ZXBeeBegin();
  
  if (lastShockStatus != shockStatus || (ct0 != 0 && clock_time() > (ct0+3000))) {
    sprintf(p, "%u", shockStatus);
    ZXBeeAdd("shockStatus", p);
    ct0 = clock_time();
    if (shockStatus == 0) {
       ct0 = 0;
    }
    lastShockStatus = shockStatus;
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
  if (0 == strcmp("shockStatus", ptag)){                        // ��ѯִ�����������
    if (0 == strcmp("?", pval)){
      updateShock();
      ret = sprintf(p, "%u", shockStatus);
      ZXBeeAdd("shockStatus", p);
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