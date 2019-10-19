/*********************************************************************************************
* �ļ���sensor.c
* ���ߣ�Zhoucj 2018.6.27
* ˵�������һ������(ZPHS01)
*       ����A0��ʾCO2��A1��ʾVOC�ȼ�/��ȩ��A2��ʾʪ�ȣ�A3��ʾ�¶ȣ�A4��ʾPM2.5
*       V0��ʾ�����ϱ�ʱ������D0(Bit0/Bit1/Bit2/Bit3/Bit4/Bit5)��ʾ�����ϱ�A0/A1/A2/A3/A4ʹ��
*       V1=0��ʾCO2���У׼(���ָ���� 400ppm���������У׼����ǰ��ȷ���������� 400ppm Ũ�����ȶ����� 20 ����)
*       Ĭ��ֵ��A0=0,A1=0,A2=0.0,A3=0.0,A4=0,V0=30,D1=1,D0=0xFF
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
#include "zxbee.h"
#include "zxbee-inf.h"
#include "OnBoard.h"
/*********************************************************************************************
* �궨��---
*********************************************************************************************/
#define RELAY1                  P0_6                            // ����̵�����������
#define RELAY2                  P0_7                            // ����̵�����������
#define ON                      0                               // �궨���״̬����ΪON
#define OFF                     1                               // �궨��ر�״̬����ΪOFF
/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static uint8 D0 = 0xFF;                                         // Ĭ�ϴ������ϱ�����
static uint8  D1 = 1;                                           // �۳�������ʼ״̬Ϊ����
static uint16 A0 = 0;                                           //CO2
static uint16 A1 = 0;                                           //VOC�ȼ�/��ȩ
static float A2 = 0;                                            //ʪ��
static float A3 = 0;                                            //�¶�
static uint16 A4 = 0;                                           //PM2.5
static uint16 V0 = 30;                                          // V0����Ϊ�ϱ�ʱ������Ĭ��Ϊ30s

static unsigned char read_data[5] = {0x11,0x02,0x01,0x00,0xEC}; //��ȡ��ǰ��������ֵ
static unsigned char co2_correct[6] = {0x11,0x03,0x03,0x01,0x90,0x58};//CO2���У׼
static unsigned char pm25_open[6] = {0x11,0x03,0x0C,0x02,0x1E,0xC0};//�����۳�����
static unsigned char pm25_stop[6] = {0x11,0x03,0x0C,0x01,0x1E,0xC1};//ֹͣ�۳�����
/*********************************************************************************************
* UART RS485��ؽӿں���
*********************************************************************************************/
static void node_uart_init(void);
static void uart_callback_func(uint8 port, uint8 event);
static void uart_485_write(uint8 *pbuf, uint16 len);
static void node_uart_callback ( uint8 port, uint8 event );

