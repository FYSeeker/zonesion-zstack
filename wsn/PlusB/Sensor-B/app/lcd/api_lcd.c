/*********************************************************************************************
* �ļ���api_lcd.c
* ���ߣ�fuyou 2018.11.1
* ˵����lcd�봫�������޸Ĳ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "api_lcd.h"


extern unsigned char D1;                                           // Ĭ�Ϲرտ����ഫ����
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
        D1 |= (1<<7);
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(1<<7);
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
        D1 |= (1<<4);
    }
    else if(status==UNSELECT)
    {
        D1 &= ~(1<<4);
    }
    sensor_control(D1);
}

/*********************************************************************************************
* ���ƣ�option_4_Handle()
* ���ܣ��˵���4�Ĵ�����
* ������status:�˵����ѡ��״̬-
* ���أ���
* �޸ģ�
* ע�ͣ��Զ���˵���4��ѡ������еĲ���
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
* ���ƣ�option_5_Handle()
* ���ܣ��˵���5�Ĵ�����
* ������status:�˵����ѡ��״̬
* ���أ���
* �޸ģ�
* ע�ͣ��Զ���˵���5��ѡ������еĲ���
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
* ���ƣ�option_6_Handle()
* ���ܣ��˵���6�Ĵ�����
* ������status:�˵����ѡ��״̬
* ���أ���
* �޸ģ�
* ע�ͣ��Զ���˵���5��ѡ������еĲ���
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
    optionDefaultName_set(1,"�򿪼̵���һ");          //�˵���Ĭ������
    optionCallFunc_set(1,option_1_Handle);      //�˵��ѡ����Ĵ�����
    
    //�˵���2�ĳ�ʼ��
    optionDefaultName_set(2,"�򿪼̵�����");          //�˵���Ĭ������
    optionCallFunc_set(2,option_2_Handle);      //�˵��ѡ����Ĵ�����

    optionDefaultName_set(3,"��LED1");
    optionCallFunc_set(3,option_3_Handle);

    optionDefaultName_set(4,"��LED2");
    optionCallFunc_set(4,option_4_Handle);
    
    if(CurrMode==1)
    {
        optionDefaultName_set(5,"�򿪲������");
        optionCallFunc_set(5,option_5_Handle);
        
        optionDefaultName_set(6,"�򿪷���");
        optionCallFunc_set(6,option_6_Handle);
    }
    else if(CurrMode==2)
    {
        optionDefaultName_set(5,"�л�RGB��");
        optionCallFunc_set(5,option_5_Handle);
        
        optionDefaultName_set(6,"�򿪷�����");
        optionCallFunc_set(6,option_6_Handle);
    }
    else
    {
        optionDefaultName_set(5,"ģʽδȷ��");
        optionDefaultName_set(6,"ģʽδȷ��");
    }
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
        optionState_set(1,SELECT);      //����ѡ��1Ϊѡ��״̬
    else
        optionState_set(1,UNSELECT);    //����ѡ��1Ϊȡ��ѡ��״̬
    
    if(D1&0x80)
        optionState_set(2,SELECT);      //����ѡ��2Ϊѡ��״̬
    else
        optionState_set(2,UNSELECT);    //����ѡ��2Ϊȡ��ѡ��״̬
    
    if(D1&(1<<4))
        optionState_set(3,SELECT);      //����ѡ��3Ϊѡ��״̬
    else
        optionState_set(3,UNSELECT);    //����ѡ��3Ϊȡ��ѡ��״̬
    
    if(D1&(1<<5))
        optionState_set(4,SELECT);      //����ѡ��4Ϊѡ��״̬
    else
        optionState_set(4,UNSELECT);    //����ѡ��4Ϊȡ��ѡ��״̬
    
    
    if(CurrMode==1)
    {
        if(D1&(1<<2))
            optionState_set(5,SELECT);      //����ѡ��5Ϊѡ��״̬
        else
            optionState_set(5,UNSELECT);    //����ѡ��5Ϊȡ��ѡ��״̬
    }
    else if(CurrMode==2)
    {
        if(D1&(0X03))
            optionState_set(5,SELECT);      //����ѡ��5Ϊѡ��״̬
        else
            optionState_set(5,UNSELECT);    //����ѡ��5Ϊȡ��ѡ��״̬
    }
    if(D1&(1<<3))
        optionState_set(6,SELECT);      //����ѡ��6Ϊѡ��״̬
    else
        optionState_set(6,UNSELECT);    //����ѡ��6Ϊȡ��ѡ��״̬
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
    //ͬ�������ഫ������ʾ״̬
    syncSensorStatus();
}

