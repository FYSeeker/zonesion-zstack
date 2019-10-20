/*********************************************************************************************
* 文件：lcdMenu.c
* 作者：fuyou 2018.11.1
* 说明：实现菜单项功能框架，提供菜单项的API函数
* 修改：
* 注释：
*********************************************************************************************/
#include "lcdMenu.h"


_SensorBuf_t sensorDataBuf={0};

char optionName_select[MenuIndexMAX][TextLen+1]={"选项示例"};
char optionName_unselect[MenuIndexMAX][40]={"选项示例"};
char HintList[MenuIndexMAX+1][40]={"K1:确认  K2:退出  K3:上移  K4:下移"};

_Menu_t menu={1};
_Table_t TableStruct={"Etherent","A","B","C","D"};

/*********************************************************************************************
设置函数
*********************************************************************************************/

/*********************************************************************************************
* 名称：optionDefaultName_set()
* 功能：菜单项被选定后的处理函数
* 参数：optionIndex:菜单项索引,func:回调函数
* 返回：0:成功,1:失败
* 修改：
* 注释：
*********************************************************************************************/
char optionCallFunc_set(unsigned char optionIndex,void (*func)(optionStatus_t))
{
    if(optionIndex>MenuIndexMAX) return 1;
    menu.optionHandle[optionIndex-1]=func;
    return 0;
}

/*********************************************************************************************
* 名称：optionDefaultName_set()
* 功能：菜单项默认名称设置
* 参数：index:菜单项索引,pbuf:名称
* 返回：0:成功,1:失败
* 修改：
* 注释：
*********************************************************************************************/
char optionDefaultName_set(unsigned char index,char* pbuf)
{
    if(index>MenuIndexMAX) return 1;
    memcpy(optionName_unselect[index-1],pbuf,strlen(pbuf)<TextLen?strlen(pbuf):TextLen);
    return 0;
}

/*********************************************************************************************
* 名称：optionDefaultName_set()
* 功能：菜单项被选定后的名称设置
* 参数：index:菜单项索引,pbuf:名称
* 返回：0:成功,1:失败
* 修改：
* 注释：
*********************************************************************************************/
char optionSelectName_set(unsigned char index,char* pbuf)
{
    if(index>MenuIndexMAX) return 1;
    memcpy(optionName_select[index-1],pbuf,strlen(pbuf)<40?strlen(pbuf):40);
    return 0;
}

/*********************************************************************************************
* 名称：optionHintInfo_set
* 功能：菜单项被选定后的提示信息
* 参数：index:菜单项索引,pbuf:信息
* 返回：0:成功,1:失败
* 修改：
* 注释：
*********************************************************************************************/
char optionHintInfo_set(unsigned char index,char* pbuf)
{
    if(index>MenuIndexMAX) return 1;
    memcpy(HintList[index],pbuf,strlen(pbuf)<40?strlen(pbuf):40);
    return 0;
}

/*********************************************************************************************
* 名称：optionState_set
* 功能：菜单项状态标志设置
* 参数：index:菜单项索引,state:设置的状态
* 返回：0:成功,1:失败
* 修改：
* 注释：
*********************************************************************************************/
char optionState_set(unsigned char index,optionStatus_t state)
{
    if(index>MenuIndexMAX) return 1;
    menu.optionState[index-1]=state;
    return 0;
}

/*********************************************************************************************
* 名称：tableExplain_set
* 功能：表格信息设置
* 参数：index:表格索引,pbuf1:信息内容
* 返回：0:成功,1:失败
* 修改：
* 注释：
*********************************************************************************************/
char tableExplain_set(unsigned char index,char* pbuf1)
{
    if(index>5) return 1;
    memcpy(TableStruct.explain[index-1],pbuf1,strlen(pbuf1)<14?strlen(pbuf1):14);
    return 0;
}

