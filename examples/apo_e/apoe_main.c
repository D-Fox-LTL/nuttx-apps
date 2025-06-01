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
    motor_init();
    servocontrol_init();
    ultrasonic_init();       // configure TRIG/ECHO pins
    //here will be webserver_init()
    mapping_init();     // prepare data structures

    // Center servo and stop motor
    servocontrol_set_angle(90);
    motor_stop();
    basic_alg();
    return 0;
}

/* basic algorithm to see if the robot moves
 * robot still hasnt moved an inch...
 * this function is DEBUG only
 * main algorithm will be developed after the robot moves - testing is quite hard without it
 */
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

/*
 * here will be advanced algorithm for robot
 * robot will scan, store values and move and repeat
 * if scanned value is too close robot will scan its surroundings if its possible to turn
 * 1. if its possible it will turn to right or left
 * 2. if not it will reverse and try step 1. again
 * this function will be send as a pointer into webserver
 * this gives the freedom to develop more algorithm and using one based on what we need
 */


