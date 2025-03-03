/*********************************************************************************************
* 文件：Infrared.h
* 作者：Lixm 2017.10.17
* 说明：触摸传感器驱动代码头文件  
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 宏条件编译
*********************************************************************************************/
#ifndef __TOUCH_H__
#define __TOUCH_H__

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <ioCC2540.h>

/*********************************************************************************************
* 内部原型函数
*********************************************************************************************/
void touch_init(void);                                          //触摸传感器初始化
unsigned char get_touch_status(void);                          //获取触摸传感器状态

#endif /*__TOUCH_H__*/

