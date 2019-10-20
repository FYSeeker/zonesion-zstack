#include "ext_led.h"

void ext_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(EXT_LED1_RCC, ENABLE);
    RCC_AHB1PeriphClockCmd(EXT_LED2_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = EXT_LED1_PIN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(EXT_LED1_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = EXT_LED2_PIN;
    GPIO_Init(EXT_LED2_GPIO, &GPIO_InitStructure);
    
    ext_led_off(0x03);
}

void ext_led_on(unsigned char cmd)
{
    if((cmd & 0x01) == 0x01)
    {
        GPIO_ResetBits(EXT_LED1_GPIO, EXT_LED1_PIN);
    }
    if((cmd & 0x02) == 0x02)
    {
        GPIO_ResetBits(EXT_LED2_GPIO, EXT_LED2_PIN);
    }
}

void ext_led_off(unsigned char cmd)
{
    if((cmd & 0x01) == 0x01)
    {
        GPIO_SetBits(EXT_LED1_GPIO, EXT_LED1_PIN);
    }
    if((cmd & 0x02) == 0x02)
    {
        GPIO_SetBits(EXT_LED2_GPIO, EXT_LED2_PIN);
    }
}

unsigned char get_ext_led(void)
{
    unsigned char data = 0;
    if(GPIO_ReadInputDataBit(EXT_LED1_GPIO, EXT_LED1_PIN) == 0)
    {
        data |= 0x01;
    }
    else
    {
        data &= ~0x01;
    }
    if(GPIO_ReadInputDataBit(EXT_LED1_GPIO, EXT_LED1_PIN) == 0)
    {
        data |= 0x02;
    }
    else
    {
        data &= ~0x02;
    }
    return data;
}



