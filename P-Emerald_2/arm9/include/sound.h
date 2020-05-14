/*
    Pokémon neo
    ------------------------------

    file        : sound.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file (item.cpp) for details.

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

#pragma once

#include <nds/ndstypes.h>
#include "soundbank.h"
#include "mapDefines.h"

void initSound( );

namespace SOUND {
#define NUM_SOUND_EFFECTS 16
    void playBGM( u16 p_id );
    void playSoundEffect( u16 p_id );

    void setVolume( u16 p_newValue );
    void dimVolume( );
    void restoreVolume( );

    u16 BGMforTrainerEncounter( u16 p_trianerClassId );
    u16 BGMforTrainerBattle( u16 p_trianerClassId );
    u16 BGMforWildBattle( u16 p_pokemonId );
    u16 BGMforLocation( u16 p_locationId );
    u16 BGMforMoveMode( MAP::moveMode p_moveMode );

    void onLocationChange( u16 p_newLocatonId );
    void onMovementTypeChange( MAP::moveMode p_newMoveMode );
    void restartBGM( );
}
