/*********************************************************************************************
* 文件：time.c
* 作者：fuyou
* 说明：cc2530定时器驱动
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "time.h"
#include "stdio.h"
#include "hal_adc.h"


/*********************************************************************************************
* 名称：time1Int_init
* 功能：定时器1中断初始化
* 参数：t1Arr：定时时间ms，取值范围：1--65ms
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void time1Int_init(unsigned int t1Arr)
{
    if(t1Arr>65) t1Arr = 65;
    t1Arr*=1000;
    
    T1CTL |= (1<<1);                                            //模计数，0--T1CC0
    T1CTL |= (1<<3);                                            //32分频
    
    T1CC0L = t1Arr&0xff;
    T1CC0H = (t1Arr>>8)&0xff;
    T1CCTL0 |= (1<<2);                                          //定时器设为比较模式

    //设置中断优先级最低
    IP0 &= ~(1<<1);
    IP1 &= ~(1<<1);
    
    IEN1 |= 0X02;                                               //定时器1中断使能
    EA=1;                                                       //开总中断 
}


/*********************************************************************************************
* 名称：T1_ISR
* 功能：定时器1中断服务程序
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
extern uint16 windSpeedCount,rainfallCount;
extern float windSpeed,rainfall;                                       //中断时间10s
#define Pi      3.1415
#define INT_TIME_SECOND     20
#define INT_TIME_WIND       (INT_TIME_SECOND*5)
#define INT_TIME_RAIN       (INT_TIME_SECOND*10)   

#pragma vector = T1_VECTOR      
__interrupt void T1_ISR(void)            
{  
    static unsigned short t1_count=0;
    
    if((t1_count%(INT_TIME_WIND))==0)
    {    
        //计算风速:s=r*w,w=2*pi*f
        windSpeed = (2.0*Pi*0.09*(float)(windSpeedCount/2.0))/5.0; 
        windSpeedCount = 0;
    }
    
    if((t1_count%(INT_TIME_RAIN))==0)
    {
        rainfall = rainfallCount;                               //获取降雨量
        rainfallCount = 0;
    }
    
    t1_count++;
    if(t1_count>59999) t1_count=0;
    
    T1IF=0;                                                     //清除中断标志位  
}