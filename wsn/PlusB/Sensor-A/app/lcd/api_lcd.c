/*********************************************************************************************
* 文件：api_lcd.c
* 作者：fuyou 2018.11.1
* 说明：lcd与传感器的修改部分
* 修改：
* 注释：
*********************************************************************************************/
#include "api_lcd.h"

extern float A0;                                                 // 温度
extern float A1;                                                 // 湿度
extern float A2;                                                 // 光照强度
extern float A3;                                                   // 空气质量报警值
extern float A4;                                                 // 大气压力值
extern unsigned char A5;                                           // 三轴跌倒检测
extern float A6;                                                   // 红外测距
extern int A7;                                        

extern char D1;
extern void sensor_control(unsigned char cmd);

void xLabInfoShow(unsigned short y,unsigned short wordColor,unsigned short backColor,unsigned char refresh)
{
    if(refresh&0x80)
    {
        lcdShowBarX1(y,"传感器信息",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
        lcdShowBarX3(y+20,"型号","跳线模式","-",wordColor,backColor,refresh,1);
        lcdShowBarX3(y+40,SuiteType,"-","-",wordColor,backColor,refresh,1);
    }
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
    lcdShowMenuBar_1(y+=30,"传感器数据",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,1);
    
    sprintf(pbuf1,"%5.1f",A0);
    sprintf(pbuf2,"%5.1f",A1);
    lcdShowBarX4(y+=20,"温度","湿度",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    sprintf(pbuf1,"%5.1f",A2);
    sprintf(pbuf2,"%5.1f",A3);
    lcdShowBarX4(y+=20,"光照强度","空气质量",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    sprintf(pbuf1,"%5.1f",A4);
    sprintf(pbuf2,"%5.1f",A6);
    lcdShowBarX4(y+=20,"大气压力","红外测距",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    if(A5)
        sprintf(pbuf1,"有人跌倒了，危险！");
    else
        sprintf(pbuf1,"没有人跌倒，安全。");
    
    lcdShowMenuBar_1(y+=30,"跌倒检测",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
    lcdShowMenuBar_1(y+=20,pbuf1,penColor,backColor,refresh,1);
    
    switch(A7)
    {
        case 1:
            sprintf(pbuf1,"打开台灯");
            break;
        case 2:
            sprintf(pbuf1,"关闭台灯");
            break;
        case 3:
            sprintf(pbuf1,"打开窗帘");
            break;
        case 4:
            sprintf(pbuf1,"关闭窗帘");
            break;
        case 5:
            sprintf(pbuf1,"打开风扇");
            break;
        case 6:
            sprintf(pbuf1,"关闭风扇");
            break;
        case 7:
            sprintf(pbuf1,"打开空调");
            break;
        case 8:
            sprintf(pbuf1,"关闭空调");
            break;
        case 9:
            sprintf(pbuf1,"打开加湿器");
            break;
        case 10:
            sprintf(pbuf1,"关闭加湿器");
            break;
        case 11:
            sprintf(pbuf1,"自动模式");
            break;
        case 12:
            sprintf(pbuf1,"手动模式");
            break;
    }
    lcdShowMenuBar_1(y+=30,"语音信息",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
    lcdShowMenuBar_1(y+=20," ",penColor,backColor,refresh,1);
    
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
* 名称：lcdSensor_poll()
* 功能：lcd与传感器部分轮询
* 参数：无
* 返回：无
* 修改：
* 注释：轮询进行传感器数据的更新
*********************************************************************************************/
void lcdSensor_poll()
{
}

