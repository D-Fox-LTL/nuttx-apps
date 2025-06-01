#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <stdbool.h>
#include <stdint.h>

void board_gpio_config_output(int pin, bool value);

void board_gpiowrite(int pin, bool value);

// Initialize motor control: open GPIO and PWM devices.
void motor_init(void);

// Move vehicle forwards
void motor_forward(void);

// Move vehicle backwards
void motor_backward(void);

void motor_left(void);

void motor_right(void);

// Stop both motors
void motor_stop(void);

#endif // MOTORCONTROL_H
