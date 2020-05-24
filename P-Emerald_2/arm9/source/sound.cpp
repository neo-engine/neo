/*
Pokémon neo
------------------------------

file        : sound.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
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

#include "sound.h"

const char SOUND_PATH[] = "nitro:/SOUND/";

void initSFX( ) {
    // load sound effects
    for( u16 i = 0; i < NUM_SOUND_EFFECTS; ++i ) { mmLoadEffect( i ); }
}

void initSound( ) {
#ifndef NO_SOUND
    std::string path = ( std::string( SOUND_PATH ) + "sound.msl" );
    mmInitDefault( (char*) path.c_str( ) );
    initSFX( );
#endif
}

namespace SOUND {
    bool BGMLoaded  = false;
    u16  currentBGM = 0;

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

    void playBGM( u16 p_id ) {
#ifndef NO_SOUND
        if( BGMLoaded && p_id == currentBGM ) { return; }
        if( BGMLoaded ) {
            mmStop( );
            mmUnload( currentBGM );
        }
        // std::string path = ( std::string( SOUND_PATH ) + std::to_string( p_id ) + ".msl" );
        // mmInitDefault( (char*) path.c_str( ) );
        // initSFX( );
        restoreVolume( );
        mmLoad( p_id );
        mmStart( p_id, MM_PLAY_LOOP );
        BGMLoaded  = true;
        currentBGM = p_id;
#else
        (void) p_id;
#endif
    }

    void playSoundEffect( u16 p_id ) {
#ifndef NO_SOUND
        auto handle = mmEffect( p_id );
        mmEffectVolume( handle, 0xFF );
        mmEffectRelease( handle );
#else
        (void) p_id;
#endif
    }
} // namespace SOUND
