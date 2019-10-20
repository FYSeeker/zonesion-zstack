/*********************************************************************************************
* �ļ���stepmotor.c
* ���ߣ�zonesion 2016.4.13
* ˵���������������
*       ͨ��P0_0��P0_1�˿ڿ��Ƶ���Ĺ���
* �޸ģ�Chenkm 2017.01.10 �޸Ĵ����ʽ�����Ӵ���ע�ͺ��ļ�˵��
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stepmotor.h"
//#include "delay.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
 

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static unsigned int dir = 0;

void stepmotor_init(void)
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
  GPIOPinWrite(GPIOA1_BASE, pin_enable, 0xff);
}

/*********************************************************************************************
* ���ƣ�step(int dir,int steps)
* ���ܣ��������
* ������int dir,int steps
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void step(int dir,int steps)
{
  int i;
  if (dir) GPIOPinWrite(GPIOA0_BASE, pin_dir, 0xff);									//���������������
  else GPIOPinWrite(GPIOA0_BASE, pin_dir, 0x00);
  SysCtlDelay(5*(g_ulClockRate/3000000)) ;		        //��ʱ5us
  for (i=0; i<steps; i++){				        //���������ת
   GPIOPinWrite(GPIOA1_BASE, pin_step, 0x00);
   SysCtlDelay(80*(g_ulClockRate/3000000)) ;
   GPIOPinWrite(GPIOA1_BASE, pin_step, 0xff);
   SysCtlDelay(80*(g_ulClockRate/3000000)) ;
  }
}

/*********************************************************************************************
* ���ƣ�forward()
* ���ܣ������ת
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void forward(int data)
{
  dir = 0;
  GPIOPinWrite(GPIOA1_BASE, pin_enable, 0x00);                  //���������������
  step(dir, data);                              	        //�����������
  GPIOPinWrite(GPIOA1_BASE, pin_enable, 0xff);

}

/*********************************************************************************************
* ���ƣ�reversion()
* ���ܣ������ת
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void reversion(int data)
{
  dir = 1;														//���������������
  GPIOPinWrite(GPIOA1_BASE, pin_enable, 0x00);                  //���������������
  step(dir, data);                              	        //�����������
  GPIOPinWrite(GPIOA1_BASE, pin_enable, 0xff);
}