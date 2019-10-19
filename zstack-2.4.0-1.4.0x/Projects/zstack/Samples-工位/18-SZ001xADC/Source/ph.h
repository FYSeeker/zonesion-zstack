#ifndef __PH_H
#define __PH_H

#include <ioCC2530.h>

#ifndef u8
typedef unsigned char u8;
#endif

#ifndef u16
typedef unsigned short u16;
#endif


float get_ph(void);
float get_ph_x(u8 num);

#endif
