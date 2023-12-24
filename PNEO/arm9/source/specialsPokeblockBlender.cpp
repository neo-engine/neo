/*
    Pokémon neo
    ------------------------------

    file        : specialsPokeblockBlender.cpp
    author      : Philip Wellnitz
    description : Mini game to create Pokeblocks

    Copyright (C) 2023 - 2023
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

#include <vector>
#include <nds.h>

#include "bag/bagViewer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/pokemonNames.h"
#include "io/choiceBox.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "map/mapDrawer.h"
#include "pokemon.h"
#include "pokemonData.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"

namespace SPX {
    struct pokeblockNPC {
        u16 m_name;      // mapstring idx, if 0 then rotom
        u8  m_skill;     // chance in % to hit circle
        u8  m_berryTier; // 0 - selected by player,
    };

    constexpr u8 BERRY_TIER_LENGTH = 5;

    u16 BERRY_TIER_1[ BERRY_TIER_LENGTH ]
        = { I_CHERI_BERRY, I_PECHA_BERRY, I_RAWST_BERRY, I_ASPEAR_BERRY, I_CHESTO_BERRY };
    u16 BERRY_TIER_2[ BERRY_TIER_LENGTH ]
        = { I_SPELON_BERRY, I_PAMTRE_BERRY, I_WATMEL_BERRY, I_DURIN_BERRY, I_BELUE_BERRY };

    // used by master if player chose tier 2 berry
    u16 BERRY_TIER_2b[ BERRY_TIER_LENGTH + 1 ] = { I_GREPA_BERRY,  I_TAMATO_BERRY, I_CORNN_BERRY,
                                                   I_MAGOST_BERRY, I_RABUTA_BERRY, I_NOMEL_BERRY };

    pokeblockNPC BLENDER_NPC[ 5 ][ 3 ] = {
        { { 717, 100, 2 } }, // berry master
        { { 711, 50, 1 } },
        { { 712, 60, 1 }, { 713, 80, 1 } },
        { { 714, 70, 1 }, { 715, 40, 1 }, { 716, 20, 1 } },
        { { 0, 100, 0 }, { 0, 100, 0 }, { 0, 100, 0 } }, // rotom
    };

    u8 computeBaseSmoothness( u8 p_chosenBerries[ 4 ], u16 p_rpm ) {
        // take average of berry smoothness, subtract number of berries
        u8 smooth = 0;
        u8 cnt    = 0;
        for( ; cnt < 4; ++cnt ) {
            if( !p_chosenBerries[ cnt ] ) {
                --cnt;
                break;
            }
            smooth += FS::getBerryData( p_chosenBerries[ cnt ] ).m_smoothness;
        }
        if( cnt == 0 ) { return 0; }
        smooth /= cnt;
        return static_cast<u8>( std::min( 255, int( smooth - cnt ) * 22000 / p_rpm ) );
    }

    u8 computeBaseLevel( u8 p_chosenBerries[ 4 ] ) {
        // for each berry, compute smoothness values

        s16 flavor[ BAG::NUM_BERRYSTATS ] = { 0 };

        u8 cnt = 0;
        for( ; cnt < 4; ++cnt ) {
            if( !p_chosenBerries[ cnt ] ) {
                --cnt;
                break;
            }
            auto data = FS::getBerryData( p_chosenBerries[ cnt ] );
            for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) { flavor[ j ] += data.m_flavor[ j ]; }
        }

        s16 flavor_reduced[ BAG::NUM_BERRYSTATS ];

        flavor_reduced[ BAG::BY_SPICY ]  = flavor[ BAG::BY_SPICY ] - flavor[ BAG::BY_DRY ];
        flavor_reduced[ BAG::BY_DRY ]    = flavor[ BAG::BY_DRY ] - flavor[ BAG::BY_SWEET ];
        flavor_reduced[ BAG::BY_SWEET ]  = flavor[ BAG::BY_SWEET ] - flavor[ BAG::BY_BITTER ];
        flavor_reduced[ BAG::BY_BITTER ] = flavor[ BAG::BY_BITTER ] - flavor[ BAG::BY_SPICY ];
        flavor_reduced[ BAG::BY_SOUR ]   = flavor[ BAG::BY_SOUR ] - flavor[ BAG::BY_SOUR ];

        u8 mx = 0;
        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) {
            if( flavor_reduced[ j ] > mx ) { mx = static_cast<u8>( flavor_reduced[ j ] ); }
        }
        return mx;
    }

    BAG::pokeblockType computeBlockType( u8 p_chosenBerries[ 4 ], u8 p_hitbonus, u16 p_rpm ) {
        // for each berry, compute smoothness values
        u8 flavor[ BAG::NUM_BERRYSTATS ] = { 0 };

        u8 cnt = 0;
        for( ; cnt < 4; ++cnt ) {
            if( !p_chosenBerries[ cnt ] ) {
                --cnt;
                break;
            }
            auto data = FS::getBerryData( p_chosenBerries[ cnt ] );
            for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) { flavor[ j ] += data.m_flavor[ j ]; }
        }

        for( u8 i = 0; i < cnt - 1; ++i ) {
            for( u8 j = i + 1; j < cnt; ++j ) {
                if( p_chosenBerries[ i ] == p_chosenBerries[ j ] ) { return BAG::PB_BLACK; }
            }
        }

        s16 flavor_reduced[ BAG::NUM_BERRYSTATS ];

        flavor_reduced[ BAG::BY_SPICY ] = s16{ flavor[ BAG::BY_SPICY ] } - flavor[ BAG::BY_DRY ];
        flavor_reduced[ BAG::BY_DRY ]   = s16{ flavor[ BAG::BY_DRY ] } - flavor[ BAG::BY_SWEET ];
        flavor_reduced[ BAG::BY_SWEET ] = s16{ flavor[ BAG::BY_SWEET ] } - flavor[ BAG::BY_BITTER ];
        flavor_reduced[ BAG::BY_BITTER ]
            = s16{ flavor[ BAG::BY_BITTER ] } - flavor[ BAG::BY_SPICY ];
        flavor_reduced[ BAG::BY_SOUR ] = s16{ flavor[ BAG::BY_SOUR ] } - flavor[ BAG::BY_SOUR ];

        u8 numneg = 0;
        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) { numneg += flavor_reduced[ j ] < 0; }

        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) {
            flavor_reduced[ j ] = std::max( 0, flavor_reduced[ j ] - numneg );
        }

        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) {
            flavor[ j ] = static_cast<u8>(
                std::min( s32( 255 ), s32( flavor_reduced[ j ] ) * p_hitbonus / 10 ) );
#ifdef DESQUID
            IO::printMessage( std::to_string( flavor[ j ] ), MSG_INFO );
#endif
        }
        return BAG::pokeblock::fromLevelSmoothness(
            flavor, computeBaseSmoothness( p_chosenBerries, p_rpm ) );
    }

    u8 computeNPCBerry( u8 p_chosenBerries[ 4 ], u8 p_berryTier ) {
        // pick berry that is best for the pokeblock from available tier

        if( p_berryTier == 1 ) {
            u8 mxlv = 0, argmx = BAG::itemToBerry( BERRY_TIER_1[ 0 ] );
            for( u8 i = 0; i < BERRY_TIER_LENGTH; ++i ) {
                bool ignore = false;
                u8   fr     = 4;
                for( u8 j = 0; j < 4; ++j ) {
                    if( p_chosenBerries[ j ] == BAG::itemToBerry( BERRY_TIER_1[ i ] ) ) {
                        ignore = true;
                        break;
                    }
                    if( !p_chosenBerries[ j ] ) {
                        fr = j;
                        break;
                    }
                }
                if( ignore ) { continue; }

                p_chosenBerries[ fr ] = BAG::itemToBerry( BERRY_TIER_1[ i ] );
                u8 nlv                = computeBaseLevel( p_chosenBerries );
                p_chosenBerries[ fr ] = 0;

                if( nlv > mxlv ) {
                    mxlv  = nlv;
                    argmx = BAG::itemToBerry( BERRY_TIER_1[ i ] );
                }
            }
            return argmx;
        }

        if( p_berryTier == 2 ) {
            // first check if a replacement berry should be picked
            bool repl = false;
            for( u8 i = 0; i < BERRY_TIER_LENGTH; ++i ) {
                for( u8 j = 0; j < 4 && !repl; ++j ) {
                    if( p_chosenBerries[ j ] == BAG::itemToBerry( BERRY_TIER_2[ i ] ) ) {
                        repl = true;
                        break;
                    }
                }
            }

            u8 fr = 4;
            for( u8 j = 0; j < 4; ++j ) {
                if( !p_chosenBerries[ j ] ) {
                    fr = j;
                    break;
                }
            }

            u8 mxlv = 0, argmx = BAG::itemToBerry( repl ? BERRY_TIER_2b[ 0 ] : BERRY_TIER_2[ 0 ] );
            for( u8 i = 0; i < BERRY_TIER_LENGTH + repl; ++i ) {
                u8 cbr = BAG::itemToBerry( repl ? BERRY_TIER_2b[ i ] : BERRY_TIER_2[ i ] );

                p_chosenBerries[ fr ] = cbr;
                u8 nlv                = computeBaseLevel( p_chosenBerries );
                p_chosenBerries[ fr ] = 0;

                if( nlv > mxlv ) {
                    mxlv  = nlv;
                    argmx = cbr;
                }
            }
            return argmx;
        }

        return 0;
    }

    std::pair<u8, u16> blenderMinigame( ) {
        // runs the blender mini game, returns (hitbonus, max rpm * 100),
        // returns rpm = 0 if the machine overheated (when reaching > 655 rpm)
        // hitbonus is # of perfect hits - # of misses

        return { 20, 100 * 650 };
        // return 100 * ( rand( ) % 655 );
    }

    void runPokeblockBlender( u8 p_numNPC, bool p_rotom, bool p_blendMater ) {
#ifdef DESQUID
        for( u8 i = 0; i < 24; ++i ) { SAVE::SAV.getActiveFile( ).m_pokeblockCount[ i ] = 0; }
#endif
        char buffer[ 200 ];
        // when playing with npc, need to pick 1 berry, when playing with rotom, need to
        // pick 4 berries

        SOUND::dimVolume( );

        u8 npctier = p_rotom ? 4 : p_numNPC;
        if( p_blendMater ) { npctier = 0; }
        u8 berriesFormNPC = p_numNPC;
        if( !berriesFormNPC ) { berriesFormNPC = 1; }
        if( p_rotom ) { berriesFormNPC = 0; }

        u8 requiredBerries = p_rotom ? 4 : 1;
        u8 berries[ 4 ]    = { 0 };
        u8 berriesPicked   = 0;
        // make player select required berries
        for( ; berriesPicked < requiredBerries; ++berriesPicked ) {
            BAG::bagViewer bv  = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                 BAG::bagViewer::CHOOSE_BERRY );
            u16            itm = bv.getItem( true );

            if( !itm ) {
                // player didn't pick a berry, abort
                SOUND::restoreVolume( );
                return;
            }
            berries[ berriesPicked ] = BAG::itemToBerry( itm );
        }

        for( u8 i = 0; i < berriesFormNPC && berriesPicked < 4; ++berriesPicked, ++i ) {
            // compute remaining berries, each NPC picks one berry
            u8 berry = computeNPCBerry( berries, BLENDER_NPC[ npctier ][ i ].m_berryTier );
            if( berry ) {
                berries[ berriesPicked ] = berry;
            } else {
                // default to berry no 1.
                berries[ berriesPicked ] = 1;
            }
        }

#ifdef DESQUID
        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( false );
        videoSetMode( MODE_5_2D );
        IO::resetScale( true, false );
        bgUpdate( );
        IO::init( );
        MAP::curMap->draw( );

        for( u8 i = 0; i < 4; ++i ) {
            IO::takeItemFromPlayer( BAG::berryToItem( berries[ i ] ), 1 );
        }
#endif

        // initialize ui
        // throw in berries
        // start rotating, allow pressing A

        auto [ hitbonus, rpm ] = blenderMinigame( );

        // compute resulting pokeblocks

        auto result = computeBlockType( berries, hitbonus, rpm );

        if( p_rotom && result == BAG::PB_ULTIMATE ) { result = BAG::PB_GOLD_DX; }
        if( p_blendMater && result == BAG::PB_GOLD_DX ) { result = BAG::PB_ULTIMATE; }

        // add p_numNPC pokeblocks of type result to bag

        snprintf( buffer, 199, "You obtained %hhu PokeBlock %s.", p_numNPC + 1,
                  GET_STRING( u8( result ) + 742 ) );

        // add pokeblocks to bag
        SAVE::SAV.getActiveFile( ).m_pokeblockCount[ u8( result ) ] += p_numNPC + 1;

        SOUND::restoreVolume( );
    }
} // namespace SPX
