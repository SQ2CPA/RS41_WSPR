# Software for WSPR Tracker based on components from RS41 radiosonde

## Check my parts list [here](https://github.com/SQ2CPA/parts)

The goal of this project is to show that you can use some components from RS41 radiosonde in your own trackers.

## Used components from RS41

1. STM32F100C8
2. 24 MHz crystal
3. 26 MHz TXCO

## Unique features

-   TX at multiple timeslots: 8 0, 4 6 so you can hit propagation easier and get more spots
-   Power GPS and SI5351+TXCO separately from STM32

## Power usage

About 3.3V 40mA max. We never power GPS and SI5351+TXCO at the same time to lower the maximum peak

## Pinouts

1. GPS TX at PA10, RX at PA9 (GPS TX could be also used as serial debug)
2. I2C SCL at PB10, SDA at PB11 (needs 3.3k/4.7k ohm resistors on the I2C line)
3. GPS VCC at PB4+PB5+PB6+PB7
4. SI5351+TXCO VCC at PA1+PA2+PA3+PA4
5. SWDIO at PA13, CLK at PA14

PCB is currently unavailable to public!

More information should available soon.

## Build

via docker
`docker run --rm -it -v E:\RS41_WSPR:/usr/local/src/RS41ng rs41ng_compiler`

(change the directory path)

## Flash

via openocd

unlock first time by `./unlock.bat`
flash `./flash.bat`

# 73, Damian SQ2CPA, Poland
