/*
Pokémon Emerald 2 Version
------------------------------

file        new : item.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, new either version 3 of the License, new or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, new see <http://www.gnu.org/licenses/>.
*/

#include <vector>
#include <algorithm>

#include "item.h"
#include "berry.h"
#include "move.h"
#include "pokemon.h"
#include "script.h"

#include "buffer.h"
#include "saveGame.h"
#include "mapDefines.h"
#include "mapDrawer.h"
#include "uio.h"
#include "messageBox.h"


#define APPLY_OP( op, tg, val, mx ) ( ( op == 1 ) ? tg += val : ( \
                                  ( op == 2 ) ? tg -= val : ( \
                                  ( op == 3 ) ? tg = val : ( \
                                  ( op == 4 ) ? tg |= val : ( \
                                  ( op == 5 ) ? tg &= val : ( \
                                  ( op == 6 ) ? tg = mx : ( \
                                  ( op == 7 ) ? tg = s16( ( mx + 0.5 ) / 2 ) : tg ) ) ) ) ) ) )

bool item::needsInformation( u8 p_num ) {
    if( !m_loaded && !load( ) )
        return false;

    u16 op = ( p_num ? m_itemData.m_itemEffect % ( 1 << 16 ) : m_itemData.m_itemEffect >> 16 );
    u8 stat = ( op >> 8 ) % 32;

    return ( stat == 2 || stat == 18 );
}

//Ya'know, this stuff here is serious voodoo,
//e.g., Zinc's effect encodes to 755051786 = 0b001'01101'0000'0001''001'01010'0000'1010
bool item::use( pokemon& p_pokemon ) {
    if( p_pokemon.m_boxdata.m_individualValues.m_isEgg || !p_pokemon.m_boxdata.m_speciesId )
        return false;
    if( !m_loaded && !load( ) )
        return false;

    bool change = false;
    pokemonData p; getAll( p_pokemon.m_boxdata.m_speciesId, p );

    //Anything that modifies the PKMN's happiness shall be second
    if( ( m_itemData.m_itemEffect >> 24 ) % 32 == 13 )
        m_itemData.m_itemEffect = ( ( m_itemData.m_itemEffect % ( 1 << 16 ) ) << 16 ) | ( m_itemData.m_itemEffect >> 16 );

    for( auto op : { m_itemData.m_itemEffect >> 16, m_itemData.m_itemEffect % ( 1 << 16 ) } ) {
        u8 operation = op >> 13;
        u8 stat = ( op >> 8 ) % 32;
        u8 value = u8( op );

        switch( stat ) {
            case 1:
            {
                if( !p_pokemon.m_stats.m_acHP )
                    break;

                s16 tmp = p_pokemon.m_stats.m_acHP;
                APPLY_OP( operation, tmp, value, s16( p_pokemon.m_stats.m_maxHP ) );
                tmp = std::min( tmp, s16( p_pokemon.m_stats.m_maxHP ) );
                tmp = std::max( (s16) 0, tmp );
                if( tmp != p_pokemon.m_stats.m_acHP ) {
                    p_pokemon.m_stats.m_acHP = tmp;
                    change = true;
                }
                break;
            }
            case 2: case 3:
            case 4: case 5:
            {
                if( !p_pokemon.m_boxdata.m_moves[ stat - 2 ] )
                    break;

                s8 tmp = p_pokemon.m_boxdata.m_acPP[ stat - 2 ];
                s8 mx = s8( AttackList[ p_pokemon.m_boxdata.m_moves[ stat - 2 ] ]->m_movePP
                            * ( 5 + p_pokemon.m_boxdata.PPupget( stat - 2 ) ) / 5.0 );
                APPLY_OP( operation, tmp, value, mx );
                tmp = std::min( tmp, mx );
                if( tmp != p_pokemon.m_boxdata.m_acPP[ stat - 2 ] ) {
                    p_pokemon.m_boxdata.m_acPP[ stat - 2 ] = tmp;
                    change = true;
                }
                break;
            }
            case 17:
            {
                for( u8 i = 0; i < 4; ++i ) {
                    if( !p_pokemon.m_boxdata.m_moves[ i ] )
                        break;
                    s8 tmp = p_pokemon.m_boxdata.m_acPP[ i ];
                    s8 mx = s8( AttackList[ p_pokemon.m_boxdata.m_moves[ i ] ]->m_movePP
                                * ( 5 + p_pokemon.m_boxdata.PPupget( i ) ) / 5.0 );
                    APPLY_OP( operation, tmp, value, mx );
                    tmp = std::min( tmp, mx );
                    if( tmp != p_pokemon.m_boxdata.m_acPP[ i ] ) {
                        p_pokemon.m_boxdata.m_acPP[ i ] = tmp;
                        change = true;
                    }
                }
                break;
            }
            case 18: case 19:
            case 20: case 21:
            {
                s8 tmp = p_pokemon.m_boxdata.PPupget( stat - 18 );

                s8 df = s8( AttackList[ p_pokemon.m_boxdata.m_moves[ stat - 18 ] ]->m_movePP
                            * ( 5 + p_pokemon.m_boxdata.PPupget( stat - 18 ) ) / 5.0 )
                    - p_pokemon.m_boxdata.m_acPP[ stat - 18 ];

                APPLY_OP( operation, tmp, value, (s8) 3 );
                tmp = std::min( tmp, (s8) 3 );
                if( tmp != p_pokemon.m_boxdata.PPupget( stat - 18 ) ) {
                    p_pokemon.m_boxdata.PPupset( stat - 18, tmp );
                    p_pokemon.m_boxdata.m_acPP[ stat - 18 ] = s8( AttackList[ p_pokemon.m_boxdata.m_moves[ stat - 18 ] ]->m_movePP
                                                                  * ( 5 + p_pokemon.m_boxdata.PPupget( stat - 18 ) ) / 5.0 ) - df;
                    change = true;
                }
                break;
            }
            case 6: case 7: case 8:
            case 9: case 10: case 11:
            {
                s16 tmp = p_pokemon.m_boxdata.m_effortValues[ stat - 6 ];
                s16 sum = 0;
                for( u8 i = 0; i < 6; ++i )
                    sum += p_pokemon.m_boxdata.m_effortValues[ i ];
                s16 mx = 510 - sum;
                if( value > 1 ) {
                    APPLY_OP( operation, tmp, value, std::min( s16( 100 ), mx ) );
                    tmp = std::min( tmp, std::min( s16( 100 ), mx ) );
                } else {
                    APPLY_OP( operation, tmp, value, std::min( s16( 252 ), mx ) );
                    tmp = std::min( tmp, std::min( s16( 252 ), mx ) );
                }
                tmp = std::max( (s16) 0, tmp );
                if( tmp != p_pokemon.m_boxdata.m_effortValues[ stat - 6 ] ) {
                    p_pokemon.m_boxdata.m_effortValues[ stat - 6 ] = tmp;
                    change = true;
                    p_pokemon.m_stats = calcStats( p_pokemon.m_boxdata, p );
                }
                break;
            }
            case 12:
            {
                s16 tmp = p_pokemon.m_level;
                APPLY_OP( operation, tmp, value, 100 );
                tmp = std::min( tmp, s16( 100 ) );
                tmp = std::max( (s16) 0, tmp );
                if( tmp != p_pokemon.m_level ) {
                    p_pokemon.m_level = tmp;
                    p_pokemon.m_boxdata.m_experienceGained = EXP[ p_pokemon.m_level - 1 ][ p.m_expType ];

                    p_pokemon.m_stats = calcStats( p_pokemon.m_boxdata, p );
                    change = true;
                }
                break;
            }
            case 13:
            {
                s16 tmp = p_pokemon.m_boxdata.m_steps;
                APPLY_OP( operation, tmp, value, s16( 255 ) );
                tmp = std::min( tmp, s16( 255 ) );
                tmp = std::max( (s16) 0, tmp );
                if( change && tmp != p_pokemon.m_boxdata.m_steps ) {
                    p_pokemon.m_boxdata.m_steps = tmp;
                }
                break;
            }
            case 14:
            {
                u8 tmp = p_pokemon.m_statusint;
                APPLY_OP( operation, tmp, value, s16( 0 ) );
                if( tmp != p_pokemon.m_boxdata.m_steps ) {
                    p_pokemon.m_boxdata.m_steps = tmp;
                }
                break;
            }
            case 15:
            {
                if( p_pokemon.m_stats.m_acHP )
                    break;

                s16 tmp = p_pokemon.m_stats.m_acHP;
                APPLY_OP( operation, tmp, value, s16( p_pokemon.m_stats.m_maxHP ) );
                tmp = std::min( tmp, s16( p_pokemon.m_stats.m_maxHP ) );
                tmp = std::max( (s16) 1, tmp );

                p_pokemon.m_stats.m_acHP = tmp;
                change = true;
                break;
            }
            default:
                break;
        }
    }

    return change;
}

