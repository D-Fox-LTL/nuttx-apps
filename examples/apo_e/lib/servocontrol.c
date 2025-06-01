#include "servocontrol.h"
#include <fcntl.h>
#include <unistd.h>
#include <nuttx/timers/pwm.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>

#define SERVO_DEV "/dev/pwm2"     // PWM device for LEDC channel 2 (GPIO17)
#define SERVO_FREQ 50             // Standard 50 Hz for servo

static int servo_fd = -1;

void servocontrol_init(void)
{
  servo_fd = open(SERVO_DEV, O_RDWR);
  if (servo_fd < 0) {
    printf("servocontrol: ERROR opening %s: %d\n", SERVO_DEV, errno);
    return;
  }

  struct pwm_info_s pwm;
  pwm.frequency = SERVO_FREQ;

#ifdef CONFIG_PWM_MULTICHAN
  pwm.channels[0].channel = 2;   // LEDC Channel 2 (→ GPIO17)
  pwm.channels[0].duty = 0;
#endif

  ioctl(servo_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)&pwm);
  ioctl(servo_fd, PWMIOC_START, 0);
}

void servocontrol_set_angle(int angle)
{
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  // Duty for 50 Hz → 1ms to 2ms → 5% to 10%
  uint32_t minDuty = 3276;
  uint32_t maxDuty = 6553;
  uint32_t duty = minDuty + (maxDuty - minDuty) * angle / 180;

  struct pwm_info_s pwm;
  pwm.frequency = SERVO_FREQ;

#ifdef CONFIG_PWM_MULTICHAN
  pwm.channels[0].channel = 2;   // Same as init!
  pwm.channels[0].duty = duty;
#endif

  ioctl(servo_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)&pwm);
}
