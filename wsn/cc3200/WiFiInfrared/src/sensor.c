/*********************************************************************************************
* �ļ���sensor.c
* ���ߣ�Xuzhy 2018.5.16
* ˵�����ڵ㹤������ģ��
*       ���ܼҾ����������豸���ɼ��ࣨ�¶Ȳɼ����������ࣨ������ⱨ��״̬���������ࣨ���õ�����
*       ���ݰ���ʽ��{Temperature=124.0,infraredStatus=0,switchStatus=0}
*       temperature��ʾ�¶�ֵ��infraredStatusΪ������ⱨ��״̬��0/1����switchStatusΪ���õ�����״̬��0���رգ�1�����յ���2������ʪ����3�����յ��ͼ�ʪ����
*       ��ѯ�����ѯ��ֵ�ο��������ѯ��ǰ�¶ȣ�{temperature=?}��
*       �������cmd=0Ϊ�رյ�����cmd=1Ϊ�򿪿յ���cmd=2Ϊ�򿪼�ʪ����cmd=3Ϊ�򿪿յ��ͼ�ʪ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zxbee.h"
#include "udma_if.h"
#include "common.h"
#include "hw_ints.h"
#include "hw_types.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "systick_if.h"
#include "infrared.h"
#include "zxbee-inf.h"
#include "sensor.h"
/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define DEBUG 1
#if DEBUG
#define DebugMsg        DBG_PRINT 
#else
#define DebugMsg(...)
#endif
/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static uint8 infraredStatus = 0;                                   // ������ⱨ��״ֵ̬
/*********************************************************************************************
* ���ƣ�updateInfraredStatus()
* ���ܣ�����������ⱨ��ֵ
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateInfraredStatus(void)
{
   static uint32 ct = 0;
   
  // ����������⴫������ֵ
  infraredStatus = get_infrared_status();
  if (infraredStatus != 0) {
    ct = clock_time();
  } else if (clock_time() > ct+1000) {
    ct = 0;
    infraredStatus = 0;
  } else {
    infraredStatus = 1;
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
  infrared_init();                                            // ������⴫������ʼ��
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
  
  // ������ⱨ��״̬����
  updateInfraredStatus();
  
  ZXBeeBegin();                                                 // ��������֡��ʽ��ͷ

  // ����infraredStatus��ֵ
  sprintf(p, "%u", infraredStatus); 
  ZXBeeAdd("infraredStatus", p); 
  
  p = ZXBeeEnd();                                               // ��������֡��ʽ��β
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	 
  }
  DebugMsg("sensor->sensorUpdate(): infraredStatus=%u\r\n", infraredStatus); 
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
  static char lastinfraredStatus=0;
  static uint32 ct0=0;
  char pData[16];
  char *p = pData;
  
  // ������ⱨ��״̬�ɼ�
  updateInfraredStatus();

  ZXBeeBegin();

  if (lastinfraredStatus != infraredStatus || (ct0 != 0 && clock_time() > (ct0+3000))) {  // ������ⱨ��״̬���
    sprintf(p, "%u", infraredStatus);
    ZXBeeAdd("infraredStatus", p);
    ct0 = clock_time();
    if (infraredStatus == 0) {
      ct0 = 0;
    }
    lastinfraredStatus = infraredStatus;
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
  if (0 == strcmp("infraredStatus", ptag)){                        // ��ѯִ�����������
    if (0 == strcmp("?", pval)){
      updateInfraredStatus();
      ret = sprintf(p, "%u", infraredStatus);
      ZXBeeAdd("infraredStatus", p);
    } 
  }
  
  return ret;
}
/*********************************************************************************************
* ���ƣ�sensorLoop()
* ���ܣ���ʱ��������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorLoop(void)
{
  static unsigned long ct_update = 0;
  static unsigned long ct_check = 0;
  
  if (t4exp(ct_update)) {
    sensorUpdate();
    ct_update = t4ms()+20*1000;
  }
  if (t4exp(ct_check)) {
    sensorCheck();
    ct_check = t4ms()+100;
  }
}