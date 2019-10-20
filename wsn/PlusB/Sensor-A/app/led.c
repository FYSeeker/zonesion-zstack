
#include <contiki.h>
#include "drive_led.h"

PROCESS(led, "led");

PROCESS_THREAD(led, ev, data)
{
    static struct etimer led_et;

    PROCESS_BEGIN();
    
    led_init();
    etimer_set(&led_et, 1000);
    
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev==PROCESS_EVENT_TIMER)
        {
            etimer_set(&led_et, 1000);
            LED1 = !LED1;
        }
    }
    PROCESS_END();
}