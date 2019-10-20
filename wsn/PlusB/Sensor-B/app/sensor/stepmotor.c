/*********************************************************************************************
* �ļ���stepmotor.c
* ���ߣ�Lixm 2017.10.17
* ˵�������������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stepmotor.h"

/*********************************************************************************************
* ���ƣ�stepmotor_init()
* ���ܣ����������������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void stepmotor_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;                          //����һ��GPIO_InitTypeDef���͵Ľṹ��
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;               //��ʱ������
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  RCC_AHB1PeriphClockCmd(STEP_RCC, ENABLE);                     //�������������ص�GPIO����ʱ��
  RCC_AHB1PeriphClockCmd(DIR_RCC, ENABLE);                      //�������������ص�GPIO����ʱ��
  RCC_AHB1PeriphClockCmd(EN_RCC, ENABLE);                       //�������������ص�GPIO����ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  GPIO_PinAFConfig(STEP_GPIO,GPIO_PinSource8,GPIO_AF_TIM4);     //PB8����Ϊ��ʱ��2
  
  GPIO_InitStructure.GPIO_Pin = DIR_PIN;                        //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //��������ģʽΪ���ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  GPIO_Init(DIR_GPIO, &GPIO_InitStructure);                     //��ʼ��GPIO����
  
  GPIO_InitStructure.GPIO_Pin = EN_PIN;                         //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_Init(EN_GPIO, &GPIO_InitStructure);                      //��ʼ��GPIO����
  
  GPIO_SetBits(EN_GPIO,EN_PIN);

  GPIO_InitStructure.GPIO_Pin = STEP_PIN;                       //PB8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //�ٶ�100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //����
  GPIO_Init(STEP_GPIO,&GPIO_InitStructure);                     //��ʼ��PB8

  TIM_TimeBaseStructure.TIM_Period = 999;                       //��������װֵ
  TIM_TimeBaseStructure.TIM_Prescaler = 83;                     //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //ʱ�ӷָ�
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //����ģʽ
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);               //���������ó�ʼ��TIM4
  //��ʼ��TIM4 Channel3 PWMģʽ

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //�������:TIM����Ƚϼ��Ե�
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);                      //����Tָ���Ĳ�����ʼ������TIM4 3OC

  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR3�ϵ�Ԥװ�ؼĴ���

  TIM_ARRPreloadConfig(TIM4,ENABLE);                            //ARPEʹ��
  TIM_Cmd(TIM4, ENABLE);                                        //ʹ��TIM4
  TIM_SetCompare3(TIM4,0);
}

/*********************************************************************************************
* ���ƣ�void stepmotor_control(unsigned char cmd)
* ���ܣ����������������
* ��������������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void stepmotor_control(unsigned char cmd)
{ 
  if(cmd & 0x01){
    GPIO_ResetBits(EN_GPIO,EN_PIN);
    if(cmd & 0x02){
      GPIO_SetBits(DIR_GPIO,DIR_PIN);
      TIM_SetCompare3(TIM4,500);
    }
    else{
      GPIO_ResetBits(DIR_GPIO,DIR_PIN);
      TIM_SetCompare3(TIM4,500);
    }
  }
  else
    GPIO_SetBits(EN_GPIO,EN_PIN);
}