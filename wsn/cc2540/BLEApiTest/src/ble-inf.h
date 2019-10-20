#ifndef __BLE_INF_H__
#define __BLE_INF_H__

void bleRecv(char *buf, int len);
void bleSend(char *buf, int len);
int bleOnRead(char *buf, int len);
int GetLinkStatus(void);

#endif
