#include "vml100.h"

_Bool vml100_RunFlag = 0;
uint8 vml100_Status=0;

/*********************************************************************************************
* UART RS485相关接口函数
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
* 名称：uart0_init()
* 功能：串口0初始化函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void node_uart_init(void)
{
    halUARTCfg_t _UartConfigure;
    
    // UART 配置信息
    _UartConfigure.configured           = TRUE;
    _UartConfigure.baudRate             = HAL_UART_BR_9600;
    _UartConfigure.flowControl          = FALSE;
    _UartConfigure.rx.maxBufSize        = 128;
    _UartConfigure.tx.maxBufSize        = 128;
    _UartConfigure.flowControlThreshold = (128 / 2);
    _UartConfigure.idleTimeout          = 6;
    _UartConfigure.intEnable            = TRUE;
    _UartConfigure.callBackFunc         = uart_callback_func;
    
    HalUARTOpen (HAL_UART_PORT_0, &_UartConfigure);               //启动UART
}

/*********************************************************************************************
* 名称：node_uart_callback()
* 功能：节点串口通讯回调函数
* 参数：port -- 输入参数，接收端口
*       event -- 输入参数，接收事件
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void uart_callback_func( uint8 port ,uint8 event)
{
#define RBUF_SIZE 32
    (void)event;
    uint8 ch;
    uint8 pbuf[16];
    static char szBuf[RBUF_SIZE];
    static uint8 rlen = 0;
    
    // 接收通过串口传来的数据
    while (Hal_UART_RxBufLen(port))
    {
        HalUARTRead (port, &ch, 1);
        if(rlen<RBUF_SIZE)
        {
            szBuf[rlen++] = ch;
            if(ch == '\n')
            {
                if(strcmp("CAR_PARK\n", szBuf) == 0)            // 告知当前有车
                {
                    strcpy((char*)pbuf,"CAR_PARK:OK\n");
                    HalUARTWrite(HAL_UART_PORT_0, pbuf, strlen((char*)pbuf));
                    vml100_Status = 1;
                }
                else if(strcmp("CAR_AWAY\n", szBuf) == 0)           // 告知当前无车
                {
                    strcpy((char*)pbuf,"CAR_AWAY:OK\n");
                    HalUARTWrite(HAL_UART_PORT_0, pbuf, strlen((char*)pbuf));
                    vml100_Status = 0;
                }
                
                else if(strcmp("MAG_OPEN:OK\n", szBuf) == 0)                 // 模块开启成功
                {
                    vml100_RunFlag = 1;
                }
                else if(strcmp("MAG_STOP:OK\n", szBuf) == 0)            // 模块中止检测成功
                {
                    vml100_RunFlag = 0;
                }
                else if((strcmp("MAG_CARS:PA\n", szBuf) == 0) && vml100_RunFlag)            // 检测到车辆
                {
                    vml100_Status = 1;
                }
                else if((strcmp("MAG_CARS:NO\n", szBuf) == 0) && vml100_RunFlag)            // 未检测到车辆
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
* 名称：vml100_init()
* 功能：传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_init(void)
{
    //CTRL_O,P0_0
    P0SEL &= ~(1<<0);               //0:通用IO
    P0DIR &= ~(1<<0);               //0:输入
    P0INP &= ~(1<<0);               //0:上下拉模式
    P2INP |= (1<<0);                //1:下拉
    
    //CTRL_I,P0_1
    P0SEL &= ~(1<<1);               //0:通用IO
    P0DIR |= (1<<1);                //1:输出
    P0INP &= ~(1<<1);               //0:上下拉模式
    P2INP |= (1<<1);                //1:下拉
    
    node_uart_init();
}

/*********************************************************************************************
* 名称：vml100_commandBegin
* 功能：发送命令开始
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_commandBegin()
{
    VML100_CTRLI_SBIT = 1;
    delay_ms(10);
}

/*********************************************************************************************
* 名称：vml100_commandEnd
* 功能：发送命令结束
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_commandEnd()
{
    delay_ms(100);
    VML100_CTRLI_SBIT = 0;
}

/*********************************************************************************************
* 名称：vml100_sendData
* 功能：发送命令
* 参数：dat:数据指针
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_sendData(char* dat)
{
    vml100_commandBegin();
    HalUARTWrite(HAL_UART_PORT_0, (uint8*)dat, strlen(dat));
    vml100_commandEnd();
}

/*********************************************************************************************
* 名称：vml100_OPEN()
* 功能：开始检测
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_OPEN(void)
{
    vml100_sendData("MAG_OPEN\n");
}

/*********************************************************************************************
* 名称：vml100_CONT()
* 功能：继续检测
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_CONT(void)
{
    vml100_sendData("MAG_CONT\n");
}

/*********************************************************************************************
* 名称：vml100_STOP()
* 功能：中止检测
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_STOP(void)
{
    vml100_sendData("MAG_STOP\n");
}

/*********************************************************************************************
* 名称：vml100_CARS()
* 功能：查询状态
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_CARS(void)
{
    vml100_sendData("MAG_CARS\n");
}

/*********************************************************************************************
* 名称：vml100_VERS()
* 功能：读取通信协议版本
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vml100_VERS(void)
{
    vml100_sendData("MAG_VERS\n");
}








