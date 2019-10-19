#ifndef RFID
#define RFID

/*********************************************************************************************
* 内部函数原型
*********************************************************************************************/
extern void node_uart_init(void);
extern void uart_485_write(uint8 *buf, uint16 len);
extern void uart_callback_func(uint8 port, uint8 event);
extern void node_uart_callback( uint8 port, uint8 event );
extern uint8 xor_check(uint8* buf, uint8 len);
char* ASCI_16(int c);
#endif