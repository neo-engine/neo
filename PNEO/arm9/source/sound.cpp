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

#include <cstdio>
#include <string>

#ifndef NO_SOUND
#ifdef MMOD
#include <maxmod9.h>
#else
#include "sound/sseq.h"
#endif
#include "gen/bgmTranslation.h"
#endif

#include "defines.h"
#include "fs/fs.h"
#include "save/saveGame.h"
#include "sound/sound.h"

#ifndef NO_SOUND
#ifdef MMOD
char SOUND_PATH[ 50 ] = { };
#endif
#endif

void initSound( ) {
#ifndef NO_SOUND
#ifdef MMOD
    snprintf( SOUND_PATH, 50, "nitro:/SOUND/sound.msl" );
    mmInitDefault( SOUND_PATH );
    mmLockChannels( BIT( 0 ) | BIT( 1 ) );
#else
    SOUND::SSEQ::installSoundSys( );
#endif
#endif
}

namespace SOUND {
    bool BGMLoaded  = false;
    s16  currentBGM = 0;

    bool BGMforced = false; // While the bgm is forced, no music changes via map/movemode change

    void initBattleSound( ) {
    }

    void deinitBattleSound( ) {
    }

    void setVolume( u16 p_newValue ) {
#ifndef NO_SOUND
#ifdef MMOD
        mmSetModuleVolume( p_newValue );
#else
        // TODO
#endif
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

    void playBGM( s16 p_id, bool p_force ) {
#ifndef NO_SOUND
        auto oa = ANIMATE_MAP;
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            if( p_force ) { BGMforced = true; }
            if( BGMLoaded && p_id == currentBGM ) { return; }
            ANIMATE_MAP = false;
#ifdef MMOD
            auto mmId = BGMIndexForName( p_id );
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
                auto oldmmId = BGMIndexForName( currentBGM );
                if( oldmmId != MOD_NONE ) { mmUnload( oldmmId ); }
            }
            restoreVolume( );
            if( mmId != MOD_NONE ) {
                mmLoad( mmId );
                mmStart( mmId, MM_PLAY_LOOP );
            }
#else
            SSEQ::playSequence( p_id % 25 );
#endif
            BGMLoaded  = true;
            currentBGM = p_id;
        } else if( BGMLoaded ) {
            ANIMATE_MAP = false;
#ifdef MMOD
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

            auto mmId = BGMIndexForName( currentBGM );
            if( mmId != MOD_NONE ) { mmUnload( mmId ); }
#else
            SSEQ::stopSequence( );
#endif
            BGMLoaded = false;
        }
        ANIMATE_MAP = oa;
#else
        (void) p_id;
        (void) p_force;
#endif
    }

    void playBGMOneshot( s16 p_id ) {
#ifndef NO_SOUND
        auto oa = ANIMATE_MAP;
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            if( BGMLoaded && p_id == currentBGM ) { return; }
            ANIMATE_MAP = false;
#ifdef MMOD
            auto mmId = BGMIndexForName( p_id );
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
                auto oldmmId = BGMIndexForName( currentBGM );
                if( oldmmId != MOD_NONE ) { mmUnload( oldmmId ); }
            }
            restoreVolume( );
            if( mmId != MOD_NONE ) {
                mmLoad( mmId );
                mmStart( mmId, MM_PLAY_ONCE );
            }
#else
            // TODO
#endif
            BGMLoaded  = true;
            currentBGM = p_id;
        } else if( BGMLoaded ) {
            ANIMATE_MAP = false;
#ifdef MMOD
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

            auto mmId = BGMIndexForName( currentBGM );
            if( mmId != MOD_NONE ) { mmUnload( mmId ); }
#else
            // TODO
#endif
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
#ifdef MMOD
            auto mmId = BGMIndexForName( currentBGM );
            mmStop( );
            swiWaitForVBlank( );
            if( mmId != MOD_NONE ) { mmUnload( mmId ); }
#else
            // TODO
#endif
            BGMLoaded = false;
        }
#endif
    }

} // namespace SOUND
