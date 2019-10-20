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
#include "zxbee.h"
#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"
#include "math.h"
#include "sensor.h"

PROCESS(sensor, "sensor");

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static float airPressure = 0.0;                              // ����ѹ��ֵ
/*********************************************************************************************
* ���ƣ�ZXBeeInfRecv()
* ���ܣ��ڵ��յ��������ݰ�
* ������*pkg -- �յ����������ݰ�
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void ZXBeeInfRecv(char *buf, int len)
{
  uint8_t val;
  char pData[16];
  char *p = pData;
  char *ptag = NULL;
  char *pval = NULL;

  printf("sensor->ZXBeeInfRecv(): Receive LTE Data!\r\n");
  
  leds_on(1);
  clock_delay_ms(50);
  ptag = buf;
  p = strchr(buf, '=');
  if (p != NULL) {
    *p++ = 0;
    pval = p;
  }
  val = atoi(pval);	
	
  // �����������
  if (0 == strcmp("cmd", ptag)){                                // ��D0��λ���в�����CD0��ʾλ�������
    sensorControl(val);
  }

  leds_off(1);
}
/*********************************************************************************************
* ���ƣ�ZXBeeInfSend()
* ���ܣ��ڵ㷢�����ݰ�������ƽ̨
* ������*p -- Ҫ���͵����ݰ�
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void ZXBeeInfSend(char *p, int len)
{
  leds_on(1);
  clock_delay_ms(50);
  zhiyun_send(p);
  leds_off(1);
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
  // ����ѹ����������ʼ��
  // �̵�����ʼ��
  relay_init();                                                 
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
  printf("sensor->sensorLinkOn(): Sensor LinkOn!\r\n"); 
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
  char pData[32];
  char *p = pData;
  
  // ����ѹ���ɼ�����100~110�������
  airPressure = 100 + (uint16_t)(rand()%10);
  
  // ���´���ѹ����ֵ���ϱ�
  sprintf(p, "airPressure=%.1f", airPressure);
  if (pData != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // �ϴ����ݵ�����ƽ̨
  }
  
  printf("sensor->sensorUpdate(): airPressure=%.1f\r\n", airPressure);
}
/*********************************************************************************************
* ���ƣ�sensorControl()
* ���ܣ�����������
* ������cmd - ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorControl(uint8_t cmd)
{
  // ����cmd���������Ӧ�Ŀ��Ƴ���
  if(cmd == 0){                                                 
    printf("sensor->sensorControl(): Light OFF\r\n");
	relay_control(cmd);
  }
  else if(cmd == 1){                                              
    printf("sensor->sensorControl(): Light ON\r\n");
	relay_control(cmd);
  }
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
  
  PROCESS_BEGIN();
  
  // ZXBeeInfInit();
  
  sensorInit();
  
  etimer_set(&et_update, CLOCK_SECOND*10); 
  
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); 
    if (etimer_expired(&et_update)) {
      printf("sensor->PROCESS_EVENT_TIMER: PROCESS_EVENT_TIMER trigger!\r\n");
      sensorUpdate();
      etimer_set(&et_update, CLOCK_SECOND*10); 
    } 
  }
  PROCESS_END();
}