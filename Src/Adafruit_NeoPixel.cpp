/*-------------------------------------------------------------------------
  Arduino library to control a wide variety of WS2811- and WS2812-based RGB
  LED devices such as Adafruit FLORA RGB Smart Pixels and NeoPixel strips.
  Currently handles 400 and 800 KHz bitstreams on 8, 12 and 16 MHz ATmega
  MCUs, with LEDs wired for RGB or GRB color order.  8 MHz MCUs provide
  output on PORTB and PORTD, while 16 MHz chips can handle most output pins
  (possible exception with upper PORT registers on the Arduino Mega).

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
  contributions by PJRC and other members of the open source community.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  -------------------------------------------------------------------------
  This file is part of the Adafruit NeoPixel library.

  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoPixel.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include "Adafruit_NeoPixel.h"

Adafruit_NeoPixel::Adafruit_NeoPixel(uint16_t n, uint8_t p, uint8_t t, uint32_t reset_pulse) :
   numLEDs(n), numBytes(n * 3 * 8 + reset_pulse), pin(p), brightness(0),
   pixels(NULL), type(t), endTime(0)
{
  if((pixels = (uint8_t *)malloc(numBytes))) {
    memset(pixels, 0, numBytes);
  }
  if(t & NEO_GRB) { // GRB vs RGB; might add others if needed
    rOffset = 8;
    gOffset = 0;
    bOffset = 16;
  } else if (t & NEO_BRG) {
    rOffset = 8;
    gOffset = 16;
    bOffset = 0;
  } else {
    rOffset = 0;
    gOffset = 8;
    bOffset = 16;
  }
  
}

Adafruit_NeoPixel::~Adafruit_NeoPixel() {
  if(pixels) free(pixels);
}

void Adafruit_NeoPixel::begin(void) {
  clear();
}

void Adafruit_NeoPixel::show(void) {
  if(!pixels) return;
  extern TIM_HandleTypeDef htim1;
  while(
    HAL_BUSY == HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t*)pixels, numBytes)
  );
}

// Set the output pin number
void Adafruit_NeoPixel::setPin(uint8_t p) {
}

// Set pixel color from separate R,G,B components:
void Adafruit_NeoPixel::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < numLEDs) {
    if(brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    uint8_t *p = &pixels[n * 3 * 8];
    for(int i = 0; i < 8; i++) {
      p[rOffset + i] = (r & (1 << (7 - i))) ? t1h : t0h;
    }
    for(int i = 0; i < 8; i++) {
      p[gOffset + i] = (g & (1 << (7 - i))) ? t1h : t0h;
    }
    for(int i = 0; i < 8; i++) {
      p[bOffset + i] = (b & (1 << (7 - i))) ? t1h : t0h;
    }
  }
}

// Set pixel color from 'packed' 32-bit RGB color:
void Adafruit_NeoPixel::setPixelColor(uint16_t n, uint32_t c) {
  if(n < numLEDs) {
    uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
    setPixelColor(n, r, g, b);
  }
}
/*
------------------------------------------------------------------------------
 HSV to RGB conversion
------------------------------------------------------------------------------
 H [0..1541]	angle 0 == 0deg, 1541 < 360deg
		sextants: [0..256], [257..513], [514..770], [771..1027], [1028..1284], [1285..1541]
	8-bit(+1) per sextant
		~0.2335 degrees per count
		This is the highest resolution possible with 8 bit target colors and is already
		slightly higher than necessay (max resolution is ~6 * 256). However, using the
		current setup makes calculation a lot easier by using 8-bit shifts.
 S [0..255]
 V [0..255]
*/
static inline void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
        int region = h / 60;         // region of the color circle
        int m = (h % 60) * 255 / 60; // remainder in the region
        uint8_t p = (v * (255 - s)) / 255;
        uint8_t q = (v * (255 - ((s * m) / 255))) / 255;
        uint8_t t = (v * (255 - ((s * (255 - m)) / 255))) / 255;
        switch (region)
        {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:
            *r = v;
            *g = p;
            *b = q;
            break;
        }    
  }

void Adafruit_NeoPixel::setPixelColorHsv(uint16_t n, uint16_t h, uint8_t s, uint8_t v) {
	uint8_t r, g, b;
	hsv_to_rgb(h, s, v, &r, &g, &b);
	setPixelColor(n, r, g, b);
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t Adafruit_NeoPixel::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t Adafruit_NeoPixel::getPixelColor(uint16_t n) const {
  if(n >= numLEDs) {
    // Out of bounds, return no color.
    return 0;
  }
  uint8_t *p = &pixels[n * 3];
  uint32_t c = ((uint32_t)p[rOffset] << 16) |
               ((uint32_t)p[gOffset] <<  8) |
                (uint32_t)p[bOffset];
  // Adjust this back up to the true color, as setting a pixel color will
  // scale it back down again.
  if(brightness) { // See notes in setBrightness()
    //Cast the color to a byte array
    uint8_t * c_ptr =reinterpret_cast<uint8_t*>(&c);
    c_ptr[0] = (c_ptr[0] << 8)/brightness;
    c_ptr[1] = (c_ptr[1] << 8)/brightness;
    c_ptr[2] = (c_ptr[2] << 8)/brightness;
  }
  return c; // Pixel # is out of bounds
}

// Returns pointer to pixels[] array.  Pixel data is stored in device-
// native format and is not translated here.  Application will need to be
// aware whether pixels are RGB vs. GRB and handle colors appropriately.
uint8_t *Adafruit_NeoPixel::getPixels(void) const {
  return pixels;
}

uint16_t Adafruit_NeoPixel::numPixels(void) const {
  return numLEDs;
}

// Adjust output brightness; 0=darkest (off), 255=brightest.  This does
// NOT immediately affect what's currently displayed on the LEDs.  The
// next call to show() will refresh the LEDs at this level.  However,
// this process is potentially "lossy," especially when increasing
// brightness.  The tight timing in the WS2811/WS2812 code means there
// aren't enough free cycles to perform this scaling on the fly as data
// is issued.  So we make a pass through the existing color data in RAM
// and scale it (subsequent graphics commands also work at this
// brightness level).  If there's a significant step up in brightness,
// the limited number of steps (quantization) in the old data will be
// quite visible in the re-scaled version.  For a non-destructive
// change, you'll need to re-render the full strip data.  C'est la vie.
void Adafruit_NeoPixel::setBrightness(uint8_t b) {
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  uint8_t newBrightness = b + 1;
  if(newBrightness != brightness) { // Compare against prior value
    // Brightness has changed -- re-scale existing data in RAM
    uint8_t  c,
            *ptr           = pixels,
             oldBrightness = brightness - 1; // De-wrap old brightness value
    uint16_t scale;
    if(oldBrightness == 0) scale = 0; // Avoid /0
    else if(b == 255) scale = 65535 / oldBrightness;
    else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
    for(uint16_t i=0; i<numBytes; i++) {
      c      = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
    brightness = newBrightness;
  }
}

//Return the brightness value
uint8_t Adafruit_NeoPixel::getBrightness(void) const {
  return brightness - 1;
}

void Adafruit_NeoPixel::clear() {
  memset(pixels, 0, numBytes);
  for(int i = 0; i < numLEDs; i++) {
    setPixelColor(i, 0, 0, 0);
  }
}

