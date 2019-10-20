#include <contiki.h>

PROCESS_NAME(helloworld); 
PROCESS_NAME(led);
PROCESS_NAME(key);
PROCESS_NAME(onboard_sensors);
PROCESS_NAME(lcd);
PROCESS_NAME(rfUart_process);
PROCESS_NAME(getHwType_process);

struct process * const autostart_processes[] = {
    &led,
    &key,
    &onboard_sensors,
    &lcd,
    &rfUart_process,
    &getHwType_process,
    NULL
};

void StartProcesslist(void)
{
    autostart_start(autostart_processes);
}