/*********************************************************************************************
* 名称：tableData_set
* 功能：表格数据设置
* 参数：index:表格索引,pbuf1:数据内容
* 返回：0:成功,1:失败
* 修改：
* 注释：
*********************************************************************************************/
char tableData_set(unsigned char index,char* pbuf1)
{
    if(index>5) return 1;
    memcpy(TableStruct.pdata[index-1],pbuf1,strlen(pbuf1)<14?strlen(pbuf1):14);
    return 0;
}

/*********************************************************************************************
菜单显示
*********************************************************************************************/

//显示条目-1
void lcdShowMenuBar_1(unsigned short y,char* buf,unsigned short wordColor,unsigned short backColor,
                      unsigned char refresh,unsigned char mode)
{
    if(refresh&0x80)
    {
        LCD_File(0,y,319,y+19,backColor);
        if(mode)
        {
            LCD_DrawLine(0,y+19,319,y+19,LCD_COLOR_WHITE);
        }
    }
    LCDShowFont16(160-strlen(buf)*4,y+2,buf,strlen(buf)*8,wordColor,backColor);
}

//显示条目-3
void lcdShowMenuBar_3(unsigned short y,char* buf1,char* buf2,char* buf3,
                      unsigned short wordColor,unsigned short backColor,unsigned char refresh,unsigned char mode)
{
    unsigned short width = (unsigned short)320/3;
    
    if(refresh&0x80)
    {
        LCD_File(0,y,319,y+19,backColor);
        LCD_DrawLine(width,y,width,y+19,LCD_COLOR_WHITE);
        LCD_DrawLine(width*2,y,width*2,y+19,LCD_COLOR_WHITE);
        if(mode)
        {
            LCD_DrawLine(0,y+19,319,y+19,LCD_COLOR_WHITE);
        }
    }
    LCDShowFont16(width/2-strlen(buf1)*4,y+2,buf1,strlen(buf1)*8,wordColor,backColor);
    LCDShowFont16((width+width/2)-strlen(buf2)*4,y+2,buf2,strlen(buf2)*8,wordColor,backColor);
    LCDShowFont16((width*2+width/2)-strlen(buf3)*4,y+2,buf3,strlen(buf3)*8,wordColor,backColor);
}

void menuOptionShow(uint16_t x,uint16_t y,char* text,uint16_t textColor,uint16_t backColor)
{
    LCD_File(x,y+1,x+(TextLen*8),y+17,backColor);
    LCDShowFont16(x+8,y+2,text,(TextLen*8)-16,textColor,backColor);
    LCD_DrawLine(x,y+18,x+(TextLen*8),y+18,LCD_COLOR_BLACK);
    LCD_DrawLine(x+(TextLen*8)+1,y+1,x+(TextLen*8)+1,y+18,LCD_COLOR_BLACK);
}


uint16_t optionColorSelect(uint8_t index,uint8_t status)
{
    uint16_t defaultColor = LCD_COLOR_GRAY;
    uint16_t indexColor1 = LCD_COLOR_DARK_GRAY;
    uint16_t selectColor = LCD_COLOR_BLUE;
    uint16_t indexColor2 = LCD_COLOR_DARK_BLUE;
    
    return status?(index?indexColor2:selectColor):(index?indexColor1:defaultColor);
}

void menuShowOptionList(unsigned char refresh)
{
    uint8_t index=0;
    uint16_t x=8,y=0;
    uint16_t penColor = LCD_COLOR_WHITE;
    uint16_t backColor = LCD_COLOR_GRAY;
    
    if(refresh&0x80)
    {
        LCDClear(LCD_COLOR_WHITE);
        lcdShowMenuBar_1(0,"xLab信息",LCD_COLOR_WHITE,LCD_COLOR_RED,refresh,1);
    }
    for(index=0;index<MenuIndexMAX;index++)
    {
        if(index==4) x=168,y=0;
        y+=20;
        backColor = optionColorSelect(menu.index==(index+1),menu.optionState[index]);  
        menuOptionShow(x,y,optionName_unselect[index],penColor,backColor);
    }
}

