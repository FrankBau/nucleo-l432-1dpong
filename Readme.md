Nucleo-L432KC driving WS2812B RGB 144 LED strip

    +---- USB ----+
    PA9         VIN
    PA10        GND
    NRST        NRST
    GND         +5V
    PA12        PA2
    PB0         PA7
    PB7         PA6
    PB6         PA5
    PB1  +----+ PA4
    N.C. |    | PA3
    N.C. +----+ PA1
    PA8         PA0
    PA11        AREF
    PB5         +3V3
    PB4         PB3
    +---- RST ----+


Adafruit_NeoPixel and one_d_pong source code
taken from [vargrearg.org](https://www.vagrearg.org/)
and ported to STM32.

    PB4:    PIN_BUT_LS: left button, input, pull-up
    PA11:   convenience "GND" pin for PIN_BUT_LS
    PB0:    PIN_BUT_RS: right button, input, pull-up
    PA7:    PIN_WSDATA output, TIM1_CH1N
    PA3:    PIN_SOUND output, TIM15_CH2

    TIM1 clock == 20 MHz == 50ns per tick
    ws2812b
    1250 ns period --> timer counter period of 25
    350 ns T0H --> timer counter ==  7
    700 ns T1H --> timer counter == 14
 