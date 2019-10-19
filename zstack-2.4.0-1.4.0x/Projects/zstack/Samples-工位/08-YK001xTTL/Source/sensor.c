/*********************************************************************************************
* 文件：sensor.c
* 作者：Zhoucj 2018.6.25
* 说明：360红外遥控
*       V0表示红外遥控键值
*       D1(Bit0)表示0-遥控/1-学习，OD1/CD1控制
*       默认值：V0=0,D1=0
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sapi.h"
#include "osal_nv.h"
#include "addrmgr.h"
#include "mt.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "hal_uart.h"
#include "sensor.h"
#include "zxbee.h"
#include "zxbee-inf.h"
/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define RELAY1                  P0_6                            // 定义继电器控制引脚
#define RELAY2                  P0_7                            // 定义继电器控制引脚
#define ON                      0                               // 宏定义打开状态控制为ON
#define OFF                     1                               // 宏定义关闭状态控制为OFF

#define LEARN_CMD                   0xF0                        // 学习模式        
#define EXIT_CMD                    0xF2                        // 遥控模式        
#define LEARN_SUCCESS_RET           0x00                        // 学习成功
#define LEARN_ERROR_RET             0xFF                        // 学习失败
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8 D0 = 0;                                            // 默认打开主动上报功能
static uint8 D1 = 0;                                            // 0-遥控/1-学习
static uint8 V0 = 30;                                           // V0表示红外遥控键值
static uint8 f_ucIrStatus = 0;                                  // 控制状态
static uint8 f_ucCmd = 0x00;
/*********************************************************************************************
* 函数声明
*********************************************************************************************/
static void node_uart_init(void);
static void ir360_send(void);
static void ir360_learn(void);
static void node_uart_callback(uint8 port, uint8 event);

