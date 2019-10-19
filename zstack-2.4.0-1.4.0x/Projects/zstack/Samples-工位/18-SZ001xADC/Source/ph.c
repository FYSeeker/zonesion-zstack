#include "ph.h"
#include  "hal_adc.h"

/*********************************************************************************************
* 名称：get_ph()
* 功能：获取传感器数据
* 参数：无
* 返回：ph_value -- 数据
* 修改：
* 注释：
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
* 名称：get_ph_x()
* 功能：获取传感器数据平均值
* 参数：num -- 获取数据个数
* 返回：ph_value -- num个数据的平均值
* 修改：
* 注释：
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
