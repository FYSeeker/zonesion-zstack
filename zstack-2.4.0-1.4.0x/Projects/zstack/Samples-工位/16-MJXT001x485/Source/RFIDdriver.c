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
*外部变量
*********************************************************************************************/
extern uint8 A0[16];                                            // A0存储卡号
extern uint16 f_usMyReadCardDelay;                              // 设置1s读卡间隔
extern uint16 f_usMyCloseDoorDelay;                             // 开门到关门延时时间
extern uint8 f_szReadCardCmd[];                                 // 读卡指令
extern uint8 D1;                                                // 电磁锁电源初始状态为关


/*********************************************************************************************
* 名称：UART_BuadCount
* 功能：串口波特率计算
* 参数：baud:波特率
* 返回：无
* 修改：
* 注释：根据波特率计算寄存器值
*********************************************************************************************/
void UART_BuadCount(double* baud,char* baud_e,char* baud_m)
{
    double sys_clk_baud = 32000000.0;							//系统时钟

    /*根据波特率选择baud_e*/
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

    /*计算baud_m*/
    *baud_m = (char)((((*baud)*pow(2,28))/(sys_clk_baud*pow(2,*baud_e)))-256.0);
}


/*********************************************************************************************
* 名称：uart0_init
* 功能：uart0初始化，复用到位置1
* 参数：baud:波特率
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void uart0_init(double baud)
{
    char baud_e,baud_m;

    P0SEL |=  0x0C;                 							//初始化UART0端口
    PERCFG&= ~0x01;                 							//选择UART0为可选位置一
    P0DIR &= ~(1<<2);                 							//P02,IN
    P0DIR |= (1<<3);                 							//PO3,OUT
    P2DIR &= ~0xC0;                 							//P0优先作为串口0

    U0CSR = 0xC0;                   							//设置为UART模式,而且使能接受器
    UART_BuadCount(&baud,&baud_e,&baud_m);						//计算波特率
    U0GCR = baud_e;
    U0BAUD = baud_m;                  							//设置波特率

    //设置中断优先级最高
    IP0 |= (1<<2);
    IP1 |= (1<<2);

    URX0IE = 1;													//串口接收中断使能
    EA = 1;														//开总中断
}


/*********************************************************************************************
* 名称：uart1_init
* 功能：uart1初始化，复用到位置2
* 参数：baud:波特率
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void uart1_init(double baud)
{
    char baud_e,baud_m;

    /*UART1，IO初始化,P16,P17*/
    P1SEL |= ((1<<6)|(1<<7));									//选择IO功能为外设
    PERCFG |= (1<<1);											//选择复用到位置2
    P1DIR &= ~(1<<7);											//设置P17为输入
    P1DIR |= (1<<6);											//设置P16为输出

    /*UART初始化*/
    U1CSR = ((1<<7)|(1<<6));									//设置为UART模式,使能接收
    UART_BuadCount(&baud,&baud_e,&baud_m);						//计算波特率
    U1GCR = baud_e;
    U1BAUD = baud_m;											//设置波特率

    //设置中断优先级最高
    IP0 |= (1<<3);
    IP1 |= (1<<3);

    URX1IE = 1;													//串口接收中断使能
    EA = 1;														//开总中断
}


