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

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define ELECLOCK_SEL            P0SEL
#define ELECLOCK_DIR            P0DIR

#define ELECLOCK_SBIT           P0_6
#define ELECLOCK_BV             BV(6)

#define MY_REPORT_EVT           0x0001
#define MY_CHECK_EVT           	0x0002
#define MY_READCARD_EVT         0x0004                          //RFID读取事件
#define MY_CLOSEDOOR_EVT        0X0008                          //电磁锁自关闭事件
#define NODE_NAME               "802"                           // 节点类型
#define NODE_CATEGORY           1
#define NODE_TYPE               NODE_ROUTER                     // 路由节点：NODE_ROUTER  终端节点：NODE_ENDDEVICE
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