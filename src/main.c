void *__dso_handle = 0;

#include "hal/system.h"
#include "hal/i2c.h"
#include "hal/delay.h"
#include "hal/datatimer.h"
#include "config.h"
#include "wspr.h"
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include "gps.h"

#define I2C_BUS_CLOCK_SPEED 100000

uint32_t counter = 0;

void handle_timer_tick()
{
    
}

void TX(uint64_t freq, uint8_t * txBuffer)
{
    unsigned long startedAt = system_get_tick() / 10;
    for (uint8_t i = 0; i < 162; i++)
    {
        unsigned long stopAt = startedAt + ((i + 1) * (unsigned long)683);
        uint64_t toneFreq = freq + ((txBuffer[i] * 146));

        setFrequency(toneFreq);

        while ((system_get_tick() / 10 < stopAt))
            ;
        {
        }
    }

    si5351_disable();
}

void usart_gps_send_byte(uint8_t data)
{
    return;
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
    USART_SendData(USART1, data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
}

void usart_gps_send_string(const char *str)
{
    while (*str) {
        usart_gps_send_byte((uint8_t)*str++);
    }
}

void gps_power_enable() 
{
    GPIO_SetBits(GPIOB, GPIO_Pin_4);
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);

    USART_Cmd(USART1, ENABLE);
}

void gps_power_disable() 
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_4);
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);

    USART_Cmd(USART1, DISABLE);
}

void si5351_power_enable() 
{
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    GPIO_SetBits(GPIOA, GPIO_Pin_2);
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    i2c_init(I2C_BUS_CLOCK_SPEED);

    int initialized = si5351_initialize();

    if (initialized == 1) {
        usart_gps_send_string("SI5351 initialized\r\n");
    } else if (initialized == -1) {
        while (1) {
            usart_gps_send_string("SI5351 failed (-1)\r\n");
            delay_ms(100);
        }
    } else if (initialized == -2) {
        while (1) {
            usart_gps_send_string("SI5351 failed (-2)\r\n");
            delay_ms(100);
        }
    } else {
        while (1) {
            usart_gps_send_string("SI5351 failed (unknown)\r\n");
            delay_ms(100);
        }
    }
}

void si5351_power_disable() 
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);

    i2c_uninit();
}

int main(void)
{
    system_handle_timer_tick = handle_timer_tick;

    system_init();

    gps_init();

    // USART_Cmd(USART1, ENABLE); // DEBUG ONLY

    si5351_power_disable();
    gps_power_disable();

    delay_ms(250);

    si5351_power_enable();

    usart_gps_send_string("System initialized\r\n");
    
    si5351_disable();

    delay_ms(250);

    #ifdef CONFIG_STARTUP_TONE_ENABLE
    usart_gps_send_string("Sending startup tone\r\n");
    si5351_startup_tone();
    usart_gps_send_string("Startup tone sent\r\n");
    #endif

    si5351_power_disable();
    gps_power_enable();

    while (true) {
        gps_receive();

        uint8_t hasFIX = gps_hasFix();

        uint8_t canTX = hasFIX;

        #ifdef CONFIG_TX_WITH_NO_FIX
        canTX = true;
        #endif

        if (canTX == 1) {
            int seconds = gps_getSeconds();
            int minutes = gps_getMinutes();

            if (wspr_isInTimeslot(minutes, seconds) == 1) {
                // si5351_disable();

                // usart_gps_send_string("We are inside timeslot\r\n");

                double latitide = 0, longitude = 0;

                if (hasFIX == 1) {
                    gps_convert_coordinates(&latitide, &longitude);
                }
                
                uint32_t altitude = gps_getAltitude();

                int current = seconds * 1000 + gps_getMilliseconds();

                // usart_gps_send_string("Disabling GPS, enabling SI+TXCO\r\n");

                gps_power_disable();

                si5351_power_enable();

                si5351_disable();

                unsigned long startedAt1 = system_get_tick() / 10;
                unsigned long stopAt1 = startedAt1 + (60000 - current + 300);

                while ((system_get_tick() / 10 < stopAt1))
                    ;
                {
                }
                
                uint64_t freq = WSPR_FREQ20m;

                uint64_t freq1 = freq + (100ULL * (rand() % 201 - 100));

                uint8_t power1 = calculatePower(altitude / 300);
                uint8_t power2 = calculatePower((altitude - (power1 * 300)) / 20);

                if (CONFIG_WSPR_SUFFIX > 0)
                    wspr_encode(power1, 2, latitide, longitude);
                else
                    wspr_encode(power1, 1, latitide, longitude);

                uint8_t* buffer1 = getBuffer();

                // usart_gps_send_string("Doing TX1\r\n");

                TX(freq1, buffer1);

                // usart_gps_send_string("TX1 done\r\n");

                unsigned long startedAt2 = system_get_tick() / 10;
                unsigned long stopAt2 = startedAt2 + 9000;

                while ((system_get_tick() / 10 < stopAt2))
                    ;
                {
                }

                uint64_t freq2 = freq + (100ULL * (rand() % 201 - 100));
                wspr_encode(power2, 3, latitide, longitude);

                uint8_t* buffer2 = getBuffer();

                // usart_gps_send_string("Doing TX2\r\n");

                TX(freq2, buffer2);

                // usart_gps_send_string("TX2 done\r\n");
                // usart_gps_send_string("Enabling GPS and disabling SI+TXCO\r\n");

                si5351_power_disable();

                gps_power_enable();

                gps_flush();
            }
        }

        counter = (counter + 1) % SYSTEM_SCHEDULER_TIMER_TICKS_PER_SECOND;
    }
}
