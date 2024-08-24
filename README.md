# Software for WSPR Tracker based on components from RS41 radiosonde

The goal of this project is to show that you can use some components from RS41 radiosonde in your own trackers.

## Used components

1. STM32F100C8
2. 24 MHz crystal
3. 26 MHz TXCO (not tested yet)

## Unique features

-   TX at multiple timeslots: 8 0, 4 6 so you can hit propagation easier and get more spots

## Pinouts

1. GPS TX at PA10, RX at PA9
2. I2C SCL at PB10, SDA at PB11
3. GPS Enable at PB7 (optional, not used yet)
4. SWDIO at PA13, CLK at PA14

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
