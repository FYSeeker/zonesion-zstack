/*********************************************************************************************
* �ļ���sensor.c
* ���ߣ�fuyou 2018.10.24
* ˵����GreenHouse�ڵ�C��������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "sensor.h"
/*********************************************************************************************
* �궨��
*********************************************************************************************/

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
static uint8  D0 = 0xFF;                                        // Ĭ�ϴ������ϱ�����
static uint8  D1 = 0;                                           // �̵�����ʼ״̬Ϊȫ��
static char  A0[16]={0};                                       // A0�洢����
static uint16 V0 = 30;                                          // V0����Ϊ�ϱ�ʱ������Ĭ��Ϊ30s
uint8 V1 = 0;                                                   // Ƶ��
uint8 V2 = 0;                                                   // ����
uint8 oledDisplay = 0;
uint16 cardTick=0;
uint16 ElectronicLockTick=0;
uint16 BeepTick=0;
/*********************************************************************************************
* ���ƣ�updateV0()
* ���ܣ�����V0��ֵ
* ������*val -- �����µı���
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void updateV0(char *val)
{
    //���ַ�������val����ת��Ϊ���ͱ�����ֵ
    V0 = atoi(val);                                 // ��ȡ�����ϱ�ʱ�����ֵ
}
/*********************************************************************************************
* ���ƣ�sensorInit()
* ���ܣ�������Ӳ����ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorInit(void)
{
    led_init();                                                 // LED�Ƴ�ʼ��
    beep_init();                                                // ��������ʼ��
    ElectronicLock_init();                                      // ��������ʼ��
    OLED_Init();
    RFID7941Init();                                             // RFID��ʼ��

    // ������ʱ���������������ϱ������¼���MY_REPORT_EVT
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
    osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
}
/*********************************************************************************************
* ���ƣ�sensorLinkOn()
* ���ܣ��������ڵ������ɹ����ú���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorLinkOn(void)
{
    sensorUpdate();
}
/*********************************************************************************************
* ���ƣ�sensorUpdate()
* ���ܣ����������ϱ�������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorUpdate(void)
{
    char pData[16];
    char *p = pData;

    ZXBeeBegin();

    sprintf(p, "%u", D1);                                  // �ϱ����Ʊ���
    ZXBeeAdd("D1", p);

    p = ZXBeeEnd();                                               // ��������֡��ʽ��β
    if (p != NULL)
    {
        ZXBeeInfSend(p, strlen(p));	                                // ����Ҫ�ϴ������ݽ��д����������ͨ��zb_SendDataRequest()���͵�Э����
    }
}

/*********************************************************************************************
* ���ƣ�idCardHandle()
* ���ܣ�id���������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void idCardHandle()
{
    char idID[5]= {0};
    static char lastIdID[5]= {0};

    if (RFID7941CheckCard125kHzRsp(idID) > 0)                   //��ͬһ�ſ�Ƭ
    {
        if (memcmp(lastIdID, idID, 5) != 0)
        {
            sprintf(A0, "%02X%02X%02X%02X%02X", idID[0],idID[1],idID[2],idID[3],idID[4]);
            ZXBeeAdd("A0", A0);
            memcpy(lastIdID, idID, 5);
            
            BeepTick=3;
        }
        cardTick = 5;
    }
    else
    {
        memcpy(lastIdID, 0, 5);
    }
    RFID7941CheckCard125kHzReq();
}

/*********************************************************************************************
* ���ƣ�icCardHandle()
* ���ܣ�ic���������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void icCardHandle()
{
    char icID[16]= {0};
    static char lastIcID[16]= {0};

    if (RFID7941CheckCard1356MHzRsp(icID) > 0)
    {
        if (memcmp(lastIcID, icID, 4) != 0)                    //��ͬһ�ſ�Ƭ
        {
            sprintf(A0,"%02X%02X%02X%02X",
                    icID[0],icID[1],icID[2],icID[3]);
            ZXBeeAdd("A0", A0);
            memcpy(lastIcID, icID, 4);
            
            BeepTick=3;
        }
        cardTick = 5;
    }
    else
    {
        memcpy(lastIcID, 0, 4);
    }
    RFID7941CheckCard1356MHzReq();
}

/*********************************************************************************************
* ���ƣ�sensorCheck()
* ���ܣ����������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorCheck(void)
{
    char buf[32]={0};
    static uint8 cardIndex = 0;
    
    ZXBeeBegin();
    switch(cardIndex)
    {
        case 1:
            icCardHandle();
            break;
        default:
            idCardHandle();
    }
    char *p = ZXBeeEnd();
    if (p != NULL)
    {
        ZXBeeInfSend(p, strlen(p));
    }

    if(cardTick>0)
    {
        cardTick--;
    }
    else
    {
        cardTick=5;
        cardIndex++;
        if(cardIndex>1)
            cardIndex=0;
    }
    
    if(BeepTick>1)
    {
        BeepTick--;
        beep_on();
    }
    else if(BeepTick==1)
    {
        BeepTick--;
        beep_off();
    }
    
    if(oledDisplay)
    {
        sprintf(buf,"%3u",V1);
        OLED_ShowString(95-18,0,(uint8*)buf,8);
        sprintf(buf,"%3u",V2);
        OLED_ShowString(95-18,3,(uint8*)buf,8);
        OLED_ShowString(48-8,1,"TV",16);
    }
}
/*********************************************************************************************
* ���ƣ�sensorControl()
* ���ܣ�����������
* ������cmd - ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensorControl(uint8 cmd)
{
    // ����cmd���������Ӧ�Ŀ��Ƴ���
    if(cmd&0x01)
        led_on(0x01);
    else
        led_off(0x01);
    
    if(cmd&0x02)
        led_on(0x02);
    else
        led_off(0x02);
    
    if(cmd&0x04)
        led_on(0x04);
    else
        led_off(0x04);
    
    if(cmd&0x08)
        led_on(0x08);
    else
        led_off(0x08);
    
    if(cmd&0x10)
        beep_on();
    else
        beep_off();
    
    if(cmd&0x20)
    {
        if(ElectronicLockTick==0)
        {
            ElectronicLockTick = 10*2;
            ElectronicLock_on();
        }
    }
    else
    {
        ElectronicLock_off();
    }
    
    if(cmd&0x40)
    {
        if(oledDisplay==0)
        {
            oledDisplay=1;
            OLED_PEN_COLOR=BLACK;
            OLED_Clear();
        }
    }
    else
    {
        oledDisplay=0;
        OLED_PEN_COLOR=WHITE;
        OLED_Clear();
    }
}
/*********************************************************************************************
* ���ƣ�ZXBeeUserProcess()
* ���ܣ������յ��Ŀ�������
* ������*ptag -- ������������
*       *pval -- �����������
* ���أ�ret -- p�ַ�������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int ZXBeeUserProcess(char *ptag, char *pval)
{
    int val;
    int ret = 0;
    char pData[16];
    char *p = pData;

    // ���ַ�������pval����ת��Ϊ���ͱ�����ֵ
    val = atoi(pval);
    // �����������
    if (0 == strcmp("CD0", ptag))                                 // ��D0��λ���в�����CD0��ʾλ�������
    {
        D0 &= ~val;
    }
    if (0 == strcmp("OD0", ptag))                                 // ��D0��λ���в�����OD0��ʾλ��һ����
    {
        D0 |= val;
    }
    if (0 == strcmp("D0", ptag))                                  // ��ѯ�ϱ�ʹ�ܱ���
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", D0);
            ZXBeeAdd("D0", p);
        }
    }
    if (0 == strcmp("CD1", ptag))                                 // ��D1��λ���в�����CD1��ʾλ�������
    {
        D1 &= ~val;
        sensorControl(D1);                                          // ����ִ������
    }
    if (0 == strcmp("OD1", ptag))                                 // ��D1��λ���в�����OD1��ʾλ��һ����
    {
        D1 |= val;
        sensorControl(D1);                                          // ����ִ������
    }
    if (0 == strcmp("D1", ptag))                                  // ��ѯִ�����������
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", D1);
            ZXBeeAdd("D1", p);
        }
    }
    if (0 == strcmp("V0", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", V0);                         	// �ϱ�ʱ����
            ZXBeeAdd("V0", p);
        }
        else
        {
            updateV0(pval);
        }
    }
    if (0 == strcmp("V1", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", V1);                         	// �ϱ�ʱ����
            ZXBeeAdd("V1", p);
        }
        else
        {
            V1 = val;
        }
    }
    if (0 == strcmp("V2", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", V2);                         	// �ϱ�ʱ����
            ZXBeeAdd("V2", p);
        }
        else
        {
            V2 = val;
            if(V2>100) V2=100;
        }
    }
    return ret;
}
/*********************************************************************************************
* ���ƣ�MyEventProcess()
* ���ܣ��Զ����¼�����
* ������event -- �¼����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void MyEventProcess( uint16 event )
{
    if (event & MY_REPORT_EVT)
    {
        sensorUpdate();
        //������ʱ���������¼���MY_REPORT_EVT
        osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, V0*1000);
    }
    if(event & MY_CHECK_EVT)
    {
        sensorCheck();
        osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
    }
}