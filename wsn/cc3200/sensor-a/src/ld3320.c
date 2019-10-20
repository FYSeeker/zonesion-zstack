/*********************************************************************************************
* �ļ���ld3320.c
* ���ߣ�zonesion
* ˵����ld3320��������
* �޸ģ�Chenkm 2017.01.04 ������ע��
* ע�ͣ�
*********************************************************************************************/
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "ld3320.h"
#include "string.h"
#include "iic.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "prcm.h"
#include "pin.h"
#include "uart.h"
#include "rom.h"
#include "rom_map.h"

#include "uart_if.h"
#include "common.h"

void uart1_handle(void);

static unsigned char rx_buf = 0;
char* cmd[13] = {"yu liu zhi ling",                             //Ԥ��ָ��
                 "da kai tai deng",                             //��̨��
                 "guan bi tai deng",                            //�ر�̨��
                 "da kai chuang lian",                          //�򿪴���
                 "guan bi chuang lian",                         //�رմ���
                 "da kai feng shan",                            //�򿪷���
                 "guan bi feng shan",                           //�رշ���
                 "da kai kong tiao",                            //�򿪿յ�
                 "guan bi kong tiao",                           //�رտյ�
                 "da kai jia shi qi",                           //�򿪼�ʪ��
                 "guan bi jia shi qi",                          //�رռ�ʪ��
                 "zi dong mo shi",                              //�Զ�ģʽ
                 "shou dong mo shi",                            //�ֶ�ģʽ
                 //"que ren",                                     //ȷ��ָ��
};
/*********************************************************************************************
* ���ƣ�uart1_init(void)
* ���ܣ�����1��ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void uart1_init(void)
{
  MAP_PRCMPeripheralClkEnable(PRCM_UARTA1, PRCM_RUN_MODE_CLK);
  
  MAP_PinTypeUART(PIN_01, PIN_MODE_7);
  MAP_PinTypeUART(PIN_02, PIN_MODE_7);
  MAP_UARTConfigSetExpClk(UARTA1_BASE,MAP_PRCMPeripheralClockGet(PRCM_UARTA1), 
                          9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));
  UARTIntRegister(UARTA1_BASE,uart1_handle);
  UARTIntEnable(UARTA1_BASE,UART_INT_RX);
  UARTFIFODisable(UARTA1_BASE);
}

/*********************************************************************************************
* ���ƣ�uart_send_char()
* ���ܣ����ڷ����ֽں���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void uart_send_char(unsigned char ch)
{
  MAP_UARTCharPut(UARTA1_BASE,ch);
}

/*********************************************************************************************
* ���ƣ�ld3320_init()
* ���ܣ�ld3320��ʼ��
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void ld3320_init()
{
  char i = 0;
  uart1_init(); 
  ld3320_clean();
  delay_ms(200);
  ld3320_reload();
  delay_ms(200);
  for (i=0; i<13; i++){
    ld3320_add(cmd[i]);
    delay_ms(150);
  }
  delay_ms(200);
  ld3320_reload();
}
/*********************************************************************************************
* ���ƣ�ld3320_add()
* ���ܣ����һ��ʶ�����
* ������s -- ������
* ���أ�
* �޸ģ�
* ע�ͣ����ε��������ٸ�0.1S,�ҵ���ld3320_reload���������Ż���Ч�������쳣ʱģ���̵ƻ���˸
*********************************************************************************************/
void ld3320_add(char *s)
{
  int i;
  int len = strlen(s);
  uart_send_char('{');
  uart_send_char('a');
  uart_send_char('0');
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
  }
  uart_send_char('}');
}

/*********************************************************************************************
* ���ƣ�ld3320_addrs()
* ���ܣ����һ��ʶ����䲢�����ַ���
* ������s -- ������ r -- �������
* ���أ�
* �޸ģ�
* ע�ͣ����ε��������ٸ�0.1S,�ҵ���ld3320_reload���������Ż���Ч�������쳣ʱģ���̵ƻ���˸
*********************************************************************************************/
void ld3320_addrs(char *s,char *r)
{
  int i;
  int len = strlen(s);
  uart_send_char('{');
  uart_send_char('a');
  uart_send_char('0');
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
  }
  uart_send_char('|');
  uart_send_char('s');
  uart_send_char('0');
  len = strlen(r);
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
  }
  uart_send_char('}');
}

/*********************************************************************************************
* ���ƣ�ld3320_addrx()
* ���ܣ����һ��ʶ����䲢����һ�ֽ�16������
* ������s -- ������ x -- ������
* ���أ�
* �޸ģ�
* ע�ͣ����ε��������ٸ�0.1S,�ҵ���ld3320_reload���������Ż���Ч�������쳣ʱģ���̵ƻ���˸
*********************************************************************************************/
void ld3320_addrx(char *s,unsigned char x)
{
  int i;
  int len = strlen(s);
  uart_send_char('{');
  uart_send_char('a');
  uart_send_char('0');
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
  }
  uart_send_char('|');
  uart_send_char('x');
  uart_send_char('0');
  uart_send_char(x);
  uart_send_char('}');
}

/*********************************************************************************************
* ���ƣ�ld3320_clean()
* ���ܣ������������б�
* ������
* ���أ�
* �޸ģ�
* ע�ͣ��������б��ģ���̵ƻ���˸
*********************************************************************************************/
void ld3320_clean(void)
{
  uart_send_char('{');
  uart_send_char('c');
  uart_send_char('0');
  uart_send_char('}');
}

/*********************************************************************************************
* ���ƣ�ld3320_reload()
* ���ܣ����¼�������б�
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�������������¼�������б�
*********************************************************************************************/
void ld3320_reload(void)
{
  uart_send_char('{');
  uart_send_char('l');
  uart_send_char('0');
  uart_send_char('}');
}

/*********************************************************************************************
* ���ƣ�ld3320_debug()
* ���ܣ�����/�رյ���ģʽ
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�������������¼�������б�
*********************************************************************************************/
void ld3320_debug(unsigned char cmd)
{
  uart_send_char('{');
  uart_send_char('d');
  if(cmd == 1)
    uart_send_char('1');
  else
    uart_send_char('0');
  uart_send_char('}');
}
/*********************************************************************************************
* ���ƣ�ld3320_read
* ���ܣ�
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char ld3320_read(void)
{
  char x = rx_buf;
  rx_buf = 0;
  return x;
}
void uart1_handle(void)
{
  if(UART_INT_RX == UARTIntStatus(UARTA1_BASE,true))
  { 
    rx_buf = UARTCharGet(UARTA1_BASE);
    UARTIntClear(UARTA1_BASE,UART_INT_RX);
  }
}