//Returns false if the original UI has not to be redrawn/will be exited
bool item::use( bool p_dryRun ) {
    u16 itm = getItemId( );
    if( !p_dryRun ) {
        bool ex = false;
        for( u8 i = 0; i < 5; ++i )
            if( FS::SAV->m_lstUsedItems[ i ] == itm ) {
                ex = true;
                break;
            }
        if( !ex ) {
            FS::SAV->m_lstUsedItems[ FS::SAV->m_lstUsedItemsIdx ] = itm;
            FS::SAV->m_lstUsedItemsIdx = ( FS::SAV->m_lstUsedItemsIdx + 1 ) % 5;
        }
    }
    switch( itm ) {
        case I_REPEL:
            if( !p_dryRun ) {
                FS::SAV->m_repelSteps = std::max( FS::SAV->m_repelSteps, (s16) 50 );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                IO::messageBox( "Schutz eingesetzt.", false );
            }
            return true;
        case I_SUPER_REPEL:
            if( !p_dryRun ) {
                FS::SAV->m_repelSteps = std::max( FS::SAV->m_repelSteps, (s16) 100 );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                IO::messageBox( "Superschutz eingesetzt.", false );
            }
            return true;
        case I_MAX_REPEL:
            if( !p_dryRun ) {
                FS::SAV->m_repelSteps = std::max( FS::SAV->m_repelSteps, (s16) 250 );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                IO::messageBox( "Top-Schutz eingesetzt.", false );
            }
            return true;
        case I_EXP_SHARE:
            if( !p_dryRun ) {
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                if( FS::SAV->m_EXPShareEnabled )
                    IO::messageBox( "EP-Teiler ausgeschaltet.", false );
                else
                    IO::messageBox( "EP-Teiler eingeschaltet.", false );
                FS::SAV->m_EXPShareEnabled = !FS::SAV->m_EXPShareEnabled;
            }
            return true;
        case I_COIN_CASE:
            if( !p_dryRun ) {
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                sprintf( buffer, "Münzen: %lu.", FS::SAV->m_coins );
                IO::messageBox( buffer, false );
            }
            return true;
        case I_POINT_CARD:
            if( !p_dryRun ) {
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                sprintf( buffer, "Kampfpunkte: %lu.", FS::SAV->m_battlePoints );
                IO::messageBox( buffer, false );
                FS::SAV->m_EXPShareEnabled = !FS::SAV->m_EXPShareEnabled;
            }
            return true;
        case I_ESCAPE_ROPE:
            if( !p_dryRun )
                AttackList[ M_DIG ]->use( );
            return false;
        case I_HONEY:
            if( !p_dryRun )
                AttackList[ M_SWEET_SCENT ]->use( );
            return false;
        case I_BIKE2:
        case I_BICYCLE:
        case I_MACH_BIKE:
        case I_ACRO_BIKE:
            if( FS::SAV->m_player.m_movement == MAP::WALK ) {
                if( !p_dryRun )
                    MAP::curMap->changeMoveMode( MAP::BIKE );
                return false;
            } else if( FS::SAV->m_player.m_movement == MAP::BIKE ) {
                if( !p_dryRun )
                    MAP::curMap->changeMoveMode( MAP::WALK );
                return false;
            } else
                return true;
        case I_OLD_ROD:
            if( MAP::curMap->canFish( FS::SAV->m_player.m_pos, FS::SAV->m_player.m_direction ) ) {
                if( !p_dryRun )
                    MAP::curMap->fishPlayer( FS::SAV->m_player.m_direction, 0 );
                return false;
            } else
                return true;
        case I_GOOD_ROD:
            if( MAP::curMap->canFish( FS::SAV->m_player.m_pos, FS::SAV->m_player.m_direction ) ) {
                if( !p_dryRun )
                    MAP::curMap->fishPlayer( FS::SAV->m_player.m_direction, 1 );
                return false;
            } else
                return true;
        case I_SUPER_ROD:
            if( MAP::curMap->canFish( FS::SAV->m_player.m_pos, FS::SAV->m_player.m_direction ) ) {
                if( !p_dryRun )
                    MAP::curMap->fishPlayer( FS::SAV->m_player.m_direction, 2 );
                return false;
            } else
                return true;
        default:
            break;
    }
    return false;
}

