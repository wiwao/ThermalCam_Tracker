# ThermalCam_Tracker
**Lepton3.0_nano &amp; Darknet_Lepton3PI_Tracker**

![ThermalCamTracker](https://github.com/wiwao/ThermalCam_Tracker/assets/10741255/df881bc8-a405-490f-85fc-ec7aa3a500c9)
## Required main Material
- FLIR Lepton 3.5 or FS
- Raspberry PI 3 or 4
- Nvidia Jetson Nano or NX or Orin Nano
- Breakout Board for FLIR Lepton
https://www.switch-science.com/products/7558

## for Raspberry Pi
**Pi4B**

install Bullseye 32bit according officail instruction.

**$ sudo nano /boot/config.txt**

#uncomment to overclock the arm. 700 MHz is the default.

over_voltage=5

arm_freq=1000

force_turbo=1

gpu_freq=400

![PiGPU](https://github.com/wiwao/ThermalCam_Tracker/assets/10741255/ad67c089-0f59-4c4b-9565-9fcbe56ae8bb)

GPU setting to 256

**$ sudo apt update**

**$ sudo apt upgrade**

-Change Pi setting
- [x] spi
- [x] ssh
- [x] i2c

or change raspi-config

**$ reboot**

## install OpenFrameworks
https://openframeworks.cc/setup/raspberrypi/raspberry-pi-getting-started/

Install packages and compile openFrameworks:

**$ cd**
wget https://github.com/openframeworks/openFrameworks/releases/download/0.11.2/of_v0.11.2_linuxarmv6l_release.tar.gz

**$ mkdir of_11.2**

**$ tar vxfz of_v0.11.2_linuxarmv6l_release.tar.gz -C of_11.2 --strip-components 1**

copy Lepton3.0_nano to myApps

**$ cd of_11.2/apps/myApps/Lepton3.0_nano**

**$ make clean**

**$ make -j4**
