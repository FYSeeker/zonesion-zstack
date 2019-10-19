#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "sapi.h"
#include "osal_nv.h"
#include "addrmgr.h"
#include "mt.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "hal_uart.h"
#include "RFIDdriver.h"
#include "OnBoard.h"
#include "sensor.h"
/*********************************************************************************************
*�ⲿ����
*********************************************************************************************/
extern uint8 A0[16];                                            // A0�洢����
extern uint16 f_usMyReadCardDelay;                              // ����1s�������
extern uint16 f_usMyCloseDoorDelay;                             // ���ŵ�������ʱʱ��
extern uint8 f_szReadCardCmd[];                                 // ����ָ��
extern uint8 D1;                                                // �������Դ��ʼ״̬Ϊ��


/*********************************************************************************************
* ���ƣ�UART_BuadCount
* ���ܣ����ڲ����ʼ���
* ������baud:������
* ���أ���
* �޸ģ�
* ע�ͣ����ݲ����ʼ���Ĵ���ֵ
*********************************************************************************************/
void UART_BuadCount(double* baud,char* baud_e,char* baud_m)
{
    double sys_clk_baud = 32000000.0;							//ϵͳʱ��

    /*���ݲ�����ѡ��baud_e*/
    if(*baud<4800)
    {
        *baud_e = 6;
    }
    else if((*baud>=4800)&&(*baud<9600))
    {
        *baud_e = 7;
    }
    else if((*baud>=9600)&&(*baud<19200))
    {
        *baud_e = 8;
    }
    else if((*baud>=19200)&&(*baud<38400))
    {
        *baud_e = 9;
    }
    else if((*baud>=38400)&&(*baud<76800))
    {
        *baud_e = 10;
    }
    else if((*baud>=76800)&&(*baud<230400))
    {
        *baud_e = 11;
    }
    else
    {
        *baud_e = 12;
    }

    /*����baud_m*/
    *baud_m = (char)((((*baud)*pow(2,28))/(sys_clk_baud*pow(2,*baud_e)))-256.0);
}


/*********************************************************************************************
* ���ƣ�uart0_init
* ���ܣ�uart0��ʼ�������õ�λ��1
* ������baud:������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void uart0_init(double baud)
{
    char baud_e,baud_m;

    P0SEL |=  0x0C;                 							//��ʼ��UART0�˿�
    PERCFG&= ~0x01;                 							//ѡ��UART0Ϊ��ѡλ��һ
    P0DIR &= ~(1<<2);                 							//P02,IN
    P0DIR |= (1<<3);                 							//PO3,OUT
    P2DIR &= ~0xC0;                 							//P0������Ϊ����0

    U0CSR = 0xC0;                   							//����ΪUARTģʽ,����ʹ�ܽ�����
    UART_BuadCount(&baud,&baud_e,&baud_m);						//���㲨����
    U0GCR = baud_e;
    U0BAUD = baud_m;                  							//���ò�����

    //�����ж����ȼ����
    IP0 |= (1<<2);
    IP1 |= (1<<2);

    URX0IE = 1;													//���ڽ����ж�ʹ��
    EA = 1;														//�����ж�
}


/*********************************************************************************************
* ���ƣ�uart1_init
* ���ܣ�uart1��ʼ�������õ�λ��2
* ������baud:������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void uart1_init(double baud)
{
    char baud_e,baud_m;

    /*UART1��IO��ʼ��,P16,P17*/
    P1SEL |= ((1<<6)|(1<<7));									//ѡ��IO����Ϊ����
    PERCFG |= (1<<1);											//ѡ���õ�λ��2
    P1DIR &= ~(1<<7);											//����P17Ϊ����
    P1DIR |= (1<<6);											//����P16Ϊ���

    /*UART��ʼ��*/
    U1CSR = ((1<<7)|(1<<6));									//����ΪUARTģʽ,ʹ�ܽ���
    UART_BuadCount(&baud,&baud_e,&baud_m);						//���㲨����
    U1GCR = baud_e;
    U1BAUD = baud_m;											//���ò�����

    //�����ж����ȼ����
    IP0 |= (1<<3);
    IP1 |= (1<<3);

    URX1IE = 1;													//���ڽ����ж�ʹ��
    EA = 1;														//�����ж�
}


