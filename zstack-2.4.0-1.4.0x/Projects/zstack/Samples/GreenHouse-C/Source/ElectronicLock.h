#ifndef _electronic_lock_
#define _electronic_lock_

#include <ioCC2530.h>                                           //����CC2530����Ӧ��ͷ�ļ���������SFR�Ķ��壩

#define ElectronicLockIO  P1_2

void ElectronicLock_init(void);
void ElectronicLock_on(void);
void ElectronicLock_off(void);

#endif