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
#define t4ms            UTUtilsGetSysTime
#define t4exp(x)        ((((signed long)UTUtilsGetSysTime()-(signed long)(x))>=0)?1:0)
#define clock_time      UTUtilsGetSysTime

#define NODE_NAME "611"
#define NODE_TYPE 3
/*********************************************************************************************
* ����ԭ��
*********************************************************************************************/
extern void sensorInit(void);
extern void sensorLinkOn(void);
extern void sensorUpdate(void);
extern void sensorCheck(void);
extern void sensorControl(unsigned char cmd);
extern void sensorLoop(void);

#endif // SENSOR_H