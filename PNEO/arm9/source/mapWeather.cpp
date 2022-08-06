/*
Pokémon neo
------------------------------

file        : mapWeather.cpp
author      : Philip Wellnitz
description : map drawing engine; weather related functions

Copyright (C) 2022 - 2022
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

#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleDefines.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/abilityNames.h"
#include "gen/pokemonNames.h"
#include "io/choiceBox.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "save/gameStart.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace MAP {
    void mapDrawer::initWeather( ) {
        // TODO: get rid of magic constants
        _weatherScrollX = 0;
        _weatherScrollY = 0;
        REG_BLDALPHA    = 0;
        switch( getWeather( ) ) {
        case CLOUDY:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "clouds",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );
            _weatherScrollX = 2;
            _weatherScrollY = 0;
            REG_BLDALPHA    = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case FOREST_CLOUDS:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "forestcloud",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );
            _weatherScrollX = 0;
            _weatherScrollY = 0;
            REG_BLDALPHA    = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case ASH_RAIN:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "ashrain",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );
            _weatherScrollX = 2;
            _weatherScrollY = -4;
            _weatherFollow  = true;
            break;
        case SANDSTORM: {
            bool goggles = SAVE::SAV.getActiveFile( ).m_bag.count(
                BAG::toBagType( BAG::ITEMTYPE_KEYITEM ), I_GO_GOGGLES );

            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "sandstorm",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );

            if( goggles ) { REG_BLDALPHA = 0xff | ( 0x05 << 8 ); }

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
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "flash",
                                                        256 * 192 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            if( getWeather( ) == DARK_FLASH_USED ) {
                bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
                bgSetScroll( IO::bg3, 64, 48 );
            }
            if( getWeather( ) == DARK_FLASH_1 ) {
                bgSetScale( IO::bg3, 1 << 7 | 1 << 6 | 1 << 5, 1 << 7 | 1 << 6 | 1 << 5 );
                bgSetScroll( IO::bg3, 112 - 96, 84 - 72 );
            }
            if( getWeather( ) == DARK_FLASH_2 ) {
                bgSetScale( IO::bg3, 1 << 7 | 1 << 6, 1 << 7 | 1 << 6 );
                bgSetScroll( IO::bg3, 96 - 64, 72 - 48 );
            }
            _weatherFollow = false;
            break;
        default:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            break;
        }
        bgSetPriority( IO::bg3, 0 );
    }

    void mapDrawer::changeWeather( mapWeather p_newWeather ) {
        if( getWeather( ) != p_newWeather ) {
            SAVE::SAV.getActiveFile( ).m_currentMapWeather = p_newWeather;
            for( const auto& fn : _newWeatherCallbacks ) { fn( getWeather( ) ); }
            initWeather( );
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
