#ifndef _VML100_H_
#define _VML100_H_

#include <ioCC2530.h>
#include "string.h"
#include "hal_uart.h"

#define VML100_READY_SBIT           P0_4
#define VML100_READY_BV             BV(4)

#define VML100_STATE_SBIT           P0_5
#define VML100_STATE_BV             BV(5)

#define VML100_CTRLO_SBIT           P0_0
#define VML100_CTRLO_BV             BV(0)

#define VML100_CTRLI_SBIT           P0_1 
#define VML100_CTRLI_BV             BV(1)

void vml100_init(void);
void vml100_OPEN(void);
void vml100_CONT(void);
void vml100_STOP(void);
void vml100_CARS(void);
void vml100_VERS(void);

uint8 vml100_StatusGet(void);
void vml100_sendData(char* dat);

#endif
