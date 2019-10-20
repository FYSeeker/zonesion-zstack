#include "systick_if.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "pin.h"
#include "gpio.h"
#define G13_UCPINS              ( 1 << (13%8))
#define G3_UCPINS               ( 1 << (3%8))

void motorInit(void)
{
    PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
    PinTypeGPIO(PIN_58,PIN_MODE_0,false);                           //ѡ������ΪGPIOģʽ��gpio4��
    //PinConfigSet(PIN_58,PIN_STRENGTH_2MA,PIN_TYPE_STD_PU);
    GPIODirModeSet(GPIOA0_BASE, G3_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO17Ϊ���ģʽ
    
    PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
    PinTypeGPIO(PIN_04,PIN_MODE_0,false);                           //ѡ������ΪGPIOģʽ��gpio5��
   // PinConfigSet(PIN_04,PIN_STRENGTH_2MA,PIN_TYPE_STD_PU);
    GPIODirModeSet(GPIOA1_BASE, G13_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO3Ϊ���ģʽ
    
    GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);                                           
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);    
}

void motorSet(int st)
{
  if (st == 0) {
    GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);                                           
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);  
  }
  if (st == 1) {
    GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);                                           
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0xff);  
  } 
  if (st == 2) {
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
    GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xff);                                             
  }
}