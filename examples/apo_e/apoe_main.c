/* radarcar_main.c – main application */
#include <nuttx/config.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/motorcontrol.h"
#include "lib/servocontrol.h"
#include "lib/ultrasoniccontrol.h"
#include "lib/roommapping.h"

#define SAFE_DISTANCE_CM 10

void basic_alg();

int apoe_main(int argc, char *argv[])
{
    // Initialize subsystems
    motor_init();       // configure GPIO6/7 as motor outputs
    servocontrol_init();       // configure PWM on GPIO17 for steering
    ultrasonic_init();       // configure TRIG/ECHO pins
    mapping_init();     // prepare data structures

    // Center servo and stop motor
    servocontrol_set_angle(90);
    motor_stop();
    basic_alg();
    return 0;
}

void basic_alg()
{
    while(1) {
        if(ultrasonic_get_distance() > SAFE_DISTANCE_CM) {
            motor_forward();
        } else {
            motor_stop();
        }
    }
}

