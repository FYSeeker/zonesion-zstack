/*********************************************************************************************
* �ļ���FAN.c
* ���ߣ�Lixm 2017.10.17
* ˵����������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "FAN.h"

/*********************************************************************************************
* ���ƣ�fan_init()
* ���ܣ����ȴ�������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;             //��ʱ������
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	    //ʹ��GPIOBʱ��
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2);      //PB10����Ϊ��ʱ��2
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                  //PB10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //����
    GPIO_Init(GPIOB,&GPIO_InitStructure);                       //��ʼ��PB10
    
    TIM_TimeBaseStructure.TIM_Period = 6000;                    //��������װֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 4199;                 //Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //����ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);             //���������ó�ʼ��TIM2
    //��ʼ��TIM2 Channel3 PWMģʽ
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;           //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //�������:TIM����Ƚϼ��Ը�
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                    //����Tָ���Ĳ�����ʼ������TIM2 3OC
    
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);           //ʹ��TIM2��CCR3�ϵ�Ԥװ�ؼĴ���
    
    TIM_ARRPreloadConfig(TIM2,ENABLE);                          //ARPEʹ��
    TIM_SetCompare3(TIM2,0);
    TIM_Cmd(TIM2, ENABLE);                                      //ʹ��TIM2
}

/*********************************************************************************************
* ���ƣ�void fan_control(unsigned int cmd)
* ���ܣ����ȿ�������
* ��������������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_control(unsigned int cmd)
{ 
    TIM_SetCompare3(TIM2,cmd);
}