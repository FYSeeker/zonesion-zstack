#include <iocc2530.h>

#include "hal_mcu.h"
#include "key.h"

void keyInit(void)
{
  P1SEL &= ~0x0C;
  P1DIR &= ~0x0C;
}

uint8 keyRead(void)
{
  uint8 k;
  k = !P1_3;
  k = (k<<1) | !P1_2;
  return k;
}