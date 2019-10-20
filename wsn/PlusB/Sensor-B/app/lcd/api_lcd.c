/*********************************************************************************************
* 文件：api_lcd.c
* 作者：fuyou 2018.11.1
* 说明：lcd与传感器的修改部分
* 修改：
* 注释：
*********************************************************************************************/
#include "api_lcd.h"


extern unsigned char D1;                                           // 默认关闭控制类传感器
extern unsigned char CurrMode;                                     // 传感器当前跳线模式
extern void sensor_control(unsigned char cmd);

void xLabInfoShow(unsigned short y,unsigned short wordColor,unsigned short backColor,unsigned char refresh)
{
    char pbuf[10]={0};
    if(refresh&0x80)
    {
        lcdShowBarX1(y,"传感器信息",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
        lcdShowBarX3(y+20,"型号","跳线模式","-",wordColor,backColor,refresh,1);
    }
    if(CurrMode==1)
        sprintf(pbuf,"模式一");
    else if(CurrMode==2)
        sprintf(pbuf,"模式二");
    else
        sprintf(pbuf,"未确定");
        
    lcdShowBarX3(y+40,SuiteType,pbuf,"-",wordColor,backColor,refresh,1);
}

/*********************************************************************************************
* 名称：option_1_Handle()
* 功能：菜单项1的处理函数
* 参数：status:菜单项的选定状态
* 返回：无
* 修改：
* 注释：自定义菜单项1被选定后进行的操作
*********************************************************************************************/
void option_1_Handle(optionStatus_t status)
{
    if(status==SELECT)
    {
        D1 |= (1<<6);
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(1<<6);
    }
    sensor_control(D1);
}

/*********************************************************************************************
* 名称：option_2_Handle()
* 功能：菜单项2的处理函数
* 参数：status:菜单项的选定状态
* 返回：无
* 修改：
* 注释：自定义菜单项2被选定后进行的操作
*********************************************************************************************/
void option_2_Handle(optionStatus_t status)
{
    if(status==SELECT)
    {
        D1 |= (1<<7);
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(1<<7);
    }
    sensor_control(D1);
}

/*********************************************************************************************
* 名称：option_3_Handle()
* 功能：菜单项3的处理函数
* 参数：status:菜单项的选定状态
* 返回：无
* 修改：
* 注释：自定义菜单项3被选定后进行的操作
*********************************************************************************************/
void option_3_Handle(optionStatus_t status)
{
    if(status==SELECT)
    {
        D1 |= (1<<4);
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(1<<4);
    }
    sensor_control(D1);
}

/*********************************************************************************************
* 名称：option_4_Handle()
* 功能：菜单项4的处理函数
* 参数：status:菜单项的选定状态-
* 返回：无
* 修改：
* 注释：自定义菜单项4被选定后进行的操作
*********************************************************************************************/
void option_4_Handle(optionStatus_t status)
{
    if(status==SELECT)
    {
        D1 |= (1<<5);
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(1<<5);
    }
    sensor_control(D1);
}

/*********************************************************************************************
* 名称：option_5_Handle()
* 功能：菜单项5的处理函数
* 参数：status:菜单项的选定状态
* 返回：无
* 修改：
* 注释：自定义菜单项5被选定后进行的操作
*********************************************************************************************/
void option_5_Handle(optionStatus_t status)
{
    static uint8_t index=0;
    
    if(CurrMode==1)
    {
        if(status==SELECT)
        {
            D1 |= (1<<2);
        }
        else if(status==UNSELECT)
        {
            D1 &= ~(1<<2);
        }
    }
    else if(CurrMode==2)
    {
        index++;
        if(index>3) index=0;
        if(index!=0)
        {
            switch(index)
            {
                case 1:
                    D1 &= ~(0X03);
                    D1 |= (1<<0);
                    break;
                case 2:
                    D1 &= ~(0X03);
                    D1 |= (1<<1);
                    break;
                case 3:
                    D1 &= ~(0X03);
                    D1 |= (1<<0)+(1<<1);
                    break;
            }
        }
        else
        {
            D1 &= ~(0X03);
        }
    }
    sensor_control(D1);
}

/*********************************************************************************************
* 名称：option_6_Handle()
* 功能：菜单项6的处理函数
* 参数：status:菜单项的选定状态
* 返回：无
* 修改：
* 注释：自定义菜单项5被选定后进行的操作
*********************************************************************************************/
void option_6_Handle(optionStatus_t status)
{
    if(status==SELECT)
    {
        D1 |= (1<<3);
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(1<<3);
    }
    sensor_control(D1);
}


/*********************************************************************************************
* 名称：MenuOption_init()
* 功能：菜单项初始化
* 参数：无
* 返回：无
* 修改：
* 注释：自定义菜单项的内容和功能
*********************************************************************************************/
void MenuOption_init()
{
    //菜单项1的初始化
    optionDefaultName_set(1,"打开继电器一");          //菜单项默认名称
    optionCallFunc_set(1,option_1_Handle);      //菜单项被选定后的处理函数
    
    //菜单项2的初始化
    optionDefaultName_set(2,"打开继电器二");          //菜单项默认名称
    optionCallFunc_set(2,option_2_Handle);      //菜单项被选定后的处理函数

    optionDefaultName_set(3,"打开LED1");
    optionCallFunc_set(3,option_3_Handle);

    optionDefaultName_set(4,"打开LED2");
    optionCallFunc_set(4,option_4_Handle);
    
    if(CurrMode==1)
    {
        optionDefaultName_set(5,"打开步进电机");
        optionCallFunc_set(5,option_5_Handle);
        
        optionDefaultName_set(6,"打开风扇");
        optionCallFunc_set(6,option_6_Handle);
    }
    else if(CurrMode==2)
    {
        optionDefaultName_set(5,"切换RGB灯");
        optionCallFunc_set(5,option_5_Handle);
        
        optionDefaultName_set(6,"打开蜂鸣器");
        optionCallFunc_set(6,option_6_Handle);
    }
    else
    {
        optionDefaultName_set(5,"模式未确定");
        optionDefaultName_set(6,"模式未确定");
    }
}

/*********************************************************************************************
* 名称：TableInfo_init()
* 功能：表格信息初始化
* 参数：无
* 返回：无
* 修改：
* 注释：自定义表格需要显示的信息
*********************************************************************************************/
void TableInfo_init()
{
    tableExplain_set(1,"网线接口");
    tableExplain_set(2,"-");
    tableExplain_set(3,"继电器一");
    tableExplain_set(4,"继电器二");
    tableExplain_set(5,"示例传感器");
}

/*********************************************************************************************
* 名称：lcdSensor_init()
* 功能：lcd与传感器部分初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void lcdSensor_init()
{
    MenuOption_init();
}

/*********************************************************************************************
* 名称：syncSensorStatus()
* 功能：同步控制类传感器状态
* 参数：无
* 返回：无
* 修改：
* 注释：控制类传感器被上层设置后，需要运行此函数同步屏幕上的显示状态
*********************************************************************************************/
void syncSensorStatus()
{
    if(D1&0x40)
        optionState_set(1,SELECT);      //设置选项1为选择状态
    else
        optionState_set(1,UNSELECT);    //设置选项1为取消选择状态
    
    if(D1&0x80)
        optionState_set(2,SELECT);      //设置选项2为选择状态
    else
        optionState_set(2,UNSELECT);    //设置选项2为取消选择状态
    
    if(D1&(1<<4))
        optionState_set(3,SELECT);      //设置选项3为选择状态
    else
        optionState_set(3,UNSELECT);    //设置选项3为取消选择状态
    
    if(D1&(1<<5))
        optionState_set(4,SELECT);      //设置选项4为选择状态
    else
        optionState_set(4,UNSELECT);    //设置选项4为取消选择状态
    
    
    if(CurrMode==1)
    {
        if(D1&(1<<2))
            optionState_set(5,SELECT);      //设置选项5为选择状态
        else
            optionState_set(5,UNSELECT);    //设置选项5为取消选择状态
    }
    else if(CurrMode==2)
    {
        if(D1&(0X03))
            optionState_set(5,SELECT);      //设置选项5为选择状态
        else
            optionState_set(5,UNSELECT);    //设置选项5为取消选择状态
    }
    if(D1&(1<<3))
        optionState_set(6,SELECT);      //设置选项6为选择状态
    else
        optionState_set(6,UNSELECT);    //设置选项6为取消选择状态
}

/*********************************************************************************************
* 名称：lcdSensor_poll()
* 功能：lcd与传感器部分轮询
* 参数：无
* 返回：无
* 修改：
* 注释：轮询用于传感器数据的更新
*********************************************************************************************/
void lcdSensor_poll()
{
    //同步控制类传感器显示状态
    syncSensorStatus();
}

