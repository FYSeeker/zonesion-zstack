/*********************************************************************************************
* 文件：stadiometry.c
* 作者：Lixm 2017.10.17
* 说明：红外测距驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stadiometry.h"

#include "adc.h"

/*********************************************************************************************
* 名称：stadiometry_init()
* 功能：红外测距传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void stadiometry_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef       ADC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);         //使能GPIOC时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);          //使能ADC1时钟
  
  //先初始化ADC1通道12 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                     //PC2 通道12
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                  //模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;             //不带上下拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);                        //初始化  
  
  
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	        //ADC1复位
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	        //复位结束	 
  
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;      //独立模式
  //两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  //DMA失能
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;   //预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);                     //初始化
  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;        //10位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                 //非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;           //关闭连续转换
  //禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;        //右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;                    //1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);                           //ADC初始化
  
  ADC_Cmd(ADC1, ENABLE);                                        //开启AD转换器	
}

/*********************************************************************************************
* 名称：unsigned char get_stadiometry_status(void)
* 功能：获取红外测距传感器状态
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
extern u16 AdcGet(u8 ch);
float get_stadiometry_data(void)
{
  unsigned int value = 0;
  value = AdcGet(3) / 8;
  if((value >= 86)&&(value <= 750))
      return (2547.8/((float)value*0.75-10.41)-0.42);          //获取距离
    else
      return 0;                    
}