#include "bms.h"
#include <windows.h>

int main()
{
    BMS_Handle_t my_battery;

    bms_init(&my_battery);

    printf("Starting BMS Real-Time Simulation...\n");
    printf("=================================================\n");

    while(1)
    {
        bms_read_sensors(&my_battery);

        bms_check_safety(&my_battery);

        bms_update_state(&my_battery);

        bms_print_status(&my_battery);
        
        if(my_battery.current_state == STATE_FAULT)
            break;
        
        Sleep(1000);
    }
    
    return 0;
}