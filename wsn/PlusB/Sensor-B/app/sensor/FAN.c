/*********************************************************************************************
* 文件：FAN.c
* 作者：Lixm 2017.10.17
* 说明：风扇驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "FAN.h"

/*********************************************************************************************
* 名称：fan_init()
* 功能：风扇传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void fan_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;             //定时器配置
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	    //使能GPIOB时钟
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2);      //PB10复用为定时器2
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                  //PB10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //上拉
    GPIO_Init(GPIOB,&GPIO_InitStructure);                       //初始化PB10
    
    TIM_TimeBaseStructure.TIM_Period = 6000;                    //计数器重装值
    TIM_TimeBaseStructure.TIM_Prescaler = 4199;                 //预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);             //按上述配置初始化TIM2
    //初始化TIM2 Channel3 PWM模式
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;           //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //输出极性:TIM输出比较极性高
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                    //根据T指定的参数初始化外设TIM2 3OC
    
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);           //使能TIM2在CCR3上的预装载寄存器
    
    TIM_ARRPreloadConfig(TIM2,ENABLE);                          //ARPE使能
    TIM_SetCompare3(TIM2,0);
    TIM_Cmd(TIM2, ENABLE);                                      //使能TIM2
}

/*********************************************************************************************
* 名称：void fan_control(unsigned int cmd)
* 功能：风扇控制驱动
* 参数：控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void fan_control(unsigned int cmd)
{ 
    TIM_SetCompare3(TIM2,cmd);
}