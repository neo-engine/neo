/*
Pokémon neo
------------------------------

file        : pokemon.cpp
author      : Philip Wellnitz
description : The main Pokémon engine

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

#include <algorithm>

#include "battle/battleDefines.h"
#include "battle/move.h"
#include "fs/data.h"
#include "gen/itemNames.h"
#include "gen/pokemonNames.h"
#include "io/strings.h"
#include "map/mapDrawer.h"
#include "pokemon.h"
#include "save/saveGame.h"

boxPokemon::boxPokemon( u16 p_pkmnId, u16 p_level, u8 p_forme, const char* p_name, u8 p_shiny,
                        bool p_hiddenAbility, bool p_isEgg, u8 p_ball, u8 p_pokerus,
                        bool p_fatefulEncounter, pkmnData* p_data )
    : boxPokemon::boxPokemon( nullptr, p_pkmnId, p_name, p_level, SAVE::SAV.getActiveFile( ).m_id,
                              SAVE::SAV.getActiveFile( ).m_sid,
                              SAVE::SAV.getActiveFile( ).m_playername, p_shiny, p_hiddenAbility,
                              p_fatefulEncounter, p_isEgg, MAP::curMap->getCurrentLocationId( ),
                              p_ball, p_pokerus, p_forme, p_data ) {
}

boxPokemon::boxPokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id,
                        u16 p_sid, const char* p_oT, u8 p_shiny, bool p_hiddenAbility,
                        bool p_fatefulEncounter, bool p_isEgg, u16 p_gotPlace, u8 p_ball,
                        u8 p_pokerus, u8 p_forme, pkmnData* p_data ) {
    pkmnData data;
    if( p_data == nullptr ) {
        data = FS::getPkmnData( p_pkmnId, p_forme );
    } else {
        data = *p_data;
    }

    m_rand = rand( ) & 0xF;

    m_oTId  = p_id;
    m_oTSid = p_sid;
    m_pid   = rand( );
    if( p_shiny == 2 ) {
        while( !isShiny( ) ) { m_pid = rand( ); }
        m_shinyType = 2;
    } else if( p_shiny == 1 ) {
        while( isShiny( ) ) m_pid = rand( );
    } else if( p_shiny ) { // Try p_shiny - 2 additional times to generate a shiny PId
        for( u8 i = 0; i < p_shiny - 2 && !isShiny( ); ++i ) {
            m_pid       = rand( );
            m_shinyType = 1;
        }
        if( SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( BAG::ITEMTYPE_KEYITEM ),
                                                    I_SHINY_CHARM ) ) {
            for( u8 i = 0; i < p_shiny - 2 && !isShiny( ); ++i ) {
                m_pid       = rand( );
                m_shinyType = 1;
            }
        }
    }

    m_experienceGained = EXP[ p_level - 1 ][ data.getExpType( ) ];
    m_nature           = m_pid % 25;

    if( data.m_baseForme.m_items[ 3 ] )
        m_heldItem = data.m_baseForme.m_items[ 3 ];
    else {
        u8 m_b1 = rand( ) % 100;
        if( m_b1 < 1 && data.m_baseForme.m_items[ 0 ] )
            m_heldItem = data.m_baseForme.m_items[ 0 ];
        else if( m_b1 < 6 && data.m_baseForme.m_items[ 1 ] )
            m_heldItem = data.m_baseForme.m_items[ 1 ];
        else if( m_b1 < 56 && data.m_baseForme.m_items[ 2 ] )
            m_heldItem = data.m_baseForme.m_items[ 2 ];
    }

    if( p_isEgg ) {
        m_steps          = data.m_eggCycles;
        m_gotDate[ 0 ]   = SAVE::CURRENT_DATE.m_day;
        m_gotDate[ 1 ]   = SAVE::CURRENT_DATE.m_month;
        m_gotDate[ 2 ]   = SAVE::CURRENT_DATE.m_year % 100;
        m_gotPlace       = p_gotPlace;
        m_hatchDate[ 0 ] = m_hatchDate[ 1 ] = m_hatchDate[ 2 ] = m_hatchPlace = 0;
    } else {
        m_steps          = data.m_baseFriend;
        m_hatchDate[ 0 ] = SAVE::CURRENT_DATE.m_day;
        m_hatchDate[ 1 ] = SAVE::CURRENT_DATE.m_month;
        m_hatchDate[ 2 ] = SAVE::CURRENT_DATE.m_year % 100;
        m_hatchPlace     = p_gotPlace;
    }
    m_origLang = 5;

    if( p_moves ) {
        memcpy( m_moves, p_moves, sizeof( m_moves ) );
    } else {
        FS::getLearnMoves( p_pkmnId, p_forme, 0, p_level, 4, m_moves );
    }
    for( u8 i = 0; i < 4; ++i ) {
        BATTLE::moveData mdata = FS::getMoveData( m_moves[ i ] );
        m_curPP[ i ]           = mdata.m_pp;
    }

    for( u8 i = 0; i < 6; ++i ) IVset( i, rand( ) & 31 );

    if( p_fatefulEncounter ) {
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
    }

    setIsNicknamed( !!p_name );
    setIsEgg( p_isEgg );
    m_fateful = p_fatefulEncounter;

    if( p_name ) {
        strcpy( m_name, p_name );
        setIsNicknamed( true );
    } else {
        FS::getDisplayName( p_pkmnId, m_name, CURRENT_LANGUAGE );
        setIsNicknamed( false );
    }
    m_hometown = 4;
    strcpy( m_oT, p_oT );
    m_pokerus  = p_pokerus;
    m_ball     = p_ball;
    m_gotLevel = p_level;

    m_abilitySlot = 2 * p_hiddenAbility + ( m_pid & 1 );
    setSpecies( p_pkmnId, &data );
    setForme( p_forme );
}

bool boxPokemon::gainExperience( u32 p_amount ) {
    if( p_amount >= MAX_EXP || m_experienceGained + p_amount > MAX_EXP ) [[unlikely]] {
        m_experienceGained = MAX_EXP;
        return false;
    }

    m_experienceGained += p_amount;
    return true;
}

bool boxPokemon::isFullyEvolved( ) const {
    auto edata = FS::getPkmnEvolveData( getSpecies( ), getForme( ) );
    return !!edata.m_evolutionCount;
}

u8 boxPokemon::getCompatibility( const boxPokemon& p_other ) const {
    auto data1 = FS::getPkmnData( getSpecies( ), getForme( ) );
    auto data2 = FS::getPkmnData( p_other.getSpecies( ), p_other.getForme( ) );

    if( data1.getEggType( 0 ) == EGG_TYPE_UNDISCOVERED
        || data1.getEggType( 1 ) == EGG_TYPE_UNDISCOVERED
        || data2.getEggType( 0 ) == EGG_TYPE_UNDISCOVERED
        || data2.getEggType( 1 ) == EGG_TYPE_UNDISCOVERED ) {
        // pkmn cannot breed
        return 0;
    }

    u8 dittocnt
        = ( data1.getEggType( 0 ) == EGG_TYPE_DITTO || data1.getEggType( 1 ) == EGG_TYPE_DITTO )
          + ( data2.getEggType( 0 ) == EGG_TYPE_DITTO || data2.getEggType( 1 ) == EGG_TYPE_DITTO );

    if( dittocnt >= 2 ) {
        // ditto can't breed
        return 0;
    }

    if( !dittocnt && data1.getEggType( 0 ) != data2.getEggType( 0 )
        && data1.getEggType( 0 ) != data2.getEggType( 1 )
        && data1.getEggType( 1 ) != data2.getEggType( 0 )
        && data1.getEggType( 1 ) != data2.getEggType( 1 ) ) {
        // incompatible pkmn
        return 0;
    }

    if( gender( ) * p_other.gender( ) > 0 ) {
        return 0;
    } else if( !dittocnt && gender( ) * p_other.gender( ) == 0 ) {
        return 0;
    }

    if( getSpecies( ) == p_other.getSpecies( ) ) {
        if( m_oTId != p_other.m_oTId || m_oTSid != p_other.m_oTSid ) [[unlikely]] {
            return 3;
        } else {
            return 2;
        }
    } else {
        if( m_oTId != p_other.m_oTId || m_oTSid != p_other.m_oTSid ) [[unlikely]] {
            return 2;
        } else {
            return 1;
        }
    }
}

u16 boxPokemon::getBaseSpecies( ) const {
    auto edata = FS::getPkmnEvolveData( getSpecies( ), getForme( ) );
    if( edata.m_baseEvolution ) { return edata.m_baseEvolution; }
    return ( getForme( ) << 16 ) | getSpecies( );
}

bool boxPokemon::breed( const boxPokemon& p_other, boxPokemon& p_result ) const {
    if( gender( ) * p_other.gender( ) > 0 ) [[unlikely]] { return false; }

    u8 dittocnt = ( getSpecies( ) == PKMN_DITTO ) + ( p_other.getSpecies( ) == PKMN_DITTO );

    if( dittocnt >= 2 ) [[unlikely]] {
        // ditto can't breed
        return 0;
    }

    const boxPokemon& mom = ( gender( ) > 0 ) ? p_other : *this;
    const boxPokemon& dad = ( gender( ) > 0 ) ? *this : p_other;

    // compute new species
    u32 species = mom.getSpecies( ) != PKMN_DITTO ? mom.getBaseSpecies( ) : dad.getBaseSpecies( );
    u8  forme   = species >> 16;
    species &= 0xFFFF;

    // special cases
    switch( species ) {
    case PKMN_PIKACHU:
        species = PKMN_PICHU;
        forme   = 0;
        break;
    case PKMN_CLEFAIRY: species = PKMN_CLEFFA; break;
    case PKMN_JIGGLYPUFF: species = PKMN_IGGLYBUFF; break;
    case PKMN_HITMONLEE:
    case PKMN_HITMONCHAN:
    case PKMN_HITMONTOP: species = PKMN_TYROGUE; break;
    case PKMN_JYNX: species = PKMN_SMOOCHUM; break;
    case PKMN_ELECTABUZZ: species = PKMN_ELEKID; break;
    case PKMN_MAGMAR:
        species = PKMN_MAGBY;
        break;
        // gender-counterpart species
    case PKMN_NIDORAN_F:
    case PKMN_NIDORAN_M: species = ( rand( ) & 1 ) ? PKMN_NIDORAN_M : PKMN_NIDORAN_F; break;
    case PKMN_VOLBEAT:
    case PKMN_ILLUMISE:
        species = ( rand( ) & 1 ) ? PKMN_VOLBEAT : PKMN_ILLUMISE;
        break;
        // manaphy
    case PKMN_MANAPHY:
        species = PKMN_PHIONE;
        break;
        // incenses
    case PKMN_MARILL:
        if( getItem( ) == I_SEA_INCENSE || p_other.getItem( ) == I_SEA_INCENSE ) {
            species = PKMN_AZURILL;
            forme   = 0;
        }
        break;
    case PKMN_WOBBUFFET:
        if( getItem( ) == I_LAX_INCENSE || p_other.getItem( ) == I_LAX_INCENSE ) {
            species = PKMN_WYNAUT;
            forme   = 0;
        }
        break;
    case PKMN_ROSELIA:
        if( getItem( ) == I_ROSE_INCENSE || p_other.getItem( ) == I_ROSE_INCENSE ) {
            species = PKMN_BUDEW;
            forme   = 0;
        }
        break;
    case PKMN_CHIMECHO:
        if( getItem( ) == I_PURE_INCENSE || p_other.getItem( ) == I_PURE_INCENSE ) {
            species = PKMN_CHINGLING;
            forme   = 0;
        }
        break;
    case PKMN_SUDOWOODO:
        if( getItem( ) == I_ROCK_INCENSE || p_other.getItem( ) == I_ROCK_INCENSE ) {
            species = PKMN_BONSLY;
            forme   = 0;
        }
        break;
    case PKMN_MR_MIME:
        if( getItem( ) == I_ODD_INCENSE || p_other.getItem( ) == I_ODD_INCENSE ) {
            species = PKMN_MIME_JR;
            forme   = 0;
        }
        break;
    case PKMN_CHANSEY:
        if( getItem( ) == I_LUCK_INCENSE || p_other.getItem( ) == I_LUCK_INCENSE ) {
            species = PKMN_HAPPINY;
            forme   = 0;
        }
        break;
    case PKMN_MANTINE:
        if( getItem( ) == I_WAVE_INCENSE || p_other.getItem( ) == I_WAVE_INCENSE ) {
            species = PKMN_MANTYKE;
            forme   = 0;
        }
        break;
    case PKMN_SNORLAX:
        if( getItem( ) == I_FULL_INCENSE || p_other.getItem( ) == I_FULL_INCENSE ) {
            species = PKMN_MUNCHLAX;
            forme   = 0;
        }
        break;
    case PKMN_ROTOM: forme = 0; break;

    default: break;
    }

    // check if the dad should inherit its special forme
    if( dad.getSpecies( ) != PKMN_ROTOM && dad.getItem( ) == I_EVERSTONE ) [[unlikely]] {
        u32 otherspe = dad.getBaseSpecies( );
        if( species == ( otherspe & 0xffff ) ) { forme = otherspe >> 16; }
    }

    // inherit hidden abilities
    bool hiddenA = mom.getAbilitySlot( ) >= 2 && ( rand( ) % 100 > 60 );

    // compute the pball the egg should be caught in
    u8 pball = mom.getBall( );
    if( BAG::ballToItem( pball ) == I_CHERISH_BALL || BAG::ballToItem( pball ) == I_MASTER_BALL
        || BAG::ballToItem( pball ) == I_PREMIER_BALL ) [[unlikely]] {
        // these balls can't be inherited
        pball = BAG::itemToBall( I_POKE_BALL );
    }

    p_result = boxPokemon( species, 1, forme, 0, 0, hiddenA, true, pball );

    // inherit stats
    u8 inheritIVcount = 3;
    if( getItem( ) == I_DESTINY_KNOT || p_other.getItem( ) == I_DESTINY_KNOT ) [[unlikely]] {
        inheritIVcount = 5;
    }

    u8 inheritmom = 0, inheritdad = 0;
    switch( mom.getItem( ) ) {
    case I_POWER_ANKLET: inheritmom |= ( 1 << BATTLE::SPEED ); break;
    case I_POWER_BAND: inheritmom |= ( 1 << BATTLE::SDEF ); break;
    case I_POWER_BELT: inheritmom |= ( 1 << BATTLE::DEF ); break;
    case I_POWER_BRACER: inheritmom |= ( 1 << BATTLE::ATK ); break;
    case I_POWER_LENS: inheritmom |= ( 1 << BATTLE::SATK ); break;
    case I_POWER_WEIGHT: inheritmom |= ( 1 << BATTLE::HP ); break;
    default: break;
    }
    switch( dad.getItem( ) ) {
    case I_POWER_ANKLET: inheritdad |= ( 1 << BATTLE::SPEED ); break;
    case I_POWER_BAND: inheritdad |= ( 1 << BATTLE::SDEF ); break;
    case I_POWER_BELT: inheritdad |= ( 1 << BATTLE::DEF ); break;
    case I_POWER_BRACER: inheritdad |= ( 1 << BATTLE::ATK ); break;
    case I_POWER_LENS: inheritdad |= ( 1 << BATTLE::SATK ); break;
    case I_POWER_WEIGHT: inheritdad |= ( 1 << BATTLE::HP ); break;
    default: break;
    }

    while( std::popcount( u8( inheritmom | inheritdad ) ) < inheritIVcount ) {
        u8 nxt = ( 1 << ( rand( ) % 6 ) );
        if( ( inheritmom | inheritdad ) & nxt ) { continue; }
        if( rand( ) & 1 ) {
            inheritmom |= nxt;
        } else {
            inheritdad |= nxt;
        }
    }

    for( u8 i = 0; i < 6; ++i ) {
        u8 stat = p_result.IVget( i );
        if( ( inheritmom & ( 1 << i ) ) && ( inheritdad & ( 1 << i ) ) ) {
            if( rand( ) & 1 ) {
                stat = mom.IVget( i );
            } else {
                stat = dad.IVget( i );
            }
        } else if( inheritmom & ( 1 << i ) ) {
            stat = mom.IVget( i );
        } else if( inheritdad & ( 1 << i ) ) {
            stat = dad.IVget( i );
        }

        p_result.IVset( i, stat );
    }

    // inherit nature

    auto nature = p_result.getNature( );

    if( dad.getItem( ) == I_EVERSTONE && mom.getItem( ) == I_EVERSTONE ) {
        if( rand( ) & 1 ) {
            nature = dad.getNature( );
        } else {
            nature = mom.getNature( );
        }
    } else if( dad.getItem( ) == I_EVERSTONE ) {
        nature = dad.getNature( );
    } else if( mom.getItem( ) == I_EVERSTONE ) {
        nature = mom.getNature( );
    }
    p_result.setNature( nature );

    // inherit moves
    u8            moveslot = 0;
    std::set<u16> moves    = std::set<u16>( );

    if( species == PKMN_PICHU
        && ( dad.getItem( ) == I_LIGHT_BALL || mom.getItem( ) == I_LIGHT_BALL ) ) [[unlikely]] {
        p_result.m_moves[ moveslot++ ] = M_VOLT_TACKLE;
        moves.insert( M_VOLT_TACKLE );
    }

    auto learnset = FS::getLearnset( p_result.getSpecies( ), p_result.getForme( ) );

    // check for egg moves
    for( u8 i = 0; i < 4 && moveslot < 4; ++i ) {
        if( !mom.m_moves[ i ] ) { break; }
        if( FS::canLearn( learnset, mom.m_moves[ i ], FS::LEARN_EGG, FS::LEARN_EGG )
            && !moves.count( mom.m_moves[ i ] ) ) {
            p_result.m_moves[ moveslot++ ] = mom.m_moves[ i ];
            moves.insert( mom.m_moves[ i ] );
        }
    }
    for( u8 i = 0; i < 4 && moveslot < 4; ++i ) {
        if( !dad.m_moves[ i ] ) { break; }
        if( FS::canLearn( learnset, dad.m_moves[ i ], FS::LEARN_EGG, FS::LEARN_EGG )
            && !moves.count( dad.m_moves[ i ] ) ) {
            p_result.m_moves[ moveslot++ ] = dad.m_moves[ i ];
            moves.insert( dad.m_moves[ i ] );
        }
    }

    // check for inherited moves: moves that both parents have and that the offspring can
    // learn via level up or TM or move tutor
    for( u8 i = 0; i < 4 && moveslot < 4; ++i ) {
        if( !dad.m_moves[ i ] ) { break; }
        for( u8 ii = 0; ii < 4; ++ii ) {
            if( !mom.m_moves[ ii ] ) { break; }
            if( mom.m_moves[ ii ] == dad.m_moves[ i ] ) {
                if( FS::canLearn( learnset, dad.m_moves[ i ], FS::LEARN_TUTOR )
                    && !moves.count( dad.m_moves[ i ] ) ) {
                    p_result.m_moves[ moveslot++ ] = dad.m_moves[ i ];
                    moves.insert( dad.m_moves[ i ] );
                }
                break;
            }
        }
    }

    // add default moves
    u16 pos = 0;
    while( moveslot < 4 && learnset[ pos ] <= 1 ) {
        if( !moves.count( learnset[ ++pos ] ) ) {
            p_result.m_moves[ moveslot++ ] = learnset[ pos ];
            moves.insert( learnset[ pos ] );
        }
        ++pos;
    }

    // set pp to correct value
    for( u8 i = 0; i < 4; ++i ) {
        if( p_result.m_moves[ i ] ) {
            BATTLE::moveData mdata = FS::getMoveData( p_result.m_moves[ i ] );
            p_result.m_curPP[ i ]  = mdata.m_pp;
        }
    }

    return true;
}

bool boxPokemon::isForeign( ) const {
    if( std::strcmp( m_oT, SAVE::SAV.getActiveFile( ).m_playername ) ) { return true; }
    if( m_oTId != SAVE::SAV.getActiveFile( ).m_id ) { return true; }
    if( m_oTSid != SAVE::SAV.getActiveFile( ).m_sid ) { return true; }
    return false;
}

void boxPokemon::setSpecies( u16 p_newSpecies, pkmnData* p_data ) {
    m_speciesId = p_newSpecies;
    setForme( 0 );
    pkmnData data;
    if( p_data == nullptr ) {
        data = FS::getPkmnData( getSpecies( ), getForme( ) );
    } else {
        data = *p_data;
    }

    pkmnGenderType A = data.m_baseForme.m_genderRatio;
    if( A == GENDER_TYPE_MALE )
        m_isFemale = m_isGenderless = false;
    else if( A == GENDER_TYPE_FEMALE )
        m_isFemale = true, m_isGenderless = false;
    else if( A == GENDER_TYPE_GENDERLESS )
        m_isFemale = false, m_isGenderless = true;
    else if( ( m_pid & 255 ) >= A )
        m_isFemale = m_isGenderless = false;
    else
        m_isFemale = true, m_isGenderless = false;

    setAbility( getAbilitySlot( ), &data );
    recalculateForme( );
}

void boxPokemon::setAbility( u8 p_newAbilitySlot, pkmnData* p_data ) {
    pkmnData data;
    if( p_data == nullptr ) {
        data = FS::getPkmnData( getSpecies( ), getForme( ) );
    } else {
        data = *p_data;
    }

    m_abilitySlot = p_newAbilitySlot;

    m_ability = ( p_newAbilitySlot >= 2 && data.m_baseForme.m_abilities[ 2 ] )
                    ? ( ( ( p_newAbilitySlot & 1 ) || !data.m_baseForme.m_abilities[ 3 ] )
                            ? data.m_baseForme.m_abilities[ 2 ]
                            : data.m_baseForme.m_abilities[ 3 ] )
                    : ( ( ( p_newAbilitySlot & 1 ) || !data.m_baseForme.m_abilities[ 1 ] )
                            ? data.m_baseForme.m_abilities[ 0 ]
                            : data.m_baseForme.m_abilities[ 1 ] );
}

void boxPokemon::recalculateForme( ) {
    switch( m_speciesId ) {
    case PKMN_GIRATINA: {
        setForme( ( m_heldItem == I_GRISEOUS_ORB ) );
        return;
    }
    case PKMN_ARCEUS: {
        if( m_heldItem >= I_FLAME_PLATE && m_heldItem <= I_IRON_PLATE ) {
            setForme( m_heldItem - I_FLAME_PLATE + 1 );
        } else if( m_heldItem == I_NULL_PLATE ) {
            setForme( 17 );
        } else if( m_heldItem == I_PIXIE_PLATE ) {
            setForme( 18 );
        } else if( m_heldItem >= I_FIRIUM_Z && m_heldItem <= I_STEELIUM_Z ) {
            setForme( m_heldItem - I_FIRIUM_Z + 1 );
        } else if( m_heldItem == I_FAIRIUM_Z ) {
            setForme( 18 );
        } else if( m_heldItem >= I_FIRIUM_Z2 && m_heldItem <= I_STEELIUM_Z2 ) {
            setForme( m_heldItem - I_FIRIUM_Z2 + 1 );
        } else if( m_heldItem == I_FAIRIUM_Z2 ) {
            setForme( 18 );
        } else {
            setForme( 0 );
        }
        return;
    }
    case PKMN_GENESECT: {
        if( m_heldItem >= I_DOUSE_DRIVE && m_heldItem <= I_CHILL_DRIVE ) {
            setForme( m_heldItem - I_DOUSE_DRIVE + 1 );
        } else {
            setForme( 0 );
        }
        return;
    }
    case PKMN_SILVALLY: {
        if( m_heldItem >= I_FIGHTING_MEMORY && m_heldItem <= I_FAIRY_MEMORY ) {
            setForme( m_heldItem - I_FIGHTING_MEMORY + 1 );
        } else {
            setForme( 0 );
        }
        return;
    }
    }
}

bool boxPokemon::setNature( pkmnNatures p_newNature ) {
    if( getNature( ) == p_newNature ) { return false; }
    m_nature = u8( p_newNature );
    return true;
}

bool boxPokemon::swapAbilities( bool p_toHidden ) {
    if( p_toHidden && m_abilitySlot >= 2 ) {
        return false; // pkmn already has a hidden ability.
    }
    auto old = m_ability;
    if( !p_toHidden ) {
        setAbility( m_abilitySlot ^ 1 );
    } else {
        setAbility( m_abilitySlot + 2 );
    }
    if( old == m_ability ) {
        m_abilitySlot ^= 1;
        return false;
    }
    return true;
}

void boxPokemon::hatch( ) {
    setIsEgg( false );
    m_hatchPlace     = MAP::curMap->getCurrentLocationId( );
    m_hatchDate[ 0 ] = SAVE::CURRENT_DATE.m_day;
    m_hatchDate[ 1 ] = SAVE::CURRENT_DATE.m_month;
    m_hatchDate[ 2 ] = SAVE::CURRENT_DATE.m_year % 100;

    m_oTId  = SAVE::SAV.getActiveFile( ).m_id;
    m_oTSid = SAVE::SAV.getActiveFile( ).m_sid;
    strcpy( m_oT, SAVE::SAV.getActiveFile( ).m_playername );
}

bool boxPokemon::learnMove( u16 p_move, std::function<void( const char* )> p_message,
                            std::function<u8( boxPokemon*, u16 )> p_getMove,
                            std::function<bool( const char* )>    p_yesNoMessage ) {
    char buffer[ 100 ];
    if( p_move == m_moves[ 0 ] || p_move == m_moves[ 1 ] || p_move == m_moves[ 2 ]
        || p_move == m_moves[ 3 ] ) {
        snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_ALREADY_KNOWS_MOVE ), m_name,
                  FS::getMoveName( p_move ).c_str( ) );
        p_message( buffer );
        return false;
    } else if( FS::canLearn( getSpecies( ), getForme( ), p_move, FS::LEARN_TM ) ) {
        auto mdata    = FS::getMoveData( p_move );
        bool freeSpot = false;
        for( u8 i = 0; i < 4; ++i )
            if( !m_moves[ i ] ) {
                m_moves[ i ] = p_move;
                m_curPP[ i ] = mdata.m_pp;

                snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_LEARNED_MOVE ), m_name,
                          FS::getMoveName( p_move ).c_str( ) );
                p_message( buffer );
                freeSpot = true;
                break;
            }
        if( !freeSpot ) {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_TRIES_TO_LEARN_MOVE ), m_name,
                      FS::getMoveName( p_move ).c_str( ) );
            p_message( buffer );
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_ALREADY_KNOWS_4_MOVES ), m_name );
            if( p_yesNoMessage( buffer ) ) {
                loop( ) {
                    u8 res = p_getMove( this, p_move );
                    if( res < 4 ) {
                        if( BATTLE::isFieldMove( m_moves[ res ] ) ) {
                            snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_CANT_FORGET_MOVE ),
                                      m_name, FS::getMoveName( m_moves[ res ] ).c_str( ) );
                            p_message( buffer );
                            snprintf( buffer, 99,
                                      GET_STRING( IO::STR_UI_PKMN_ALREADY_KNOWS_4_MOVES ), m_name );
                            p_message( buffer );
                            continue;
                        } else {
                            m_moves[ res ] = p_move;
                            m_curPP[ res ] = std::min( m_curPP[ res ], mdata.m_pp );
                        }
                        return true;
                    }
                    break;
                }
            }
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_DIDNT_LEARN_MOVE ), m_name,
                      FS::getMoveName( p_move ).c_str( ) );
            p_message( buffer );
            return false;
        }
    } else {
        snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_CANT_LEARN_MOVE ), m_name,
                  FS::getMoveName( p_move ).c_str( ) );
        p_message( buffer );
        return false;
    }
    return true;
}
