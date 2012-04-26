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

/* MAX3421E functions */


#ifndef _MAX3421E_H_
#define _MAX3421E_H_

#include <SPI.h>

#include <Arduino.h>
#include "Max3421e_constants.h"

class MAX3421E : public SPIClass {
    // byte vbusState;
    public:
        MAX3421E( void );
        byte getVbusState( void );
        void toggle( byte pin );
        static void regWr( byte, byte );
        char * bytesWr( byte, byte, char * );
        static void gpioWr( byte );
        byte regRd( byte );
        char * bytesRd( byte, byte, char * );
        byte gpioRd( void );
        boolean reset();
        boolean vbusPwr ( boolean );
        void busprobe( void );
        void powerOn();
        byte IntHandler();
        byte GpxHandler();
        byte Task();
    private:
        void init();
    friend class Max_LCD;        
};




#endif //_MAX3421E_H_
