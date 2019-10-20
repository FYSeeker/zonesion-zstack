/*********************************************************************************************
* �ļ���sensor.c
* ���ߣ�Xuzhy 2018.5.16
* ˵�����ڵ㹤������ģ��
*       �ǻ�����ϵͳ���������豸���ɼ��ࣨ�к�����ɼ����������ࣨ�����դ�������������ࣨ�ŷ��ȣ�
*       ���ݰ���ʽ��{gas=101.0,gratingStatus=0,fanStatus=0}
*       gas��ʾ�к�����ֵ��gratingStatusΪ�����դ����״̬��0/1����fanStatusΪ�ŷ��ȵ�״̬
*       ��ѯ�����ѯ��ֵ�ο��������ѯ��ǰ�к�����ֵ��{gas=?}��
*       �������cmd=0Ϊ�ŷ��ȹرգ�cmd=1Ϊ�ŷ��ȴ�
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
#include "grating.h"
#include "zxbee.h"
#include "zxbee-inf.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static uint8_t gratingStatus = 0;                                   // �����դ״ֵ̬
/*********************************************************************************************
* ���ƣ�updateGrating()
* ���ܣ����º����դ״ֵ̬
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateGrating(void)
{
  gratingStatus = get_grating_status();                         // ���¹�դ��������ֵ
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
  printf("sensor->sensorInit(): Sensor init!\r\n");
  // ��ʼ������������
  grating_init();                                               // ��դ��������ʼ��
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
  
  // ���º����դ״̬
  updateGrating();
  ZXBeeBegin();                                                 // ��������֡��ʽ��ͷ
  
  sprintf((char*)p, "%u", gratingStatus); 
  ZXBeeAdd("gratingStatus", p); 
  
  p = ZXBeeEnd();                                               // ��������֡��ʽ��β
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // ����Ҫ�ϴ��������ϴ�������ƽ̨
  }
  printf("sensor->sensorUpdate(): gratingStatus=%u\r\n", gratingStatus); 
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
  static char lastgratingStatus = 0;
  static uint32_t ct0=0;
  char pData[16];
  char *p = pData;
  
  // ���º����դ״̬
  updateGrating();

  ZXBeeBegin();
  
  if (lastgratingStatus != gratingStatus || (ct0 != 0 && clock_time() > (ct0+3000))) {
    sprintf(p, "%u", gratingStatus);
    ZXBeeAdd("gratingStatus", p);
    ct0 = clock_time();
    if (gratingStatus == 0) {
       ct0 = 0;
    }
    lastgratingStatus = gratingStatus;
  }

  p = ZXBeeEnd();
  if (p != NULL) {
    int len = strlen(p); 
    ZXBeeInfSend(p, len);
	printf("sensor->sensorCheck: Grating alarm!\r\n");
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

  printf("sensor->ZXBeeUserProcess(): Receive LoRa Data!\r\n");
  // �����������
  if (0 == strcmp("gratingStatus", ptag)){                        // ��ѯִ�����������
    if (0 == strcmp("?", pval)){
      updateGrating();
      ret = sprintf(p, "%u", gratingStatus);
      ZXBeeAdd("gratingStatus", p);
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