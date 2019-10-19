#include "vml100.h"

_Bool vml100_RunFlag = 0;
uint8 vml100_Status=0;

/*********************************************************************************************
* UART RS485��ؽӿں���
*********************************************************************************************/
static void node_uart_init(void);
static void uart_callback_func ( uint8 port, uint8 event );

void delay_ms(unsigned short times)
{
    for(unsigned short i=0; i<times; i++)
        for(unsigned char j=0; j<100; j++); 
}

uint8 vml100_StatusGet()
{
    return vml100_Status;
}

/*********************************************************************************************
* ���ƣ�uart0_init()
* ���ܣ�����0��ʼ������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void node_uart_init(void)
{
    halUARTCfg_t _UartConfigure;
    
    // UART ������Ϣ
    _UartConfigure.configured           = TRUE;
    _UartConfigure.baudRate             = HAL_UART_BR_9600;
    _UartConfigure.flowControl          = FALSE;
    _UartConfigure.rx.maxBufSize        = 128;
    _UartConfigure.tx.maxBufSize        = 128;
    _UartConfigure.flowControlThreshold = (128 / 2);
    _UartConfigure.idleTimeout          = 6;
    _UartConfigure.intEnable            = TRUE;
    _UartConfigure.callBackFunc         = uart_callback_func;
    
    HalUARTOpen (HAL_UART_PORT_0, &_UartConfigure);               //����UART
}

/*********************************************************************************************
* ���ƣ�node_uart_callback()
* ���ܣ��ڵ㴮��ͨѶ�ص�����
* ������port -- ������������ն˿�
*       event -- ��������������¼�
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void uart_callback_func( uint8 port ,uint8 event)
{
#define RBUF_SIZE 32
    (void)event;
    uint8 ch;
    uint8 pbuf[16];
    static char szBuf[RBUF_SIZE];
    static uint8 rlen = 0;
    
    // ����ͨ�����ڴ���������
    while (Hal_UART_RxBufLen(port))
    {
        HalUARTRead (port, &ch, 1);
        if(rlen<RBUF_SIZE)
        {
            szBuf[rlen++] = ch;
            if(ch == '\n')
            {
                if(strcmp("CAR_PARK\n", szBuf) == 0)            // ��֪��ǰ�г�
                {
                    strcpy((char*)pbuf,"CAR_PARK:OK\n");
                    HalUARTWrite(HAL_UART_PORT_0, pbuf, strlen((char*)pbuf));
                    vml100_Status = 1;
                }
                else if(strcmp("CAR_AWAY\n", szBuf) == 0)           // ��֪��ǰ�޳�
                {
                    strcpy((char*)pbuf,"CAR_AWAY:OK\n");
                    HalUARTWrite(HAL_UART_PORT_0, pbuf, strlen((char*)pbuf));
                    vml100_Status = 0;
                }
                
                else if(strcmp("MAG_OPEN:OK\n", szBuf) == 0)                 // ģ�鿪���ɹ�
                {
                    vml100_RunFlag = 1;
                }
                else if(strcmp("MAG_STOP:OK\n", szBuf) == 0)            // ģ����ֹ���ɹ�
                {
                    vml100_RunFlag = 0;
                }
                else if((strcmp("MAG_CARS:PA\n", szBuf) == 0) && vml100_RunFlag)            // ��⵽����
                {
                    vml100_Status = 1;
                }
                else if((strcmp("MAG_CARS:NO\n", szBuf) == 0) && vml100_RunFlag)            // δ��⵽����
                {
                    vml100_Status = 0;
                }
                memset(szBuf, 0, rlen);
                rlen = 0;
            }
        }
        else
            rlen=0;
    }
}

/*********************************************************************************************
* ���ƣ�vml100_init()
* ���ܣ���������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_init(void)
{
    //CTRL_O,P0_0
    P0SEL &= ~(1<<0);               //0:ͨ��IO
    P0DIR &= ~(1<<0);               //0:����
    P0INP &= ~(1<<0);               //0:������ģʽ
    P2INP |= (1<<0);                //1:����
    
    //CTRL_I,P0_1
    P0SEL &= ~(1<<1);               //0:ͨ��IO
    P0DIR |= (1<<1);                //1:���
    P0INP &= ~(1<<1);               //0:������ģʽ
    P2INP |= (1<<1);                //1:����
    
    node_uart_init();
}

/*********************************************************************************************
* ���ƣ�vml100_commandBegin
* ���ܣ��������ʼ
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_commandBegin()
{
    VML100_CTRLI_SBIT = 1;
    delay_ms(10);
}

/*********************************************************************************************
* ���ƣ�vml100_commandEnd
* ���ܣ������������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_commandEnd()
{
    delay_ms(100);
    VML100_CTRLI_SBIT = 0;
}

/*********************************************************************************************
* ���ƣ�vml100_sendData
* ���ܣ���������
* ������dat:����ָ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_sendData(char* dat)
{
    vml100_commandBegin();
    HalUARTWrite(HAL_UART_PORT_0, (uint8*)dat, strlen(dat));
    vml100_commandEnd();
}

/*********************************************************************************************
* ���ƣ�vml100_OPEN()
* ���ܣ���ʼ���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_OPEN(void)
{
    vml100_sendData("MAG_OPEN\n");
}

/*********************************************************************************************
* ���ƣ�vml100_CONT()
* ���ܣ��������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_CONT(void)
{
    vml100_sendData("MAG_CONT\n");
}

/*********************************************************************************************
* ���ƣ�vml100_STOP()
* ���ܣ���ֹ���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_STOP(void)
{
    vml100_sendData("MAG_STOP\n");
}

/*********************************************************************************************
* ���ƣ�vml100_CARS()
* ���ܣ���ѯ״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_CARS(void)
{
    vml100_sendData("MAG_CARS\n");
}

/*********************************************************************************************
* ���ƣ�vml100_VERS()
* ���ܣ���ȡͨ��Э��汾
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vml100_VERS(void)
{
    vml100_sendData("MAG_VERS\n");
}








