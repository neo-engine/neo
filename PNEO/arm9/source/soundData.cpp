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

#include <nds.h>

#include "fs/data.h"
#include "gen/bgmNames.h"
#include "gen/locationNames.h"
#include "gen/pokemonNames.h"
#include "sound/sound.h"

namespace SOUND {
    u16             currentLocation = 0;
    MAP::moveMode   currentMoveMode = MAP::WALK;
    MAP::mapWeather currentWeather  = MAP::REGULAR;

    s16 currentJBoxBGM = JBOX_DISABLED;

    bool tracerActive = false;

    u16 LAST_CRY = -1;

    void playCry( u16 p_pokemonId, u8 p_formeId, bool ) {
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
            u32 len;
            u8* cry = FS::readCry( p_pokemonId, p_formeId, len );
            if( cry == nullptr ) {
                if( p_pokemonId != 1 ) { playCry( 1 ); }
                return;
            }
#ifdef MMOD
            len <<= 1;
            if( LAST_CRY != u16( -1 ) ) { soundKill( LAST_CRY ); }
            LAST_CRY = soundPlaySample( cry, SoundFormat_8Bit, len, 22050, 127, 64, false, 0 );
#else
            if( LAST_CRY != u16( -1 ) ) { SSEQ::stopSample( LAST_CRY ); }
            auto sInfo = SSEQ::sampleInfo{ SSEQ::sampleInfo::WT_PCM8, false, 22050, 760, 0, len };
            auto pInfo = SSEQ::playInfo{ 127, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0 };
            LAST_CRY   = SSEQ::playSample( cry, sInfo, pInfo );
#endif
        }
#else
        (void) p_pokemonId;
        (void) p_formeId;
#endif
    }

    void playSoundEffect( u16 p_id ) {
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
            u16 len;
            u8* sfx = FS::readSFX( p_id, len );
            if( sfx == nullptr ) { return; }

#ifdef MMOD
            len <<= 1;
            if( LAST_CRY != u16( -1 ) ) { soundKill( LAST_CRY ); }
            LAST_CRY = soundPlaySample( sfx, SoundFormat_8Bit, len, 22050, 127, 64, false, 0 );
#else
            if( LAST_CRY != u16( -1 ) ) { SSEQ::stopSample( LAST_CRY ); }
            auto sInfo = SSEQ::sampleInfo{ SSEQ::sampleInfo::WT_PCM8, false, 22050, 760, 0, len };
            auto pInfo = SSEQ::playInfo{ 127, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0 };
            LAST_CRY   = SSEQ::playSample( sfx, sInfo, pInfo );
#endif
        }
#else
        (void) p_id;
#endif
    }

    s16 getJBoxBGM( ) {
        return currentJBoxBGM;
    }

    void setJBoxBGM( s16 p_id ) {
        if( p_id != currentJBoxBGM ) {
            if( p_id != JBOX_DISABLED ) {
                playBGM( p_id );
            } else {
                playBGM( BGMforWeather( currentWeather ) );
            }
            currentJBoxBGM = p_id;
        }
    }

    void setTracerStatus( bool p_active, bool p_silent ) {
        if( !tracerActive && !p_active ) { return; }
        tracerActive = p_active;
        if( !p_silent ) { restartBGM( ); }
    }

    void onLocationChange( u16 p_newLocation ) {
        if( currentLocation == p_newLocation ) { return; }

        currentLocation = p_newLocation;

        if( currentJBoxBGM != JBOX_DISABLED || BGMforced || tracerActive ) { return; }
        if( currentMoveMode == MAP::WALK ) { playBGM( FS::BGMforLocation( currentLocation ) ); }
    }

    void onMovementTypeChange( MAP::moveMode p_newMoveMode ) {
        if( currentMoveMode == p_newMoveMode ) { return; }

        currentMoveMode = p_newMoveMode;
        if( currentJBoxBGM != JBOX_DISABLED || BGMforced || tracerActive ) { return; }
        playBGM( BGMforWeather( currentWeather ) );
    }

    void onWeatherChange( MAP::mapWeather p_newWeather ) {
        if( currentWeather == p_newWeather ) { return; }

        currentWeather = p_newWeather;
        if( currentJBoxBGM != JBOX_DISABLED || BGMforced || tracerActive ) { return; }
        playBGM( BGMforWeather( currentWeather ) );
    }

    void restartBGM( ) {
        BGMforced = false;
        if( tracerActive ) {
            playBGM( BGM_POKE_RADAR );
        } else if( currentJBoxBGM != JBOX_DISABLED ) {
            playBGM( currentJBoxBGM );
        } else {
            playBGM( BGMforWeather( currentWeather ) );
        }
    }

    s16 BGMforWeather( MAP::mapWeather p_weather ) {
        switch( p_weather ) {
        case MAP::SANDSTORM: return BGM_DESERT;
        default: return BGMforMoveMode( currentMoveMode );
        }
    }

    s16 BGMforMoveMode( MAP::moveMode p_moveMode ) {
        switch( p_moveMode ) {
        case MAP::DIVE: return BGM_DIVING;
        case MAP::SURF: return BGM_SURFING;
        case MAP::BIKE:
        case MAP::ACRO_BIKE:
        case MAP::MACH_BIKE:
        case MAP::BIKE_JUMP: return BGM_CYCLING;
        case MAP::WALK:
        case MAP::ROCK_CLIMB:
        case MAP::SIT:
        default: return FS::BGMforLocation( currentLocation );
        }
    }
} // namespace SOUND
