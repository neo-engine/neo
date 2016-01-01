/*
Pokémon Emerald 2 Version
------------------------------

file        : main.c
author      : Philip Wellnitz (modifications)
description :

Copyright (C) 2012 - 2016
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

/*---------------------------------------------------------------------------------

default ARM7 core

Copyright (C) 2005 - 2010
Michael Noland (joat)
Jason Rogers (dovoto)
Dave Murphy (WinterMute)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <dswifi7.h>
#include <maxmod7.h>

//---------------------------------------------------------------------------------
void VblankHandler( void ) {
    //---------------------------------------------------------------------------------
    Wifi_Update( );
}


//---------------------------------------------------------------------------------
void VcountHandler( ) {
    //---------------------------------------------------------------------------------
    inputGetAndSend( );
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB( ) {
    //---------------------------------------------------------------------------------
    exitflag = true;
}

//---------------------------------------------------------------------------------
int main( ) {
    //---------------------------------------------------------------------------------
    readUserSettings( );

    rtcReset( );

    irqInit( );
    // Start the RTC tracking IRQ
    initClockIRQ( );
    fifoInit( );

    mmInstall( FIFO_MAXMOD );

    SetYtrigger( 80 );

    installWifiFIFO( );
    installSoundFIFO( );

    installSystemFIFO( );

    irqSet( IRQ_VCOUNT, VcountHandler );
    irqSet( IRQ_VBLANK, VblankHandler );

    irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK );

    setPowerButtonCB( powerButtonCB );

    // Keep the ARM7 mostly idle
    while( !exitflag ) {
        if( 0 == ( REG_KEYINPUT & ( KEY_SELECT | KEY_START | KEY_L | KEY_R ) ) ) {
            exitflag = true;
        }
        swiWaitForVBlank( );
    }
    return 0;
}
