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
#include "zxbee.h"
#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"
#include "math.h"
#include "sensor.h"
#include "config.h"

int zhiyun_send_coap(char *pkg);
int zhiyun_send_udp(char *pkg);

#define COAP    0
#define UDP     1

PROCESS(sensor, "sensor");
PROCESS_NAME(u_zhiyun_coap);
PROCESS_NAME(u_zhiyun_udp);
/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static float airGas = 0.0;                              // ��������ֵ

/*********************************************************************************************
* ���ƣ�ZXBeeInfInit
* ���ܣ�ZXBee�ӿڵײ��ʼ��
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void ZXBeeInfInit(void)
{
  config_init();

  if (nbConfig.mode == 0) {
    process_start(&u_zhiyun_coap, NULL);
  } else {
     process_start(&u_zhiyun_udp, NULL);
  }
}

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

  printf("sensor->ZXBeeInfRecv(): Receive NB-IoT Data!\r\n");
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
  if (nbConfig.mode == COAP) {
    zhiyun_send_coap(p);
  } else {
    zhiyun_send_udp(p);
  }
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
  // ����������������ʼ��
  // �̵�����ʼ��
  relay_init();                                                 
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
  
  // ���������ɼ�����100~110�������
  airGas = 100 + (uint16_t)(rand()%10);
  
  // ���¿���������ֵ���ϱ�
  sprintf(p, "airGas=%.1f", airGas);
  if (pData != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // �ϴ����ݵ�����ƽ̨
  }
  
  printf("sensor->sensorUpdate(): airGas=%.1f\r\n", airGas);
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
    printf("sensor->sensorControl(): Beep OFF\r\n");
	relay_control(cmd);
  }
  else if(cmd == 1){                                              
    printf("sensor->sensorControl(): Beep ON\r\n");
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
  
  ZXBeeInfInit();
  
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