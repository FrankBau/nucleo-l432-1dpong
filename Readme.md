Nucleo-L432KC driving WS2812B RGB 144 LED strip

Adafruit_NeoPixel code and one_d_pong.ino
taken from vargrearg.org, partly ported to STM32.

PIN_BUT_LS: left button, input, pull-up
GND_LS:     convenience "GND" pin for PIN_BUT_LS
PIN_BUT_RS: right button, input, pull-up

timer clock == 20 MHz == 50ns per tick
ws2812b
1250 ns period --> timer counter period of 25
 350 ns T0H --> timer counter ==  7
 700 ns T1H --> timer counter == 14
 