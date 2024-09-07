#include "gps.h"

int dd_lat = 0;
int mm_lat = 0;
int last_mm_lat = 0;
char direction_lat = 'N';

int dd_long = 0;
int mm_long = 0;
int last_mm_long = 0;
char direction_long = 'E';

int quality_indicator = 0;
int satellites = 0;
long int altitude = 0;
char raw_time[10];

int speed = 0;
int course = 0;

char gps_buffer[GPS_BUFFER_SIZE];
uint16_t gps_buffer_index = 0;

int hours, minutes, seconds, milliseconds;

int lastMinutes = -1;

void gps_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
}

void gps_encode(void) {
    if (strstr(gps_buffer, "$GNRMC") != NULL) {        
        sscanf(gps_buffer, "$GNRMC,%*[^,],%*c,%*[^,],%*c,%*[^,],%*c,%d.%*d,%d", &speed, &course);
    }

    if (strstr(gps_buffer, "$GNGGA") != NULL) {   
        sscanf(gps_buffer, "$GNGGA,%10[^,],%2d%2d.%2d%*[^,],%c,%3d%2d.%2d%*[^,],%c,%d,%d,%*[^,],%ld", raw_time, &dd_lat, &mm_lat, &last_mm_lat, &direction_lat, &dd_long, &mm_long, &last_mm_long, &direction_long, &quality_indicator, &satellites, &altitude);
        sscanf(raw_time, "%2d%2d%2d.%3d", &hours, &minutes, &seconds, &milliseconds);
    }

    if (lastMinutes != -1) {
        if (lastMinutes == minutes) {
            minutes = -1;
        }
    }
}

char UART_Receive(void) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

    return USART_ReceiveData(USART1);
}

uint8_t gps_receive(void) {
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
        char data = UART_Receive();
        
        gps_buffer[gps_buffer_index++] = data;
        
        if (gps_buffer_index >= GPS_BUFFER_SIZE || data == '\n') {
            gps_buffer[gps_buffer_index] = '\0';  
            gps_buffer_index = 0;  

            gps_encode();

            return 1;
        }
    }

    return 0;
}

void gps_convert_coordinates(double *latitude, double *longitude)
{
    *latitude = dd_lat + (mm_lat + last_mm_lat / 10000.0) / 60.0;
    if(direction_lat == 'S') *latitude = - *latitude;

    *longitude = dd_long + (mm_long + last_mm_long / 10000.0) / 60.0;
    if(direction_long == 'W') *longitude = - *longitude;
}

int gps_getSeconds() {
    return seconds;
}

int gps_getMinutes() {
    return minutes;
}

void gps_set_time(int minute, int second) {
    minutes = minute;
    seconds = second;
}

void gps_flush() {
    lastMinutes = minutes;

    seconds = -1;
    minutes = -1;
    satellites = -1;

    dd_lat = 0;
    mm_lat = 0;
    last_mm_lat = 0;
    direction_lat = 'N';

    dd_long = 0;
    mm_long = 0;
    last_mm_long = 0;
    direction_long = 'E';
}

int gps_getMilliseconds() {
    return milliseconds;
}

uint32_t gps_getAltitude() {
    return altitude;
}

uint8_t gps_hasFix() {
    return satellites > 0;
}