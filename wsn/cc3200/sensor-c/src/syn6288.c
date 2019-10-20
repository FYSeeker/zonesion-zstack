/*********************************************************************************************
* �ļ���syn6288.c
* ���ߣ�zonesion
* ˵����syn6288��������
* �޸ģ�Chenkm 2017.01.04 ������ע��
* ע�ͣ�
*********************************************************************************************/
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "syn6288.h"
#include "string.h"

#define UART_IF_BUFFER           64
/*********************************************************************************************
* ���ƣ�uart0_init(unsigned char StopBits,unsigned char Parity)
* ���ܣ�����0��ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void uart1_init()
{
  
  MAP_PRCMPeripheralClkEnable(PRCM_UARTA1,PRCM_RUN_MODE_CLK);
  MAP_PinTypeUART(PIN_01, PIN_MODE_7);
  MAP_PinTypeUART(PIN_02, PIN_MODE_7);
  
  MAP_UARTConfigSetExpClk(CONSOLE1,MAP_PRCMPeripheralClockGet(CONSOLE_PERIPH1), 
                          9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                           UART_CONFIG_PAR_NONE));
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
  MAP_UARTCharPut(CONSOLE1,ch);

}

/*********************************************************************************************
* ���ƣ�syn6288_init()
* ���ܣ�syn6288��ʼ��
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void syn6288_init()
{
  uart1_init(); 
 PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK); //ʹ��ʱ��
  PinTypeGPIO(PIN_58,PIN_MODE_0,false);                    //ѡ������ΪGPIOģʽ��gpio16��
  GPIODirModeSet(GPIOA0_BASE, G03_UCPINS, GPIO_DIR_MODE_IN);   //����GPIO16Ϊ����ģʽ
  PinConfigSet(PIN_58,PIN_TYPE_STD_PU,PIN_MODE_0);         //����
}

int syn6288_busy(void)
{ 
  if((unsigned char)GPIOPinRead(GPIOA0_BASE,G03_UCPINS) > 0)                                                      //æ�������
    return 0;                                                   //û�м�⵽�źŷ��� 0
  else
    return 1;                                                   //��⵽�źŷ��� 1
}
/*********************************************************************************************
* ���ƣ�syn6288_play()
* ���ܣ�
* ������s -- ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void syn6288_play(char *s)
{
  int i;
  int len = strlen(s);
  unsigned char c = 0;  
  unsigned char head[] = {0xFD,0x00,0x00,0x01,0x00};            //���ݰ�ͷ  
  head[1] = (len+3)>>8;
  head[2] = (len+3)&0xff;
  for (i=0; i<5; i++){
    uart_send_char(head[i]);
    c ^= head[i];
  }
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
    c ^= s[i];
  }
  uart_send_char(0xA5);
}

/*******************************************************************************
* ����: hex2unicode()
* ����: ��16�����ַ���ת����bin��ʽ
* ����: 
* ����: 
* �޸�:
* ע��: 
*******************************************************************************/
char *hex2unicode(char *str)
{       
  static char uni[64];
  int n = strlen(str)/2;
  if (n > 64) n = 64;
  
  for (int i=0; i<n; i++) {
    unsigned int x = 0;
    for (int j=0; j<2; j++) {
      char c = str[2*i+j];
      char o;
      if (c>='0' && c<='9') o = c - '0';
      else if (c>='A' && c<='F') o = 10+(c-'A');
      else if (c>='a' && c<='f') o = 10+(c-'a');
      else o = 0;
      x = (x<<4) | (o&0x0f);
    }
    uni[i] = x;
  }
  uni[n] = 0;
  return uni;
}


/*******************************************************************************
* syn6288_play_unicode()
* ���ܣ�
* ������s -- ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*******************************************************************************/
void syn6288_play_unicode(char *s, int len)
{
  int i;
  char c = 0;  
  unsigned char head[] = {0xFD,0x00,0x00,0x01,0x03};            //���ݰ�ͷ  
  head[1] = (len+3)>>8;
  head[2] = (len+3)&0xff;
  for (i=0; i<5; i++){
    uart_send_char(head[i]);
    c ^= head[i];
  }
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
    c ^= s[i];
  }
  uart_send_char(c);
}
