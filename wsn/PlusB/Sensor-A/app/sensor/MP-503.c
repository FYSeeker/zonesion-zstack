/*********************************************************************************************
* �ļ���MP-503.c
* ���ߣ�Lixm 2017.10.17
* ˵��������������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "MP-503.h"

/*********************************************************************************************
* ���ƣ�airgas_init()
* ���ܣ�����������������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void airgas_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef       ADC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);         //ʹ��GPIOCʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);          //ʹ��ADC1ʱ��
  
  //�ȳ�ʼ��ADC1ͨ��11 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                     //PC1 ͨ��11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                  //ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;             //����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);                        //��ʼ��  
  
  
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	        //ADC1��λ
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	        //��λ����	 
  
  
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;      //����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;   //Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);                     //��ʼ��
  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;        //12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                 //��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;           //�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;        //�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;                    //1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);                           //ADC��ʼ��
  
  ADC_Cmd(ADC1, ENABLE);                                        //����ADת����	
}

/*********************************************************************************************
* ���ƣ�unsigned char get_airgas_status(void)
* ���ܣ���ȡ��������������״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
extern u16 AdcGet(u8 ch);
unsigned int get_airgas_data(void)
{ 
    return AdcGet(2);
}