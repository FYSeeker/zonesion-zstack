/*********************************************************************************************
* �ļ���Infrared.c
* ���ߣ�Lixm 2017.10.17
* ˵�������������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Infrared.h"

/*********************************************************************************************
* ���ƣ�infrared_init()
* ���ܣ�������⴫������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void infrared_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;                          //����һ��GPIO_InitTypeDef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(INFRARED_RCC, ENABLE);                 //�������������ص�GPIO����ʱ��
  
  GPIO_InitStructure.GPIO_Pin = INFRARED_PIN;                   //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //��������ģʽΪ����ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  
  GPIO_Init(INFRARED_GPIO, &GPIO_InitStructure);                //��ʼ��GPIO����
}

/*********************************************************************************************
* ���ƣ�unsigned char get_infrared_status(void)
* ���ܣ���ȡ������⴫����״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_infrared_status(void)
{ 
  if(GPIO_ReadInputDataBit(INFRARED_GPIO,INFRARED_PIN))
    return 1;
  else
    return 0;
}