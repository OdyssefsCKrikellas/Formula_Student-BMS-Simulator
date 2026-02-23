#include "bms.h"

void bms_init(BMS_Handle_t *bms)
{
    srand(time(NULL));

    int i;

    for(i= 0; i < CELL_COUNT; i++)
        bms->cell_voltages[i] = 3.8;
    
    bms->pack_current = 0.0;
    bms->temperature = 25.0;
    
    bms->current_state = STATE_STARTUP;
    bms->fault_code = 0;

    bms->soc = 90.0;  
}

void bms_read_sensors(BMS_Handle_t *bms)
{

    if(bms->current_state == STATE_FAULT)
        return;
    
    bms->pack_current = ((float)rand() /RAND_MAX) * 250.0 - 200.0 ; /*Amps*/

    if(bms->soc > 90.0 && bms->pack_current > 0.0)
    {
        bms->pack_current = 0.0; 
    }
    
    float resting_voltage = 3.0 + ((bms->soc / 100.0) * 1.2);

    float internal_resistance = 0.002;
    float dynamic_voltage = resting_voltage + (bms->pack_current * internal_resistance);

    for(int i = 0; i < CELL_COUNT; i++)
    {
     
        float noise = (((float)rand() / RAND_MAX) * 0.04) - 0.02;
        bms->cell_voltages[i] = dynamic_voltage + noise;
    }

    bms->temperature += ((float)rand() /RAND_MAX) * 1.5; /*degress Celcius*/

    float capacity_Ah = 50.0;
    float elapsed_hours = 1.0/3600.0;
    bms->soc += (bms->pack_current * elapsed_hours / capacity_Ah) * 100.0;
}

void bms_check_safety(BMS_Handle_t *bms)
{
    if(bms->temperature > MAX_TEMP)
    {
        bms->fault_code = 3;
        bms->current_state = STATE_FAULT;
    }
    else if(bms->temperature < MIN_TEMP)
    {
        bms->fault_code = 4;
        bms->current_state = STATE_FAULT;
    }

    for(int i = 0; i < CELL_COUNT; i++)
    {
        if(bms->cell_voltages[i] > MAX_VOLTAGE)
        {
            bms->fault_code = 1;
            bms->current_state = STATE_FAULT;
            return;
        }
        
        if(bms->cell_voltages[i] < MIN_VOLTAGE)
        {
            bms->fault_code = 2;
            bms->current_state = STATE_FAULT;
            return;
        }

        if(bms->soc < MIN_SOC)
        {
            bms->fault_code = 5;
            bms->current_state = STATE_FAULT;
        }

    }
}

void bms_update_state(BMS_Handle_t *bms)
{
    switch(bms->current_state)
    {
        case STATE_FAULT: 
             break;

        case STATE_STARTUP: 
            bms->current_state = STATE_IDLE;
            break;
        
        case STATE_IDLE:
            if(bms->pack_current < -10.0) 
            {
                bms->current_state = STATE_ACTIVE;
            }
            break;

        case STATE_ACTIVE: 
            if(bms->pack_current > -1.0)
             {
                 bms->current_state = STATE_IDLE;
             }          
            break;
        
    }
}

void bms_print_status(BMS_Handle_t *bms)
{
    float max_v = 0.0;

    for(int i = 0; i < CELL_COUNT; i++)
    {
        if(bms->cell_voltages[i] > max_v)
        {
            max_v =  bms->cell_voltages[i];
        }
    }

    const char* state_names[] = {"STARTUP", "IDLE", "ACTIVE", "FAULT"};

    printf("[STATE: %-7s] SoC: %5.2f%% | Temp: %5.2fC | Current: %7.2fA | Peak Cell: %.2fV\n", state_names[bms->current_state], bms->soc, bms->temperature,  bms->pack_current,  max_v);

    if(bms->fault_code != 0)
    {
        printf("\n=================================================\n");
        printf("   [!] EMERGENCY SHUTDOWN TRIGGERED [!]\n");
        printf("   Fault Code: %d\n", bms->fault_code);
        printf("=================================================\n\n");
    }
}

