#include "beep.h"

void beep_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(BEEP_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);
    
    beep_off();
}

void beep_on(void)
{
    GPIO_ResetBits(BEEP_GPIO, BEEP_PIN);
}

void beep_off(void)
{
    GPIO_SetBits(BEEP_GPIO, BEEP_PIN);
}



