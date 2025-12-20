PIN_BUT_LS: left button, input, pull-up
GND_LS: convenience "GND" pin for PIN_BUT_LS
PIN_BUT_RS: right button, input, pull-up
GND_RS: convenience "GND" pin for PIN_BUT_RS
PIN_WSDATA: timer channel PWM output
PIN_SOUND: sound output (GPIO or DAC) 

SYSCLK == timer clock == 20 MHz == 50ns per tick
ws2812b
1250 ns period --> timer counter period of 25
 350 ns T0H --> timer counter ==  7
 700 ns T1H --> timer counter == 14
 