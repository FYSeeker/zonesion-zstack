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

#include "beep.h"
#include "ext_led.h"
#include "fan.h"
#include "rgb.h"
#include "stepmotor.h"

#define SENSOR_TYPE  "602"                                      // ����������

void sensor_init(void);
void sensor_poll(unsigned int t);
unsigned short sensor_check(void);

#endif