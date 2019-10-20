#ifndef __RGB_H
#define __RGB_H
#include "stm32f4xx.h"

#define LEDR_RCC    RCC_AHB1Periph_GPIOB
#define LEDR_GPIO   GPIOB
#define LEDR_PIN    GPIO_Pin_8

#define LEDG_RCC    RCC_AHB1Periph_GPIOB
#define LEDG_GPIO   GPIOB
#define LEDG_PIN    GPIO_Pin_9

#define LEDB_RCC    RCC_AHB1Periph_GPIOB
#define LEDB_GPIO   GPIOB
#define LEDB_PIN    GPIO_Pin_11

void rgb_init(void);
void rgb_on(unsigned char cmd);
void rgb_off(unsigned char cmd);
unsigned char get_rgb(void);


#endif
