/*********************************************************************************************
* 文件：sensor.h
* 作者：
* 说明：通用传感器控制接口程序头文件
* 修改：
* 注释：
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

#include "ext_htu21d.h"
#include "bh1750.h"
#include "mp-503.h"
#include "fbm320.h"
#include "stadiometry.h"
#include "lis3dh.h"
#include "ld3320.h"

#define CheckTime   200                                         // sensor_check函数轮询时间

#define SENSOR_TYPE  "601"                                      // 传感器编码

void sensor_init(void);
void sensor_poll(unsigned int t);
unsigned short sensor_check(void);

#endif