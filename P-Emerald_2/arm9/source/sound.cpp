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
#include "defines.h"
#include "saveGame.h"

const char SOUND_PATH[] = "nitro:/SOUND/";

void initSFX( ) {
    mmLoadEffect( SFX_CANCEL );
    mmLoadEffect( SFX_CHOOSE );
    mmLoadEffect( SFX_SELECT );
    mmLoadEffect( SFX_USE_ITEM );

    mmLoadEffect( SFX_BATTLE_BALLDROP );
    mmLoadEffect( SFX_BATTLE_BALLSHAKE );
    mmLoadEffect( SFX_BATTLE_DAMAGE_NORMAL );
    mmLoadEffect( SFX_BATTLE_DAMAGE_SUPER );
    mmLoadEffect( SFX_BATTLE_DAMAGE_WEAK );
    mmLoadEffect( SFX_BATTLE_DECREASE );
    mmLoadEffect( SFX_BATTLE_ESCAPE );
    mmLoadEffect( SFX_BATTLE_EXPFULL );
    mmLoadEffect( SFX_BATTLE_FAINT );
    mmLoadEffect( SFX_BATTLE_INCREASE );
    mmLoadEffect( SFX_BATTLE_JUMPTOBALL );
    mmLoadEffect( SFX_BATTLE_RECALL );
    mmLoadEffect( SFX_BATTLE_THROW );
    mmLoadEffect( SFX_SHINY );

    mmLoadEffect( SFX_CRY_123 );

    mmLoadEffect( SFX_BUMP );
    mmLoadEffect( SFX_CAVE_WARP );
    mmLoadEffect( SFX_ENTER_DOOR );
    mmLoadEffect( SFX_EXMARK );
    mmLoadEffect( SFX_JUMP );
    mmLoadEffect( SFX_MENU );
    mmLoadEffect( SFX_OBTAIN_EGG );
    mmLoadEffect( SFX_OBTAIN_ITEM );
    mmLoadEffect( SFX_OBTAIN_KEY_ITEM );
    mmLoadEffect( SFX_SAVE );
    mmLoadEffect( SFX_WARP );
}

void initBattleSFX( ) {
    /*
    mmLoadEffect( SFX_BATTLE_BALLDROP );
    mmLoadEffect( SFX_BATTLE_BALLSHAKE );
    mmLoadEffect( SFX_BATTLE_DAMAGE_NORMAL );
    mmLoadEffect( SFX_BATTLE_DAMAGE_SUPER );
    mmLoadEffect( SFX_BATTLE_DAMAGE_WEAK );
    mmLoadEffect( SFX_BATTLE_DECREASE );
    mmLoadEffect( SFX_BATTLE_ESCAPE );
    mmLoadEffect( SFX_BATTLE_EXPFULL );
    mmLoadEffect( SFX_BATTLE_FAINT );
    mmLoadEffect( SFX_BATTLE_INCREASE );
    mmLoadEffect( SFX_BATTLE_JUMPTOBALL );
    mmLoadEffect( SFX_BATTLE_RECALL );
    mmLoadEffect( SFX_BATTLE_THROW );
    mmLoadEffect( SFX_SHINY );

    mmUnloadEffect( SFX_BUMP );
    mmUnloadEffect( SFX_CAVE_WARP );
    mmUnloadEffect( SFX_ENTER_DOOR );
    mmUnloadEffect( SFX_EXMARK );
    mmUnloadEffect( SFX_JUMP );
    mmUnloadEffect( SFX_MENU );
    mmUnloadEffect( SFX_OBTAIN_EGG );
    mmUnloadEffect( SFX_OBTAIN_ITEM );
    mmUnloadEffect( SFX_OBTAIN_KEY_ITEM );
    mmUnloadEffect( SFX_SAVE );
    mmUnloadEffect( SFX_WARP );
    */
}

void deinitBattleSFX( ) {
    /*
    mmUnloadEffect( SFX_BATTLE_BALLDROP );
    mmUnloadEffect( SFX_BATTLE_BALLSHAKE );
    mmUnloadEffect( SFX_BATTLE_DAMAGE_NORMAL );
    mmUnloadEffect( SFX_BATTLE_DAMAGE_SUPER );
    mmUnloadEffect( SFX_BATTLE_DAMAGE_WEAK );
    mmUnloadEffect( SFX_BATTLE_DECREASE );
    mmUnloadEffect( SFX_BATTLE_ESCAPE );
    mmUnloadEffect( SFX_BATTLE_EXPFULL );
    mmUnloadEffect( SFX_BATTLE_FAINT );
    mmUnloadEffect( SFX_BATTLE_INCREASE );
    mmUnloadEffect( SFX_BATTLE_JUMPTOBALL );
    mmUnloadEffect( SFX_BATTLE_RECALL );
    mmUnloadEffect( SFX_BATTLE_THROW );
    mmUnloadEffect( SFX_SHINY );

    mmLoadEffect( SFX_BUMP );
    mmLoadEffect( SFX_CAVE_WARP );
    mmLoadEffect( SFX_ENTER_DOOR );
    mmLoadEffect( SFX_EXMARK );
    mmLoadEffect( SFX_JUMP );
    mmLoadEffect( SFX_MENU );
    mmLoadEffect( SFX_OBTAIN_EGG );
    mmLoadEffect( SFX_OBTAIN_ITEM );
    mmLoadEffect( SFX_OBTAIN_KEY_ITEM );
    mmLoadEffect( SFX_SAVE );
    mmLoadEffect( SFX_WARP );
    */
}

void initSound( ) {
#ifndef NO_SOUND
    std::string path = ( std::string( SOUND_PATH ) + "sound.msl" );
    mmInitDefault( const_cast<char*>( path.c_str( ) ) );
    initSFX( );
#endif
}

namespace SOUND {
    bool BGMLoaded  = false;
    u16  currentBGM = 0;

    void initBattleSound( ) {
#ifndef NO_SOUND
        initBattleSFX( );
#endif
    }

    void deinitBattleSound( ) {
#ifndef NO_SOUND
        deinitBattleSFX( );
#endif
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

    void playBGM( u16 p_id ) {
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            if( BGMLoaded && p_id == currentBGM ) { return; }
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
                mmUnload( currentBGM );
            }
            restoreVolume( );
            mmLoad( p_id );
            mmStart( p_id, MM_PLAY_LOOP );
            BGMLoaded  = true;
            currentBGM = p_id;
        }
#else
        (void) p_id;
#endif
    }

    void stopBGM( ) {
#ifndef NO_SOUND
        if( BGMLoaded ) {
            mmStop( );
            mmUnload( currentBGM );
            BGMLoaded = false;
        }
#endif
    }

    void playSoundEffect( u16 p_id ) {
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
            auto handle = mmEffect( p_id );
            mmEffectVolume( handle, 0xFF );
            mmEffectRelease( handle );
        }
#else
        (void) p_id;
#endif
    }
} // namespace SOUND
