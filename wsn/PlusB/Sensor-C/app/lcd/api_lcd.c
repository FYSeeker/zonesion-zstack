/*********************************************************************************************
* �ļ���api_lcd.c
* ���ߣ�fuyou 2018.11.1
* ˵����lcd�봫�������޸Ĳ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "api_lcd.h"


extern unsigned char D1;                                           // Ĭ�Ϲرտ����ഫ����
extern char* V1;
extern unsigned char A0;                                           // ����/����������
extern unsigned char A1;                                           // �񶯴�����
extern unsigned char A2;                                           // ����������
extern unsigned char A3;                                           // ���洫����
extern unsigned char A4;                                           // ȼ��������
extern unsigned char A5;                                           // ��դ������
extern unsigned char CurrMode;                                     // ��������ǰ����ģʽ
extern void sensor_control(unsigned char cmd);

void xLabInfoShow(unsigned short y,unsigned short wordColor,unsigned short backColor,unsigned char refresh)
{
    char pbuf[10]={0};
    if(refresh&0x80)
    {
        lcdShowBarX1(y,"��������Ϣ",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
        lcdShowBarX3(y+20,"�ͺ�","����ģʽ","-",wordColor,backColor,refresh,1);
    }
    if(CurrMode==1)
        sprintf(pbuf,"ģʽһ");
    else if(CurrMode==2)
        sprintf(pbuf,"ģʽ��");
    else
        sprintf(pbuf,"δȷ��");
        
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
        lcdShowMenuBar_1(0,"��Ϣҳ��",LCD_COLOR_WHITE,LCD_COLOR_RED,refresh,1);
    }
    lcdShowMenuBar_1(y+=40,"����������",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,1);
    
    
    if(CurrMode==1)
    {
        if(A0)
            sprintf(pbuf1,"����");
        else
            sprintf(pbuf1,"����");
        if(A1)
            sprintf(pbuf2,"����");
        else
            sprintf(pbuf2," ��ֹ ");
        lcdShowBarX4(y+=20,"�������","��",pbuf1,pbuf2,penColor,backColor,refresh,1);
    }
    else if(CurrMode==2)
    {
        if(A0)
            sprintf(pbuf1,"������");
        else
            sprintf(pbuf1,"δ����");
        lcdShowBarX4(y+=20,"����","-",pbuf1,"-",penColor,backColor,refresh,1);
    }
    else
        lcdShowBarX4(y+=20,"ģʽδ֪","ģʽδ֪","-","-",penColor,backColor,refresh,1);
    
    sprintf(pbuf1,"%u",A2);
    sprintf(pbuf2,"%u",A3);
    if(CurrMode==1)
    {
        if(A2)
            sprintf(pbuf1,"��������");
        else
            sprintf(pbuf1," �޴��� ");
        if(A3)
            sprintf(pbuf2,"������");
        else
            sprintf(pbuf2,"������");
        lcdShowBarX4(y+=20,"����","����",pbuf1,pbuf2,penColor,backColor,refresh,1);
    }
    else if(CurrMode==2)
        lcdShowBarX4(y+=20,"-","-","-","-",penColor,backColor,refresh,1);
    else
        lcdShowBarX4(y+=20,"ģʽδ֪","ģʽδ֪","-","-",penColor,backColor,refresh,1);
    
    if(A4)
        sprintf(pbuf1,"ȼ������");
    else
        sprintf(pbuf1,"ȼ������");
    if(A5)
        sprintf(pbuf2,"���ڵ�");
    else
        sprintf(pbuf2,"���ڵ�");
    lcdShowBarX4(y+=20,"ȼ��","��դ",pbuf1,pbuf2,penColor,backColor,refresh,1);
    
    if(CurrMode==1)
    {
        sprintf(pbuf1,"��ģʽ�²�����");
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
        sprintf(pbuf1,"ģʽδȷ��");
    }
    lcdShowMenuBar_1(y+=40,"������Ϣ",LCD_COLOR_WHITE,LCD_COLOR_BLACK,refresh,0);
    lcdShowMenuBar_1(y+=20,pbuf1,penColor,backColor,refresh,1);
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
* ���ƣ�syncSensorStatus()
* ���ܣ�ͬ�������ഫ����״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ������ഫ�������ϲ����ú���Ҫ���д˺���ͬ����Ļ�ϵ���ʾ״̬
*********************************************************************************************/
void syncSensorStatus()
{
    if(D1&0x40)
        optionState_set(2,SELECT);      //����ѡ��2Ϊѡ��״̬
    else
        optionState_set(2,UNSELECT);    //����ѡ��2Ϊȡ��ѡ��״̬
    
    if(D1&0x80)
        optionState_set(3,SELECT);      //����ѡ��3Ϊѡ��״̬
    else
        optionState_set(3,UNSELECT);    //����ѡ��3Ϊȡ��ѡ��״̬
}

/*********************************************************************************************
* ���ƣ�lcdSensor_poll()
* ���ܣ�lcd�봫����������ѯ
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ���ѯ���ڴ��������ݵĸ���
*********************************************************************************************/
void lcdSensor_poll()
{
    char pbuf1[16]={0};
    char pbuf2[16]={0};
    char pbuf3[16]={0};
    
    //��������/״̬
    if(D1&0x40)
        sprintf(pbuf1,"��");
    else
        sprintf(pbuf1,"�ر�");
    
    if(D1&0x80)
        sprintf(pbuf2,"��");
    else
        sprintf(pbuf2,"�ر�");
    
    sprintf(pbuf3,"%3u",A0);
    
    //����ҳ��1�Ĵ���������
    SensorData_set(pbuf1,pbuf2,pbuf3);
    //����ҳ��2�ı������
    tableData_set(1,"-");
    tableData_set(2,"-");
    tableData_set(3,pbuf1);
    tableData_set(4,pbuf2);
    tableData_set(5,pbuf3);
    //ͬ�������ഫ������ʾ״̬
    syncSensorStatus();
}

