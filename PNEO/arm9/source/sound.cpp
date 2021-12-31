/*
Pokémon neo
------------------------------

file        : sound.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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

#include <string>
#include <maxmod9.h>

#include "defines.h"
#include "fs.h"
#include "saveGame.h"
#include "sound.h"

const char SOUND_PATH[] = "nitro:/SOUND/";

void initSound( ) {
#ifndef NO_SOUND
    std::string path = ( std::string( SOUND_PATH ) + "sound.msl" );
    mmInitDefault( const_cast<char*>( path.c_str( ) ) );
    mmLockChannels( BIT( 0 ) | BIT( 1 ) );
#endif
}

namespace SOUND {
    bool BGMLoaded  = false;
    u16  currentBGM = 0;

    bool BGMforced = false; // While the bgm is forced, no music changes via map/movemode change

    void initBattleSound( ) {
    }

    void deinitBattleSound( ) {
    }

    void setVolume( u16 p_newValue ) {
#ifndef NO_SOUND
        mmSetModuleVolume( p_newValue );
#else
        (void) p_newValue;
#endif
    }

    void dimVolume( ) {
#ifndef NO_SOUND
        setVolume( 0x50 );
#endif
    }
    void restoreVolume( ) {
#ifndef NO_SOUND
        setVolume( 0x100 );
#endif
    }

    void playBGM( u16 p_id, bool p_force ) {
#ifndef NO_SOUND
        auto oa = ANIMATE_MAP;
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            if( p_force ) { BGMforced = true; }
            if( BGMLoaded && p_id == currentBGM ) { return; }
            ANIMATE_MAP = false;
            if( BGMLoaded ) {
                setVolume( 0x50 );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                setVolume( 0x25 );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                mmStop( );
                swiWaitForVBlank( );
                mmUnload( currentBGM );
            }
            restoreVolume( );
            mmLoad( p_id );
            mmStart( p_id, MM_PLAY_LOOP );
            BGMLoaded  = true;
            currentBGM = p_id;
        } else if( BGMLoaded ) {
            ANIMATE_MAP = false;
            setVolume( 0x50 );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            setVolume( 0x25 );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            mmStop( );
            swiWaitForVBlank( );
            mmUnload( currentBGM );
            BGMLoaded = false;
        }
        ANIMATE_MAP = oa;
#else
        (void) p_id;
        (void) p_force;
#endif
    }

    void playBGMOneshot( u16 p_id ) {
#ifndef NO_SOUND
        auto oa = ANIMATE_MAP;
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            if( BGMLoaded && p_id == currentBGM ) { return; }
            ANIMATE_MAP = false;
            if( BGMLoaded ) {
                setVolume( 0x50 );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                setVolume( 0x25 );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                mmStop( );
                swiWaitForVBlank( );
                mmUnload( currentBGM );
            }
            restoreVolume( );
            mmLoad( p_id );
            mmStart( p_id, MM_PLAY_ONCE );
            BGMLoaded  = true;
            currentBGM = p_id;
        } else if( BGMLoaded ) {
            ANIMATE_MAP = false;
            setVolume( 0x50 );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            setVolume( 0x25 );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            mmStop( );
            swiWaitForVBlank( );
            mmUnload( currentBGM );
            BGMLoaded = false;
        }
        ANIMATE_MAP = oa;
#else
        (void) p_id;
#endif
    }

    void stopBGM( ) {
#ifndef NO_SOUND
        if( BGMLoaded ) {
            mmStop( );
            swiWaitForVBlank( );
            mmUnload( currentBGM );
            BGMLoaded = false;
        }
#endif
    }

} // namespace SOUND
