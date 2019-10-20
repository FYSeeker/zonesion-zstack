/*********************************************************************************************
* �ļ���iic.h
* ���ߣ�zonesion
* ˵����iicͷ�ļ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef _IIC_H_
#define _IIC_H_

#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "prcm.h"
#include "pinmux.h"
#include "rom.h"
#include "rom_map.h"
#include "gpio.h"
#include "pin.h"

/*********************************************************************************************
* �ⲿԭ�ͺ���
*********************************************************************************************/
void iic_init(void);
void iic_start(void);
void iic_stop(void);

unsigned char iic_write_byte(unsigned char data);
unsigned char iic_read_byte(unsigned char ack);

int iic_write_buf(char addr, char r, char *buf, int len);
int iic_read_buf(char addr, char r, char *buf, int len);

#endif 