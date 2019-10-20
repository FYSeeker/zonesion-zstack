/*********************************************************************************************
* �ļ���htu21d.c
* ���ߣ�zonesion
* ˵����htu21d��������
* �޸ģ�Chenkm 2017.01.10 �޸Ĵ����ʽ�����Ӵ���ע�ͺ��ļ�˵��
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include <math.h>
#include <stdio.h>
#include "ext_htu21d.h"
#include "iic.h"
#include "delay.h"

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
unsigned char flag;

/*********************************************************************************************
* ���ƣ�htu21d_init()
* ���ܣ�htu21d��ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void ext_htu21d_init(void)
{
    iic_init();		                                        //I2C��ʼ��
    iic_start();		                                        //����I2C
    iic_write_byte(EXT_HTU21DADDR&0xfe);		                //дSHT21��I2C��ַ
    iic_write_byte(0xfe);
    iic_stop();		                                        //ֹͣI2C
}

/*********************************************************************************************
* ���ƣ�htu21d_read_reg()
* ���ܣ�htu21��ȡ�Ĵ���
* ������cmd -- �Ĵ�����ַ
* ���أ�data �Ĵ�������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char ext_htu21d_read_reg(unsigned char cmd)
{
    unsigned char data = 0; 
    iic_start();
    if(iic_write_byte(EXT_HTU21DADDR & 0xfe) == 0){
        if(iic_write_byte(cmd) == 0){
            do{
                delay(5);
                iic_start();	
            }
            while(iic_write_byte(EXT_HTU21DADDR | 0x01) == 1);
            data = iic_read_byte(0);
            iic_stop();
        }
    }
    return data;
}

/*********************************************************************************************
* ���ƣ�htu21d_get_data()
* ���ܣ�htu21d������ʪ��
* ������order -- ָ��
* ���أ�temperature -- �¶�ֵ  humidity -- ʪ��ֵ
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int ext_htu21d_get_data(unsigned char order)
{
    float temp = 0,TH = 0;
    unsigned char MSB,LSB;
    unsigned short humidity,temperature; 
    iic_start();
    if(iic_write_byte(EXT_HTU21DADDR & 0xfe) == 0){
        if(iic_write_byte(order) == 0){
            do{
                delay(20);
                iic_start();	
            }
            while(iic_write_byte(EXT_HTU21DADDR | 0x01) == 1);
            MSB = iic_read_byte(0);
            delay(5);
            LSB = iic_read_byte(0);
            iic_read_byte(1);
            iic_stop();
            LSB &= 0xfc;
            temp = MSB*256+LSB;
            if (order == 0xf3){                                       //���������¶ȼ��
                TH=(175.72)*temp/65536-46.85;                           //�¶�:T= -46.85 + 175.72 * ST/2^16
                temperature =(unsigned short)(fabs(TH)*100);
                if(TH >= 0)
                    flag = 0;
                else
                    flag = 1;
                return temperature;	
            }else{
                TH = (temp*125)/65536-6;
                humidity = (unsigned short)(fabs(TH)*100);                //ʪ��: RH%= -6 + 125 * SRH/2^16
                return humidity;
            }
        }
    }
    return 0;
}