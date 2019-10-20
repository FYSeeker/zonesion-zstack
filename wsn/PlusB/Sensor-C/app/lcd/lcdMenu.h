/*********************************************************************************************
* �ļ���lcdMenu.h
* ���ߣ�fuyou 2018.11.1
* ˵����lcdMenu.c��ͷ�ļ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef _lcdmenu_h_
#define _lcdmenu_h_

#include <stdio.h>
#include <string.h>
#include "ili93xx.h"
#include "hw.h"
#include "lcd.h"

#define MenuIndexMAX    4

typedef enum
{
    UNSELECT=0,
    SELECT=1,
}optionStatus_t;

typedef struct
{
    unsigned char index;                                    //�˵�����
    optionStatus_t optionState[MenuIndexMAX];               //ѡ��״̬
    char* optionName[MenuIndexMAX];                         //ѡ������
    void (*optionHandle[MenuIndexMAX])(optionStatus_t);     //ѡ�����
}_Menu_t;

typedef struct
{
    char name[5][14];
    char explain[5][14];
    char pdata[5][14];
}_Table_t;

typedef struct
{
    char name[3][14];
    char pdata[3][14];
}_SensorBuf_t;


void lcdShowMenuBar_1(unsigned short y,char* buf,unsigned short wordColor,unsigned short backColor,
                      unsigned char refresh,unsigned char mode);
void lcdShowMenuBar_3(unsigned short y,char* buf1,char* buf2,char* buf3,
                      unsigned short wordColor,unsigned short backColor,unsigned char refresh,unsigned char mode);
void menuShowHandle(unsigned char refresh);
void menuKeyHandle(unsigned char keyStatus);
void menuExitHandle(void);
void lcdShowSensorInfo(unsigned short y,unsigned short wordColor,
                       unsigned short backColor_1,unsigned short backColor_2,unsigned char refresh);

char optionCallFunc_set(unsigned char optionIndex,void (*func)(optionStatus_t));
char optionDefaultName_set(unsigned char index,char* pbuf);
char optionSelectName_set(unsigned char index,char* pbuf);
char optionHintInfo_set(unsigned char index,char* pbuf);
char optionState_set(unsigned char index,optionStatus_t state);
char tableExplain_set(unsigned char index,char* pbuf1);
char tableData_set(unsigned char index,char* pbuf1);

void SensorName_set(char* pbuf1,char* pbuf2,char* pbuf3);
void SensorData_set(char* pbuf1,char* pbuf2,char* pbuf3);

#endif