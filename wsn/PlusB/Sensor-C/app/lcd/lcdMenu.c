/*********************************************************************************************
* �ļ���lcdMenu.c
* ���ߣ�fuyou 2018.11.1
* ˵����ʵ�ֲ˵���ܿ�ܣ��ṩ�˵����API����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "lcdMenu.h"


_SensorBuf_t sensorDataBuf={0};

char optionName_select[MenuIndexMAX][40]={  " 1-����������"};
char optionName_unselect[MenuIndexMAX][40]={" 1-����������"};
char HintList[MenuIndexMAX+1][40]={"K1:ȷ��  K2:�˳�  K3:����  K4:����"};

_Menu_t menu={1};
_Table_t TableStruct={"Etherent","A","B","C","D"};

/*********************************************************************************************
���ú���
*********************************************************************************************/

/*********************************************************************************************
* ���ƣ�optionDefaultName_set()
* ���ܣ��˵��ѡ����Ĵ�����
* ������optionIndex:�˵�������,func:�ص�����
* ���أ�0:�ɹ�,1:ʧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char optionCallFunc_set(unsigned char optionIndex,void (*func)(optionStatus_t))
{
    if(optionIndex>MenuIndexMAX) return 1;
    menu.optionHandle[optionIndex-1]=func;
    return 0;
}

/*********************************************************************************************
* ���ƣ�optionDefaultName_set()
* ���ܣ��˵���Ĭ����������
* ������index:�˵�������,pbuf:����
* ���أ�0:�ɹ�,1:ʧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char optionDefaultName_set(unsigned char index,char* pbuf)
{
    if(index>MenuIndexMAX) return 1;
    memcpy(optionName_unselect[index-1],pbuf,strlen(pbuf)<40?strlen(pbuf):40);
    return 0;
}

/*********************************************************************************************
* ���ƣ�optionDefaultName_set()
* ���ܣ��˵��ѡ�������������
* ������index:�˵�������,pbuf:����
* ���أ�0:�ɹ�,1:ʧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char optionSelectName_set(unsigned char index,char* pbuf)
{
    if(index>MenuIndexMAX) return 1;
    memcpy(optionName_select[index-1],pbuf,strlen(pbuf)<40?strlen(pbuf):40);
    return 0;
}

/*********************************************************************************************
* ���ƣ�optionHintInfo_set
* ���ܣ��˵��ѡ�������ʾ��Ϣ
* ������index:�˵�������,pbuf:��Ϣ
* ���أ�0:�ɹ�,1:ʧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char optionHintInfo_set(unsigned char index,char* pbuf)
{
    if(index>MenuIndexMAX) return 1;
    memcpy(HintList[index],pbuf,strlen(pbuf)<40?strlen(pbuf):40);
    return 0;
}

/*********************************************************************************************
* ���ƣ�optionState_set
* ���ܣ��˵���״̬��־����
* ������index:�˵�������,state:���õ�״̬
* ���أ�0:�ɹ�,1:ʧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char optionState_set(unsigned char index,optionStatus_t state)
{
    if(index>MenuIndexMAX) return 1;
    menu.optionState[index-1]=state;
    return 0;
}

/*********************************************************************************************
* ���ƣ�tableExplain_set
* ���ܣ������Ϣ����
* ������index:�������,pbuf1:��Ϣ����
* ���أ�0:�ɹ�,1:ʧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char tableExplain_set(unsigned char index,char* pbuf1)
{
    if(index>5) return 1;
    memcpy(TableStruct.explain[index-1],pbuf1,strlen(pbuf1)<14?strlen(pbuf1):14);
    return 0;
}

/*********************************************************************************************
* ���ƣ�tableData_set
* ���ܣ������������
* ������index:�������,pbuf1:��������
* ���أ�0:�ɹ�,1:ʧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
char tableData_set(unsigned char index,char* pbuf1)
{
    if(index>5) return 1;
    memcpy(TableStruct.pdata[index-1],pbuf1,strlen(pbuf1)<14?strlen(pbuf1):14);
    return 0;
}

/*********************************************************************************************
�˵���ʾ
*********************************************************************************************/

//��ʾ��Ŀ-1
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

//��ʾ��Ŀ-3
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

void menuShowOption(unsigned short y,char* buf,unsigned short color,unsigned short backColor)
{
    LCD_File(301,y+1,313,y+19,backColor);
    LCD_DrawLine(5,y+1,313,y+1,backColor);
    LCD_DrawLine(5,y+18,313,y+18,backColor);
    LCD_DrawLine(6,y+19,314,y+19,LCD_COLOR_BLACK);
    LCD_DrawLine(314,y+2,314,y+18,LCD_COLOR_BLACK);
    LCDShowFont16(5,y+2,buf,296,color,backColor);
}

void menuShowOptionList(unsigned char refresh)
{
    u16 menuColor=LCD_COLOR_DARK_GREEN;
    u16 inverseColor=LCD_COLOR_BLUE;
    u16 tempColor = menuColor;
    short position_y=0;
    
    switch(hwType_Get())
    {
        case 1:
            menuColor = LCD_COLOR_RED;
            inverseColor = LCD_COLOR_DARK_BLUE;
            break;
        case 2:
            menuColor = LCD_COLOR_BLUE;
            inverseColor = LCD_COLOR_BROWN;
            break;
        case 3:
            menuColor = LCD_COLOR_DARK_GREEN;
            inverseColor = LCD_COLOR_DARK_BLUE;
            break;
        case 4:
            menuColor = LCD_COLOR_PURPLE;
            inverseColor = LCD_COLOR_DARK_BLUE;
            break;
    }
    
    if(refresh&0x80)
    {
        LCDClear(LCD_COLOR_WHITE);
        lcdShowMenuBar_1(0,"ϵͳ����",LCD_COLOR_WHITE,menuColor,refresh,1);
    }
    
    for(unsigned char i=0;i<MenuIndexMAX;i++)
    {
        position_y+=20;
        tempColor = (menu.index==i+1) ? inverseColor : menuColor;
        menu.optionName[i] = (char*)(menu.optionState[i]==SELECT ? optionName_select[i] : optionName_unselect[i]);
        menuShowOption(position_y,menu.optionName[i],LCD_COLOR_WHITE,tempColor);
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
    lcdShowMenuBar_1(100,(char*)HintList[hintIndex],LCD_COLOR_RED,LCD_COLOR_YELLOW,refresh,0);
    refresh &= ~0X80;
}

void lcdShowTable(unsigned char refresh)
{
    unsigned char i,mode;
    unsigned short y=120;
    
    if(refresh&0x80)
    {
        lcdShowMenuBar_3(y,"�˿�����","�˿�˵��","�˿�����",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
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
    xLabPageShow(refresh);
}

/*********************************************************************************************
�˵�����
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
��������ʾ����
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