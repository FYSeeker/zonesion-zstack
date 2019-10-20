/*********************************************************************************************
* �ļ���sensor.h
* ���ߣ�
* ˵����ͨ�ô��������ƽӿڳ���ͷ�ļ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef _sensor_h_
#define _sensor_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "zxbee.h"
#include "Relay.h"
#include "htu21d.h"
#include "BatteryVoltage.h"
#include "rfUart.h"

#include "flame.h"
#include "grating.h"
#include "hall.h"
#include "infrared.h"
#include "MP-4.h"
#include "touch.h"
#include "vibration.h"
#include "syn6288.h"

#define CheckTime   200                                         

#define SENSOR_TYPE  "603"                                      // ����������

void sensor_init(void);
void sensor_poll(unsigned int t);
unsigned short sensor_check(void);

#endif