#include "ultrasoniccontrol.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <nuttx/ioexpander/gpio.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <nuttx/board.h>


#define TRIG_GPIO 0  // GPIO0
#define ECHO_GPIO 1  // GPIO1
#ifndef GPIOC_SETDIR
#  define GPIOC_SETDIR  _GPIOC(0x0003)
#endif

#ifndef GPIO_DIR_OUT
#  define GPIO_DIR_OUT  1
#endif

#ifndef GPIO_DIR_IN
#  define GPIO_DIR_IN   0
#endif


static int trig_fd = -1;
static int echo_fd = -1;

static uint64_t micros(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000ULL) + (ts.tv_nsec / 1000);
}

void ultrasonic_init(void)
{
    trig_fd = open("/dev/gpio0", O_RDWR);
    echo_fd = open("/dev/gpio1", O_RDONLY);
    if (trig_fd < 0 || echo_fd < 0) {
        printf("Failed to open GPIOs\n");
        return;
    }

    // If ioctl for direction fails, just log and continue
    ioctl(trig_fd, GPIOC_SETDIR, (unsigned long)GPIO_DIR_OUT);
    ioctl(echo_fd, GPIOC_SETDIR, (unsigned long)GPIO_DIR_IN);
}



int ultrasonic_get_distance(void)
{
    // Send 10us trigger pulse
    ioctl(trig_fd, GPIOC_WRITE, 0); // Low
    usleep(2);                      // Wait a bit
    ioctl(trig_fd, GPIOC_WRITE, 1); // High
    usleep(10);                     // 10 us pulse
    ioctl(trig_fd, GPIOC_WRITE, 0); // Low again

    // Wait for echo to go HIGH
    uint64_t timeout = micros() + 20000; // 20ms timeout
    while (1) {
        int value;
        ioctl(echo_fd, GPIOC_READ, (unsigned long)(uintptr_t)&value);
        if (value)
            break;
        if (micros() > timeout)
            return -1; // Timeout
    }

    uint64_t start = micros();

    // Wait for echo to go LOW
    while (1) {
        int value;
        ioctl(echo_fd, GPIOC_READ, (unsigned long)(uintptr_t)&value);
        if (!value)
            break;
        if (micros() - start > 30000)
            return -2; // Timeout again
    }

    uint64_t end = micros();
    uint64_t duration = end - start;

    // Sound speed = 34300 cm/s → distance = (duration/2) * 0.0343
    int distance_cm = (int)(duration * 0.0343 / 2.0);
    return distance_cm;
}
