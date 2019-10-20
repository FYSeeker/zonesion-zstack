#ifndef __BEEP_H
#define __BEEP_H
#include "stm32f4xx.h"

#define BEEP_RCC    RCC_AHB1Periph_GPIOB 
#define BEEP_GPIO   GPIOB
#define BEEP_PIN    GPIO_Pin_10

void beep_init(void);
void beep_on(void);
void beep_off(void);


#endif

