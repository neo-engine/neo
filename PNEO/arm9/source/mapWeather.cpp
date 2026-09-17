/*
Pokémon neo
------------------------------

file        : mapWeather.cpp
author      : Philip Wellnitz
description : map drawing engine; weather related functions

Copyright (C) 2022 - 2026
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <io/choiceBox.h>
#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleDefines.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/abilityNames.h"
#include "gen/pokemonNames.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/util.h"
#include "map/mapDrawer.h"
#include "save/gameStart.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace MAP {
    void mapDrawer::initWeather( bool p_bgUpdate ) {
        GFX_GUARD( gfGuard );
        _weatherScrollX = 0;
        _weatherScrollY = 0;
        REG_BLDALPHA    = 0;

        // gather data from ROM
        u32  dataSize = COMPLETE_SCREEN_SQ;
        bool partial  = false;
        bool bgWrap   = false;
        u16  bgAlpha  = 0;

        switch( getWeather( ) ) {
        case RAINY:
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "rain",
                                                        dataSize / 4, TEMP, 256, TEMP_PAL );
            bgWrap          = true;
            _weatherScrollX = 32;
            _weatherScrollY = -64;
            bgAlpha         = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case FOG:
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "fog", dataSize / 4,
                                                        TEMP, 256, TEMP_PAL );
            bgWrap          = true;
            _weatherScrollX = 2;
            _weatherScrollY = 0;
            bgAlpha         = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case MIST:
        case DENSE_MIST:
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "mist",
                                                        dataSize / 4, TEMP, 256, TEMP_PAL );
            bgWrap = true;
            if( getWeather( ) == MIST ) {
                _weatherScrollX = 1;
                _weatherScrollY = 1;
                bgAlpha         = 0xff | ( 0x05 << 8 );
            } else {
                _weatherScrollX = 2;
                _weatherScrollY = 2;
                bgAlpha         = 0xff | ( 0x0A << 8 );
            }
            _weatherFollow = true;
            break;

        case CLOUDY:
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "clouds",
                                                        dataSize / 4, TEMP, 256, TEMP_PAL );
            bgWrap          = true;
            _weatherScrollX = 2;
            _weatherScrollY = 0;
            bgAlpha         = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case FOREST_CLOUDS:
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "forestcloud",
                                                        dataSize / 4, TEMP, 256, TEMP_PAL );
            bgWrap          = true;
            _weatherScrollX = 0;
            _weatherScrollY = 0;
            bgAlpha         = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case ASH_RAIN:
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "ashrain",
                                                        dataSize / 4, TEMP, 256, TEMP_PAL );
            bgWrap          = true;
            _weatherScrollX = 2;
            _weatherScrollY = -4;
            _weatherFollow  = true;
            break;
        case SANDSTORM: {
            bool goggles = SAVE::CURRENT_FILE->m_bag.count( BAG::toBagType( BAG::ITEMTYPE_KEYITEM ),
                                                            I_GO_GOGGLES );
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "sandstorm",
                                                        dataSize / 4, TEMP, 256, TEMP_PAL );
            if( goggles ) { bgAlpha = 0xff | ( 0x05 << 8 ); }
            bgWrap          = true;
            _weatherScrollX = 40;
            _weatherScrollY = 10;
            _weatherFollow  = false;
            break;
        }
        case DARK_FLASHABLE:
        case DARK_PERMANENT:
        case DARK_FLASH_USED:
        case DARK_FLASH_1:
        case DARK_FLASH_2:
            dataSize = COMPLETE_SCREEN;
            partial  = true;
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "flash",
                                                        dataSize / 4, TEMP, 256, TEMP_PAL );
            _weatherFollow = false;
            break;
        default:
            partial = true;
            dmaFillWords( 0, TEMP, dataSize );
            break;
        }

        _weatherScrollX = ( _weatherScrollX << 8 ) / 10;
        _weatherScrollY = ( _weatherScrollY << 8 ) / 10;

        swiWaitForVBlank( );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
        if( bgWrap ) { bgWrapOn( IO::bg3 ); }
        if( partial ) { dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), COMPLETE_SCREEN_SQ ); }

        dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), dataSize );
        dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );

        switch( getWeather( ) ) {
        case DARK_FLASH_USED:
            bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
            bgSetScroll( IO::bg3, 64, 48 );
            break;
        case DARK_FLASH_1:
            bgSetScale( IO::bg3, 1 << 7 | 1 << 6 | 1 << 5, 1 << 7 | 1 << 6 | 1 << 5 );
            bgSetScroll( IO::bg3, 112 - 96, 84 - 72 );
            break;
        case DARK_FLASH_2:
            bgSetScale( IO::bg3, 1 << 7 | 1 << 6, 1 << 7 | 1 << 6 );
            bgSetScroll( IO::bg3, 96 - 64, 72 - 48 );
            break;
        default: bgSetScroll( IO::bg3, 0, 0 ); break;
        }
        if( p_bgUpdate ) {
            // this will unfade the screen
            REG_BLDALPHA = bgAlpha;
            if( REG_BLDALPHA ) {
                REG_BLDCNT = WEATHER_BLEND;
            } else {
                REG_BLDCNT = BLEND_NONE;
            }
        }
        bgSetPriority( IO::bg3, 0 );
        if( p_bgUpdate ) { bgUpdate( ); }
    }

    void mapDrawer::changeWeather( mapWeather p_newWeather, bool p_bgUpdate ) {
        if( getWeather( ) != p_newWeather ) {
            SAVE::CURRENT_FILE->m_currentMapWeather = p_newWeather;
            for( const auto& fn : _newWeatherCallbacks ) { fn( getWeather( ) ); }
            initWeather( p_bgUpdate );
            if( ANIMATE_MAP ) {
                if( REG_BLDALPHA ) {
                    REG_BLDCNT = WEATHER_BLEND;
                } else {
                    REG_BLDCNT = BLEND_NONE;
                }
            }
        }
    }
} // namespace MAP
