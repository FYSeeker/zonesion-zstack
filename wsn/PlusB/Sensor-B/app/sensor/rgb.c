#include "rgb.h"

void rgb_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(LEDR_RCC, ENABLE);
    RCC_AHB1PeriphClockCmd(LEDG_RCC, ENABLE);
    RCC_AHB1PeriphClockCmd(LEDB_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = LEDR_PIN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LEDR_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LEDG_PIN;
    GPIO_Init(LEDG_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LEDB_PIN;
    GPIO_Init(LEDB_GPIO, &GPIO_InitStructure);
    
    rgb_off(0x07);
}

void rgb_on(unsigned char cmd)
{
    if((cmd & 0x01) == 0x01)
    {
        GPIO_ResetBits(LEDR_GPIO, LEDR_PIN);
    }
    if((cmd & 0x02) == 0x02)
    {
        GPIO_ResetBits(LEDG_GPIO, LEDG_PIN);
    }
    if((cmd & 0x04) == 0x04)
    {
        GPIO_ResetBits(LEDB_GPIO, LEDB_PIN);
    }
}

void rgb_off(unsigned char cmd)
{
    if((cmd & 0x01) == 0x01)
    {
        GPIO_SetBits(LEDR_GPIO, LEDR_PIN);
    }
    if((cmd & 0x02) == 0x02)
    {
        GPIO_SetBits(LEDG_GPIO, LEDG_PIN);
    }
    if((cmd & 0x04) == 0x04)
    {
        GPIO_SetBits(LEDB_GPIO, LEDB_PIN);
    }
}

unsigned char get_rgb(void)
{
    unsigned char data = 0;
    if(GPIO_ReadInputDataBit(LEDR_GPIO, LEDR_PIN) == 0)
    {
        data |= 0x01;
    }
    else
    {
        data &= ~0x01;
    }
    if(GPIO_ReadInputDataBit(LEDG_GPIO, LEDG_PIN) == 0)
    {
        data |= 0x02;
    }
    else
    {
        data &= ~0x02;
    }
    if(GPIO_ReadInputDataBit(LEDB_GPIO, LEDB_PIN) == 0)
    {
        data |= 0x04;
    }
    else
    {
        data &= ~0x04;
    }
    return data;
}


