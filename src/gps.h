#ifndef __GPS_H
#define __GPS_H

#include <stdint.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stdio.h>
#include <string.h>

#define GPS_BUFFER_SIZE 512

void gps_init(void);
uint8_t gps_receive(void);
void gps_encode(void);
void gps_convert_coordinates(double *latitude, double *longitude);
int gps_getSeconds();
int gps_getMilliseconds();
void gps_flush();
int gps_getMinutes();
uint8_t gps_hasFix();
uint32_t gps_getAltitude();
void gps_set_time(int minute, int second);

#endif
