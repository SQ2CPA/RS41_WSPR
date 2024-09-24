#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_WSPR_CALLSIGN "N0CALL" // WSPR CALLSIGN without SUFFIX/PREFIX, ONLY CALLSIGN

// Single number or letter suffix from 0 to 35,
// 0-9 = 0-9
// 10-35 = A-Z

// Double number suffix from 36 to 125
// 36 - 125 = 10-99

#define CONFIG_WSPR_SUFFIX 1 // Suffix

#define CONFIG_STARTUP_TONE_ENABLE // enable startup tone at 144.025 MHz

#define CONFIG_TX_WITH_NO_FIX // TX also without gps fix

#endif
