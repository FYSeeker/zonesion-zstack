/*********************************************************************************************
* 文件：api_lcd.c
* 作者：fuyou 2018.11.1
* 说明：lcd与传感器的修改部分
* 修改：
* 注释：
*********************************************************************************************/
#include "api_lcd.h"


extern unsigned char D1;                                           // 默认关闭控制类传感器
extern char* V1;
extern unsigned char A0;                                           // 人体/触摸传感器
extern unsigned char A1;                                           // 振动传感器
extern unsigned char A2;                                           // 霍尔传感器
extern unsigned char A3;                                           // 火焰传感器
extern unsigned char A4;                                           // 燃气传感器
extern unsigned char A5;                                           // 光栅传感器
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

void xLabPageShow(uint8_t refresh)
{
    uint16_t y=0;
    uint16_t penColor=LCD_COLOR_BLACK;
    uint16_t backColor=LCD_COLOR_LIGHT_GRAY;
    char pbuf1[32]={0};
    char pbuf2[32]={0};
    
    if(refresh&0x80)
    {
        LCDClear(LCD_COLOR_WHITE);
        lcdShowMenuBar_1(0,"信息页面",LCD_COLOR_WHITE,LCD_COLOR_RED,refresh,1);
    }
    lcdShowMenuBar_1(y+=40,"传感器数据",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,1);
    
    
    if(CurrMode==1)
    {
        if(A0)
            sprintf(pbuf1,"有人");
        else
            sprintf(pbuf1,"无人");
        if(A1)
            sprintf(pbuf2,"震动中");
        else
            sprintf(pbuf2," 静止 ");
        lcdShowBarX4(y+=20,"人体红外","震动",pbuf1,pbuf2,penColor,backColor,refresh,1);
    }
    else if(CurrMode==2)
    {
        if(A0)
            sprintf(pbuf1,"触摸中");
        else
            sprintf(pbuf1,"未触摸");
        lcdShowBarX4(y+=20,"触摸","-",pbuf1,"-",penColor,backColor,refresh,1);
    }
    else
        lcdShowBarX4(y+=20,"模式未知","模式未知","-","-",penColor,backColor,refresh,1);
    
    sprintf(pbuf1,"%u",A2);
    sprintf(pbuf2,"%u",A3);
    if(CurrMode==1)
    {
        if(A2)
            sprintf(pbuf1,"磁铁靠近");
        else
            sprintf(pbuf1," 无磁铁 ");
        if(A3)
            sprintf(pbuf2,"有明火");
        else
            sprintf(pbuf2,"无明火");
        lcdShowBarX4(y+=20,"霍尔","火焰",pbuf1,pbuf2,penColor,backColor,refresh,1);
    }
    else if(CurrMode==2)
        lcdShowBarX4(y+=20,"-","-","-","-",penColor,backColor,refresh,1);
    else
        lcdShowBarX4(y+=20,"模式未知","模式未知","-","-",penColor,backColor,refresh,1);
    
    if(A4)
        sprintf(pbuf1,"燃气超标");
    else
        sprintf(pbuf1,"燃气正常");
    if(A5)
        sprintf(pbuf2,"有遮挡");
    else
        sprintf(pbuf2,"无遮挡");
    lcdShowBarX4(y+=20,"燃气","光栅",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    if(CurrMode==1)
    {
        sprintf(pbuf1,"此模式下不可用");
    }
    else if(CurrMode==2)
    {
        if(V1!=NULL)
            sprintf(pbuf1,V1);
        else
            sprintf(pbuf1," ");
    }
    else
    {
        sprintf(pbuf1,"模式未确定");
    }
    lcdShowMenuBar_1(y+=40,"语音信息",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
    lcdShowMenuBar_1(y+=20,pbuf1,penColor,backColor,refresh,1);
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
    menuExitHandle();
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
        D1 |= 0x40;
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(0x40);
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
        D1 |= 0x80;
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(0x80);
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
    optionDefaultName_set(1," 1-返回主界面");     //菜单项默认名称
    optionSelectName_set(1," 1-返回主界面");      //菜单项被选定后的名称
    optionHintInfo_set(1," ");                  //菜单项被选定后的提示信息，空格就是什么都不显示
    optionCallFunc_set(1,option_1_Handle);      //菜单项被选定后的处理函数
    
    //菜单项2的初始化
    optionDefaultName_set(2," 2-打开继电器一");   //菜单项默认名称
    optionSelectName_set(2," 2-关闭继电器一");    //菜单项被选定后的名称
    optionHintInfo_set(2,"继电器一已打开");        //菜单项被选定后的提示信息
    optionCallFunc_set(2,option_2_Handle);      //菜单项被选定后的处理函数
    
    //菜单项3的初始化
    optionDefaultName_set(3," 3-打开继电器二");
    optionSelectName_set(3," 3-关闭继电器二");
    optionHintInfo_set(3,"继电器二已打开");
    optionCallFunc_set(3,option_3_Handle);
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
    TableInfo_init();
    SensorName_set("继电器一","继电器二","示例传感器");
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
        optionState_set(2,SELECT);      //设置选项2为选择状态
    else
        optionState_set(2,UNSELECT);    //设置选项2为取消选择状态
    
    if(D1&0x80)
        optionState_set(3,SELECT);      //设置选项3为选择状态
    else
        optionState_set(3,UNSELECT);    //设置选项3为取消选择状态
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
    char pbuf1[16]={0};
    char pbuf2[16]={0};
    char pbuf3[16]={0};
    
    //更新数据/状态
    if(D1&0x40)
        sprintf(pbuf1,"打开");
    else
        sprintf(pbuf1,"关闭");
    
    if(D1&0x80)
        sprintf(pbuf2,"打开");
    else
        sprintf(pbuf2,"关闭");
    
    sprintf(pbuf3,"%3u",A0);
    
    //更新页面1的传感器数据
    SensorData_set(pbuf1,pbuf2,pbuf3);
    //更新页面2的表格数据
    tableData_set(1,"-");
    tableData_set(2,"-");
    tableData_set(3,pbuf1);
    tableData_set(4,pbuf2);
    tableData_set(5,pbuf3);
    //同步控制类传感器显示状态
    syncSensorStatus();
}

