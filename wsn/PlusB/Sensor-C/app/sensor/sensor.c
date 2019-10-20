/*********************************************************************************************
* 文件：sensor.c
* 作者：
* 说明：通用传感器控制接口程序
* 修改：
* 注释：
*********************************************************************************************/
#include "sensor.h"

unsigned char D0 = 0xFF;                                        // 主动上报使能，默认只允许A0主动上报
unsigned char D1 = 0;                                           // 默认关闭控制类传感器
unsigned int V0 = 30;                                           // 主动上报时间间隔，单位秒,0不主动上报
char* V1 = NULL;
unsigned char A0 = 0;                                           // 人体/触摸传感器
unsigned char A1 = 0;                                           // 振动传感器
unsigned char A2 = 0;                                           // 霍尔传感器
unsigned char A3 = 0;                                           // 火焰传感器
unsigned char A4 = 0;                                           // 燃气传感器
unsigned char A5 = 0;                                           // 光栅传感器
float A8 =0, A9=0, A10=0;                                       // 板载温度、湿度、电压
unsigned char CurrMode = 0;                                     // 传感器当前跳线模式

/*********************************************************************************************
* 名称：sensor_init()
* 功能：
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void sensor_init(void)
{
    relay_init();                                               // 初始化控制类传感器（继电器）
    grating_init();                                             // 光栅传感器初始化 
    vibration_init();
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
    {
        CurrMode = 1;
        infrared_init();
        flame_init();
        hall_init();
    }
    else
    {
        CurrMode = 2;
        touch_init();                                           // 触摸传感器初始化
        syn6288_init();                                         // 语音识别初始化
        syn6288_play("你好");
    }
}

/*********************************************************************************************
* 名称：sensor_type()
* 功能：
* 参数：
* 返回：返回传感器类型，3字节字符串表示
* 修改：
* 注释：
*********************************************************************************************/
char *sensor_type(void)
{
    return SENSOR_TYPE;                                           //返回传感器类型
}

/*********************************************************************************************
* 名称：sensor_control()
* 功能：传感器控制
* 参数：cmd - 控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensor_control(unsigned char cmd)
{
    if((cmd & 0x40) == 0x40)
    {
        relay_on(1);
    }
    else
    {
        relay_off(1);
    }
    if((cmd & 0x80) == 0x80)
    {
        relay_on(2);
    }
    else
    {
        relay_off(2);
    }
}

/*********************************************************************************************
* 名称：updateA0
* 功能：更新A0的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA0(void)
{
    if(CurrMode == 1)
    {
        A0 = get_infrared_status();
    }
    else
    {
        A0 = get_touch_status();
    }
}

/*********************************************************************************************
* 名称：updateA1
* 功能：更新A1的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA1(void)
{
    static unsigned char ct = 0;
    if(CurrMode == 1)
    {
        if(A1 == 0)
        {
            A1 = get_vibration_status();
        }
        else
        {
            ct++;
            if(ct >= 2000/CheckTime)
            {
                ct = 0;
                A1 = 0;
            }
        }
    }
}

/*********************************************************************************************
* 名称：updateA2
* 功能：更新A2的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA2(void)
{
    if(CurrMode == 1)
    {
        A2 = get_hall_status();
    }
}

/*********************************************************************************************
* 名称：updateA3
* 功能：更新A3的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA3(void)
{
    if(CurrMode == 1)
    {
        A3 = get_flame_status();
    }
}

/*********************************************************************************************
* 名称：updateA4
* 功能：更新A4的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA4(void)
{
    A4 = get_combustiblegas_data();
}

/*********************************************************************************************
* 名称：updateA5
* 功能：更新A5的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA5(void)
{
    A5 = get_grating_status();
}

/*********************************************************************************************
* 名称：updateA8
* 功能：更新A8的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA8(void)
{
    A8 = Htu21dTemperature_Get();                               // 更新A5 板载温度
}

/*********************************************************************************************
* 名称：updateA9
* 功能：更新A9的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA9(void)
{
    A9 = Htu21dHumidity_Get();                                  // 更新A6 板载湿度
}

/*********************************************************************************************
* 名称：updateA10
* 功能：更新A10的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA10(void)
{
    A10 = BatteryVotage_Get();                                   // 更新A7 电池电压
}

/*********************************************************************************************
* 名称：sensor_poll()
* 功能：轮询传感器，并主动上报传感器数据
* 参数：t: 调用次数
* 返回：
* 修改：
* 注释：此函数每秒钟调用1次，t为调用次数
*********************************************************************************************/
void sensor_poll(unsigned int t)
{
    char buf[16] = {0};
    
    if (V0 != 0)
    {
        if (t % V0 == 0)
        {
            zxbeeBegin();
            if (D0 & 0x01)
            {
                sprintf(buf, "%u", A0);
                zxbeeAdd("A0", buf);
            }
            if (D0 & 0x02)
            {
               //A1值打包
                sprintf(buf, "%u", A1);
                zxbeeAdd("A1", buf);
            }
            if (D0 & 0x04)
            {
               //A2值打包
                sprintf(buf, "%u", A2);
                zxbeeAdd("A2", buf);
            }
            if (D0 & 0x08)
            {
               //A3值打包
                sprintf(buf, "%u", A3);
                zxbeeAdd("A3", buf);
            }
            if (D0 & 0x10)
            {
               //A4值打包
                sprintf(buf, "%u", A4);
                zxbeeAdd("A4", buf);
            }
            if (D0 & 0x20)
            {
                sprintf(buf, "%u", A5);
                zxbeeAdd("A5", buf);
            }
            if (D0 & 0x40)
            {
                
            }
            if (D0 & 0x80)
            {
                
            }
            sprintf(buf, "%.1f", A8);
            zxbeeAdd("A8", buf);
            sprintf(buf, "%.1f", A9);
            zxbeeAdd("A9", buf);
            sprintf(buf, "%.1f", A10);
            zxbeeAdd("A10", buf);
            char *p = zxbeeEnd();
            if (p != NULL)
            {
                rfUartSendData(p);                              //发送无线数据
            }
        }
    }
}

