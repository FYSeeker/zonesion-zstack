/*********************************************************************************************
* �ļ���sensor.h
* ���ߣ�Xuzhy 2018.5.16
* ˵����sensorͷ�ļ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef SENSOR_H
#define SENSOR_H
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
#include "Flame.h"
#include "grating.h"
#include "hall.h"
#include "relay.h"
#include "infrared.h"
#include "MP-4.h"
#include "Touch.h"
#include "vibration.h"
#include "syn6288.h"
#include "zxbee.h"
#include "zxbee-inf.h"
#include "rgb.h"
/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define MY_REPORT_EVT           0x0001
#define MY_CHECK_EVT           	0x0002
#define NODE_NAME               "702"                           // �ڵ�����
#define NODE_CATEGORY           1
#define NODE_TYPE               NODE_ENDDEVICE                  // ·�ɽڵ㣺NODE_ROUTER  �ն˽ڵ㣺NODE_ENDDEVICE
/*********************************************************************************************
* ����ԭ��
*********************************************************************************************/
extern void sensorInit(void);
extern void sensorLinkOn(void);
extern void sensorUpdate(void);
extern void sensorCheck(void);
extern void sensorControl(uint8 cmd);
extern void MyEventProcess( uint16 event );

#endif // SENSOR_H