bool item::useable( ) {
    u16 itm = getItemId( );
    switch( itm ) {
        case I_REPEL:
        case I_SUPER_REPEL:
        case I_MAX_REPEL:
        case I_EXP_SHARE:
        case I_COIN_CASE:
        case I_POINT_CARD:
            return true;
        case I_ESCAPE_ROPE:
            return AttackList[ M_DIG ]->possible( );
        case I_HONEY:
            return AttackList[ M_SWEET_SCENT ]->possible( );
        case I_BIKE2:
        case I_BICYCLE:
        case I_MACH_BIKE:
        case I_ACRO_BIKE:
            return FS::SAV->m_player.m_movement == MAP::WALK || FS::SAV->m_player.m_movement == MAP::BIKE;
        case I_OLD_ROD:
        case I_GOOD_ROD:
        case I_SUPER_ROD:
            return MAP::curMap->canFish( FS::SAV->m_player.m_pos, FS::SAV->m_player.m_direction );
        default:
            break;
    }
    return false;
}

item* ItemList[ MAX_ITEMS ] = {
    new item( "Null" ),
    new ball( "Meisterball" ), new ball( "Hyperball" ),
    new ball( "Superball" ), new ball( "Pokeball" ),
    new ball( "Safariball" ), new ball( "Netzball" ),
    new ball( "Tauchball" ), new ball( "Nestball" ),
    new ball( "Wiederball" ), new ball( "Timerball" ),
    new ball( "Luxusball" ), new ball( "Premierball" ),
    new ball( "Finsterball" ), new ball( "Heilball" ),
    new ball( "Flottball" ), new ball( "Jubelball" ),
    new medicine( "Trank" ), new medicine( "Gegengift" ),
    new medicine( "Feuerheiler" ), new medicine( "Eisheiler" ),
    new medicine( "Aufwecker" ), new medicine( "Para-Heiler" ),
    new medicine( "Top-Genesung" ), new medicine( "Top-Trank" ),
    new medicine( "Hypertrank" ), new medicine( "Supertrank" ),
    new medicine( "Hyperheiler" ), new medicine( "Beleber" ),
    new medicine( "Top-Beleber" ), new medicine( "Tafelwasser" ),
    new medicine( "Sprudel" ), new medicine( "Limonade" ),
    new medicine( "KuhMuh-Milch" ), new medicine( "Energiestaub" ),
    new medicine( "Kraftwurzel" ), new medicine( "Heilpuder" ),
    new medicine( "Vitalkraut" ), new medicine( "AEther" ),
    new medicine( "Top-AEther" ), new medicine( "Elixir" ),
    new medicine( "Top-Elixir" ), new medicine( "Lavakeks" ),
    new medicine( "Beerensaft" ), new item( "Zauberasche" ),
    new medicine( "KP-Plus" ), new medicine( "Protein" ),
    new medicine( "Eisen" ), new medicine( "Carbon" ),
    new medicine( "Kalzium" ), new medicine( "Sonderbonbon" ),
    new medicine( "AP-Plus" ), new medicine( "Zink" ),
    new medicine( "AP-Top" ), new medicine( "Spezialitaet" ),

    new battleItem( "Megablock" ), new battleItem( "Angriffsplus" ),
    new battleItem( "X-Angriff" ), new battleItem( "X-Abwehr" ),
    new battleItem( "X-Tempo" ), new battleItem( "X-Treffer" ),
    new battleItem( "X-Spezial" ), new battleItem( "X-SpezialVer" ),
    new battleItem( "Pokepuppe" ), new battleItem( "Eneco-Rute" ),
    new battleItem( "BlaueFloete" ), new battleItem( "GelbeFloete" ),
    new battleItem( "RoteFloete" ), new battleItem( "SchwarzeFloete" ),
    new battleItem( "WeisseFloete" ),

    new item( "Kuestensalz" ), new item( "Kuestenschale" ),
    new item( "Purpurstueck" ), new item( "Indigostueck" ),
    new item( "Gelbstueck" ), new item( "Gruenstueck" ),
    new item( "Superschutz" ), new item( "Top-Schutz" ),
    new item( "Fluchtseil" ), new item( "Schutz" ),
    new item( "Sonnenstein" ), new item( "Mondstein" ),
    new item( "Feuerstein" ), new item( "Donnerstein" ),
    new item( "Wasserstein" ), new item( "Blattstein" ),
    new item( "Minipilz" ), new item( "Riesenpilz" ),
    new item( "Perle" ), new item( "Riesenperle" ),
    new item( "Sternenstaub" ), new item( "Sternenstueck" ),
    new item( "Nugget" ), new item( "Herzschuppe" ),
    new item( "Honig" ), new item( "Wachsmulch" ),
    new item( "Feuchtmulch" ), new item( "Stabilmulch" ),
    new item( "Neumulch" ), new item( "Wurzelfossil" ),
    new item( "Klauenfossil" ), new item( "Helixfossil" ),
    new item( "Domfossil" ), new item( "Altbernstein" ),
    new item( "Schildfossil" ), new item( "Kopffossil" ),
    new item( "Steinknochen" ), new item( "Funkelstein" ),
    new item( "Finsterstein" ), new item( "Leuchtstein" ),
    new item( "OvalerStein" ), new item( "Spiritkern" ),
    new item( "Platinum-Orb" ),

    new item( "Null" ),
    new item( "Null" ),
    new item( "Null" ),

    new item( "Aquamodul" ), new item( "Blitzmodul" ),
    new item( "Flammenmodul" ), new item( "Gefriermodul" ),

    new mail( "Wiesenbrief" ), new mail( "Feuerbrief" ),
    new mail( "Wasserbrief" ), new mail( "Bluetenbrief" ),
    new mail( "Minenbrief" ), new mail( "Stahlbrief" ),
    new mail( "Rosabrief" ), new mail( "Schneebrief" ),
    new mail( "Sternbrief" ), new mail( "Luftbrief" ),
    new mail( "Mosaikbrief" ), new mail( "Ziegelbrief" ),

    new item( "Null" ),
    new item( "Null" ),

    new item( "Herzkonfekt" ), new item( "Adamant-Orb" ),
    new item( "Weiss-Orb" ),

    new mail( "Grussbrief" ), new mail( "Faiblebrief" ),
    new mail( "Einladebrief" ), new mail( "Dankesbrief" ),
    new mail( "Fragebrief" ), new mail( "Insiderbrief" ),
    new mail( "Rueckbrief" ), new mail( "BrueckbriefH" ),
    new mail( "BrueckbriefM" ), new mail( "BrueckbriefZ" ),
    new mail( "BrueckbriefD" ), new mail( "BrueckbriefW" ),

    new berry( "Amrenabeere" ), new berry( "Maronbeere" ),
    new berry( "Pirsifbeere" ), new berry( "Fragiabeere" ),
    new berry( "Wilbirbeere" ), new berry( "Jonagobeere" ),
    new berry( "Sinelbeere" ), new berry( "Persimbeere" ),
    new berry( "Prunusbeere" ), new berry( "Tsitrubeere" ),
    new berry( "Giefebeere" ), new berry( "Wikibeere" ),
    new berry( "Magobeere" ), new berry( "Gauvebeere" ),
    new berry( "Yapabeere" ), new berry( "Himmihbeere" ),
    new berry( "Morbbeere" ), new berry( "Nanabbeere" ),
    new berry( "Nirbebeere" ), new berry( "Sananabeere" ),
    new berry( "Granabeere" ), new berry( "Setangbeere" ),
    new berry( "Qualotbeere" ), new berry( "Honmelbeere" ),
    new berry( "Labrusbeere" ), new berry( "Tamotbeere" ),
    new berry( "Saimbeere" ), new berry( "Magostbeere" ),
    new berry( "Rabutabeere" ), new berry( "Tronzibeere" ),
    new berry( "Kiwanbeere" ), new berry( "Pallmbeere" ),
    new berry( "Wasmelbeere" ), new berry( "Durinbeere" ),
    new berry( "Myrtilbeere" ), new berry( "Koakobeere" ),
    new berry( "Foepasbeere" ), new berry( "Kerzalbeere" ),
    new berry( "Grindobeere" ), new berry( "Kiroyabeere" ),
    new berry( "Rospelbeere" ), new berry( "Grarzbeere" ),
    new berry( "Schukebeere" ), new berry( "Kobabeere" ),
    new berry( "Pyapabeere" ), new berry( "Tanigabeere" ),
    new berry( "Chiaribeere" ), new berry( "Zitarzbeere" ),
    new berry( "Terirobeere" ), new berry( "Burleobeere" ),
    new berry( "Babiribeere" ), new berry( "Latchibeere" ),
    new berry( "Lydzibeere" ), new berry( "Linganbeere" ),
    new berry( "Salkabeere" ), new berry( "Tahaybeere" ),
    new berry( "Apikobeere" ), new berry( "Lansatbeere" ),
    new berry( "Krambobeere" ), new berry( "Enigmabeere" ),
    new berry( "Wunfrubeere" ), new berry( "Eipfelbeere" ),
    new berry( "Jabocabeere" ), new berry( "Roselbeere" ),

    new item( "Blendpuder" ), new item( "Schlohkraut" ),
    new item( "Machoband" ), new keyItem( "EP-Teiler" ),
    new item( "Flinkklaue" ), new item( "Sanftglocke" ),
    new item( "Mentalkraut" ), new item( "Wahlband" ),
    new item( "King-Stein" ), new item( "Silberstaub" ),
    new item( "Muenzamulett" ), new item( "Schutzband" ),
    new item( "Seelentau" ), new item( "Abysszahn" ),
    new item( "Abyssplatte" ), new item( "Rauchball" ),
    new item( "Ewigstein" ), new item( "Fokus-Band" ),
    new item( "Gluecks-Ei" ), new item( "Scope-Linse" ),
    new item( "Metallmantel" ), new item( "UEberreste" ),
    new item( "Drachenhaut" ), new item( "Kugelblitz" ),
    new item( "Pudersand" ), new item( "Granitstein" ),
    new item( "Wundersaat" ), new item( "Schattenglas" ),
    new item( "Schwarzgurt" ), new item( "Magnet" ),
    new item( "Zauberwasser" ), new item( "Hackattack" ),
    new item( "Giftstich" ), new item( "EwigesEis" ),
    new item( "Bannsticker" ), new item( "Krummloeffel" ),
    new item( "Holzkohle" ), new item( "Drachenzahn" ),
    new item( "Seidenschal" ), new item( "Up-Grade" ),
    new item( "Seegesang" ), new item( "Seerauch" ),
    new item( "Laxrauch" ), new item( "LuckyPunch" ),
    new item( "Metallstaub" ), new item( "Kampfknochen" ),
    new item( "Lauchstange" ), new item( "RoterSchal" ),
    new item( "BlauerSchal" ), new item( "RosaSchal" ),
    new item( "GruenerSchal" ), new item( "GelberSchal" ),
    new item( "Grosslinse" ), new item( "Muskelband" ),
    new item( "Schlauglas" ), new item( "Expertengurt" ),
    new item( "Lichtlehm" ), new item( "Leben-Orb" ),
    new item( "Energiekraut" ), new item( "Toxik-Orb" ),
    new item( "Heiss-Orb" ), new item( "Flottstaub" ),
    new item( "Fokusgurt" ), new item( "Zoomlinse" ),
    new item( "Metronom" ), new item( "Eisenkugel" ),
    new item( "Schwerschweif" ), new item( "Fatumknoten" ),
    new item( "Giftschleim" ), new item( "Eisbrocken" ),
    new item( "Glattbrocken" ), new item( "Heissbrocken" ),
    new item( "Nassbrocken" ), new item( "Griffklaue" ),
    new item( "Wahlschal" ), new item( "Klettdorn" ),
    new item( "Machtreif" ), new item( "Machtgurt" ),
    new item( "Machtlinse" ), new item( "Machtband" ),
    new item( "Machtkette" ), new item( "Machtgewicht" ),
    new item( "Wechselhuelle" ), new item( "Grosswurzel" ),
    new item( "Wahlglas" ), new item( "Feuertafel" ),
    new item( "Wassertafel" ), new item( "Blitztafel" ),
    new item( "Wiesentafel" ), new item( "Frosttafel" ),
    new item( "Fausttafel" ), new item( "Gifttafel" ),
    new item( "Erdtafel" ), new item( "Wolkentafel" ),
    new item( "Hirntafel" ), new item( "Kaefertafel" ),
    new item( "Steintafel" ), new item( "Spuktafel" ),
    new item( "Dracotafel" ), new item( "Furchttafel" ),
    new item( "Eisentafel" ), new item( "Schraegrauch" ),
    new item( "Steinrauch" ), new item( "Lahmrauch" ),
    new item( "Wellenrauch" ), new item( "Rosenrauch" ),
    new item( "Gluecksrauch" ), new item( "Scheurauch" ),
    new item( "Schuetzer" ), new item( "Elektrisierer" ),
    new item( "Magmaisierer" ), new item( "Dubiosdisk" ),
    new item( "Duesterumhang" ), new item( "Scharfklaue" ),
    new item( "Scharfzahn" ),

    new TM( "TM01", M_FOCUS_PUNCH ), new TM( "TM02", M_DRAGON_CLAW ), new TM( "TM03", M_PSYSHOCK ), new TM( "TM04", M_CALM_MIND ),
    new TM( "TM05", M_ROAR ), new TM( "TM06", M_TOXIC ), new TM( "TM07", M_HAIL ), new TM( "TM08", M_BULK_UP ),
    new TM( "TM09", M_VENOSHOCK ), new TM( "TM10", M_HIDDEN_POWER ), new TM( "TM11", M_SUNNY_DAY ), new TM( "TM12", M_TAUNT ),
    new TM( "TM13", M_ICE_BEAM ), new TM( "TM14", M_BLIZZARD ), new TM( "TM15", M_HYPER_BEAM ), new TM( "TM16", M_LIGHT_SCREEN ),
    new TM( "TM17", M_PROTECT ), new TM( "TM18", M_RAIN_DANCE ), new TM( "TM19", M_ROOST ), new TM( "TM20", M_SAFEGUARD ),
    new TM( "TM21", M_FRUSTRATION ), new TM( "TM22", M_SOLAR_BEAM ), new TM( "TM23", M_IRON_TAIL ), new TM( "TM24", M_THUNDERBOLT ),
    new TM( "TM25", M_THUNDER ), new TM( "TM26", M_EARTHQUAKE ), new TM( "TM27", M_RETURN ), new TM( "TM28", M_DIG ),
    new TM( "TM29", M_PSYCHIC ), new TM( "TM30", M_SHADOW_BALL ), new TM( "TM31", M_BRICK_BREAK ), new TM( "TM32", M_DOUBLE_TEAM ),
    new TM( "TM33", M_REFLECT ), new TM( "TM34", M_SHOCK_WAVE ), new TM( "TM35", M_FLAMETHROWER ), new TM( "TM36", M_FIRE_BLAST ),
    new TM( "TM37", M_SANDSTORM ), new TM( "TM38", M_SLUDGE_BOMB ), new TM( "TM39", M_ROCK_TOMB ), new TM( "TM40", M_AERIAL_ACE ),
    new TM( "TM41", M_TORMENT ), new TM( "TM42", M_FACADE ), new TM( "TM43", M_SECRET_POWER ), new TM( "TM44", M_REST ),
    new TM( "TM45", M_ATTRACT ), new TM( "TM46", M_THIEF ), new TM( "TM47", M_STEEL_WING ), new TM( "TM48", M_ROUND ),
    new TM( "TM49", M_ECHOED_VOICE ), new TM( "TM50", M_OVERHEAT ),

    new TM( "TM51", M_LOW_SWEEP ), new TM( "TM52", M_FOCUS_BLAST ),
    new TM( "TM53", M_ENERGY_BALL ), new TM( "TM54", M_FALSE_SWIPE ), new TM( "TM55", M_SCALD ), new TM( "TM56", M_FLING ),
    new TM( "TM57", M_CHARGE_BEAM ), new TM( "TM58", M_SKY_DROP ), new TM( "TM59", M_INCINERATE ), new TM( "TM60", M_DRAIN_PUNCH ),
    new TM( "TM61", M_WILL_O_WISP ), new TM( "TM62", M_ACROBATICS ), new TM( "TM63", M_EMBARGO ), new TM( "TM64", M_EXPLOSION ),
    new TM( "TM65", M_SHADOW_CLAW ), new TM( "TM66", M_PAYBACK ), new TM( "TM67", M_RETALIATE ), new TM( "TM68", M_GIGA_IMPACT ),
    new TM( "TM69", M_ROCK_POLISH ), new TM( "TM70", M_BATON_PASS ), new TM( "TM71", M_STONE_EDGE ), new TM( "TM72", M_VOLT_SWITCH ),
    new TM( "TM73", M_THUNDER_WAVE ), new TM( "TM74", M_GYRO_BALL ), new TM( "TM75", M_SWORDS_DANCE ), new TM( "TM76", M_STRUGGLE_BUG ),
    new TM( "TM77", M_PSYCH_UP ), new TM( "TM78", M_BULLDOZE ), new TM( "TM79", M_DARK_PULSE ), new TM( "TM80", M_ROCK_SLIDE ),
    new TM( "TM81", M_X_SCISSOR ), new TM( "TM82", M_DRAGON_TAIL ), new TM( "TM83", M_WORK_UP ), new TM( "TM84", M_POISON_JAB ),
    new TM( "TM85", M_DREAM_EATER ), new TM( "TM86", M_GRASS_KNOT ), new TM( "TM87", M_SWAGGER ), new TM( "TM88", M_SLEEP_TALK ),
    new TM( "TM89", M_PLUCK ), new TM( "TM90", M_SUBSTITUTE ), new TM( "TM91", M_FLASH_CANNON ), new TM( "TM92", M_TRICK_ROOM ),

    new TM( "TM93", M_WILD_CHARGE ),
    new TM( "TM94", M_AVALANCHE ),
    new TM( "TM95", M_SNARL ),
    new TM( "TM96", M_NATURE_POWER ),
    new TM( "TM97", M_FALSE_SWIPE ),
    new TM( "TM98", M_HYPER_VOICE ),
    new TM( "TM99", M_BRINE ),
    new TM( "TM100", M_HEADBUTT ),

    new keyItem( "Forschersack" ), new keyItem( "Beutesack" ),
    new keyItem( "Regelbuch" ), new keyItem( "Poke-Radar" ),
    new keyItem( "Punktekarte" ), new keyItem( "Tagebuch" ),
    new keyItem( "StickKoffer" ), new keyItem( "Modekoffer" ),
    new keyItem( "Stickertuete" ), new keyItem( "Adressbuch" ),
    new keyItem( "K-Schluessel" ), new keyItem( "Talisman" ),
    new keyItem( "G-Schluessel" ), new keyItem( "RoteKette" ),
    new keyItem( "Karte" ), new keyItem( "Kampffahnder" ),
    new keyItem( "Muenzkorb" ), new keyItem( "Angel" ),
    new keyItem( "Profiangel" ), new keyItem( "Superangel" ),
    new keyItem( "Entonkanne" ), new keyItem( "Knurspbox" ),
    new keyItem( "Fahrrad" ), new keyItem( "B-Schluessel" ),
    new keyItem( "EichsBrief" ), new keyItem( "Lunarfeder" ),
    new keyItem( "Mitglkarte" ), new keyItem( "Azurfloete" ),
    new keyItem( "Bootsticket" ), new keyItem( "Null" ),
    new keyItem( "Magmastein" ), new keyItem( "Paket" ),
    new keyItem( "Kupon1" ), new keyItem( "Kupon2" ),
    new keyItem( "Kupon3" ), new keyItem( "L-Schluessel" ),
    new keyItem( "Geheimtrank" ), new keyItem( "Kampfkamera" ),
    new keyItem( "Gracidea" ), new keyItem( "F-OEffner" ),
    new keyItem( "Aprikokobox" ), new keyItem( "Icognitoheft" ),
    new keyItem( "Pflanzset" ), new keyItem( "Itemradar" ),
    new keyItem( "BlaueKarte" ), new keyItem( "Flegmon-Rute" ),
    new keyItem( "Klarglocke" ), new keyItem( "Tueroeffner" ),
    new keyItem( "Null" ), new keyItem( "Schiggykanne" ),
    new keyItem( "RoteHaut" ), new keyItem( "Fundsache" ),
    new keyItem( "Fahrschein" ), new keyItem( "Spule" ),
    new keyItem( "Silberfluegel" ), new keyItem( "Buntschwinge" ),
    new keyItem( "Raetsel-Ei" ), new keyItem( "Null" ),
    new keyItem( "Auroraticket" ), new keyItem( "AlteKarte" ),

    new TM( "VM01", M_CUT ),
    new TM( "VM02", M_ROCK_SMASH ),
    new TM( "VM03", M_FLY ),
    new TM( "VM04", M_FLASH ),

    new ball( "Turboball" ), new ball( "Levelball" ),
    new ball( "Koederball" ), new ball( "Schwerball" ),
    new ball( "Sympaball" ), new ball( "Freundesball" ),
    new ball( "Mondball" ), new ball( "Turnierball" ),
    new ball( "Parkball" ),

    new keyItem( "Fotoalbum" ), new keyItem( "GB-Player" ),
    new keyItem( "Gischtglocke" ), new medicine( "Wutkeks" ),

    new berry( "Pumkinbeere" ), new berry( "Drashbeere" ),
    new berry( "Eggantbeere" ), new berry( "Stribbeere" ),
    new berry( "Chilianbeere" ), new berry( "Nutpeabeere" ),
    new berry( "Ginemabeere" ), new berry( "Kuobeere" ),
    new berry( "Yagobeere" ), new berry( "Tougabeere" ),
    new berry( "Ninikubeere" ), new berry( "Topobeere" ),

    new keyItem( "Null" ), new keyItem( "Null" ),
    new keyItem( "Null" ), new keyItem( "Null" ),
    new keyItem( "Null" ), new keyItem( "Null" ),
    new keyItem( "Null" ), new keyItem( "Null" ),
    new keyItem( "Null" ), new keyItem( "Null" ),
    new keyItem( "Null" ), new keyItem( "Null" ),
    new keyItem( "Null" ), new keyItem( "Null" ),

    new keyItem( "Sphaerensegm" ), new keyItem( "GrueneKugel" ),
    new keyItem( "Tresorkapsel" ), new keyItem( "RoteKugel" ),
    new keyItem( "BlaueKugel" ), new keyItem( "Mytokristall" ),

    new item( "Schoenschuppe" ), new item( "Evolith" ),
    new item( "Leichtstein" ), new item( "Beulenhelm" ),
    new item( "Luftballon" ), new item( "RoteKarte" ),
    new item( "Zielscheibe" ), new item( "Klammerband" ),
    new item( "Knolle" ), new item( "Akku" ),
    new item( "Fluchtknopf" ),

    new item( "Feuerjuwel" ), new item( "Wasserjuwel" ),
    new item( "Elektrojuwel" ), new item( "Pflanzjuwel" ),
    new item( "Eisjuwel" ), new item( "Kampfjuwel" ),
    new item( "Giftjuwel" ), new item( "Bodenjuwel" ),
    new item( "Flugjuwel" ), new item( "Psychojuwel" ),
    new item( "Kaeferjuwel" ), new item( "Gesteinjuwel" ),
    new item( "Geistjuwel" ), new item( "Drakojuwel" ),
    new item( "Unlichtjuwel" ), new item( "Stahljuwel" ),
    new item( "Normaljuwel" ),

    new medicine( "Heilfeder" ), new medicine( "Kraftfeder" ),
    new medicine( "Abwehrfeder" ), new medicine( "Geniefeder" ),
    new medicine( "Espritfeder" ), new medicine( "Flinkfeder" ),
    new item( "Prachtfeder" ),

    new item( "Schildfossil" ), new item( "Federfossil" ),

    new keyItem( "Gartenpass" ),
    new item( "Transferorb" ), new item( "Traumball" ),
    new keyItem( "Deko-Box" ), new keyItem( "Drakoschaedel" ),

    new item( "Duftpilz" ), new item( "Riesennugget" ),
    new item( "Triperle" ), new item( "Kometstueck" ),
    new item( "AlterHeller" ), new item( "AlterTaler" ),
    new item( "AlterDukat" ), new item( "AlteVase" ),
    new item( "AlterReif" ), new item( "AlteStatue" ),
    new item( "AlteKrone" ),

    new medicine( "Stratos-Eis" ),

    new item( "Null" ), new item( "Null" ), new item( "Null" ), new item( "Null" ),
    new item( "Null" ), new item( "Null" ), new item( "Null" ), new item( "Null" ),
    new item( "Null" ), new item( "Null" ), new item( "Null" ), new item( "Null" ),
    new item( "Null" ), new item( "Null" ), new item( "Null" ), new item( "Null" ),
    new item( "Null" ), new item( "Null" ), new item( "Null" ), new item( "Null" ),
    new item( "Null" ), new item( "Null" ), new item( "Null" ), new item( "Null" ),

    new keyItem( "Lichtstein" ),
    new keyItem( "Dunkelstein" ),

    new TM( "VM05", M_WHIRLPOOL ),
    new TM( "VM06", M_SURF ),
    new TM( "VM07", M_DIVE ),

    new keyItem( "Viso-Casterm" ),
    new keyItem( "Nebelstein" ),
    new keyItem( "Briefpost" ),
    new keyItem( "Briefpost" ),
    new keyItem( "Briefpost" ),
    new keyItem( "Viso-Caster" ),
    new keyItem( "Medaillenbox" ),
    new keyItem( "DNS-Keil" ),
    new keyItem( "Genehmigung" ),
    new keyItem( "Ovalpin" ),
    new keyItem( "Schillerpin" ),
    new keyItem( "Plasmakarte" ),
    new keyItem( "Schnaeuztuch" ),
    new keyItem( "Achromat" ),
    new keyItem( "Fundsache" ),
    new keyItem( "Fundsache2" ),
    new keyItem( "Wahrspiegel" ),

    new item( "Schwaechenschutz" ),
    new item( "Offensivweste" ),

    new keyItem( "Holo-Log" ),
    new keyItem( "BriefVomProf" ),
    new keyItem( "Rollerskates" ),

    new item( "Feentafel" ),
    new item( "Faehigk-Kapsel" ),
    new item( "Sahnehaeubchen" ),
    new item( "Duftbeutel" ),
    new item( "Leuchtmoos" ),
    new item( "Schneeball" ),
    new item( "Schutzbrille" ),

    new keyItem( "Pokefloete" ),

    new item( "Spriessmulch" ),
    new item( "Wundermulch" ),
    new item( "Wuchermulch" ),
    new item( "Ultramulch" ),

    new item( "Gengarnit" ),
    new item( "Guardevoirnit" ),
    new item( "Ampharosnit" ),
    new item( "Bisaflornit" ),
    new item( "GluraknitX" ),
    new item( "Turtoknit" ),
    new item( "MewtunitX" ),
    new item( "MewtunitY" ),
    new item( "Lohgocknit" ),
    new item( "Meditalisnit" ),
    new item( "Hundemonit" ),
    new item( "Stollossnit" ),
    new item( "Banetteonit" ),
    new item( "Despotarnit" ),
    new item( "Scheroxnit" ),
    new item( "Pinsirnit" ),
    new item( "Aerodactylonit" ),
    new item( "Lucarionit" ),
    new item( "Rexblisarnit" ),
    new item( "Kangamanit" ),
    new item( "Garadosnit" ),
    new item( "Absolnit" ),
    new item( "GluraknitY" ),
    new item( "Simsalanit" ),
    new item( "Skarabornit" ),
    new item( "Flunkifernit" ),
    new item( "Voltensonit" ),
    new item( "Knackracknit" ),

    new berry( "Hibisbeere" ),
    new berry( "Akibeere" ),
    new berry( "Tarabeere" ),

    new keyItem( "Loturzelkanne" ),

    new TM( "VM08", M_DEFOG ),
    new TM( "VM09", M_STRENGTH ),
    new TM( "VM10", M_ROCK_CLIMB ),
    new TM( "VM11", M_WATERFALL ),
    new TM( "VM12", M_TELEPORT ),

    new keyItem( "Kraftwerkspass" ),
    new keyItem( "MegaRing" ),
    new keyItem( "KurioserStein" ),
    new keyItem( "Stein" ),
    new item( "Rabattmarke" ),
    new keyItem( "Liftschluessel" ),
    new keyItem( "TMV-Pass" ),
    new keyItem( "Kalos-Emblem" ),
    new keyItem( "Abenteuerfibel" ),
    new item( "Skurriloskulptur" ),
    new keyItem( "Linsenetui" ),
    new keyItem( "Skurriloskulptur" ),

    new keyItem( "Null" ),
    new keyItem( "Null" ),

    new item( "Yantara-Sable" ),
    new item( "Kieferfossil" ),
    new item( "Flossenfossil" ),

    new keyItem( "LeBelle-Ticket" ),
    new keyItem( "Fahrrad" ),
    new keyItem( "Holo-Log" ),

    new item( "Feenjuwel" ),

    new keyItem( "Null" ),
    new keyItem( "Null" ),

    new keyItem( "Eilrad" ),
    new keyItem( "Kunstrad" ),
    new keyItem( "Wailmerkanne" ),
    new keyItem( "Devon-Waren" ),
    new keyItem( "Aschetasche" ),
    new keyItem( "Kelleroeffner" ),
    new keyItem( "PokeRiegelBox" ),
    new keyItem( "Brief" ),
    new keyItem( "AEon-Ticket" ),
    new keyItem( "Scanner" ),
    new keyItem( "Wuestenglas" ),
    new keyItem( "Meteorit" ),
    new keyItem( "K1-Schluessel" ), new keyItem( "K2-Schluessel" ),
    new keyItem( "K4-Schluessel" ), new keyItem( "K6-Schluessel" ),
    new keyItem( "Lagerschluessel" ),
    new keyItem( "Devon-Scope" ),

    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),

    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),

    new keyItem( "Meteorit" ),
    new item( "Sumpexnit" ),
    new item( "Gewaldronit" ),
    new item( "Zobirisnit" ),
    new item( "Altarianit" ),
    new item( "Galagladinit" ),
    new item( "Ohrdochnit" ),
    new item( "Metagrossnit" ),
    new item( "Tohaidonit" ),
    new item( "Lahmusnit" ),
    new item( "Stahlosnit" ),
    new item( "Taubossnit" ),
    new item( "Firnontornit" ),
    new item( "Diancienit" ),

    new keyItem( "Banngefaess" ),//Prison Bottle
    new keyItem( "Null" ),

    new item( "Cameruptnit" ),
    new item( "Schlapornit" ),
    new item( "Brutalandanit" ),
    new item( "Bibornit" ),

    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "Null" ),
    new keyItem( "AEon-Floete" )
};
