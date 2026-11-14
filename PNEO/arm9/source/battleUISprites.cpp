/*
Pokémon neo
------------------------------

file        : battleUISprites.cpp
author      : Philip Wellnitz
description : the fighters on screen: stat plates, sprites, and the start / send-out / recall
presentations

Copyright (C) 2012 - 2026
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
#include "bag/item.h"
#include "battle/ability.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "battle/battleUI.h"
#include "battle/move.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/moveNames.h"
#include "gen/pokemonNames.h"
#include "io/kana.h"
#include "io/keyboard.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/simpleWidget.h"
#include "io/sprite.h"
#include "io/util.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace BATTLE {
    void battleUI::updatePkmnStats( bool p_opponent, u8 p_pos, pokemon* p_pokemon, bool p_redraw ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        u16          anchorx
            = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x + ( p_opponent ? -88 : 34 );
        u16 anchory = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y + 4;
        u16 hpx     = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x;
        u16 hpy     = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y;

        IO::regularFont->setColor( 250, 1 );
        IO::regularFont->setColor( 251, 2 );
        IO::smallFont->setColor( 250, 1 );
        IO::smallFont->setColor( 251, 2 );
        // show/hide hp bar
        bool disabled = p_pokemon == nullptr || ( p_pos && _mode != BM_DOUBLE );
        oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].isHidden = disabled;
        if( p_redraw ) {
            // clear relevant part of the screen
            dmaFillWords( 0,
                          bgGetGfxPtr( IO::bg2 )
                              + ( oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y ) * 128,
                          32 * 256 );
        }

        if( !p_pos || _mode == BM_DOUBLE ) {
            if( !p_opponent ) {
                if( p_pokemon == nullptr ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 3 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, disabled, OBJPRIORITY_0, false );
                } else if( !p_pokemon->canBattle( ) ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 2 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, false, OBJPRIORITY_0, false );
                } else if( p_pokemon->m_statusint ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 1 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, false, OBJPRIORITY_0, false );
                } else {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 0 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, false, OBJPRIORITY_0, false );
                }
            } else if( !_isWildBattle ) {
                if( p_pokemon == nullptr ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 3 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, disabled, OBJPRIORITY_0, false );
                } else if( !p_pokemon->canBattle( ) ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 2 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                } else if( p_pokemon->m_statusint ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 1 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                } else {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 0 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                }
            } else {
                if( p_pokemon != nullptr && IN_DEX( p_pokemon->getSpecies( ) ) ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 0 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                } else {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 3 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                }
            }
        }

        if( p_pokemon != nullptr && ( !p_pos || _mode == BM_DOUBLE ) ) {
            // pkmn name
            u16 namewd = IO::regularFont->stringWidth( p_pokemon->m_boxdata.getName( ) );
            if( namewd > 60 ) {
                IO::regularFont->printStringC( p_pokemon->m_boxdata.getName( ), anchorx, anchory,
                                               false );
                namewd = IO::regularFont->stringWidthC( p_pokemon->m_boxdata.getName( ) );
            } else {
                IO::regularFont->printString( p_pokemon->m_boxdata.getName( ), anchorx, anchory,
                                              false );
            }

            // Level
            IO::smallFont->setColor( 0, 0 );
            IO::smallFont->setColor( 250, 1 );
            IO::smallFont->setColor( 251, 2 );
            IO::smallFont->printString( ( "!" + std::to_string( p_pokemon->m_level ) ).c_str( ),
                                        anchorx - 2, anchory + 9, false );

            // Gender
            if( p_pokemon->getSpecies( ) != PKMN_NIDORAN_F
                && p_pokemon->getSpecies( ) != PKMN_NIDORAN_M ) {
                if( p_pokemon->m_boxdata.m_isFemale ) {
                    IO::regularFont->setColor( 254, 1 );
                    IO::regularFont->setColor( 255, 2 );
                    IO::regularFont->printString( "}", anchorx + 2 + namewd, anchory, false );
                    IO::regularFont->setColor( 250, 1 );
                    IO::regularFont->setColor( 251, 2 );
                } else if( !p_pokemon->m_boxdata.m_isGenderless ) {
                    IO::regularFont->setColor( 252, 1 );
                    IO::regularFont->setColor( 253, 2 );
                    IO::regularFont->printString( "{", anchorx + 2 + namewd, anchory, false );
                    IO::regularFont->setColor( 250, 1 );
                    IO::regularFont->setColor( 251, 2 );
                }
            }

            // Hp bars
            if( p_redraw ) {
                IO::displayHP( 0, _curHP[ !p_opponent ][ p_pos ], hpx, hpy,
                               HP_COL( p_opponent, p_pos ), HP_COL( p_opponent, p_pos ) + 1,
                               false );
            }

            IO::smallFont->setColor( HP_COL( p_opponent, p_pos ), 1 );
            IO::smallFont->setColor( HP_OUTLINE_COL, 2 );
            IO::smallFont->setColor( HP_COL( p_opponent, p_pos ) + 1, 3 );
            IO::smallFont->printString(
                HP_ICON, oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x + 10,
                oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y - 6, false ); // HP "icon"

            IO::displayHP( _curHP[ !p_opponent ][ p_pos ],
                           p_pokemon->m_stats.m_curHP * 100 / p_pokemon->m_stats.m_maxHP, hpx, hpy,
                           HP_COL( p_opponent, p_pos ), HP_COL( p_opponent, p_pos ) + 1,
                           !p_redraw && _curHP[ !p_opponent ][ p_pos ] );
            _curHP[ !p_opponent ][ p_pos ]
                = p_pokemon->m_stats.m_curHP * 100 / p_pokemon->m_stats.m_maxHP;

            IO::smallFont->printString(
                HP_ICON, oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x + 10,
                oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y - 6, false ); // HP "icon"

            // hp

            if( !p_redraw ) {
                IO::printRectangle( anchorx + 96 - 32 - 28, anchory + 16, anchorx + 96 - 6,
                                    anchory + 9 + 15, false, 0 );
            }
            IO::smallFont->setColor( 250, 1 );
            IO::smallFont->setColor( 251, 2 );
            std::array<char, 10> buffer{ };
            snprintf( buffer.data( ), buffer.size( ), "%3d", p_pokemon->m_stats.m_curHP );
            IO::smallFont->printString( buffer.data( ), anchorx + 96 - 32 - 28, anchory + 9,
                                        false );
            snprintf( buffer.data( ), buffer.size( ), "/%d", p_pokemon->m_stats.m_maxHP );
            IO::smallFont->printString( buffer.data( ), anchorx + 96 - 32 - 4, anchory + 9, false );
        }

        // Status / shiny
        if( !p_pokemon->m_stats.m_curHP ) {
            IO::loadUIIcon(
                IO::ICON::STATUS_FNT_START, SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isParalyzed ) {
            IO::loadUIIcon(
                IO::ICON::STATUS_PAR_START, SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isAsleep ) {
            IO::loadUIIcon(
                IO::ICON::STATUS_SLP_START, SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBadlyPoisoned ) {
            IO::loadUIIcon(
                IO::ICON::STATUS_TXC_START, SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBurned ) {
            IO::loadUIIcon(
                IO::ICON::STATUS_BRN_START, SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isFrozen ) {
            IO::loadUIIcon(
                IO::ICON::STATUS_FRZ_START, SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isPoisoned ) {
            IO::loadUIIcon(
                IO::ICON::STATUS_PSN_START, SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else {
            oam[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden = true;
        }

        oam[ SPR_SHINY_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden = !p_pokemon->isShiny( );
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden   = disabled;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].isHidden = disabled;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].isHidden = disabled;
        if( disabled ) {
            oam[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden = true;
        }

        IO::updateOAM( false );
    }

    void battleUI::hidePkmnStats( bool p_opponent, u8 p_pos ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].isHidden    = true;
        oam[ SPR_STATUS_BALL_OAM( 6 * !p_opponent + p_pos ) ].isHidden = true;

        // clear relevant part of the screen
        dmaFillWords( 0,
                      bgGetGfxPtr( IO::bg2 )
                          + ( oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y ) * 128,
                      32 * 256 );

        oam[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden  = true;
        oam[ SPR_SHINY_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden   = true;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden     = true;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].isHidden = true;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].isHidden = true;

        IO::updateOAM( false );
    }

    void battleUI::updatePkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        if( p_pokemon == nullptr ) {
            hidePkmn( p_opponent, p_pos );
        } else {
            loadPkmnSprite( p_opponent, p_pos, p_pokemon );
        }
        _curHP[ !p_opponent ][ p_pos ] = 0;
        updatePkmnStats( p_opponent, p_pos, p_pokemon );
    }

    void battleUI::showPkmn( bool p_opponent, u8 p_pos ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        for( u8 i = 0; i < 4; ++i ) {
            if( !p_opponent ) {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble  = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].rotationIndex = 1;
            } else {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden = false;
            }
        }
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble = true;
        }

        IO::updateOAM( false );
    }

    void battleUI::hidePkmn( bool p_opponent, u8 p_pos ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale = false;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden      = true;
        }
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = false;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden = true;
        }

        IO::updateOAM( false );
    }

    void battleUI::faintPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        SpriteEntry*          oam = IO::OamTop->oamBuffer;
        std::array<char, 100> buffer{ };
        SOUND::playSoundEffect( SFX_BATTLE_FAINT );

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = false;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden = true;
        }

        for( u8 f = 0; f < 24 * ( 1 + !p_opponent ); ++f ) {
            for( u8 i = 0; i < 4; ++i ) {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].y += 1;
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }

        hidePkmn( p_opponent, p_pos );
        hidePkmnStats( p_opponent, p_pos );

        auto fmt = std::string( GET_STRING( IO::STR_BATTLE_PKMN_FAINTED ) );
        if( p_pokemon != nullptr ) [[likely]] {
            snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                      getPkmnName( p_pokemon, p_opponent ).c_str( ) );
        } else {
#ifdef DESQUID
            snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), "[it's a nullptr]" );
#endif
        }
        log( std::string( buffer.data( ) ) );
    }

    void battleUI::loadPkmnSprite( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        u16 x = ( _mode == BM_SINGLE )
                    ? ( p_opponent ? PKMN_OPP_1_X_SINGLE : PKMN_OWN_1_X_SINGLE )
                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_X : PKMN_OPP_1_X )
                                     : ( p_pos ? PKMN_OWN_2_X : PKMN_OWN_1_X ) ) );
        u16 y = ( _mode == BM_SINGLE )
                    ? ( p_opponent ? PKMN_OPP_1_Y_SINGLE : PKMN_OWN_1_Y_SINGLE )
                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_Y : PKMN_OPP_1_Y )
                                     : ( p_pos ? PKMN_OWN_2_Y : PKMN_OWN_1_Y ) ) );

        auto pinfo = p_pokemon->getSpriteInfo( );
        y += IO::pkmnSpriteHeight( pinfo );

        if( p_opponent ) {
            IO::loadPKMNSprite( pinfo, x, y, SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ),
                                SPR_PKMN_PAL( 2 * ( !p_opponent ) + p_pos ),
                                SPR_PKMN_GFX( 2 * ( !p_opponent ) + p_pos ), false );
        } else {
            IO::loadPKMNSpriteBack( pinfo, x, y, SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ),
                                    SPR_PKMN_PAL( 2 * ( !p_opponent ) + p_pos ),
                                    SPR_PKMN_GFX( 2 * ( !p_opponent ) + p_pos ), false );

            for( u8 i = 0; i < 4; ++i ) {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble  = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].rotationIndex = 1;
            }
        }
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].priority = OBJPRIORITY_3;
        }

        u16 emptyPal[ 32 ]                = { 0, 0, 0 };
        IO::OamTop->matrixBuffer[ 2 ].hdx = ( 1LLU << 8 );
        IO::OamTop->matrixBuffer[ 2 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 2 ].vdx = ( 1LLU << 9 );
        IO::OamTop->matrixBuffer[ 2 ].vdy = ( 1LLU << 10 );

        IO::OamTop->matrixBuffer[ 3 ].hdx = 187;
        IO::OamTop->matrixBuffer[ 3 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 3 ].vdx = 187 << 1;
        IO::OamTop->matrixBuffer[ 3 ].vdy = 187 << 2;

        u16 sy    = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0 ].y + 8
                    - ( IO::pkmnSpriteHeight( pinfo ) / 2 ),
            sx    = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0 ].x + 2
                    - 3 * IO::pkmnSpriteHeight( pinfo ) / 4;
        s8  diffx = 80, diffy = 44;
        s8  ske = -23;
        if( !p_opponent ) {
            sy += 4;
            diffx = 97;
            diffy = 48;
            ske   = -36;
        }
        IO::loadSprite(
            SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0, SPR_PKMN_SHADOW_PAL,
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0 ].gfxIndex, sx, sy, 64, 64,
            emptyPal, 0, 0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite(
            SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 1, SPR_PKMN_SHADOW_PAL,
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].gfxIndex, diffx + sx, sy,
            32, 64, 0, 0, 0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 2,
                        SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].gfxIndex,
                        ske + sx, diffy + sy, 64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3,
                        false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 3,
                        SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 3 ].gfxIndex,
                        ske + diffx + sx, diffy + sy, 32, 32, 0, 0, 0, false, false, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );

        if( !p_opponent ) {
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) ].x -= 48;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) ].y -= 48;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].y -= 48;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].x -= 48;
        }

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].rotationIndex
                = 3 - p_opponent;
        }

        IO::updateOAM( false );
    }

    void battleUI::startWildBattle( pokemon* p_pokemon ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        IO::fadeScreen( IO::UNFADE, true, true );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
        bgUpdate( );
        // Load pkmn sprite
        pkmnSpriteInfo pinfo = p_pokemon->getSpriteInfo( );
        IO::loadPKMNSprite( pinfo, WILD_BATTLE_SPRITE_X_START,
                            OPP_PLAT_Y + 35 - 96 + IO::pkmnSpriteHeight( pinfo ),
                            SPR_PKMN_START_OAM( 0 ), SPR_PKMN_PAL( 0 ), SPR_PKMN_GFX( 0 ), false );

        for( u8 i = 0; i < 4; ++i ) { oam[ SPR_PKMN_START_OAM( 0 ) + i ].priority = OBJPRIORITY_3; }
        u16 emptyPal[ 32 ]                = { 0, 0, 0 };
        IO::OamTop->matrixBuffer[ 2 ].hdx = ( 1LLU << 8 );
        IO::OamTop->matrixBuffer[ 2 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 2 ].vdx = ( 1LLU << 9 );
        IO::OamTop->matrixBuffer[ 2 ].vdy = ( 1LLU << 10 );

        u16 sy = oam[ SPR_PKMN_START_OAM( 0 ) + 0 ].y + 8 - ( IO::pkmnSpriteHeight( pinfo ) / 2 ),
            sx = oam[ SPR_PKMN_START_OAM( 0 ) + 0 ].x + 2 - 3 * IO::pkmnSpriteHeight( pinfo ) / 4;
        s8  diffx = 80, diffy = 44;
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 0, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 0 ].gfxIndex, sx, sy, 64, 64, emptyPal, 0, 0,
                        false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 1, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 1 ].gfxIndex, diffx + sx, sy, 32, 64, 0, 0,
                        0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 2, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 2 ].gfxIndex, -23 + sx, diffy + sy, 64, 32,
                        0, 0, 0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 3, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 3 ].gfxIndex, -23 + diffx + sx, diffy + sy,
                        32, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + i ].isRotateScale = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + i ].isSizeDouble  = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + i ].rotationIndex = 2;
        }

        IO::updateOAM( false );

        for( u16 i = WILD_BATTLE_SPRITE_X_START; i < WILD_BATTLE_SPRITE_X; ++i ) {
            for( u8 j = 0; j < 4; ++j ) {
                oam[ SPR_PKMN_START_OAM( 0 ) + j ].x++;
                oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + j ].x++;
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }

        SOUND::playCry( p_pokemon->getSpecies( ), p_pokemon->getForme( ), p_pokemon->isFemale( ) );

        for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }

        if( p_pokemon->isShiny( ) ) { animateShiny( true, 0, p_pokemon->m_boxdata.m_shinyType ); }
        _curHP[ 0 ][ 0 ] = 0;
        updatePkmnStats( true, 0, p_pokemon );

        std::array<char, 64> buffer{ }; // headroom for name markers
        snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_ENCOUNTERED_WILD ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ) );
        log( std::string( buffer.data( ) ) );
    }

    void battleUI::startTrainerBattle( battleTrainer* p_trainer ) {
        _battleTrainer = p_trainer;

        SpriteEntry* oam = IO::OamTop->oamBuffer;

        IO::fadeScreen( IO::UNFADE, true, true );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
        bgUpdate( );
        // Load trainer sprite

        IO::loadTrainerSprite( _battleTrainer->m_data.m_trainerBG, WILD_BATTLE_SPRITE_X + 16,
                               OPP_PLAT_Y - 96 + 35, SPR_PKMN_START_OAM( 0 ), SPR_PKMN_PAL( 0 ),
                               SPR_PKMN_GFX( 0 ), false );

        IO::OamTop->matrixBuffer[ 5 ].hdx            = 1 << 7 | 1 << 6 | 1 << 5;
        IO::OamTop->matrixBuffer[ 5 ].vdy            = 1 << 7 | 1 << 6 | 1 << 5;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isRotateScale = true;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isSizeDouble  = true;
        oam[ SPR_PKMN_START_OAM( 0 ) ].rotationIndex = 5;

        IO::updateOAM( false );
        std::array<char, 50> buffer{ };
        auto                 fmt = std::string( GET_STRING( IO::STR_BATTLE_CHALLENGED ) );
        snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                  FS::getTrainerClassName( _battleTrainer->getClass( ) ).c_str( ),
                  _battleTrainer->m_strings.m_name );
        log( std::string( buffer.data( ) ) );

        // Slide trainer out

        for( u8 i = 40; i > 0; --i ) {
            oam[ SPR_PKMN_START_OAM( 0 ) ].x += 1;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        oam[ SPR_PKMN_START_OAM( 0 ) ].isRotateScale = false;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isSizeDouble  = false;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isHidden      = true;
        IO::updateOAM( false );
    }

    void battleUI::sendOutPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        if( p_pokemon != nullptr ) {
            SAVE::CURRENT_FILE->registerSeenPkmn( p_pokemon->getSpecies( ) );
        }
        IO::fadeScreen( IO::UNFADE, true, true );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
        bgUpdate( );

        std::array<char, 100> buffer{ };
        if( p_opponent ) {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_SENDS_OUT ) );
            snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                      FS::getTrainerClassName( _battleTrainer->getClass( ) ).c_str( ),
                      _battleTrainer->m_strings.m_name, p_pokemon->m_boxdata.getNameS( ).c_str( ) );
            log( std::string( buffer.data( ) ) );
        } else {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_SEND_OUT_GO ),
                      p_pokemon->m_boxdata.getNameS( ).c_str( ) );
            log( std::string( buffer.data( ) ) );
        }

        // play pokeball animation
        animateBallRelease( p_opponent, p_pos, p_pokemon->getBall( ) );
        loadPkmnSprite( p_opponent, p_pos, p_pokemon );
        for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }

        SOUND::playCry( p_pokemon->getSpecies( ), p_pokemon->getForme( ), p_pokemon->isFemale( ) );

        for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }

        if( p_pokemon->isShiny( ) ) {
            animateShiny( p_opponent, p_pos, p_pokemon->m_boxdata.m_shinyType );
        }

        _curHP[ !p_opponent ][ p_pos ] = 0;
        updatePkmnStats( p_opponent, p_pos, p_pokemon );
    }

    void battleUI::sendOutFollowPkmn( u8 p_pos, pokemon* p_pokemon ) {
        if( p_pokemon != nullptr ) {
            SAVE::CURRENT_FILE->registerSeenPkmn( p_pokemon->getSpecies( ) );
        }
        IO::fadeScreen( IO::UNFADE, true, true );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
        bgUpdate( );

        std::array<char, 100> buffer{ };
        snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_SEND_OUT_GO ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ) );
        log( std::string( buffer.data( ) ) );

        // No ball animation, pkmn slides in from left

        constexpr s16 diffx2 = 60;

        SpriteEntry* oam = IO::OamTop->oamBuffer;

        u16 x = ( _mode == BM_SINGLE ) ? ( PKMN_OWN_1_X_SINGLE )
                                       : ( p_pos ? PKMN_OWN_2_X : PKMN_OWN_1_X );
        u16 y = ( _mode == BM_SINGLE ) ? ( PKMN_OWN_1_Y_SINGLE )
                                       : ( p_pos ? PKMN_OWN_2_Y : PKMN_OWN_1_Y );

        auto pinfo = p_pokemon->getSpriteInfo( );
        y += IO::pkmnSpriteHeight( pinfo );

        s16 stx = s16( x ) - diffx2;

        IO::loadPKMNSpriteBack( pinfo, stx, y, SPR_PKMN_START_OAM( 2 + p_pos ),
                                SPR_PKMN_PAL( 2 + p_pos ), SPR_PKMN_GFX( 2 + p_pos ), false );

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + i ].isRotateScale = true;
            oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + i ].isSizeDouble  = true;
            oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + i ].rotationIndex = 1;
        }
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + i ].priority = OBJPRIORITY_3;
        }

        u16 emptyPal[ 32 ]                = { 0, 0, 0 };
        IO::OamTop->matrixBuffer[ 2 ].hdx = ( 1LLU << 8 );
        IO::OamTop->matrixBuffer[ 2 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 2 ].vdx = ( 1LLU << 9 );
        IO::OamTop->matrixBuffer[ 2 ].vdy = ( 1LLU << 10 );

        IO::OamTop->matrixBuffer[ 3 ].hdx = 187;
        IO::OamTop->matrixBuffer[ 3 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 3 ].vdx = 187 << 1;
        IO::OamTop->matrixBuffer[ 3 ].vdy = 187 << 2;

        u16 sy    = oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 0 ].y + 8
                    - ( IO::pkmnSpriteHeight( pinfo ) / 2 ),
            sx    = oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 0 ].x + 2
                    - 3 * IO::pkmnSpriteHeight( pinfo ) / 4;
        s8  diffx = 80, diffy = 44;
        s8  ske = -23;
        sy += 4;
        diffx = 97;
        diffy = 48;
        ske   = -36;

        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + 0, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 0 ].gfxIndex, sx, sy, 64, 64,
                        emptyPal, 0, 0, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + 1, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 1 ].gfxIndex, diffx + sx, sy, 32, 64,
                        0, 0, 0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + 2, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 2 ].gfxIndex, ske + sx, diffy + sy,
                        64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + 3, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 3 ].gfxIndex, ske + diffx + sx,
                        diffy + sy, 32, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );

        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) ].x -= 48;
        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) ].y -= 48;
        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 1 ].y -= 48;
        oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + 2 ].x -= 48;

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + i ].isRotateScale = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + i ].isSizeDouble  = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + i ].rotationIndex = 3;
        }

        IO::updateOAM( false );

        // slide by diffx2 px to the right

        for( u8 d = 0; d < diffx2; d += 2 ) {
            swiWaitForVBlank( );
            for( u8 i = 0; i < 4; ++i ) {
                oam[ SPR_PKMN_SHADOW_START_OAM( 2 + p_pos ) + i ].x += 2;
                oam[ SPR_PKMN_START_OAM( 2 + p_pos ) + i ].x += 2;
            }
            IO::updateOAM( false );
        }

        SOUND::playCry( p_pokemon->getSpecies( ), p_pokemon->getForme( ), p_pokemon->isFemale( ) );

        for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }

        if( p_pokemon->isShiny( ) ) {
            animateShiny( false, p_pos, p_pokemon->m_boxdata.m_shinyType );
        }

        _curHP[ !false ][ p_pos ] = 0;
        updatePkmnStats( false, p_pos, p_pokemon );
    }

    void battleUI::recallPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon, bool p_forced ) {
        std::array<char, 100> buffer{ };
        if( p_opponent && _battleTrainer != nullptr ) {
            // TODO
            //            snprintf( buffer.data(), buffer.size(), GET_STRING(
            //            IO::STR_BATTLE_PKMN_RETURNED + p_forced ),
            //                    FS::getTrainerClassName( _battleTrainer->getClass( ) ).c_str( ),
            //                    _battleTrainer->m_strings.m_name, p_pokemon->m_boxdata.m_name );
            //            log( std::string( buffer.data()) );
        } else {
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_PKMN_RETURNED + p_forced ),
                      p_pokemon->m_boxdata.getNameS( ).c_str( ) );
            log( std::string( buffer.data( ) ) );
        }

        // Hide pkmn and status
        SOUND::playSoundEffect( SFX_BATTLE_RECALL );
        for( u8 i = 0; i < 10; ++i ) { swiWaitForVBlank( ); }
        hidePkmn( p_opponent, p_pos );
        hidePkmnStats( p_opponent, p_pos );
        for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
    }

} // namespace BATTLE
