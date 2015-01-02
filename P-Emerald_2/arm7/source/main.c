/*
Pokémon Emerald 2 Version
------------------------------

file        : main.c
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

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

#define NDS_IPC_INCLUDE

#include <nds.h>
#include "as_lib7.h"
#include "ipc.h"

#include <nds/ipc.h>

int vcount;

static u8 last_time_touched = 0;

static u8 range_counter_1 = 0;
static u8 range_counter_2 = 0;
static u8 range = 20;
static u8 min_range = 20;

//---------------------------------------------------------------------------------
u8 tweakCheckStylus( ) {
    //---------------------------------------------------------------------------------

    SerialWaitBusy( );

    REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS; //0x8A01;
    REG_SPIDATA = TSC_MEASURE_TEMP1;

    SerialWaitBusy( );

    REG_SPIDATA = 0;

    SerialWaitBusy( );

    REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;// 0x8201;
    REG_SPIDATA = 0;

    SerialWaitBusy( );

    if( last_time_touched == 1 ) {
        if( !( REG_KEYXY & 0x40 ) )
            return 1;
        else {
            REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS;
            REG_SPIDATA = TSC_MEASURE_TEMP1;

            SerialWaitBusy( );

            REG_SPIDATA = 0;

            SerialWaitBusy( );

            REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;
            REG_SPIDATA = 0;

            SerialWaitBusy( );

            return !( REG_KEYXY & 0x40 ) ? 2 : 0;
        }
    } else {
        return !( REG_KEYXY & 0x40 ) ? 1 : 0;
    }
    inputGetAndSend( );
}

//---------------------------------------------------------------------------------
uint16 tweakRead( uint32 command ) {
    //---------------------------------------------------------------------------------
    uint16 result, result2;

    uint32 oldIME = REG_IME;

    REG_IME = 0;

    SerialWaitBusy( );

    // Write the command and wait for it to complete
    REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS; //0x8A01;
    REG_SPIDATA = command;
    SerialWaitBusy( );

    // Write the second command and clock in part of the data
    REG_SPIDATA = 0;
    SerialWaitBusy( );
    result = REG_SPIDATA;

    // Clock in the rest of the data (last transfer)
    REG_SPICNT = SPI_ENABLE | 0x201;
    REG_SPIDATA = 0;
    SerialWaitBusy( );

    result2 = REG_SPIDATA >> 3;

    REG_IME = oldIME;

    // Return the result
    return ( ( result & 0x7F ) << 5 ) | result2;
}


//---------------------------------------------------------------------------------
uint32 tweakReadTemperature( int * t1, int * t2 ) {
    //---------------------------------------------------------------------------------
    *t1 = tweakRead( TSC_MEASURE_TEMP1 );
    *t2 = tweakRead( TSC_MEASURE_TEMP2 );
    return 8490 * ( *t2 - *t1 ) - 273 * 4096;
}

static s32 xscale, yscale;
static s32 xoffset, yoffset;

//---------------------------------------------------------------------------------
int16 readTweakValue( uint32 command, int16 *dist_max, u8 *err ) {
    //---------------------------------------------------------------------------------
    int16 values[ 5 ];
    int32 aux1, aux2, aux3, dist, dist2, result = 0;
    u8 i, j, k;

    *err = 1;

    SerialWaitBusy( );

    REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS;
    REG_SPIDATA = command;

    SerialWaitBusy( );

    for( i = 0; i < 5; i++ ) {
        REG_SPIDATA = 0;
        SerialWaitBusy( );

        aux1 = REG_SPIDATA;
        aux1 = aux1 & 0xFF;
        aux1 = aux1 << 16;
        aux1 = aux1 >> 8;

        values[ 4 - i ] = aux1;

        REG_SPIDATA = command;
        SerialWaitBusy( );

        aux1 = REG_SPIDATA;
        aux1 = aux1 & 0xFF;
        aux1 = aux1 << 16;

        aux1 = values[ 4 - i ] | ( aux1 >> 16 );
        values[ 4 - i ] = ( ( aux1 & 0x7FF8 ) >> 3 );
    }

    REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;
    REG_SPIDATA = 0;
    SerialWaitBusy( );

    dist = 0;
    for( i = 0; i < 4; i++ ) {
        aux1 = values[ i ];

        for( j = i + 1; j < 5; j++ ) {
            aux2 = values[ j ];
            aux2 = abs( aux1 - aux2 );
            if( aux2 > dist ) dist = aux2;
        }
    }

    *dist_max = dist;

    for( i = 0; i < 3; i++ ) {
        aux1 = values[ i ];

        for( j = i + 1; j < 4; j++ ) {
            aux2 = values[ j ];
            dist = abs( aux1 - aux2 );

            if( dist <= range ) {
                for( k = j + 1; k < 5; k++ ) {
                    aux3 = values[ k ];
                    dist2 = abs( aux1 - aux3 );

                    if( dist2 <= range ) {
                        result = aux2 + ( aux1 << 1 );
                        result = result + aux3;
                        result = result >> 2;
                        result = result & ( ~7 );

                        *err = 0;

                        break;
                    }
                }
            }
        }
    }

    if( ( *err ) == 1 ) {
        result = values[ 0 ] + values[ 4 ];
        result = result >> 1;
        result = result & ( ~7 );
    }

    return ( result & 0xFFF );
}

//---------------------------------------------------------------------------------
void tweakUpdateRange( uint8 *this_range, int16 last_dist_max, u8 data_error, u8 tsc_touched ) {
    //---------------------------------------------------------------------------------
    //range_counter_1 = counter_0x380A98C
    //range_counter_2 = counter_0x380A990
    //Initial values:
    // range = 20
    // min_range = 20

    if( tsc_touched != 0 ) {
        if( data_error == 0 ) {
            range_counter_2 = 0;

            if( last_dist_max >= ( ( *this_range ) >> 1 ) ) {
                range_counter_1 = 0;
            } else {
                range_counter_1++;

                if( range_counter_1 >= 4 ) {
                    range_counter_1 = 0;

                    if( ( *this_range ) > min_range ) {
                        ( *this_range )--;
                        range_counter_2 = 3;
                    }
                }
            }
        } else {
            range_counter_1 = 0;
            range_counter_2++;

            if( range_counter_2 >= 4 ) {

                range_counter_2 = 0;

                if( ( *this_range ) < 35 ) {  //0x23 = 35
                    *this_range = ( *this_range ) + 1;
                }
            }
        }
    } else {
        range_counter_2 = 0;
        range_counter_1 = 0;
    }
}

void tweakReadDSMode( touchPosition *touchPos ) {
    //---------------------------------------------------------------------------------

    int16 dist_max_y, dist_max_x, dist_max;
    u8 error, error_where, first_check, i;

    uint32 oldIME = REG_IME;

    REG_IME = 0;

    first_check = tweakCheckStylus( );
    if( first_check != 0 ) {
        error_where = 0;

        touchPos->z1 = readTweakValue( TSC_MEASURE_Z1 | 1, &dist_max, &error );
        touchPos->z2 = readTweakValue( TSC_MEASURE_Z2 | 1, &dist_max, &error );

        touchPos->rawx = readTweakValue( TSC_MEASURE_X | 1, &dist_max_x, &error );
        if( error == 1 ) error_where += 1;

        touchPos->rawy = readTweakValue( TSC_MEASURE_Y | 1, &dist_max_y, &error );
        if( error == 1 ) error_where += 2;

        REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS;
        for( i = 0; i < 12; i++ ) {
            REG_SPIDATA = 0;

            SerialWaitBusy( );
        }

        REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH;
        REG_SPIDATA = 0;

        SerialWaitBusy( );

        if( first_check == 2 ) error_where = 3;

        switch( tweakCheckStylus( ) ) {
            case 0:
                last_time_touched = 0;
                break;
            case 1:
                last_time_touched = 1;

                if( dist_max_x > dist_max_y )
                    dist_max = dist_max_x;
                else
                    dist_max = dist_max_y;

                break;
            case 2:
                last_time_touched = 0;
                error_where = 3;

                break;
        }

    } else {
        error_where = 3;
        touchPos->rawx = 0;
        touchPos->rawy = 0;
        last_time_touched = 0;
    }

    tweakUpdateRange( &range, dist_max, error_where, last_time_touched );

    REG_IME = oldIME;

}


void tweakReadXY( touchPosition *touchPos ) {
    //---------------------------------------------------------------------------------
    tweakReadDSMode( touchPos );

    s16 px = ( touchPos->rawx * xscale - xoffset + xscale / 2 ) >> 19;
    s16 py = ( touchPos->rawy * yscale - yoffset + yscale / 2 ) >> 19;

    if( px < 0 ) px = 0;
    if( py < 0 ) py = 0;
    if( px > ( SCREEN_WIDTH - 1 ) ) px = SCREEN_WIDTH - 1;
    if( py > ( SCREEN_HEIGHT - 1 ) ) py = SCREEN_HEIGHT - 1;

    touchPos->px = px;
    touchPos->py = py;

}



// read stylus position
void VcountHandler( ) {
    static touchPosition first, tempPos;
    static int lastbut = -1;

    uint16 but = 0, x = 0, y = 0, xpx = 0, ypx = 0, z1 = 0, z2 = 0;

    but = REG_KEYXY;

    if( !( ( but ^ lastbut ) & ( 1 << 6 ) ) ) {

        tweakReadXY( &tempPos );

        if( tempPos.rawx == 0 || tempPos.rawy == 0 ) {
            but |= ( 1 << 6 );
            lastbut = but;
        } else {
            but &= ~( 1 << 6 );
            x = tempPos.rawx;
            y = tempPos.rawy;
            xpx = tempPos.px;
            ypx = tempPos.py;
            z1 = tempPos.z1;
            z2 = tempPos.z2;
        }

    } else {
        lastbut = but;
        but |= ( 1 << 6 );
    }

    IPC->touchX = x;
    IPC->touchY = y;
    IPC->touchXpx = xpx;
    IPC->touchYpx = ypx;
    IPC->touchZ1 = z1;
    IPC->touchZ2 = z2;
    IPC->buttons = but;

}

volatile u8 exitflag = 0;
//---------------------------------------------------------------------------------
void powerButtonCB( ) {
    //---------------------------------------------------------------------------------
    exitflag = 1;
}
int main( int argc, char ** argv ) {
    rtcReset( );
    // enable sound
    REG_POWERCNT = 1;
    SOUND_CR = SOUND_ENABLE | SOUND_VOL( 127 );

    // init LED status to always on, as some flashcards make it blink
    writePowerManagement( PM_CONTROL_REG, ~PM_LED_BLINK & readPowerManagement( PM_CONTROL_REG ) );

    // set interrupts
    irqInit( );
    // Start the RTC tracking IRQ
    initClockIRQ( );

    SetYtrigger( 80 );
    vcount = 80;
    irqSet( IRQ_VCOUNT, VcountHandler );
    irqSet( IRQ_VBLANK, AS_SoundVBL );    // the sound engine
    irqEnable( IRQ_VBLANK | IRQ_VCOUNT );

    xscale = ( ( PersonalData->calX2px - PersonalData->calX1px ) << 19 ) / ( ( PersonalData->calX2 ) - ( PersonalData->calX1 ) );
    yscale = ( ( PersonalData->calY2px - PersonalData->calY1px ) << 19 ) / ( ( PersonalData->calY2 ) - ( PersonalData->calY1 ) );

    xoffset = ( ( PersonalData->calX1 + PersonalData->calX2 ) * xscale - ( ( PersonalData->calX1px + PersonalData->calX2px ) << 19 ) ) / 2;
    yoffset = ( ( PersonalData->calY1 + PersonalData->calY2 ) * yscale - ( ( PersonalData->calY1px + PersonalData->calY2px ) << 19 ) ) / 2;

    // main loop
    while( 1 ) {
        if( 0 == ( REG_KEYINPUT & ( KEY_SELECT | KEY_START | KEY_L | KEY_R ) ) ) {
            exitflag = 1;
        }
        AS_MP3Engine( );     // the mp3 engine

        // don't wait for for VBlank if your plan to use high bitrates
        // or real-time pitching with faster rates than original.
        // note that it causes the arm9 to wait more because of ram locking,
        // ending up to ~5-7% arm9 cpu usage increase.
        // uncomment it to use nearly no arm9 cpu usage, but this may cause
        // freeze/glitches with high birates/faster pitched mp3s.
        swiWaitForVBlank();
    }

    return 0;
}

