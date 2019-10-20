/*********************************************************************************************
* 文件：Flame.c
* 作者：Lixm 2017.10.17
* 说明：火焰驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Flame.h"

/*********************************************************************************************
* 名称：flame_init()
* 功能：火焰传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void flame_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;                          //定义一个GPIO_InitTypeDef类型的结构体
  RCC_AHB1PeriphClockCmd(FLAME_RCC, ENABLE);                    //开启火焰相关的GPIO外设时钟
  
  GPIO_InitStructure.GPIO_Pin = FLAME_PIN;                      //选择要控制的GPIO引脚
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //设置引脚模式为输入模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //设置引脚为下拉模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //设置引脚速率为2MHz
  
  GPIO_Init(FLAME_GPIO, &GPIO_InitStructure);                   //初始化GPIO配置
}

/*********************************************************************************************
* 名称：unsigned char get_flame_status(void)
* 功能：获取火焰传感器状态
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_flame_status(void)
{ 
  if(GPIO_ReadInputDataBit(FLAME_GPIO, FLAME_PIN))
    return 1;
  else
    return 0;
}