/*********************************************************************************************
* 名称：Uart1_Send_char
* 功能：串口1发送字节函数
* 参数：ch:要发送的字节
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Uart1_Send_char(unsigned char ch)
{
    U1DBUF = ch;
    while(UTX1IF == 0);
    UTX1IF = 0;
}


/*********************************************************************************************
* 名称：Uart1_Send_String
* 功能：串口1发送字符串函数
* 参数：*Data:要发送字符串首地址
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Uart1_Send_String(unsigned char *Data)
{
    while (*Data != '\0')
    {
        Uart1_Send_char(*Data++);
    }
}


/*********************************************************************************************
* 名称：node_uart_init()
* 功能：RS485通讯初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void node_uart_init(void)
{
    halUARTCfg_t _UartConfig;                                     // 初始化结构体

    // UART配置
    _UartConfig.configured           = TRUE;                      // 启用串口
    _UartConfig.baudRate             = HAL_UART_BR_19200;         // 配置波特率为19200
    _UartConfig.flowControl          = FALSE;                     // 关闭硬件数据流控制
    _UartConfig.rx.maxBufSize        = 128;                       // 接收最大字节数为128
    _UartConfig.tx.maxBufSize        = 128;                       // 发送最大字节数为128
    _UartConfig.flowControlThreshold = (128 / 2);
    _UartConfig.idleTimeout          = 6;
    _UartConfig.intEnable            = TRUE;                      // 启动串口
    _UartConfig.callBackFunc         = uart_callback_func;        // 配置串口数据回调函数

    // 启动 UART
    HalUARTOpen (HAL_UART_PORT_0, &_UartConfig);
    U0UCR = 0x38| U0UCR;                                          // 偶校验

    uart1_init(115200);
}

/*********************************************************************************************
* 名称：uart_485_write()
* 功能：RS485串口写数据
* 参数：*buf -- 输入，数据缓冲区
*       len -- 输入，数据长度
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void uart_485_write(uint8 *buf, uint16 len)
{
    HalUARTWrite(HAL_UART_PORT_0, buf, len);
}

/*********************************************************************************************
* 名称：uart_callback_func()
* 功能：RS485通讯回调函数
* 参数：port -- 输入，端口号
*       event -- 输入，事件编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void uart_callback_func(uint8 port, uint8 event)
{
    if (event & HAL_UART_TX_EMPTY)
    {
        uint16 szDelay[] = {2000,1000,500,100,10,10000};
        MicroWait(szDelay[HAL_UART_BR_19200]);                      // 波特率越慢延时时间需越长,延时太长将影响数据接收
        P2_0 = 0;
    }
    node_uart_callback(port, event);
}

/*********************************************************************************************
* 名称：node_uart_callback()
* 功能：RS485通讯回调函数
* 参数：port -- 输入，端口号
*       event -- 输入，事件编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void node_uart_callback( uint8 port, uint8 event )
{
#define RBUFSIZE 32
    (void)event;                                                  // 事件暂未定义
    uint8  ch;
//    static uint8 rbuf[RBUFSIZE] = {0x0A,0x41,0x31,0x46,0x30,0x00}; // 定义数据存储数组
//    static uint8  rlen = 5;
//    static uint8 state = 0;
//    char *chk_sum;
//    while (Hal_UART_RxBufLen(port))                               // 获取串口接收到的参数
//    {
//        HalUARTRead (port, &ch, 1);                                 // 提取数据
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
//                //向串口打印数据
//                sprintf(A0, "%s", &rbuf[5]);
//                //向szData以{A0=%02X%02X%02X%02X}格式写入读取的ID值
//                len = sprintf(szData, "{A0=%s}", &rbuf[5]);
//                HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
//                HalLedSet( HAL_LED_1, HAL_LED_MODE_BLINK );             // LED灯闪烁一次
//                uint16 cmd = 0;
//                //通过zb_SendDataRequest将数据发送协调器
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
    
    
    while (Hal_UART_RxBufLen(port))                               // 获取串口接收到的参数
    {
        HalUARTRead (port, &ch, 1);                                 // 提取数据
        Uart1_Send_char(ch);
    }
}

/*********************************************************************************************
* 名称：xor_check()
* 功能：通用异或校验
* 参数：buf --- 输入，校验起始指针
*       len --- 输入，校验数组长度
* 返回：xor --- 校验和
* 修改：
* 注释：
*********************************************************************************************/
static uint8 xor_check(uint8* buf, uint8 len)
{
    uint8 xor = 0;
    for (int k = 0; k < len; k++)
    {
        xor = xor ^ buf[k];                                         // 循环异或获取校验值
    }

    return xor;                                                   // 返回结果
}

/*********************************************************************************************
函数名称：ASCI_16
功    能：将ASCII码转换成16进制
参    数：a要转化的值
返回值：b为十六进制数组
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
