#ifndef __EXT_LED_H
#define __EXT_LED_H
#include "stm32f4xx.h"

#define EXT_LED1_RCC    RCC_AHB1Periph_GPIOC 
#define EXT_LED1_GPIO   GPIOC
#define EXT_LED1_PIN    GPIO_Pin_2

#define EXT_LED2_RCC    RCC_AHB1Periph_GPIOC
#define EXT_LED2_GPIO   GPIOC
#define EXT_LED2_PIN    GPIO_Pin_1

void ext_led_init(void);
void ext_led_on(unsigned char cmd);
void ext_led_off(unsigned char cmd);
unsigned char get_ext_led(void);

#endif
