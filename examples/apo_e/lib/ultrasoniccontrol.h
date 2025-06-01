#ifndef ULTRASONICCONTROL_H
#define ULTRASONICCONTROL_H

#include <stdint.h>

// Initialize ultrasonic sensor (open GPIOs)
void ultrasonic_init(void);

// Trigger the HC-SR04 and return distance in centimeters (float)
int ultrasonic_get_distance(void);

#endif // ULTRASONICCONTROL_H
