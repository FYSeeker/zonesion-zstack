#ifndef __SYN6288_H
#define __SYN6288_H
#include "stm32f4xx.h"

#define SYN6288_RCC     RCC_AHB1Periph_GPIOB
#define SYN6288_GPIO    GPIOB
#define SYN6288_PIN     GPIO_Pin_9

void syn6288_io_init(void);
void syn6288_init(void);
unsigned char syn6288_busy(void);
void syn6288_play(char *s);
char *hex2unicode(char *str);
void syn6288_play_unicode(char *s, int len);


#endif
