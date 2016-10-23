/*
Pokémon Emerald 2 Version
------------------------------

file        : battleUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2016
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

#pragma once

namespace BATTLE {
    class battle;

    class battleUI {
        battle* _battle;
        s8      _oldPKMNStats[ 6 ][ 2 ][ 10 ];
        u16     initStsBalls( bool p_bottom, u16& p_tilecnt );
        void    loadSpritesTop( );
        void    loadSpritesSub( );
        void    loadBattleUITop( );
        void    drawPKMNChoiceScreen( bool p_firstIsChosen );
        void    drawAttackTargetChoice( bool p_selected[ 4 ], bool p_neverTarget[ 4 ], u8 p_pokemonPos );
        void    drawPkmnChoicePkmn( u8 p_index, bool p_firstIsChosen, bool p_pressed );
    public:
        friend class battle;

        static void initLogScreen( );
        static void clearLogScreen( );
        static void setLogTextColor( u16 p_color );
        static void setLogText2Color( u16 p_color );
        static void writeLogText( const std::string& p_message );

        void    init( );
        void    trainerIntro( );
        void    pokemonIntro( );

        void    redrawBattle( );

        bool    declareBattleMove( u8 p_pokemonPos, bool p_showBack );

        u16     chooseAttack( u8 p_pokemonPos );
        u8      chooseAttackTarget( u8 p_pokemonPos, u16 p_moveNo );
        u16     chooseItem( );
        u8      choosePKMN( bool p_firstIsChosen, bool p_back = true, bool p_noRestrict = false );

        void    showAttack( bool p_opponent, u8 p_pokemonPos );
        void    updateHP( bool p_opponent, u8 p_pokemonPos, u16 p_oldHP, u16 p_oldHPmax = -1 );
        void    showStatus( bool p_opponent, u8 p_pokemonPos );
        void    updateStatus( bool p_opponent, u8 p_pokemonPos );
        void    applyEXPChanges( bool p_opponent, u8 p_pokemonPos, u32 p_gainedExp );
        void    updateStats( bool p_opponent, u8 p_pokemonPos, bool p_move = true );

        void    capture( u16 p_pokeBall, u8 p_ticks );
        void    handleCapture( );

        bool    isVisiblePKMN( bool p_opponent, u8 p_pokemonPos );

        void    hidePKMN( bool p_opponent, u8 p_pokemonPos );
        void    sendPKMN( bool p_opponent, u8 p_pokemonPos, bool p_silent = false );
        void    evolvePKMN( bool p_opponent, u8 p_pokemonPos );

        void    learnMove( u8 p_pokemonPos, u16 p_move );

        void    showEndScreen( );

        battleUI( ) { }
        battleUI( battle* p_battle )
            :_battle( p_battle ) { }
    };
}