/*********************************************************************************************
* 名称：sensor_check()
* 功能：周期性检查函数，可设定轮询时间
* 参数：无
* 返回：设置轮询的时间，单位ms,范围:1-65535
* 修改：
* 注释：此函数用于需要快速做出相应的传感器
*********************************************************************************************/
unsigned short sensor_check()
{
    char buf[16] = {0};
    static unsigned char last_A0 = 0,last_A1 = 0,last_A2 = 0,last_A3 = 0,last_A4 = 0,last_A5 = 0;
    static unsigned char time_A0 = 0,time_A2 = 0,time_A3 = 0,time_A4 = 0,time_A5 = 0;
    updateA0();
    updateA1();
    updateA2();
    updateA3();
    updateA4();
    updateA5();
    zxbeeBegin();
    if (last_A0 != A0 
        || ( //模式1，人体红外传感器选通，报警后循环3S上报；模式2：触摸传感器选通，状态反转后上报1次，  
            (CurrMode == 1)&& time_A0 != 0 && clock_time() > (time_A0+3000)) //超过3秒上报
            ) {
                if (D0 & 0x01) {
                    sprintf(buf, "%u", A0);
                    zxbeeAdd("A0", buf);
                    time_A0 = clock_time();
                }
                if (A0 == 0) {
                    time_A0 = 0;
                }
                last_A0 = A0;
            }
    if(CurrMode == 1)
    {
        if(last_A1 != A1)
        {
            if(D0 & 0x02)
            {
                sprintf(buf, "%u", A1);
                zxbeeAdd("A1", buf);
            }
            last_A1 = A1;
        }
        
        if(A2 == 1)
            time_A2++;
        if(last_A2 != A2 || (time_A2 >= 3000/CheckTime))
        {
            if(D0 & 0x04)
            {
                sprintf(buf, "%u", A2);
                zxbeeAdd("A2", buf); 
            }
            time_A2 = 0;
            last_A2 = A2;
        }
        
        if(A3 == 1)
            time_A3++;
        if(last_A3 != A3 || (time_A3 >= 3000/CheckTime))
        {
            if(D0 & 0x08)
            {
                sprintf(buf, "%u", A3);
                zxbeeAdd("A3", buf); 
            }
            time_A3 = 0;
            last_A3 = A3;
        }
    }
    if(A4 == 1)
        time_A4++;
    if(last_A4 != A4 || (time_A4 >= 3000/CheckTime))
    {
        if(D0 & 0x10)
        {
            sprintf(buf, "%u", A4);
            zxbeeAdd("A4", buf); 
        }
        time_A4 = 0;
        last_A4 = A4;
    }
    
    if(A5 == 1)
        time_A5++;
    if(last_A5 != A5 || (time_A5 >= 3000/CheckTime))
    {
        if(D0 & 0x20)
        {
            sprintf(buf, "%u", A5);
            zxbeeAdd("A5", buf); 
        }
        time_A5 = 0;
        last_A5 = A5;
    }
    char *p = zxbeeEnd();
    if (p != NULL)
    {
        rfUartSendData(p);                              //发送无线数据
    }
    
    return CheckTime;                                                 //返回值决定下次调用时间，此处为200ms
}