/*********************************************************************************************
* ���ƣ�Uart1_Send_char
* ���ܣ�����1�����ֽں���
* ������ch:Ҫ���͵��ֽ�
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void Uart1_Send_char(unsigned char ch)
{
    U1DBUF = ch;
    while(UTX1IF == 0);
    UTX1IF = 0;
}


/*********************************************************************************************
* ���ƣ�Uart1_Send_String
* ���ܣ�����1�����ַ�������
* ������*Data:Ҫ�����ַ����׵�ַ
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void Uart1_Send_String(unsigned char *Data)
{
    while (*Data != '\0')
    {
        Uart1_Send_char(*Data++);
    }
}


/*********************************************************************************************
* ���ƣ�node_uart_init()
* ���ܣ�RS485ͨѶ��ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void node_uart_init(void)
{
    halUARTCfg_t _UartConfig;                                     // ��ʼ���ṹ��

    // UART����
    _UartConfig.configured           = TRUE;                      // ���ô���
    _UartConfig.baudRate             = HAL_UART_BR_19200;         // ���ò�����Ϊ19200
    _UartConfig.flowControl          = FALSE;                     // �ر�Ӳ������������
    _UartConfig.rx.maxBufSize        = 128;                       // ��������ֽ���Ϊ128
    _UartConfig.tx.maxBufSize        = 128;                       // ��������ֽ���Ϊ128
    _UartConfig.flowControlThreshold = (128 / 2);
    _UartConfig.idleTimeout          = 6;
    _UartConfig.intEnable            = TRUE;                      // ��������
    _UartConfig.callBackFunc         = uart_callback_func;        // ���ô������ݻص�����

    // ���� UART
    HalUARTOpen (HAL_UART_PORT_0, &_UartConfig);
    U0UCR = 0x38| U0UCR;                                          // żУ��

    uart1_init(115200);
}

/*********************************************************************************************
* ���ƣ�uart_485_write()
* ���ܣ�RS485����д����
* ������*buf -- ���룬���ݻ�����
*       len -- ���룬���ݳ���
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void uart_485_write(uint8 *buf, uint16 len)
{
    HalUARTWrite(HAL_UART_PORT_0, buf, len);
}

/*********************************************************************************************
* ���ƣ�uart_callback_func()
* ���ܣ�RS485ͨѶ�ص�����
* ������port -- ���룬�˿ں�
*       event -- ���룬�¼����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void uart_callback_func(uint8 port, uint8 event)
{
    if (event & HAL_UART_TX_EMPTY)
    {
        uint16 szDelay[] = {2000,1000,500,100,10,10000};
        MicroWait(szDelay[HAL_UART_BR_19200]);                      // ������Խ����ʱʱ����Խ��,��ʱ̫����Ӱ�����ݽ���
        P2_0 = 0;
    }
    node_uart_callback(port, event);
}

/*********************************************************************************************
* ���ƣ�node_uart_callback()
* ���ܣ�RS485ͨѶ�ص�����
* ������port -- ���룬�˿ں�
*       event -- ���룬�¼����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void node_uart_callback( uint8 port, uint8 event )
{
#define RBUFSIZE 32
    (void)event;                                                  // �¼���δ����
    uint8  ch;
//    static uint8 rbuf[RBUFSIZE] = {0x0A,0x41,0x31,0x46,0x30,0x00}; // �������ݴ洢����
//    static uint8  rlen = 5;
//    static uint8 state = 0;
//    char *chk_sum;
//    while (Hal_UART_RxBufLen(port))                               // ��ȡ���ڽ��յ��Ĳ���
//    {
//        HalUARTRead (port, &ch, 1);                                 // ��ȡ����
//        Uart1_Send_char(ch);
//        switch(state)
//        {
//        case 0:
//            if(ch == 0x0A)
//                state = 1;
//            else state = 0;
//            break;
//        case 1:
//            if(ch == 0x41)
//                state = 2;
//            else state = 0;
//            break;
//        case 2:
//            if(ch == 0x31)
//                state = 3;
//            else state = 0;
//            break;
//        case 3:
//            if(ch == 0x46)
//                state = 4;
//            else state = 0;
//            break;
//        case 4:
//            if(ch == 0x30)
//                state = 5;
//            else state = 0;
//            break;
//        case 5:
//            rbuf[rlen++]  = ch;
//            if(rlen == 16)
//                state = 6;
//            break;
//        case 6:
//            int x = xor_check(rbuf, 13);
//            chk_sum=ASCI_16(x);
//
//            if ((chk_sum[0]==rbuf[13])&&(chk_sum[1]==rbuf[14]))
//            {
//                char szData[32];
//                rbuf[13]='\0';
//                int len;
//                uint8 buf[8] = {0};
//                for(int x=0; x<=8; x+=2)
//                {
//                    buf[x] = rbuf[(8-x)+3];
//                    buf[(x+1)] = rbuf[(8-x)+4];
//                }
//                for(int y=0; y<8; y++)
//                {
//                    rbuf[y+5] = buf[y];
//                }
//                //�򴮿ڴ�ӡ����
//                sprintf(A0, "%s", &rbuf[5]);
//                //��szData��{A0=%02X%02X%02X%02X}��ʽд���ȡ��IDֵ
//                len = sprintf(szData, "{A0=%s}", &rbuf[5]);
//                HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
//                HalLedSet( HAL_LED_1, HAL_LED_MODE_BLINK );             // LED����˸һ��
//                uint16 cmd = 0;
//                //ͨ��zb_SendDataRequest�����ݷ���Э����
//                zb_SendDataRequest( 0, cmd, len, (uint8 *)szData, 0,
//                                    AF_ACK_REQUEST,
//                                    AF_DEFAULT_RADIUS );
//                rlen = 5;
//            }
//            else state = 0;
//            break;
//        default:
//            state = 0;
//            break;
//        }
//    }
    
    
    while (Hal_UART_RxBufLen(port))                               // ��ȡ���ڽ��յ��Ĳ���
    {
        HalUARTRead (port, &ch, 1);                                 // ��ȡ����
        Uart1_Send_char(ch);
    }
}

/*********************************************************************************************
* ���ƣ�xor_check()
* ���ܣ�ͨ�����У��
* ������buf --- ���룬У����ʼָ��
*       len --- ���룬У�����鳤��
* ���أ�xor --- У���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static uint8 xor_check(uint8* buf, uint8 len)
{
    uint8 xor = 0;
    for (int k = 0; k < len; k++)
    {
        xor = xor ^ buf[k];                                         // ѭ������ȡУ��ֵ
    }

    return xor;                                                   // ���ؽ��
}

/*********************************************************************************************
�������ƣ�ASCI_16
��    �ܣ���ASCII��ת����16����
��    ����aҪת����ֵ
����ֵ��bΪʮ����������
*********************************************************************************************/
char* ASCI_16(int c)
{
    static unsigned char b[2];
    unsigned char a[2]= {0};
    a[0] |=(c>>4);
    a[1] |=(c&0x0f);
    if(a[0]<=0x09)
        b[0]=a[0]+0x30;
    else if(a[0]>=0x0A&&a[0]<=0x0F)
        b[0]=a[0]+0x37;
    if(a[1]<=0x09)
        b[1]=a[1]+0x30;
    else if(a[1]>=0x0A&&a[1]<=0x0F)
        b[1]=a[1]+0x37;
    return (char*)b;
}
