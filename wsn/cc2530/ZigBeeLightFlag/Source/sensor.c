/*********************************************************************************************
* �ļ���sensor.c
* ���ߣ�Xuzhy 2018.5.16
* ˵�����ڵ㹤������ģ��
*       �ǻ�ũҵϵͳ���������豸���ɼ��ࣨ���նȲɼ����������ࣨ����ǿ�ȷ�ֵ����״̬���������ࣨ���������
*       ���ݰ���ʽ��{lightIntensity=124.0,lightIntensityStatus=0,mortorStatus=0}
*       lightIntensity��ʾ��ǿֵ��lightIntensityStatusΪ��ǿ������ֵ��0/1����motorStatusΪ���������״̬��ON/OFF��
*       ��ѯ�����ѯ��ֵ�ο��������ѯ��ǰ��ǿ�ȣ�{lightIntensity=?}��
*       �������cmd=0Ϊ�ر����������cmd=1Ϊ�򿪵��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
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
#include "BH1750.h"
#include "iic.h"
#include "zxbee.h"
#include "zxbee-inf.h"
/*********************************************************************************************
* �궨��
*********************************************************************************************/

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static float lightIntensity = 0.0;                              // ����ǿ��ֵ
static uint8 lightStatus = 0;                                   // ���շ�ֵ����״ֵ̬
/*********************************************************************************************
* ���ƣ�updatelightIntensity()
* ���ܣ����¹�ǿֵ
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateLightIntensity(void)
{  
  // ��ȡ����ǿ��ֵ
  lightIntensity = bh1750_get_data();
  
  // ����ǿ��ֵ����״ֵ̬
  if(lightIntensity <= 800)
    lightStatus = 0;
  else
    lightStatus = 1;
  // printf("sensor->updateLightIntensity(): lightIntensity=%.1f,lightStatus=%u\r\n", lightIntensity,lightStatus); 
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
  bh1750_init();                                                // ��ǿ��������ʼ��

  // ������ʱ���������������ϱ������¼���MY_REPORT_EVT
  osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
  // ������ʱ������������������¼���MY_CHECK_EVT
  osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
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
  
  // ��ǿ�Ȳɼ�
  updateLightIntensity();
  
  ZXBeeBegin();                                                 // ��������֡��ʽ��ͷ

  // ���¹�ǿֵ
  sprintf(p, "%.1f", lightIntensity); 
  ZXBeeAdd("lightIntensity", p);
  
  // ����lightStatus��ֵ
  sprintf(p, "%u", lightStatus); 
  ZXBeeAdd("lightStatus", p); 
  
  p = ZXBeeEnd();                                               // ��������֡��ʽ��β
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // ����Ҫ�ϴ������ݽ��д����������ͨ��zb_SendDataRequest()���͵�Э����
  }
  printf("sensor->sensorUpdate(): lightIntensity=%.1f,lightStatus=%u\r\n", lightIntensity,lightStatus); 
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
  static char lastlightStatus=0;
  static uint32 ct0=0;
  char pData[16];
  char *p = pData;
  
  // ��ǿ��ֵ����״̬�ɼ�
  updateLightIntensity();

  ZXBeeBegin();

  if (lastlightStatus != lightStatus || (ct0 != 0 && osal_GetSystemClock() > (ct0+3000))) {  // ��ǿ��ֵ����״̬���
    sprintf(p, "%u", lightStatus);
    ZXBeeAdd("lightStatus", p);
	ct0 = osal_GetSystemClock();
    if (lightStatus == 0) {
      ct0 = 0;
    }
    lastlightStatus = lightStatus;
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
*       *pout -- ������Ӧ���ݣ������ݷ��ظ��ϼ����ã�ͨ��zb_SendDataRequest{}���͸�Э����
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
  if (0 == strcmp("lightIntensity", ptag)){                     // ��ѯִ�����������
    if (0 == strcmp("?", pval)){
      updateLightIntensity();
      ret = sprintf(p, "%.1f", lightIntensity);
      ZXBeeAdd("lightIntensity", p);
    } 
  }
  if (0 == strcmp("lightStatus", ptag)){                        // ��ѯִ�����������
    if (0 == strcmp("?", pval)){
	  updateLightIntensity();
      ret = sprintf(p, "%u", lightStatus);
      ZXBeeAdd("lightStatus", p);
    } 
  }
  
  return ret;
}
/*********************************************************************************************
* ���ƣ�MyEventProcess()
* ���ܣ��Զ����¼�����
* ������event -- �¼����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void MyEventProcess( uint16 event )
{
  if (event & MY_REPORT_EVT) { 
    sensorUpdate();                                             // ���������ݶ�ʱ�ϱ�
    //������ʱ���������¼���MY_REPORT_EVT 
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, 20*1000);
  }  
  if (event & MY_CHECK_EVT) { 
    sensorCheck();                                              // ������״̬ʵʱ���
    // ������ʱ���������¼���MY_CHECK_EVT 
    osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
  } 
}