/*********************************************************************************************
* ���ƣ�updateV0()
* ���ܣ�����V0��ֵ
* ������*val -- �����µı���
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateV0(char *val)
{
  //���ַ�������val����ת��Ϊ���ͱ�����ֵ
  V0 = atoi(val);                                               // ��ȡ�����ϱ�ʱ�����ֵ
}
/*********************************************************************************************
* ���ƣ�updateA0()
* ���ܣ�����A0��ֵ
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateA0(void)
{
  // ���Ͷ�ȡ��������������ֵָ��
  uart_485_write(read_data, 5);
}
/*********************************************************************************************
* ���ƣ�updateA1()
* ���ܣ�����A1��ֵ
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateA1(void)
{

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
  node_uart_init();

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
  uart_485_write(read_data, 5);
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
  
  ZXBeeBegin();                                                 // ��������֡��ʽ��ͷ
  
  // ����D0��λ״̬�ж���Ҫ�����ϱ�����ֵ
  if ((D0 & 0x01) == 0x01){                                     // ���ϱ�������pData�����ݰ����������
    updateA0();
    sprintf(p, "%d", A0); 
    ZXBeeAdd("A0", p);
  }
  if ((D0 & 0x02) == 0x02){                                     // ���ϱ�������pData�����ݰ����������
    updateA0();
    sprintf(p, "%d", A1); 
    ZXBeeAdd("A1", p);
  }
  if ((D0 & 0x04) == 0x04){                                     // ���ϱ�������pData�����ݰ����������
    updateA0();
    sprintf(p, "%.1f", A2); 
    ZXBeeAdd("A2", p);
  }
  if ((D0 & 0x08) == 0x08){                                     // ���ϱ�������pData�����ݰ����������
    updateA0();
    sprintf(p, "%.1f", A3); 
    ZXBeeAdd("A3", p);
  }
  if ((D0 & 0x10) == 0x10){                                     // ���ϱ�������pData�����ݰ����������
    updateA0();
    sprintf(p, "%d", A4); 
    ZXBeeAdd("A4", p);
  }
  
  sprintf(p, "%u", D1);                                         // �ϱ����Ʊ��� 
  ZXBeeAdd("D1", p);
  
  p = ZXBeeEnd();                                               // ��������֡��ʽ��β
  if (p != NULL) {												
    ZXBeeInfSend(p, strlen(p));	                                // ����Ҫ�ϴ������ݽ��д����������ͨ��zb_SendDataRequest()���͵�Э����
  }
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
  updateA0();
}
/*********************************************************************************************
* ���ƣ�sensorControl()
* ���ܣ�����������
* ������cmd - ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorControl(uint8 cmd)
{
  // ����cmd���������Ӧ�Ŀ��Ƴ���
   if(cmd & 0x01){ 
    uart_485_write((unsigned char*)pm25_open, 6);                     //�����۳�����
  }
  else{
    uart_485_write((unsigned char*)pm25_stop, 6);                     //�رշ۳�����
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
  int val;
  int ret = 0;	
  char pData[16];
  char *p = pData;
  
  // ���ַ�������pval����ת��Ϊ���ͱ�����ֵ
  val = atoi(pval);	
  // �����������
  if (0 == strcmp("CD0", ptag)){                                // ��D0��λ���в�����CD0��ʾλ�������
    D0 &= ~val;
  }
  if (0 == strcmp("OD0", ptag)){                                // ��D0��λ���в�����OD0��ʾλ��һ����
    D0 |= val;
  }
  if (0 == strcmp("D0", ptag)){                                 // ��ѯ�ϱ�ʹ�ܱ���
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", D0);
      ZXBeeAdd("D0", p);
    } 
  }
  if (0 == strcmp("CD1", ptag)){                                // ��D1��λ���в�����CD1��ʾλ�������
    D1 &= ~val;
    sensorControl(D1);                                          // ����ִ������
  }
  if (0 == strcmp("OD1", ptag)){                                // ��D1��λ���в�����OD1��ʾλ��һ����
    D1 |= val;
    sensorControl(D1);                                          // ����ִ������
  }
  if (0 == strcmp("D1", ptag)){                                 // ��ѯִ�����������
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", D1);
      ZXBeeAdd("D1", p);
    } 
  }
  if (0 == strcmp("A0", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%d", A0);     
      ZXBeeAdd("A0", p);
    } 
  }
  if (0 == strcmp("A1", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%d", A1);  
      ZXBeeAdd("A1", p);
    } 
  }
  if (0 == strcmp("A2", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%.1f", A2);  
      ZXBeeAdd("A2", p);
    } 
  }
  if (0 == strcmp("A3", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%.f", A3);  
      ZXBeeAdd("A3", p);
    } 
  }
  if (0 == strcmp("A4", ptag)){ 
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%d", A4);  
      ZXBeeAdd("A4", p);
    } 
  }
  if (0 == strcmp("V0", ptag)){
    if (0 == strcmp("?", pval)){
      ret = sprintf(p, "%u", V0);                         	// �ϱ�ʱ����
      ZXBeeAdd("V0", p);
    }else{
      updateV0(pval);
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
    sensorUpdate();
    //������ʱ���������¼���MY_REPORT_EVT 
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, V0*1000);
  }  
  if (event & MY_CHECK_EVT) { 
    sensorCheck(); 
    // ������ʱ���������¼���MY_CHECK_EVT 
    osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 1000);
  } 
}
/*********************************************************************************************
* ���ƣ�uart0_init()
* ���ܣ�����0��ʼ������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void node_uart_init(void)
{
  halUARTCfg_t _UartConfigure;
  
  // UART ������Ϣ
  _UartConfigure.configured           = TRUE;
  _UartConfigure.baudRate             = HAL_UART_BR_9600;
  _UartConfigure.flowControl          = FALSE;
  _UartConfigure.rx.maxBufSize        = 128;
  _UartConfigure.tx.maxBufSize        = 128;
  _UartConfigure.flowControlThreshold = (128 / 2);
  _UartConfigure.idleTimeout          = 6;
  _UartConfigure.intEnable            = TRUE;
  _UartConfigure.callBackFunc         = uart_callback_func;
  
  HalUARTOpen (HAL_UART_PORT_0, &_UartConfigure);               //����UART
}

/*********************************************************************************************
* ���ƣ�uart_485_write()
* ���ܣ�д485ͨѶ
* ������pbuf -- �����������������ָ��
*       len -- �������,���������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void uart_485_write(uint8 *pbuf, uint16 len)
{
  P2_0 = 1;
  
  HalUARTWrite(HAL_UART_PORT_0, pbuf, len);
}

/*********************************************************************************************
* ���ƣ�uart_callback_func()
* ���ܣ�RS485ͨѶ�ص�������ʹ����ʱ
* ������port -- ������������ݽ��ն˿�
*       event -- ��������������¼�
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void uart_callback_func(uint8 port, uint8 event) 
{
  if (event & HAL_UART_TX_EMPTY) 
  {
    uint16 szDelay[] = {2000,1000,500,100,10,10000};
    MicroWait(szDelay[HAL_UART_BR_9600]);                       // ������Խ����ʱʱ����Խ��,��ʱ̫����Ӱ�����ݽ���
    P2_0 = 0;
  }
  node_uart_callback(port, event);
}

/*********************************************************************************************
* ���ƣ�node_uart_callback()
* ���ܣ��ڵ㴮��ͨѶ�ص�����
* ������port -- ������������ն˿�
*       event -- ��������������¼�
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void node_uart_callback( uint8 port ,uint8 event)
{
#define RBUFSIZE 32
  (void)event;
  uint8  ch;
  static uint8 szBuf[RBUFSIZE];
  static uint8 rlen = 0;
  static uint8 data_len = 0;
  static uint8 flag_ = 0x00;
  static uint8 data_mode = 0xff;
  
  // ����ͨ�����ڴ���������
  while (Hal_UART_RxBufLen(port))
  {
    HalUARTRead (port, &ch, 1);
    if((ch == 0x16) &&(flag_ == 0x00))
    {
      flag_ = 0x01;
    }
    else if(flag_ == 0x01){
      data_len = ch;
      flag_ = 0x02;
    }
    else if(flag_ == 0x02)
    {
      data_mode = ch;
      flag_ = 0x03;
    }
    else if((flag_ == 0x03)){
      szBuf[rlen++] = ch;
      if(rlen>=data_len)
      {
        flag_ = 0x00;
        switch (data_mode){
        case 0x01:
          A0 = szBuf[0]*256+szBuf[1];
          A1 = szBuf[2]*256+szBuf[3];
          A2 = (szBuf[4]*256+szBuf[5])*0.1;
          A3 = (szBuf[6]*256+szBuf[7]-500)*0.1;
          A4 = szBuf[8]*256+szBuf[9];
          break;
        case 0x03:
          break;
        case 0x0c:
          if(szBuf[0] == 0x01)
            D1 = 0;
          else
            D1 = 1;
          break;
        default:
          rlen = 0;
          break;
        }
        rlen = 0;
      }
    }
  }
}