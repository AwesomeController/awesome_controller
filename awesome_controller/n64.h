/*

Copyright 2011-2012 Awesome Controller

This file is part of Awesome Controller.

Awesome Controller is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Awesome Controller is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Awesome Controller.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _N64_SYSTEM_H_
#define _N64_SYSTEM_H_

#include "WiiController.h"

// Dumb toggle so we can actually fake buttons
// These used to be 0x04 which I think is B00000100, but if something breaks, change these
#define SET_N64_PIN_HIGH DDRD &= ~B00000100
#define SET_N64_PIN_LOW  DDRD |=  B00000100
#define QUERY_N64_PIN    (PIND &  B00000100)

#define WHITE_LED_ON   PORTD |= B00001000
#define WHITE_LED_OFF  PORTD &= B11110111

#define RED_LED_ON     PORTD |= B00010000
#define RED_LED_OFF    PORTD &= B11101111


class N64 {
  public:
    N64();
    ~N64();
    void init();
    static void commandForWiiController(unsigned char command[], WiiController &controller);
    int DATA_PIN;
};

#endif
