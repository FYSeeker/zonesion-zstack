#include "stm32f4xx.h"

static unsigned int relay_status = 0;

void relay_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
  //GPIOC->BSRRL = GPIO_Pin_0 | GPIO_Pin_1;
}
unsigned int relay_get(void)
{
  return relay_status;
}
void relay_on(unsigned int s)
{
  relay_status |= s;
  if (s & 0x01) {
    GPIOC->BSRRL = GPIO_Pin_12;
  }
  if (s & 0x02) {
    GPIOC->BSRRL = GPIO_Pin_13;
  }
}
void relay_off(unsigned int s)
{
  relay_status &= ~s;
  if (s & 0x01) {
    GPIOC->BSRRH = GPIO_Pin_12;
  }
  if (s & 0x02) {
    GPIOC->BSRRH = GPIO_Pin_13;
  }
}