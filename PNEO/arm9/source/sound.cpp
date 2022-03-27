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
#include "sound/sseq.h"
#include "sound/sseqData.h"
#endif

#include "defines.h"
#include "fs/fs.h"
#include "save/saveGame.h"
#include "sound/sound.h"

void initSound( ) {
#ifndef NO_SOUND
    SOUND::SSEQ::installSoundSys( );
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
        SSEQ::setMasterVolume( p_newValue >> 1 );
#else
        (void) p_newValue;
#endif
    }

    void dimVolume( ) {
#ifndef NO_SOUND
        setVolume( 0x7F );
#endif
    }
    void restoreVolume( ) {
#ifndef NO_SOUND
        setVolume( 0xFF );
#endif
    }

    void playBGM( s16 p_id, bool p_force ) {
#ifndef NO_SOUND
        auto oa = ANIMATE_MAP;
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            if( p_force ) { BGMforced = true; }
            if( BGMLoaded && p_id == currentBGM ) { return; }
            ANIMATE_MAP = false;

            auto sseqId = SSEQ::BGMIndexForName( p_id );
            if( sseqId != SSEQ::SSEQ_NONE ) { SSEQ::playSequence( sseqId ); }

            BGMLoaded  = true;
            currentBGM = p_id;
        } else if( BGMLoaded ) {
            ANIMATE_MAP = false;

            // TODO: Fade seq instead?
            SSEQ::stopSequence( );

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
            // looping is done via sseq commands; no need for different code here
            // TODO: (blockingly) wait for OS to complete?

            auto sseqId = SSEQ::BGMIndexForName( p_id );
            if( sseqId != SSEQ::SSEQ_NONE ) { SSEQ::playSequence( sseqId ); }
            BGMLoaded  = true;
            currentBGM = p_id;
        } else if( BGMLoaded ) {
            ANIMATE_MAP = false;
            SSEQ::stopSequence( );
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
            SSEQ::stopSequence( );
            BGMLoaded = false;
        }
#endif
    }

} // namespace SOUND
