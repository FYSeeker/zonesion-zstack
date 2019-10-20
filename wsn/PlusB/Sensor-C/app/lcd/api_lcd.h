/*********************************************************************************************
* 文件：api_lcd.c
* 作者：fuyou 2018.11.1
* 说明：api_lcd.c的头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _api_lcd_h_
#define _api_lcd_h_

#include <stdio.h>
#include <string.h>
#include "ili93xx.h"
#include "hw.h"
#include "lcd.h"

//套件类型
#define SuiteType       "工业套件"

void lcdSensor_init(void);

void lcdSensor_poll(void);
void xLabInfoShow(unsigned short y,unsigned short wordColor,unsigned short backColor,unsigned char refresh);
void xLabPageShow(uint8_t refresh);

#endif