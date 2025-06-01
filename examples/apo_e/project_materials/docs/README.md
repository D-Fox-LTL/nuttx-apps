
# APO-E Radar Car on NuttX (ESP32-C6)

## Placement
Put all source files in:
    apps/examples/apo_e/

Namely: 
  - motorcontrol.c/.h
  - servocontrol.c/.h
  - ultrasoniccontrol.c/.h
  - roommapping.c/.h
  - webserver.c/.h
  - index.html, script.js, style.css (in `assets/` subdirectory).

## Menuconfig
- Select CONFIG_EXAMPLES_APO-E_RADARCAR=y to build the example.
- Enable PWM driver (`CONFIG_PWM=y`) and multi-channel if needed.
- Enable MOTOR CONTROL PWM driver (`CONFIG_MCPWM=y`).
- Enable LEDC PWM peripheral for ESP32C6 (`CONFIG_ESP32C6_LEDC` or similar).
- Enable networking: 
    CONFIG_NETUTILS_NETLIB=y  
    CONFIG_NETUTILS_DHCPC=y  
    CONFIG_NETDB_DNSCLIENT=y  
    CONFIG_NETUTILS_WEBSERVER=y  
  (These are required for the webserver)
- For GPIO: you may enable `CONFIG_ESPRESSIF_DEDICATED_GPIO` to use dedicated I/O pins for faster control. Configure the specific GPIOs (0,1,2,3,6,7,10,11,17) in board.h or menuconfig as needed.
- Ensure PWM channels are mapped: In **Peripherals → PWM (LEDC)** assign channel 0 to GPIO6, channel 1 to GPIO7, channel 2 to GPIO17 (or as your board requires).

OR

use configured nuttx_config inside '/examples/apo_e/project_materials/docs/nuttx_config' into nuttx/.config which is NOT part of nuttx-apps but different nuttx git repository

## Features
- Servo-based ultrasonic scanning
- Basic motor control
- Web interface for remote control
- Runs as built-in NuttX app

## Build Instructions
1. Clone 'apo_e/' from github
2. Copy `apo_e/` to `apps/examples/` 
3. Run configuration:
   ./tools/configure.sh esp32c6-devkitc:sta_softap
4. Build & flash:
   make -j$(nproc)
   make flash ESPTOOL_PORT=/dev/ttyUSBX (change X for your ttyUSB where esp32c6 is connected)

## Start-up instructions
1. After flashing nuttx onto esp32c6 power microcontroller up
2. App should automatically start up
3. Connect to the SoftAP and open http://10.0.2.1
4. Control the robot through webserver

## GPIO Pins
- Servo (PWM): GPIO21
- HC-SR04: Trig GPIO1, Echo GPIO0
- Motors: AIN1 GPIO11, AIN2 GPIO10, BIN1 GPIO2, BIN2 GPIO3, PWMA GPIO6, PWMB GPIO7
