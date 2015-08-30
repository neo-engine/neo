/*
Pokémon Emerald 2 Version
------------------------------

file        : screenFade.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2015
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
#include "uio.h"
#include "screenFade.h"
#include "fs.h"
#include "defines.h"

namespace IO {
    void fadeScreen( fadeType p_type, bool p_bottom ) {
        switch( p_type ) {
            case IO::CLEAR_DARK:
                FS::readPictureData( bgGetGfxPtr( p_bottom * bg2sub ), "nitro:/PICS/", "ClearD", 512, 49152, p_bottom );
                if( p_bottom )
                    FS::readPictureData( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "ClearD", 512, 49152, p_bottom );
                break;
            case IO::CLEAR_WHITE:
                FS::readPictureData( bgGetGfxPtr( p_bottom * bg2sub ), "nitro:/PICS/", "Clear", 512, 49152, p_bottom );
                if( p_bottom )
                    FS::readPictureData( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "Clear", 512, 49152, p_bottom );
                break;
            case IO::CAVE_ENTRY:
                break;
            case IO::CAVE_EXIT:
                break;
            case IO::BATTLE:
                break;
            case IO::BATTLE_STRONG_OPPONENT:
                break;
            default:
                break;
        }
    }

    void clearScreens( ) {
        FS::readPictureData( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "Clear", 512, 49152, true );
        FS::readPictureData( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "Clear", 512, 49152, true );

        FS::readPictureData( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "Clear" );

        regularFont->setColor( RGB( 0, 31, 31 ), 0 );
    }
    void clearScreenConsoles( ) {
        consoleSelect( &Top );
        consoleSetWindow( &Top, 0, 0, 32, 24 );
        consoleClear( );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleClear( );
    }
}