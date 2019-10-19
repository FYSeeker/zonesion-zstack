/*********************************************************************************************
* �ļ���sensor.h
* ���ߣ�Xuzhy 2018.5.16
* ˵����sensorͷ�ļ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef SENSOR_H
#define SENSOR_H
/*********************************************************************************************
* �궨��
*********************************************************************************************/

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define ELECLOCK_SEL            P0SEL
#define ELECLOCK_DIR            P0DIR

#define ELECLOCK_SBIT           P0_6
#define ELECLOCK_BV             BV(6)

#define MY_REPORT_EVT           0x0001
#define MY_CHECK_EVT           	0x0002
#define MY_READCARD_EVT         0x0004                          //RFID��ȡ�¼�
#define MY_CLOSEDOOR_EVT        0X0008                          //������Թر��¼�
#define NODE_NAME               "802"                           // �ڵ�����
#define NODE_CATEGORY           1
#define NODE_TYPE               NODE_ROUTER                     // ·�ɽڵ㣺NODE_ROUTER  �ն˽ڵ㣺NODE_ENDDEVICE
/*********************************************************************************************
* ����ԭ��
*********************************************************************************************/
extern void sensorInit(void);
extern void sensorLinkOn(void);
extern void sensorUpdate(void);
extern void sensorCheck(void);
extern void sensorControl(uint8 cmd);
extern void MyEventProcess( uint16 event );

#endif // SENSOR_H