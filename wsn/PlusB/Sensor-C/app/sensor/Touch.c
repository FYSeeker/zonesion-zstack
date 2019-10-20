/*********************************************************************************************
* �ļ���Touch.c
* ���ߣ�Lixm 2017.10.17
* ˵����������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Touch.h"

/*********************************************************************************************
* ���ƣ�touch_init()
* ���ܣ�������������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void touch_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;                          //����һ��GPIO_InitTypeDef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(TOUCH_RCC, ENABLE);                    //����������ص�GPIO����ʱ��
  
  GPIO_InitStructure.GPIO_Pin = TOUCH_PIN;                      //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //��������ģʽΪ����ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  
  GPIO_Init(TOUCH_GPIO, &GPIO_InitStructure);                   //��ʼ��GPIO����
}

/*********************************************************************************************
* ���ƣ�unsigned char get_touch_status(void)
* ���ܣ���ȡ����������״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_touch_status(void)
{
  static unsigned char touch_status = 0;
  if(GPIO_ReadInputDataBit(TOUCH_GPIO, TOUCH_PIN)){
    if(touch_status == 0){
      touch_status = 1;
      return 1;
    }
    else
      return 0;
  }
  else{
    if(touch_status == 1){
      touch_status = 0;
      return 1;
    }
    else
      return 0;
  }
}