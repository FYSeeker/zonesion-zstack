/*********************************************************************************************
* �ļ���SensorUart.h
* ���ߣ�
* ˵����sensor uartͷ�ļ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef MC3200_H
#define MC3200_H

#include <ioCC2530.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "SensorUart.h"

void Mc3200Init(void);
char Mc3200GetUartReady(void);
void Mc3200CheckUart(void);
void Mc3200Set(void);
char Mc3200GetResult(void);
char Mc3200GetResultData(char* buf);

#endif