/*********************************************************************************************
* 文件：fan.h
* 作者：Lixm 2017.10.17
* 说明：风扇驱动代码头文件  
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __FAN_H__
#define __FAN_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <ioCC2540.h>

#define FANIO                   P0_3                            //定义风扇控制引脚

#define FAN_ON                  1
#define FAN_OFF                 0

/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void fan_init(void);                                            //继电器初始化

#endif /*__FAN_H__*/

