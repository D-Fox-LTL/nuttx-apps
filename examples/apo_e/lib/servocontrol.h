#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

// Initialize servo control (open PWM)
void servocontrol_init(void);

// Set servo angle in degrees (0..180)
void servocontrol_set_angle(int angle);

#endif // SERVOCONTROL_H
