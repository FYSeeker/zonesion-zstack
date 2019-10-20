/*********************************************************************************************
* �ļ���api_lcd.c
* ���ߣ�fuyou 2018.11.1
* ˵����lcd�봫�������޸Ĳ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "api_lcd.h"

extern float A0;                                                 // �¶�
extern float A1;                                                 // ʪ��
extern float A2;                                                 // ����ǿ��
extern float A3;                                                   // ������������ֵ
extern float A4;                                                 // ����ѹ��ֵ
extern unsigned char A5;                                           // ����������
extern float A6;                                                   // ������
extern int A7;                                        

extern char D1;
extern void sensor_control(unsigned char cmd);

void xLabInfoShow(unsigned short y,unsigned short wordColor,unsigned short backColor,unsigned char refresh)
{
    if(refresh&0x80)
    {
        lcdShowBarX1(y,"��������Ϣ",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
        lcdShowBarX3(y+20,"�ͺ�","����ģʽ","-",wordColor,backColor,refresh,1);
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
        lcdShowMenuBar_1(0,"��Ϣҳ��",LCD_COLOR_WHITE,LCD_COLOR_RED,refresh,1);
    }
    lcdShowMenuBar_1(y+=30,"����������",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,1);
    
    sprintf(pbuf1,"%5.1f",A0);
    sprintf(pbuf2,"%5.1f",A1);
    lcdShowBarX4(y+=20,"�¶�","ʪ��",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    sprintf(pbuf1,"%5.1f",A2);
    sprintf(pbuf2,"%5.1f",A3);
    lcdShowBarX4(y+=20,"����ǿ��","��������",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    sprintf(pbuf1,"%5.1f",A4);
    sprintf(pbuf2,"%5.1f",A6);
    lcdShowBarX4(y+=20,"����ѹ��","������",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    if(A5)
        sprintf(pbuf1,"���˵����ˣ�Σ�գ�");
    else
        sprintf(pbuf1,"û���˵�������ȫ��");
    
    lcdShowMenuBar_1(y+=30,"�������",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
    lcdShowMenuBar_1(y+=20,pbuf1,penColor,backColor,refresh,1);
    
    switch(A7)
    {
        case 1:
            sprintf(pbuf1,"��̨��");
            break;
        case 2:
            sprintf(pbuf1,"�ر�̨��");
            break;
        case 3:
            sprintf(pbuf1,"�򿪴���");
            break;
        case 4:
            sprintf(pbuf1,"�رմ���");
            break;
        case 5:
            sprintf(pbuf1,"�򿪷���");
            break;
        case 6:
            sprintf(pbuf1,"�رշ���");
            break;
        case 7:
            sprintf(pbuf1,"�򿪿յ�");
            break;
        case 8:
            sprintf(pbuf1,"�رտյ�");
            break;
        case 9:
            sprintf(pbuf1,"�򿪼�ʪ��");
            break;
        case 10:
            sprintf(pbuf1,"�رռ�ʪ��");
            break;
        case 11:
            sprintf(pbuf1,"�Զ�ģʽ");
            break;
        case 12:
            sprintf(pbuf1,"�ֶ�ģʽ");
            break;
    }
    lcdShowMenuBar_1(y+=30,"������Ϣ",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
    lcdShowMenuBar_1(y+=20," ",penColor,backColor,refresh,1);
    
}


/*********************************************************************************************
* ���ƣ�option_1_Handle()
* ���ܣ��˵���1�Ĵ�����
* ������status:�˵����ѡ��״̬
* ���أ���
* �޸ģ�
* ע�ͣ��Զ���˵���1��ѡ������еĲ���
*********************************************************************************************/
void option_1_Handle(optionStatus_t status)
{
    menuExitHandle();
}

/*********************************************************************************************
* ���ƣ�option_2_Handle()
* ���ܣ��˵���2�Ĵ�����
* ������status:�˵����ѡ��״̬
* ���أ���
* �޸ģ�
* ע�ͣ��Զ���˵���2��ѡ������еĲ���
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
* ���ƣ�option_3_Handle()
* ���ܣ��˵���3�Ĵ�����
* ������status:�˵����ѡ��״̬
* ���أ���
* �޸ģ�
* ע�ͣ��Զ���˵���3��ѡ������еĲ���
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
* ���ƣ�MenuOption_init()
* ���ܣ��˵����ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ��Զ���˵�������ݺ͹���
*********************************************************************************************/
void MenuOption_init()
{
    //�˵���1�ĳ�ʼ��
    optionDefaultName_set(1," 1-����������");     //�˵���Ĭ������
    optionSelectName_set(1," 1-����������");      //�˵��ѡ���������
    optionHintInfo_set(1," ");                  //�˵��ѡ�������ʾ��Ϣ���ո����ʲô������ʾ
    optionCallFunc_set(1,option_1_Handle);      //�˵��ѡ����Ĵ�����
    
    //�˵���2�ĳ�ʼ��
    optionDefaultName_set(2," 2-�򿪼̵���һ");   //�˵���Ĭ������
    optionSelectName_set(2," 2-�رռ̵���һ");    //�˵��ѡ���������
    optionHintInfo_set(2,"�̵���һ�Ѵ�");        //�˵��ѡ�������ʾ��Ϣ
    optionCallFunc_set(2,option_2_Handle);      //�˵��ѡ����Ĵ�����
    
    //�˵���3�ĳ�ʼ��
    optionDefaultName_set(3," 3-�򿪼̵�����");
    optionSelectName_set(3," 3-�رռ̵�����");
    optionHintInfo_set(3,"�̵������Ѵ�");
    optionCallFunc_set(3,option_3_Handle);
}

/*********************************************************************************************
* ���ƣ�TableInfo_init()
* ���ܣ������Ϣ��ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ��Զ�������Ҫ��ʾ����Ϣ
*********************************************************************************************/
void TableInfo_init()
{
    tableExplain_set(1,"���߽ӿ�");
    tableExplain_set(2,"-");
    tableExplain_set(3,"�̵���һ");
    tableExplain_set(4,"�̵�����");
    tableExplain_set(5,"ʾ��������");
}

/*********************************************************************************************
* ���ƣ�lcdSensor_init()
* ���ܣ�lcd�봫�������ֳ�ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void lcdSensor_init()
{
    MenuOption_init();
    TableInfo_init();
    SensorName_set("�̵���һ","�̵�����","ʾ��������");
}

/*********************************************************************************************
* ���ƣ�lcdSensor_poll()
* ���ܣ�lcd�봫����������ѯ
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ���ѯ���д��������ݵĸ���
*********************************************************************************************/
void lcdSensor_poll()
{
}

