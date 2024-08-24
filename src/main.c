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

bool led = false;

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

int main(void)
{
    // Set up interrupt handlers
    system_handle_timer_tick = handle_timer_tick;

    system_init();

    i2c_init(I2C_BUS_CLOCK_SPEED);

    si5351_initialize();

    delay_ms(250);

    #ifdef CONFIG_STARTUP_TONE_ENABLE
    si5351_startup_tone();
    #endif

    delay_ms(250);

    gps_init();

    uint8_t wsprWait = 0;

    while (true) {
        gps_receive();

        uint8_t hasFIX = gps_hasFix();

        uint8_t canTX = hasFIX;

        #ifdef CONFIG_TX_WITH_NO_FIX
        canTX = true;
        #endif

        if (canTX == 1) {
            if (wsprWait == 0) {
                int seconds = gps_getSeconds();
                int minutes = gps_getMinutes();

                if (wspr_isInTimeslot(minutes, seconds) == 1) {
                    si5351_disable();

                    double latitide, longitude;

                    if (hasFIX == 1) {
                        gps_convert_coordinates(&latitide, &longitude);
                    }

                    int current = seconds * 1000 + gps_getMilliseconds();

                    unsigned long startedAt1 = system_get_tick() / 10;
                    unsigned long stopAt1 = startedAt1 + (60000 - current + 300);

                    while ((system_get_tick() / 10 < stopAt1))
                        ;
                    {
                    }

                    uint32_t altitude = gps_getAltitude();

                    uint64_t freq = WSPR_FREQ20m;

                    uint64_t freq1 = freq + (100ULL * (rand() % 201 - 100));

                    uint8_t power1 = calculatePower(altitude / 300);
                    uint8_t power2 = calculatePower((altitude - (power1 * 300)) / 20);

                    if (CONFIG_WSPR_SUFFIX > 0)
                        wspr_encode(power1, 2, latitide, longitude);
                    else
                        wspr_encode(power1, 1, latitide, longitude);

                    uint8_t* buffer1 = getBuffer();

                    TX(freq1, buffer1);

                    unsigned long startedAt2 = system_get_tick() / 10;
                    unsigned long stopAt2 = startedAt2 + 9000;

                    while ((system_get_tick() / 10 < stopAt2))
                        ;
                    {
                    }

                    uint64_t freq2 = freq + (100ULL * (rand() % 201 - 100));
                    wspr_encode(power2, 3, latitide, longitude);

                    uint8_t* buffer2 = getBuffer();

                    TX(freq2, buffer2);

                    wsprWait = 100;

                    gps_flush();
                }
            } else if (counter == 0) {
                wsprWait--;
            }
        }

        counter = (counter + 1) % SYSTEM_SCHEDULER_TIMER_TICKS_PER_SECOND;
    }
}
