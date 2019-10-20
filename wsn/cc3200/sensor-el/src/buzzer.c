/*********************************************************************************************
* �ļ���beep.c
* ���ߣ�Lixm 2017.10.17
* ˵������������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
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
#include "buzzer.h"

#define BEEP_GPIO_PIN   (1<<(4%8))
/*********************************************************************************************
* ���ƣ�beep_init()
* ���ܣ��̵�����������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void buzzer_init(void)
{
  
  MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
  PinTypeGPIO(PIN_59,PIN_MODE_0, false);
  
    
  GPIODirModeSet(GPIOA0_BASE, BEEP_GPIO_PIN, GPIO_DIR_MODE_OUT);
    
  

  GPIOPinWrite(GPIOA0_BASE, BEEP_GPIO_PIN, 0xff);
}

/*********************************************************************************************
* ���ƣ�beep_control()
* ���ܣ��̵������������ƺ���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void buzzer_on(void)
{
  GPIOPinWrite(GPIOA0_BASE, BEEP_GPIO_PIN, 0);
}
void buzzer_off(void)
{
  GPIOPinWrite(GPIOA0_BASE, BEEP_GPIO_PIN, 0xff);
}