void menuShowHint(unsigned char refresh)
{
    unsigned char i=0;
    static unsigned short tick=0;
    static unsigned char hintState=0,hintIndex=0;
    
    if(tick>0) tick--;
    if(tick==0)
    {
        if((hintState&0xff)==0)
        {
            hintState |= (1<<0);
            for(i=1;i<MenuIndexMAX;i++)
            {
                hintState = (menu.optionState[i-1]==SELECT) ? hintState|(1<<i) : hintState&(~(1<<i));
            }
        }
        
        for(i=0;i<(MenuIndexMAX+1);i++)
        {
            if(hintState&(1<<i))
            {
                hintState&=~(1<<i);
                hintIndex = i;
                if((HintList[hintIndex][0] != NULL)&&(HintList[hintIndex][0] != ' '))
                {
                    refresh |= 0X80;
                    tick = 10;
                    break;
                }
            }
        }
    }
    lcdShowMenuBar_1(220,(char*)HintList[hintIndex],LCD_COLOR_RED,LCD_COLOR_YELLOW,refresh,0);
    refresh &= ~0X80;
}

void lcdShowTable(unsigned char refresh)
{
    unsigned char i,mode;
    unsigned short y=120;
    
    if(refresh&0x80)
    {
        lcdShowMenuBar_3(y,"端口名称","端口说明","端口数据",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
    }
    
    for(i=0;i<5;i++)
    {
        y+=20;
        mode = (i==4)?0:1;
        lcdShowMenuBar_3(y,TableStruct.name[i],TableStruct.explain[i],TableStruct.pdata[i],LCD_COLOR_BLACK,LCD_COLOR_LIGHT_GRAY,refresh,mode);
    }
}

void menuShowHandle(unsigned char refresh)
{
    menuShowOptionList(refresh);
    menuShowHint(refresh);
}

/*********************************************************************************************
菜单操作
*********************************************************************************************/

void menuConfirmHandle(void)
{
    menu.optionState[menu.index-1] = (menu.optionState[menu.index-1]==SELECT)?UNSELECT:SELECT;
    if(menu.optionHandle[menu.index-1] != NULL)
        menu.optionHandle[menu.index-1](menu.optionState[menu.index-1]);
}

void menuExitHandle()
{
    menu.index=1;
    lcdPageIndex=0;
    lcdShowPage(0x80);
}

void menuKeyUpHandle()
{
    menu.index--;
    if(menu.index<1)
    {
        menu.index=MenuIndexMAX;
    }
}

void menuKeyDownHandle()
{
    menu.index++;
    if(menu.index>MenuIndexMAX)
    {
        menu.index=1;
    }
}

void menuKeyHandle(unsigned char keyStatus)
{
    switch(keyStatus)
    {
        case 0x01:
            menuConfirmHandle();
            break;
        case 0x02:
            menuExitHandle();
            break;
        case 0x04:
            menuKeyUpHandle();
            break;
        case 0x08:
            menuKeyDownHandle();
            break;
    }
}


/*********************************************************************************************
传感器显示操作
*********************************************************************************************/
void SensorName_set(char* pbuf1,char* pbuf2,char* pbuf3)
{
    memcpy(sensorDataBuf.name[0],pbuf1,strlen(pbuf1));
    memcpy(sensorDataBuf.name[1],pbuf2,strlen(pbuf2));
    memcpy(sensorDataBuf.name[2],pbuf3,strlen(pbuf3));
}

void SensorData_set(char* pbuf1,char* pbuf2,char* pbuf3)
{
    memcpy(sensorDataBuf.pdata[0],pbuf1,strlen(pbuf1));
    memcpy(sensorDataBuf.pdata[1],pbuf2,strlen(pbuf2));
    memcpy(sensorDataBuf.pdata[2],pbuf3,strlen(pbuf3));
}

void lcdShowSensorInfo(unsigned short y,unsigned short wordColor,
                       unsigned short backColor_1,unsigned short backColor_2,unsigned char refresh)
{
    xLabInfoShow(y,wordColor,backColor_2,refresh);
}