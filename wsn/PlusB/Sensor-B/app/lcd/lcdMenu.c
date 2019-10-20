/*********************************************************************************************
* �ļ���lcdMenu.c
* ���ߣ�fuyou 2018.11.1
* ˵����ʵ�ֲ˵���ܿ�ܣ��ṩ�˵����API����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "lcdMenu.h"


_SensorBuf_t sensorDataBuf={0};

char optionName_select[MenuIndexMAX][TextLen+1]={"ѡ��ʾ��"};
char optionName_unselect[MenuIndexMAX][40]={"ѡ��ʾ��"};
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
    memcpy(optionName_unselect[index-1],pbuf,strlen(pbuf)<TextLen?strlen(pbuf):TextLen);
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
        lcdShowMenuBar_1(0,"xLab��Ϣ",LCD_COLOR_WHITE,LCD_COLOR_RED,refresh,1);
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
    menuShowOptionList(refresh);
    menuShowHint(refresh);
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