/*********************************************************************************************
* 名称：z_process_command_call()
* 功能：处理上层应用发过来的指令
* 参数：ptag: 指令标识 D0，D1， A0 ...
*       pval: 指令值， “？”表示读取，
*       obuf: 指令处理结果存放地址
* 返回：>0指令处理结果返回数据长度，0：没有返回数据，<0：不支持指令。
* 修改：
* 注释：
*********************************************************************************************/
int z_process_command_call(char* ptag, char* pval, char* obuf)
{
    int ret = -1;
    if (memcmp(ptag, "D0", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "D0=%d", D0);
        }
    }
    if (memcmp(ptag, "CD0", 3) == 0)
    {
        int v = atoi(pval);
        if (v > 0)
        {
            D0 &= ~v;
        }
    }
    if (memcmp(ptag, "OD0", 3) == 0)
    {
        int v = atoi(pval);
        if (v > 0)
        {
            D0 |= v;
        }
    }
    if (memcmp(ptag, "D1", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "D1=%d", D1);
        }
    }
    if (memcmp(ptag, "CD1", 3) == 0)  				            //若检测到CD1指令
    {
        int v = atoi(pval);                                     //获取CD1数据
        D1 &= ~v;                                               //更新D1数据
        sensor_control(D1);                                     //更新电磁阀直流电机状态
    }
    if (memcmp(ptag, "OD1", 3) == 0)  				            //若检测到OD1指令
    {
        int v = atoi(pval);                                     //获取OD1数据
        D1 |= v;                                                //更新D1数据
        sensor_control(D1);                                     //更新电磁阀直流电机状态
    }
    if (memcmp(ptag, "V0", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "V0=%d", V0);
        }
        else
        {
            V0 = atoi(pval);
        }
    }
    if (memcmp(ptag, "V1", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "V1=%s", V1);
        }
        else
        {
            if(CurrMode == 2)
            {
                int n = strlen(pval)/2;
                syn6288_play_unicode(hex2unicode(pval),n);
            }
        }
    }
    if (memcmp(ptag, "A0", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "A0=%d", A0);
        }
    }
    if (memcmp(ptag, "A1", 2) == 0)
    {
        if (pval[0] == '?')
        {
            //
        }
    }
    if (memcmp(ptag, "A2", 2) == 0)
    {
        if (pval[0] == '?')
        {
            //
        }
    }
    if (memcmp(ptag, "A3", 2) == 0)
    {
        if (pval[0] == '?')
        {
           //
        }
    }
    if (memcmp(ptag, "A4", 2) == 0)
    {
        if (pval[0] == '?')
        {
            //
        }
    }
    if (memcmp(ptag, "A5", 2) == 0)
    {
        if (pval[0] == '?')
        {
            
        }
    }
    if (memcmp(ptag, "A8", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "A8=%.1f", A8);
        }
    }
    if (memcmp(ptag, "A9", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "A9=%.1f", A9);
        }
    }
    if (memcmp(ptag, "A10", 2) == 0)
    {
        if (pval[0] == '?')
        {
            ret = sprintf(obuf, "A10=%.1f", A10);
        }
    }
    return ret;
}

