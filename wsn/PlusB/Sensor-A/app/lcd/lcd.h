#ifndef _lcd_h_
#define _lcd_h_

#include <stdio.h>
#include <contiki.h>
#include "zigbee-net.h"
#include "ble-net.h"
#include "wifi-net.h"
#include "lora-net.h"
#include "ili93xx.h"
//#include "image.h"
#include "htu21d.h"
#include "BatteryVoltage.h"
#include "hw.h"
#include "key.h"
#include "api_lcd.h"
#include "lcdMenu.h"


#define INTERFACE_TYPE 0        //界面类型

extern char lcdPageIndex;
extern uint8_t lcdEvent_InitFlag;
extern process_event_t lcd_event;
PROCESS_NAME(lcd);

void lcdShowBarX1(unsigned short y,char* buf,unsigned short wordColor,unsigned short backColor,
                  unsigned char refresh,unsigned char mode);
void lcdShowBarX3(unsigned short y,char* buf1,char* buf2,char* buf3,
                  unsigned short wordColor,unsigned short backColor,unsigned char refresh,unsigned char mode);
//显示条目-4
void lcdShowBarX4(unsigned short y,char* buf1,char* buf2,char* buf3,char* buf4,
                  unsigned short wordColor,unsigned short backColor,unsigned char refresh,unsigned char mode);

void lcdShowPage(unsigned char refresh);

#endif