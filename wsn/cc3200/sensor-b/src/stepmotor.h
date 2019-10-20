/*********************************************************************************************
* �ļ���stepmotor.h
* ���ߣ�zonesion
* ˵����stepmotorͷ�ļ�
* �޸ģ�Chenkm 2017.01.10 �޸Ĵ����ʽ�����Ӵ���ע�ͺ��ļ�˵��
* ע�ͣ�
*********************************************************************************************/
#ifndef STEPMOTOR_H
#define STEPMOTOR_H
#include "gpio.h"
#include "hw_memmap.h"
#include "systick_if.h"
#include "pin.h"
#include "prcm.h"
#include <stdio.h>
#include "rom_map.h"
#include "systick.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define CLKDIV  ( CLKCONCMD & 0x07 )  

#define G13_UCPINS  ( 1 << (13%8))
#define G03_UCPINS  ( 1 << (3%8))
#define G11_UCPINS  ( 1 << (11%8))

#define pin_step        G13_UCPINS
#define pin_dir         G03_UCPINS
#define pin_enable      G11_UCPINS

/*********************************************************************************************
* �ⲿԭ�ͺ���
*********************************************************************************************/
void stepmotor_init(void);
void reversion(int step);
void forward(int step);
void step(int dir,int steps);

#endif  //STEPMOTOR_H