#ifndef BMS_H
#define BMS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* --- SYSTEM CONSTANTS (HARD LIMITS) --- */
#define CELL_COUNT  100
#define MAX_VOLTAGE 4.20
#define MIN_VOLTAGE 3.00
#define MAX_TEMP    60.0
#define MIN_TEMP    0.0

/* --- SYSTEM STATES (FINITE STATE MACHINE) --- */
typedef enum {
    STATE_STARTUP,      /* System booting up and running checks */
    STATE_IDLE,         /* System ready, tractive system off */
    STATE_ACTIVE,       /* Tractive system active (Driving) */
    STATE_FAULT         /* Critical error, system isolated */
} BMS_State_t;

/* --- MAIN DATA STRUCTURE (THE OBJECT) --- */
typedef struct {
    float cell_voltages[CELL_COUNT];  /* Array holding voltage for all 100 cells */
    float pack_current;               /* Current in Amperes: (+) Charging, (-) Discharging */
    float temperature;                /* Overall pack temperature in Celsius */
    
    BMS_State_t current_state;        /* Current operational state of the vehicle */
    int fault_code;                   /* 0 = System OK, 1 = Over-Volt, 2 = Under-Volt, 3 = Over-Temp */
} BMS_Handle_t;

/* --- FUNCTION PROTOTYPES (THE INTERFACE) --- */

/* Initializes the BMS object with safe default values */
void bms_init(BMS_Handle_t *bms);

/* Simulates reading data from physical hardware sensors */
void bms_read_sensors(BMS_Handle_t *bms);

/* Verifies all sensor data against the hard limits */
void bms_check_safety(BMS_Handle_t *bms);

/* Transitions the state machine based on data and faults */
void bms_update_state(BMS_Handle_t *bms);

/* Outputs the current system telemetry to the console */
void bms_print_status(BMS_Handle_t *bms);

#endif /* BMS_H */