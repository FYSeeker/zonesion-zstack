/*********************************************************************************************
* �ļ���rgb.c
* ���ߣ�Meixin 2017.09.15
* ˵����rgb����������     
* �޸ģ�liutong 20171027 �޸���rgb�������ų�ʼ���������ơ��Ż���ע�͡�������rgb�򿪺͹رպ���
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "rgb.h"

/*********************************************************************************************
* ���ƣ�rgb_init()
* ���ܣ�rgb�������ų�ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void rgb_init(void)
{
  
  PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_04,PIN_MODE_0,0);                           //ѡ������ΪGPIOģʽ��gpio03��
  GPIODirModeSet(GPIOA1_BASE, G13_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO14Ϊ���ģʽ
  
  
  PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_58,PIN_MODE_0,0);                           //ѡ������ΪGPIOģʽ��gpio16��
  GPIODirModeSet(GPIOA0_BASE, G03_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO16Ϊ���ģʽ
  
  PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_02,PIN_MODE_0,0);                           //ѡ������ΪGPIOģʽ��gpio16��
  GPIODirModeSet(GPIOA1_BASE, G11_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO16Ϊ���ģʽ
  
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0xff);
  GPIOPinWrite(GPIOA0_BASE, G03_UCPINS, 0xff);                  //��ʼ���Ͽ��̵���
  GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0xff);                  //��ʼ���Ͽ��̵���
}
  
/*********************************************************************************************
* ���ƣ�rgb_on()
* ���ܣ�rgb���ƴ򿪺���
* ������rgb�ţ���rgb.h�к궨��ΪRGB_R��RGB_G��RGB_B
* ���أ�0����rgb�ɹ���-1����������
* �޸ģ�
* ע�ͣ�����ֻ������RGB_R��RGB_G��RGB_B����᷵��-1
*********************************************************************************************/
signed char rgb_on(unsigned char rgb)
{
  if(rgb & 0x01){                                                //���Ҫ��RGB_R
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0x00);
    return 0;
  }
    
  if(rgb & 0x02){                                                //���Ҫ��RGB_G
    GPIOPinWrite(GPIOA0_BASE, G03_UCPINS, 0x00);
    return 0;
  }
  
  if(rgb & 0x04){                                                //���Ҫ��RGB_B
    GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0x00);
    return 0;
  }
   
  return -1;                                                    //�������󣬷���-1
}

/*********************************************************************************************
* ���ƣ�rgb_off()
* ���ܣ�rgb���ƹرպ���
* ������rgb�ţ���rgb.h�к궨��ΪRGB_R��RGB_G��RGB_B
* ���أ�0���ر�rgb�ɹ���-1����������
* �޸ģ�
* ע�ͣ�����ֻ������RGB_R��RGB_G��RGB_B����᷵��-1
*********************************************************************************************/
signed char rgb_off(unsigned char rgb)
{
  if(rgb & 0x01){                                                //���Ҫ�ر�RGB_R
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0xff);
    return 0;
  }
    
  if(rgb & 0x02){                                                //���Ҫ�ر�RGB_G
    GPIOPinWrite(GPIOA0_BASE, G03_UCPINS, 0xff);
    return 0;
  }

  if(rgb & 0x04){                                                //���Ҫ�ر�RGB_B
    GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0xff);
    return 0;
  }
  
  return -1;                                                    //�������󣬷���-1
}