/*********************************************************************************************
* �ļ���api_lcd.c
* ���ߣ�fuyou 2018.11.1
* ˵����api_lcd.c��ͷ�ļ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef _api_lcd_h_
#define _api_lcd_h_

#include <stdio.h>
#include <string.h>
#include "ili93xx.h"
#include "hw.h"
#include "lcd.h"

//�׼�����
#define SuiteType       "Sensor-B"

void lcdSensor_init(void);

void lcdSensor_poll(void);
void xLabInfoShow(unsigned short y,unsigned short wordColor,unsigned short backColor,unsigned char refresh);
void xLabPageShow(uint8_t refresh);

#endif