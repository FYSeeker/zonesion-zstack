/*********************************************************************************************
* 文件：sensor.h
* 作者：Xuzhy 2018.5.16
* 说明：sensor头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef SENSOR_H
#define SENSOR_H
/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define MY_REPORT_EVT           0x0010
#define MY_CHECK_EVT           	0x0020
#define NODE_NAME               "611"  

extern uint8 simpleBLEPeripheral_TaskID;
/*********************************************************************************************
* 函数原型
*********************************************************************************************/
extern void sensorInit(void);
extern void sensorLinkOn(void);
extern void sensorUpdate(void);
extern void sensorCheck(void);
extern void sensorControl(uint8 cmd);
extern void MyEventProcess( uint16 event );

#endif // SENSOR_H