/*********************************************************************************************
* 名称：updateV0()
* 功能：更新V0的值
* 参数：*val -- 待更新的变量
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateV0(char *val)
{
    V0 = atoi(val) + 63;                                        // 计算键值
    if(D1 == 1 && f_ucIrStatus==0)                              // 如果为学习模式
    {
        osal_start_timerEx(sapi_TaskID, IR_LEARN_EVT, (uint16)(1 * 1000));
    }
    else
    {
        if (f_ucIrStatus == 0)
        {
            ir360_send();                                       // 否则发送键值
        }
    }
}
/*********************************************************************************************
* 名称：updateA0()
* 功能：更新A0的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA0(void)
{

}
/*********************************************************************************************
* 名称：updateA1()
* 功能：更新A1的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA1(void)
{

}
/*********************************************************************************************
* 名称：sensorInit()
* 功能：传感器硬件初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorInit(void)
{
    node_uart_init();                                           //串口初始化

    // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
}
/*********************************************************************************************
* 名称：sensorLinkOn()
* 功能：传感器节点入网成功调用函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorLinkOn(void)
{
    sensorUpdate();
}
/*********************************************************************************************
* 名称：sensorUpdate()
* 功能：处理主动上报的数据
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorUpdate(void)
{
    char pData[16];
    char *p = pData;

    ZXBeeBegin();                                               // 智云数据帧格式包头

    // 根据D0的位状态判定需要主动上报的数值
    sprintf(p, "%u", D1);                                       // 上报控制编码
    ZXBeeAdd("D1", p);

    p = ZXBeeEnd();                                             // 智云数据帧格式包尾
    if (p != NULL)
    {
        ZXBeeInfSend(p, strlen(p));	                            // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
    }
}
/*********************************************************************************************
* 名称：sensorCheck()
* 功能：传感器监测
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorCheck(void)
{

}
/*********************************************************************************************
* 名称：sensorControl()
* 功能：传感器控制
* 参数：cmd - 控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorControl(uint8 cmd)
{
    // 根据cmd参数处理对应的控制程序
    if(cmd & 0x01)
    {
        RELAY1 = ON;                                                // 开启继电器1
    }
    else
    {
        RELAY1 = OFF;                                               // 关闭继电器1
    }
    if(cmd & 0x02)
    {
        RELAY2 = ON;                                                // 开启继电器2
    }
    else
    {
        RELAY2 = OFF;                                               // 关闭继电器2
    }
}
/*********************************************************************************************
* 名称：ZXBeeUserProcess()
* 功能：解析收到的控制命令
* 参数：*ptag -- 控制命令名称
*       *pval -- 控制命令参数
* 返回：ret -- pout字符串长度
* 修改：
* 注释：
*********************************************************************************************/
int ZXBeeUserProcess(char *ptag, char *pval)
{
    int val;
    int ret = 0;
    char pData[16];
    char *p = pData;

    // 将字符串变量pval解析转换为整型变量赋值
    val = atoi(pval);
    // 控制命令解析
    if (0 == strcmp("CD0", ptag))                                 // 对D0的位进行操作，CD0表示位清零操作
    {
        D0 &= ~val;
    }
    if (0 == strcmp("OD0", ptag))                                 // 对D0的位进行操作，OD0表示位置一操作
    {
        D0 |= val;
    }
    if (0 == strcmp("D0", ptag))                                  // 查询上报使能编码
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", D0);
            ZXBeeAdd("D0", p);
        }
    }
    if (0 == strcmp("CD1", ptag))                                 // 对D1的位进行操作，CD1表示位清零操作
    {
        D1 &= ~val;
        //sensorControl(D1);                                          // 处理执行命令
    }
    if (0 == strcmp("OD1", ptag))                                 // 对D1的位进行操作，OD1表示位置一操作
    {
        D1 |= val;
        //sensorControl(D1);                                          // 处理执行命令
    }
    if (0 == strcmp("D1", ptag))                                  // 查询执行器命令编码
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", D1);
            ZXBeeAdd("D1", p);
        }
    }
    if (0 == strcmp("V0", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", V0);                         	// 上报时间间隔
            ZXBeeAdd("V0", p);
        }
        else
        {
            updateV0(pval);
        }
    }
    return ret;
}
/*********************************************************************************************
* 名称：MyEventProcess()
* 功能：自定义事件处理
* 参数：event -- 事件编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void MyEventProcess( uint16 event )
{
    if (event & MY_REPORT_EVT)
    {
        sensorUpdate();
        //启动定时器，触发事件：MY_REPORT_EVT
        osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, 30*1000);
    }
    if(event & IR_LEARN_EVT)                                      // 检测event是否与IR_LEARN_EVT事件一致
    {
        ir360_learn();                                              // 执行操作
    }
}

/*********************************************************************************************
* 名称：ir360_send()
* 功能：红外遥控工作模式
* 参数：key -- 事件编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void ir360_send()
{
    HalUARTWrite(HAL_UART_PORT_0, &V0, 1);                        // 向360°红外遥控发送键值
}

/*********************************************************************************************
* 名称：ir360_learn()
* 功能：红外遥控学习模式
* 参数：key -- 事件编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void ir360_learn(void)
{
    uint8 cmd = 0x00;
    if(f_ucIrStatus == 0)                                       // 红外遥控模式操作状态一
    {
        f_ucIrStatus = 1;                                       // 状态升级到状态二
        cmd = LEARN_CMD;
        HalUARTWrite(HAL_UART_PORT_0, &cmd, 1);                 // 发送学习指令
    }
    if(f_ucIrStatus == 2)                                       // 红外遥控模式操作状态二
    {
        f_ucIrStatus = 3;                                       // 模式升级到状态三
        cmd = V0;
        HalUARTWrite(HAL_UART_PORT_0, &cmd, 1);                 // 发送键值
        osal_start_timerEx( sapi_TaskID, IR_LEARN_EVT, (uint16)(30 * 1000));
    }
    else if (f_ucIrStatus == 3)
    {
        f_ucIrStatus = 0;                                       // 超时
        cmd = EXIT_CMD;
        HalUARTWrite(HAL_UART_PORT_0, &cmd, 1);
    }
    if(f_ucIrStatus == 4)                                       // 红外遥控模式操作状态四
    {
        f_ucIrStatus = 0;                                       // 模式回归到状态零
        cmd = EXIT_CMD;
        HalUARTWrite(HAL_UART_PORT_0, &cmd, 1);                 // 发送控制模式指令
    }
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
static void node_uart_callback ( uint8 port ,uint8 event)
{
    (void)event;
    uint8  ch;
    while (Hal_UART_RxBufLen(port))
    {
        HalUARTRead (port, &ch, 1);                                 // 读取串口接收到的数据
        if(ch == 0xf0 && f_ucIrStatus == 1)                         // 接收到数据为0XF0同时状态为一
        {
            f_ucIrStatus = 2;                                         // 进入判断状态二
            osal_start_timerEx( sapi_TaskID, IR_LEARN_EVT, (uint16)(1 * 1000));
        }
        else if(ch == 0x00 && f_ucIrStatus == 3)                    // 接收到数据为0X00同时状态为三
        {
            f_ucIrStatus = 4;                                         // 进入状态判断四
            osal_start_timerEx( sapi_TaskID, IR_LEARN_EVT, (uint16)(2 * 1000));
        }
        else if(ch == 0xff && f_ucIrStatus == 1)                    // 接收到数据为0XFF同时状态为一
        {
            f_ucIrStatus = 0;                                         // 进入状态判断零
            osal_start_timerEx( sapi_TaskID, IR_LEARN_EVT, (uint16)(2 * 1000));
        }
        else if(ch == 0xff && f_ucIrStatus == 3)                    // 接收到数据为0XFF同时状态为三
        {
            f_ucIrStatus = 2;                                         // 进入判断状态二
            osal_start_timerEx( sapi_TaskID, IR_LEARN_EVT, (uint16)(2 * 1000));
        }
        else if(ch == 0xf2 && f_ucIrStatus == 0)                    // 接收到数据为0XF2同时状态为零
        {
            // 什么都不做
        }
        else
        {
            f_ucIrStatus = 0;                                         //否则设置状态为零
            f_ucCmd = EXIT_CMD;
            HalUARTWrite(HAL_UART_PORT_0, &f_ucCmd, 1);               //发送控制指令
        }
    }
}

/*********************************************************************************************
* 名称：node_uart_init()
* 功能：RS232通讯初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void node_uart_init(void)
{
    halUARTCfg_t _UartConfig;

    // UART 配置
    _UartConfig.configured           = TRUE;                      // 开启串口
    _UartConfig.baudRate             = HAL_UART_BR_9600;          // 波特率配置为9600
    _UartConfig.flowControl          = FALSE;                     // 关闭数据流控制
    _UartConfig.rx.maxBufSize        = 128;                       // 最大接收字节数128
    _UartConfig.tx.maxBufSize        = 128;                       // 最大发送字节数128
    _UartConfig.flowControlThreshold = (128 / 2);
    _UartConfig.idleTimeout          = 6;                         // 空闲配置
    _UartConfig.intEnable            = TRUE;                      // 启用停止位
    _UartConfig.callBackFunc         = node_uart_callback;

    // 启动 UART
    HalUARTOpen (HAL_UART_PORT_0, &_UartConfig);                  // 初始化串口
}
