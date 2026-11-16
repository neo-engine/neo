/*
Pokémon neo
------------------------------

file        : battleUIInput.cpp
author      : Philip Wellnitz
description : the input screens: move and attack menus, target and yes/no choices

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
    const u16 MOVEBOX3_SPR_PAL[ 6 ] = { 0x5208, 0x294A };
    const u16 MOVEBOX4_SPR_PAL[ 6 ] = { 0x18C6, 0x294A };
    void battleUI::prepareMove( pokemon* p_pokemon, bool p_opponent, u8 p_pos, battleMove p_move ) {
        auto                  pkmnstr  = getPkmnName( p_pokemon, p_opponent );
        bool                  hidepkmn = true;
        std::array<char, 100> buffer{ };
        switch( p_move.m_param ) {
        case M_DIVE:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_DIVES ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
            break;
        case M_DIG:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_DUG_HOLE ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
            break;
        case M_FLY:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_FLIES_UP ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
            break;
        case M_BOUNCE:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_JUMPS_UP ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
            break;
        case M_SKY_DROP:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_SKYDROP ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
            break;
        case M_PHANTOM_FORCE:
        case M_SHADOW_FORCE:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_DISAPPEARED ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
            break;
        default: hidepkmn = false; break;
        }
        if( hidepkmn ) { hidePkmn( p_opponent, p_pos ); }
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    battleUI::showMoveSelection( pokemon* p_pokemon, u8 p_slot, u8 p_highlightedButton ) {
        SpriteEntry*          oam = IO::Oam->oamBuffer;
        std::array<char, 100> buffer{ };

        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        if( p_highlightedButton == u8( -1 ) ) {
            // initialize stuff
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub", 512, 49152,
                                 true );

            for( u8 i = 0; i < 2; ++i ) {
                u16* pal   = IO::BG_PAL( i );
                pal[ 0 ]   = 0;
                pal[ 250 ] = IO::WHITE;
                pal[ 251 ] = IO::GRAY;
                pal[ 252 ] = IO::RGB( 18, 22, 31 );
                pal[ 254 ] = IO::RGB( 31, 18, 18 );
                pal[ 253 ] = IO::RGB( 0, 0, 25 );
                pal[ 255 ] = IO::RGB( 23, 0, 0 );
            }

            // Clear log window
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), COMPLETE_SCREEN );
            for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = true; }

            IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
            REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
            bgUpdate( );

            for( u8 i = 0; i < 8; ++i ) { oam[ SPR_SMALL_MESSAGE_OAM_SUB + i ].isHidden = false; }

            for( u8 i = 0; i < 2; ++i ) {
                oam[ SPR_BATTLE_FITE_OAM_SUB + i ].isHidden = false;
                oam[ SPR_BATTLE_PKMN_OAM_SUB + i ].isHidden = false;
                oam[ SPR_BATTLE_RUN_OAM_SUB + i ].isHidden  = false;
                oam[ SPR_BATTLE_BAG_OAM_SUB + i ].isHidden  = false;
            }

            if( !_isWildBattle && !p_slot ) {
                IO::loadUIIcon( IO::ICON::BATTLE_INACTIVE_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                                SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                128, 68 + 64 + 18, 32, 32, true, true, false, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            } else {
                IO::loadUIIcon( IO::ICON::BATTLE_RUN_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                                SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                128, 68 + 64 + 18, 32, 32, true, true, false, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }

            pkmnSpriteInfo pinfo = p_pokemon->getSpriteInfo( );
            IO::loadPKMNIcon( pinfo, oam[ SPR_BATTLE_ICON_OAM_SUB ].x,
                              oam[ SPR_BATTLE_ICON_OAM_SUB ].y, SPR_BATTLE_ICON_OAM_SUB,
                              SPR_BATTLE_ICON_PAL_SUB, oam[ SPR_BATTLE_ICON_OAM_SUB ].gfxIndex,
                              true );

            for( u8 i = 0; i < 4; ++i ) {
                oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = true;
                oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
                for( u8 j = 0; j < 6; j++ ) { oam[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = true; }
            }
            IO::updateOAM( true );

            IO::regularFont->printString( GET_STRING( IO::STR_BATTLE_MENU_FIGHT ), 128,
                                          oam[ SPR_BATTLE_FITE_OAM_SUB ].y + 34, true,
                                          IO::font::CENTER );
            IO::regularFont->printStringC(
                GET_STRING( IO::STR_BATTLE_MENU_POKEMON ), oam[ SPR_BATTLE_PKMN_OAM_SUB ].x + 32,
                oam[ SPR_BATTLE_PKMN_OAM_SUB ].y + 9, true, IO::font::CENTER );
            if( _isWildBattle || !p_slot ) {
                IO::regularFont->printStringC(
                    GET_STRING( IO::STR_BATTLE_MENU_RUN ), oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                    oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true, IO::font::CENTER );
            } else {
                IO::regularFont->printStringC(
                    GET_STRING( IO::STR_BATTLE_MENU_BACK ), oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                    oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true, IO::font::CENTER );
            }
            IO::regularFont->printStringC(
                GET_STRING( IO::STR_BATTLE_MENU_BAG ), oam[ SPR_BATTLE_BAG_OAM_SUB ].x + 32,
                oam[ SPR_BATTLE_BAG_OAM_SUB ].y + 9, true, IO::font::CENTER );

            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_WHAT_TO_DO ),
                      p_pokemon->m_boxdata.getNameS( ).c_str( ) );
            IO::regularFont->printStringC( buffer.data( ), 128,
                                           oam[ SPR_SMALL_MESSAGE_OAM_SUB ].y + 4, true,
                                           IO::font::CENTER );
        }
        if( p_highlightedButton == 0 ) {
            IO::loadUIIcon( IO::ICON::BATTLE_FITE_2_START, SPR_BATTLE_FITE_OAM_SUB + 1,
                            SPR_BATTLE_FITE_PAL_SUB, oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].y, 64, 64, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadUIIcon( IO::ICON::BATTLE_FITE_START, SPR_BATTLE_FITE_OAM_SUB + 1,
                            SPR_BATTLE_FITE_PAL_SUB, oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].y, 64, 64, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_FITE_OAM_SUB ].x,
                                                   oam[ SPR_BATTLE_FITE_OAM_SUB ].y,
                                                   oam[ SPR_BATTLE_FITE_OAM_SUB ].x + 128,
                                                   oam[ SPR_BATTLE_FITE_OAM_SUB ].y + 60 ),
                                  0 ) );

        if( p_highlightedButton == 1 ) {
            IO::loadUIIcon( IO::ICON::BATTLE_PKMN_2_START, SPR_BATTLE_PKMN_OAM_SUB + 1,
                            SPR_BATTLE_PKMN_PAL_SUB, oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadUIIcon( IO::ICON::BATTLE_PKMN_START, SPR_BATTLE_PKMN_OAM_SUB + 1,
                            SPR_BATTLE_PKMN_PAL_SUB, oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_PKMN_OAM_SUB ].x,
                                                   oam[ SPR_BATTLE_PKMN_OAM_SUB ].y,
                                                   oam[ SPR_BATTLE_PKMN_OAM_SUB ].x + 64,
                                                   oam[ SPR_BATTLE_PKMN_OAM_SUB ].y + 32 ),
                                  1 ) );

        if( p_highlightedButton == 2 ) {
            if( !p_slot && !_isWildBattle ) {
                IO::loadUIIcon( IO::ICON::BATTLE_INACTIVE_2_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                                SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadUIIcon( IO::ICON::BATTLE_RUN_2_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                                SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        } else {
            if( !p_slot && !_isWildBattle ) {
                IO::loadUIIcon( IO::ICON::BATTLE_INACTIVE_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                                SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadUIIcon( IO::ICON::BATTLE_RUN_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                                SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        }

        if( _isWildBattle ) {
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      2 ) );
        }

        if( p_highlightedButton == 3 ) {
            if( _allowItems ) {
                IO::loadUIIcon( IO::ICON::BATTLE_BAG_2_START, SPR_BATTLE_BAG_OAM_SUB + 1,
                                SPR_BATTLE_BAG_PAL_SUB, oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadUIIcon( IO::ICON::BATTLE_INACTIVE_2_START, SPR_BATTLE_BAG_OAM_SUB + 1,
                                SPR_BATTLE_BAG_PAL_SUB, oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        } else {
            if( _allowItems ) {
                IO::loadUIIcon( IO::ICON::BATTLE_BAG_START, SPR_BATTLE_BAG_OAM_SUB + 1,
                                SPR_BATTLE_BAG_PAL_SUB, oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadUIIcon( IO::ICON::BATTLE_INACTIVE_START, SPR_BATTLE_BAG_OAM_SUB + 1,
                                SPR_BATTLE_BAG_PAL_SUB, oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        }

        if( _allowItems ) {
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_BAG_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_BAG_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_BAG_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_BAG_OAM_SUB ].y + 32 ),
                                      3 ) );
        }
        return res;
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    battleUI::showAttackSelection( boxPokemon* p_pokemon, bool p_canUseMove[ 4 ],
                                   bool p_showMegaEvolution, u8 p_highlightedButton,
                                   bool p_megaButtonActive ) {
        (void) p_megaButtonActive;

        auto                  res = std::vector<std::pair<IO::inputTarget, u8>>( );
        SpriteEntry*          oam = IO::Oam->oamBuffer;
        std::array<char, 100> buffer{ };

        // hide yn choice boxes here b/c learn move.
        for( u8 i = 0; i < 4; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
            }
        }

        if( p_highlightedButton == u8( -1 ) ) {
            // initialize stuff
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub3", 512,
                                 49152, true );
            u16* pal   = IO::BG_PAL( true );
            pal[ 0 ]   = 0;
            pal[ 250 ] = IO::WHITE;
            pal[ 251 ] = IO::GRAY;
            pal[ 252 ] = IO::RGB( 18, 22, 31 );
            pal[ 254 ] = IO::RGB( 31, 18, 18 );
            pal[ 253 ] = IO::RGB( 0, 0, 25 );
            pal[ 255 ] = IO::RGB( 23, 0, 0 );

            // Clear log window
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), COMPLETE_SCREEN );
            for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = true; }

            for( u8 i = 0; i < 8; ++i ) { oam[ SPR_SMALL_MESSAGE_OAM_SUB + i ].isHidden = false; }

            for( u8 i = 0; i < 2; ++i ) {
                oam[ SPR_BATTLE_FITE_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_PKMN_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_RUN_OAM_SUB + i ].isHidden  = false;
                oam[ SPR_BATTLE_BAG_OAM_SUB + i ].isHidden  = true;
            }

            oam[ SPR_BATTLE_ICON_OAM_SUB ].isHidden = true;

            IO::loadUIIcon( IO::ICON::BATTLE_RUN_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                            SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex, 128,
                            68 + 64 + 18, 32, 32, true, true, false, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );

            // Moves
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( 0, 2 );
            auto mdatas = std::vector<moveData>( );
            for( int i = 0; i < 4; i++ ) {
                oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = !p_pokemon->getMove( i );
                oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = !p_pokemon->getMove( i );
                for( u8 j = 0; j < 6; j++ ) { oam[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = false; }

                res.push_back( std::pair( IO::inputTarget( oam[ SPR_MOVE_OAM_SUB( i ) ].x,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].y,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].x + 96,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].y + 32 ),
                                          p_pokemon->getMove( i ) && p_canUseMove[ i ]
                                              ? i
                                              : IO::choiceBox::DISABLED_CHOICE ) );

                if( !p_pokemon->getMove( i ) ) {
                    IO::loadTypeIcon( TYPE_UNKNOWN, oam[ SPR_TYPE_OAM_SUB( i ) ].x,
                                      oam[ SPR_TYPE_OAM_SUB( i ) ].y, SPR_TYPE_OAM_SUB( i ),
                                      SPR_TYPE_PAL_SUB( i ), oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex,
                                      true, CURRENT_LANGUAGE );
                    oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden = true;
                    continue;
                }
                auto mdata = FS::getMoveData( p_pokemon->getMove( i ) );
                mdatas.push_back( mdata );

                type t;
                if( p_pokemon->getMove( i ) != M_HIDDEN_POWER ) {
                    t = mdata.m_type;
                } else {
                    t = p_pokemon->getHPType( );
                }
                IO::loadTypeIcon( t, oam[ SPR_TYPE_OAM_SUB( i ) ].x, oam[ SPR_TYPE_OAM_SUB( i ) ].y,
                                  SPR_TYPE_OAM_SUB( i ), SPR_TYPE_PAL_SUB( i ),
                                  oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex, true, CURRENT_LANGUAGE );
                IO::loadDamageCategoryIcon( mdata.m_category, oam[ SPR_DMG_CAT_OAM_SUB( i ) ].x,
                                            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].y,
                                            SPR_DMG_CAT_OAM_SUB( i ), SPR_DMG_CAT_PAL_SUB( i ),
                                            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].gfxIndex, true );
                IO::copySpritePal( MOVEBOX4_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
            }

            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      4 ) );
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      IO::choiceBox::BACK_CHOICE ) );

            // Mega button
            if( p_showMegaEvolution ) {
                res.push_back( std::pair( IO::inputTarget( 0, 0, 0 ), 5 ) );
            }

            IO::updateOAM( true );

            IO::regularFont->printStringC(
                GET_STRING( IO::STR_BATTLE_MENU_BACK ), oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true, IO::font::CENTER );

            IO::regularFont->printStringC( GET_STRING( IO::STR_BATTLE_SELECT_A_MOVE ), 128,
                                           oam[ SPR_SMALL_MESSAGE_OAM_SUB ].y + 4, true,
                                           IO::font::CENTER );

            for( int i = 0; i < 4; i++ ) {
                if( !p_pokemon->getMove( i ) ) continue;
                auto mname = std::string{ FS::kanaWrap(
                    FS::getMoveName( p_pokemon->getMove( i ) ).c_str( ), CURRENT_LANGUAGE ) };
                if( mname.length( ) > 18 ) {
                    if( CURRENT_LANGUAGE == SAVE::JAP ) {
                        snprintf( buffer.data( ), buffer.size( ), "%s", mname.c_str( ) );
                    } else {
                        snprintf( buffer.data( ), buffer.size( ), "%s.", mname.c_str( ) );
                    }
                } else {
                    snprintf( buffer.data( ), buffer.size( ), "%s", mname.c_str( ) );
                }

                if( !p_canUseMove[ i ] ) {
                    IO::regularFont->setColor( 255, 1 );
                } else {
                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                }
                IO::regularFont->printStringC( buffer.data( ), oam[ SPR_MOVE_OAM_SUB( i ) ].x + 48,
                                               oam[ SPR_MOVE_OAM_SUB( i ) ].y + 7, true,
                                               IO::font::CENTER );

                snprintf( buffer.data( ), buffer.size( ),
                          GET_STRING_RAW( IO::STR_BATTLE_HP_FORMAT ), p_pokemon->m_curPP[ i ],
                          s8( mdatas[ i ].m_pp * ( ( 5 + p_pokemon->PPupget( i ) ) / 5.0 ) ) );
                IO::smallFont->printStringC( buffer.data( ), oam[ SPR_MOVE_OAM_SUB( i ) ].x + 84,
                                             oam[ SPR_MOVE_OAM_SUB( i ) ].y + 13, true,
                                             IO::font::RIGHT );
            }
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        }

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = !p_pokemon->getMove( i );
            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = !p_pokemon->getMove( i );
            IO::copySpritePal( MOVEBOX4_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
        }

        if( p_highlightedButton < 4 ) {
            IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( p_highlightedButton ), 4, 2 * 4,
                               true );
        }

        if( p_highlightedButton == 4 ) {
            IO::loadUIIcon( IO::ICON::BATTLE_RUN_2_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                            SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadUIIcon( IO::ICON::BATTLE_RUN_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                            SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        IO::updateOAM( true );
        return res;
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    battleUI::showTargetSelection( u8 p_choices, bool p_hasChoice,
                                   std::function<pokemon*( bool, u8 )> p_getPkmn,
                                   u8                                  p_highlightedButton ) {

        auto                  res = std::vector<std::pair<IO::inputTarget, u8>>( );
        SpriteEntry*          oam = IO::Oam->oamBuffer;
        std::array<char, 100> buffer{ };

        // hide yn choice boxes here b/c learn move.
        for( u8 i = 0; i < 4; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
            }
        }

        if( p_highlightedButton == u8( -1 ) ) {
            // initialize stuff
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub3", 512,
                                 49152, true );
            u16* pal   = IO::BG_PAL( true );
            pal[ 0 ]   = 0;
            pal[ 250 ] = IO::WHITE;
            pal[ 251 ] = IO::GRAY;
            pal[ 252 ] = IO::RGB( 18, 22, 31 );
            pal[ 254 ] = IO::RGB( 31, 18, 18 );
            pal[ 253 ] = IO::RGB( 0, 0, 25 );
            pal[ 255 ] = IO::RGB( 23, 0, 0 );

            // Clear log window
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), COMPLETE_SCREEN );
            for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = true; }

            for( u8 i = 0; i < 8; ++i ) { oam[ SPR_SMALL_MESSAGE_OAM_SUB + i ].isHidden = false; }

            for( u8 i = 0; i < 2; ++i ) {
                oam[ SPR_BATTLE_FITE_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_PKMN_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_RUN_OAM_SUB + i ].isHidden  = false;
                oam[ SPR_BATTLE_BAG_OAM_SUB + i ].isHidden  = true;
            }

            oam[ SPR_BATTLE_ICON_OAM_SUB ].isHidden = true;

            IO::loadUIIcon( IO::ICON::BATTLE_RUN_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                            SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex, 128,
                            68 + 64 + 18, 32, 32, true, true, false, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( 0, 2 );
            for( int i = 0; i < 4; i++ ) {
                oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = true;
                oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
                for( u8 j = 0; j < 6; j++ ) { oam[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = false; }

                res.push_back( std::pair(
                    IO::inputTarget( oam[ SPR_MOVE_OAM_SUB( i ) ].x, oam[ SPR_MOVE_OAM_SUB( i ) ].y,
                                     oam[ SPR_MOVE_OAM_SUB( i ) ].x + 96,
                                     oam[ SPR_MOVE_OAM_SUB( i ) ].y + 32 ),
                    !p_hasChoice || ( p_choices & ( 1 << i ) ) ? i
                                                               : IO::choiceBox::DISABLED_CHOICE ) );

                if( !( p_choices & ( 1 << i ) ) ) {
                    IO::loadTypeIcon( TYPE_NORMAL, oam[ SPR_TYPE_OAM_SUB( i ) ].x,
                                      oam[ SPR_TYPE_OAM_SUB( i ) ].y, SPR_TYPE_OAM_SUB( i ),
                                      SPR_TYPE_PAL_SUB( i ), oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex,
                                      true, CURRENT_LANGUAGE );
                } else {
                    IO::loadTypeIcon( TYPE_WATER, oam[ SPR_TYPE_OAM_SUB( i ) ].x,
                                      oam[ SPR_TYPE_OAM_SUB( i ) ].y, SPR_TYPE_OAM_SUB( i ),
                                      SPR_TYPE_PAL_SUB( i ), oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex,
                                      true, CURRENT_LANGUAGE );
                }
            }

            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      4 ) );
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      IO::choiceBox::BACK_CHOICE ) );

            IO::updateOAM( true );

            IO::regularFont->printStringC( GET_STRING( IO::STR_BATTLE_MENU_BACK ),
                                           oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                                           oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true,
                                           IO::font::CENTER ); // "BACK"

            IO::regularFont->printStringC( GET_STRING( 160 ), 128,
                                           oam[ SPR_SMALL_MESSAGE_OAM_SUB ].y + 4, true,
                                           IO::font::CENTER ); // "Choose a target"

            for( int i = 0; i < 4; i++ ) {
                if( !( p_choices & ( 1 << i ) ) ) { continue; }

                auto pk = p_getPkmn( i < 2, ( i < 2 ) ^ ( i % 2 ) );
                if( pk == nullptr ) { continue; }

                snprintf( buffer.data( ), buffer.size( ), "%s", pk->m_boxdata.getName( ) );

                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->printStringC( buffer.data( ), oam[ SPR_MOVE_OAM_SUB( i ) ].x + 48,
                                               oam[ SPR_MOVE_OAM_SUB( i ) ].y + 7, true,
                                               IO::font::CENTER );
            }
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        }

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = true;
            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
            IO::copySpritePal( MOVEBOX4_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
        }

        if( p_highlightedButton < 4 && p_hasChoice ) {
            IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( p_highlightedButton ), 4, 2 * 4,
                               true );
        }

        // If the selection is forced, the actual selection of the player doesn't matter.
        if( !p_hasChoice && p_highlightedButton < 4 ) {
            for( u8 i = 0; i < 4; ++i ) {
                if( p_choices & ( 1 << i ) ) {
                    IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
                }
            }
        }

        if( p_highlightedButton == 4 ) {
            IO::loadUIIcon( IO::ICON::BATTLE_RUN_2_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                            SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadUIIcon( IO::ICON::BATTLE_RUN_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                            SPR_BATTLE_RUN_PAL_SUB, oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        IO::updateOAM( true );
        return res;
    }

} // namespace BATTLE
