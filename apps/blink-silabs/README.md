# Blink

Blink is almost the simplest application you can build. It initializes the OS
and starts blinking three LEDs. Each LED is driven by a different timer, the
first running at 1Hz, the second at 2Hz and the third at 4Hz.

The application is intended to test and demonstrate how the system is set up,
how threads and timers work.

For platforms with less than three LEDs, only the faster timers are indicated.

Additionally led changes are indicated in debug output.

The application is inspired byt the TinyOS Blink application.
https://github.com/tinyos/tinyos-main/tree/master/apps/Blink

# Platforms
The application has been tested and should work with the following platforms:
 * Thinnect TestSystemBoard (tsb)
 * SiLabs Thunderboard Sense (thunderboard)
 * SiLabs Thunderboard Sense 2 (thunderboard)
 * SiLabs BRD4001A + BRD4161A (brd4161a-mb)
 * SiLabs BRD4001A + BRD4162A (brd4162a-mb)
 * SiLabs BRD4001A + BRD4304A (brd4304a-mb)

TODO Series2 boards:
 * SiLabs BRD4001A + BRD4180A (brd4180a-mb)
