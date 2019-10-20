/*********************************************************************************************
* �ļ���led.c
* ���ߣ�Meixin 2017.09.15
* ˵����LED����������     
* �޸ģ�liutong 20171027 �޸���LED�������ų�ʼ���������ơ��Ż���ע�͡�������LED�򿪺͹رպ���
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "leds.h"

/*********************************************************************************************
* ���ƣ�led_init()
* ���ܣ�LED�������ų�ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_init(void)
{
  PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_59,PIN_MODE_0,0);                           //ѡ������ΪGPIOģʽ��gpio17��
  GPIODirModeSet(GPIOA0_BASE, G04_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO17Ϊ���ģʽ
  
  PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_60,PIN_MODE_0,0);                           //ѡ������ΪGPIOģʽ��gpio17��
  GPIODirModeSet(GPIOA0_BASE, G05_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO17Ϊ���ģʽ
  
  GPIOPinWrite(GPIOA0_BASE, G04_UCPINS, 0xff);
  GPIOPinWrite(GPIOA0_BASE, G05_UCPINS, 0xff);
}
  
/*********************************************************************************************
* ���ƣ�led_on()
* ���ܣ�LED���ƴ򿪺���
* ������LED�ţ���led.h�к궨��ΪLEDS1��LEDS2
* ���أ�0����LED�ɹ���-1����������
* �޸ģ�
* ע�ͣ�����ֻ������LEDS1��LEDS2,����᷵��-1
*********************************************************************************************/
signed char led_on(unsigned char led)
{
  if(led & 0x01){                                                //���Ҫ��LED1
    GPIOPinWrite(GPIOA0_BASE, G04_UCPINS, 0x00);
    return 0;
  }
    
  if(led & 0x02){                                                //���Ҫ��LED2
    GPIOPinWrite(GPIOA0_BASE, G05_UCPINS, 0x00);
    return 0;
  }
  
  return -1;                                                    //�������󣬷���-1
}

/*********************************************************************************************
* ���ƣ�led_off()
* ���ܣ�LED���ƹرպ���
* ������LED�ţ���led.h�к궨��ΪLEDS1��LEDS2
* ���أ�0���ر�LED�ɹ���-1����������
* �޸ģ�
* ע�ͣ�����ֻ������LEDS1��LEDS2,����᷵��-1
*********************************************************************************************/
signed char led_off(unsigned char led)
{
  if(led &0x01){                                                //���Ҫ�ر�LED1
    GPIOPinWrite(GPIOA0_BASE, G04_UCPINS, 0xff);
    return 0;
  }
    
  if(led &0x2){                                                //���Ҫ�ر�LED2
    GPIOPinWrite(GPIOA0_BASE, G05_UCPINS, 0xff);
    return 0;
  }
  
  return -1;                                                    //�������󣬷���-1
}