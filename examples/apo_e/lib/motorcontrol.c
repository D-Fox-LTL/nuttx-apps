#include "motorcontrol.h"
#include <nuttx/timers/pwm.h>
#include <nuttx/board.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <nuttx/ioexpander/gpio.h>

/* Motor Direction GPIOs */
#define AIN1_GPIO 11
#define AIN2_GPIO 10
#define BIN1_GPIO 2
#define BIN2_GPIO 3

/* PWM LEDC Channels → GPIO6 (CH0), GPIO7 (CH1) */
#define MOTOR_LEFT_PWM_CHANNEL  0
#define MOTOR_RIGHT_PWM_CHANNEL 1

/* PWM Device */
#define MOTOR_PWM_DEV "/dev/pwm0"

static int pwm_fd = -1;


void board_gpio_config_output(int pin, bool value) {
    char devname[16];
    snprintf(devname, sizeof(devname), "/dev/gpio%d", pin);
    int fd = open(devname, O_RDWR);
    if (fd >= 0) {
        // We just write to it — assume already configured as output
        ioctl(fd, GPIOC_WRITE, (unsigned long)value);
        close(fd);
    } else {
        printf("Failed to open %s\n", devname);
    }
}

void board_gpiowrite(int pin, bool value) {
    char devname[16];
    snprintf(devname, sizeof(devname), "/dev/gpio%d", pin);
    int fd = open(devname, O_RDWR);
    if (fd >= 0) {
        ioctl(fd, GPIOC_WRITE, (unsigned long)value);
        close(fd);
    }
}

void motor_init(void)
{
  board_gpio_config_output(AIN1_GPIO, false);
  board_gpio_config_output(AIN2_GPIO, false);
  board_gpio_config_output(BIN1_GPIO, false);
  board_gpio_config_output(BIN2_GPIO, false);

  pwm_fd = open(MOTOR_PWM_DEV, O_RDWR);
  if (pwm_fd < 0) {
    printf("motor_init: ERROR opening %s: %d\n", MOTOR_PWM_DEV, errno);
    return;
  }

  // Start with motors stopped
  motor_stop();
}

static void motor_pwm(uint16_t left_duty, uint16_t right_duty)
{
  struct pwm_info_s info;
  info.frequency = 1000; // 1kHz, safe default for TB6612FNG

#ifdef CONFIG_PWM_MULTICHAN
  info.channels[0].channel = MOTOR_LEFT_PWM_CHANNEL;
  info.channels[0].duty    = left_duty;

  info.channels[1].channel = MOTOR_RIGHT_PWM_CHANNEL;
  info.channels[1].duty    = right_duty;
#endif

  ioctl(pwm_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)&info);
  ioctl(pwm_fd, PWMIOC_START, 0);
}

void motor_forward(void)
{
  board_gpiowrite(AIN1_GPIO, true);
  board_gpiowrite(AIN2_GPIO, false);
  board_gpiowrite(BIN1_GPIO, true);
  board_gpiowrite(BIN2_GPIO, false);
  motor_pwm(0xC000, 0xC000);  // 75% duty cycle
}

void motor_backward(void)
{
  board_gpiowrite(AIN1_GPIO, false);
  board_gpiowrite(AIN2_GPIO, true);
  board_gpiowrite(BIN1_GPIO, false);
  board_gpiowrite(BIN2_GPIO, true);
  motor_pwm(0xC000, 0xC000);
}

void motor_left(void)
{
  board_gpiowrite(AIN1_GPIO, false);
  board_gpiowrite(AIN2_GPIO, true);
  board_gpiowrite(BIN1_GPIO, true);
  board_gpiowrite(BIN2_GPIO, false);
  motor_pwm(0x8000, 0x8000);  // ~50% duty for smoother turn
}

void motor_right(void)
{
  board_gpiowrite(AIN1_GPIO, true);
  board_gpiowrite(AIN2_GPIO, false);
  board_gpiowrite(BIN1_GPIO, false);
  board_gpiowrite(BIN2_GPIO, true);
  motor_pwm(0x8000, 0x8000);
}

void motor_stop(void)
{
  board_gpiowrite(AIN1_GPIO, false);
  board_gpiowrite(AIN2_GPIO, false);
  board_gpiowrite(BIN1_GPIO, false);
  board_gpiowrite(BIN2_GPIO, false);
  motor_pwm(0, 0);
}
