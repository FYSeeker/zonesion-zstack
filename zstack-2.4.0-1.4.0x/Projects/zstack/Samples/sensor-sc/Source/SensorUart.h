/*********************************************************************************************
* 文件：SensorUart.h
* 作者：
* 说明：sensor uart头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef SENSORUART_H
#define SENSORUART_H

#include <ioCC2530.h>
#include <stdio.h>
#include <math.h>

void uart0_init(unsigned char StopBits,unsigned char Parity);
void UartCallbackSet(void (*func)(char dat));
void uart_send_char(char ch);

#endif