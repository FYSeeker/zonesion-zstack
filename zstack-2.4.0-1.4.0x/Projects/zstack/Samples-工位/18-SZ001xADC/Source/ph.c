#include "ph.h"
#include  "hal_adc.h"

/*********************************************************************************************
* ���ƣ�get_ph()
* ���ܣ���ȡ����������
* ��������
* ���أ�ph_value -- ����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
float get_ph()
{  
    u16 adcValue=0;
    float voltage=0;
    float ph_value=0;
    
    adcValue = HalAdcRead(5,HAL_ADC_RESOLUTION_12);
    voltage = ((float)adcValue * (3.3/2047.0)) / 10.0 * 16.8;
    ph_value = 23.925 - voltage*5.6409;
    
    if(ph_value<0) ph_value=0;
    else if(ph_value>14) ph_value=14;
    
    return ph_value;
}


/*********************************************************************************************
* ���ƣ�get_ph_x()
* ���ܣ���ȡ����������ƽ��ֵ
* ������num -- ��ȡ���ݸ���
* ���أ�ph_value -- num�����ݵ�ƽ��ֵ
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
float get_ph_x(u8 num)
{
    u8 i=0;
    float ph_value=0;
    
    for(i=0;i<num;i++)
    {
        ph_value += get_ph();
    }
    ph_value = ph_value/num;
    
    return ph_value;
}
