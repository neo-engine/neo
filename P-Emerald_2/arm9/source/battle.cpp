/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : battle.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for Pokémon battles

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */


#include <cwchar>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <functional>

#include <nds.h>

#include "battle.h"
#include "pokemon.h"
#include "move.h"
#include "sprite.h"
#include "item.h"
#include "screenLoader.h"
#include "messageBox.h"
#include "saveGame.h"
#include "bag.h"
#include "buffer.h"
#include "fs.h"

#include "Message.h"
#include "Border_1.h"
#include "Border_2.h"
#include "Border_3.h"
#include "Border_4.h"
#include "Border_5.h"
#include "Back.h"

#include "Battle1.h"
#include "Battle2.h"

#include "BattleSub1.h"
#include "BattleSub2.h"
#include "BattleSub3.h"
#include "BattleSub4.h"
#include "BattleSub5.h"
#include "BattleSub6.h"

#include "Choice_1.h"
#include "Choice_2.h"
#include "Choice_3.h"

#include "BattleBall1.h" //Normal
#include "BattleBall2.h" //Fainted
#include "BattleBall3.h" //Statused

#include "PokeBall1.h"
#include "PokeBall2.h"
#include "PokeBall3.h"
#include "PokeBall4.h"
#include "PokeBall5.h"
#include "PokeBall6.h"
#include "PokeBall7.h"
#include "PokeBall8.h"
#include "PokeBall9.h"
#include "PokeBall10.h"
#include "PokeBall11.h"
#include "PokeBall12.h"
#include "PokeBall13.h"
#include "PokeBall14.h"
#include "PokeBall15.h"
#include "PokeBall16.h"
#include "PokeBall17.h"

#include "Shiny1.h"
#include "Shiny2.h"

#include "TestBattleBack.h"
#include "Border.h"

extern OAMTable *Oam;
extern SpriteInfo spriteInfo[ SPRITE_COUNT ];

extern OAMTable *OamTop;
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

extern FONT::Font cust_font;
extern FONT::Font cust_font2;

namespace BATTLE {
    const char* trainerclassnames[ ] = { "Pokémon-Trainer" };

    const char* ailmentnames[ ] = {
        "none",
        "wurde paralysiert.",
        "schläft ein.",
        "wurde eingefroren.",
        "fängt an zu brennen.",
        "wurde vergiftet.",
        "wurde verwirrt.",
        "Infatuation",
        "ist gefangen.",
        "wurde in Nachtmahr\ngefangen.",
        "wurde Folterknecht\nunterworfen.",
        "wurde blockiert.",
        "gähnt.",
        "kann nicht mehr\ngeheilt werden.",
        "No_type_immunity",
        "wurde bepflanzt",
        "fällt unter ein\nEmbargo.",
        "hört Abgesang.",
        "Ingrain"
    };

    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, bool p_big ) {
        if( p_big )
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 20, 24 );
        else
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 8, 12 );
    }
    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR ) {
        p_HP = std::max( std::min( (u16)101, p_HP ), u16( 0 ) );
        u16 factor = std::max( 1, p_outerR / 15 );
        if( p_HP > 100 || p_HP < 0 ) {
            BG_PALETTE[ p_freecolor1 ] = GREEN;
            for( u16 i = 0; i < factor * 100; ++i )
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                u8 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor1 ) << 8 ) | (u8)p_freecolor1;
                //printf("%i %i; ",nx,ny);
                }
        } else {
            BG_PALETTE[ p_freecolor2 ] = NORMAL_;
            for( u16 i = factor * 100 - factor*p_HPstart; i < factor*p_HP; ++i ) {
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u8 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                    ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor2 ) << 8 ) | (u8)p_freecolor2;
                    if( i == factor * 50 )
                        BG_PALETTE[ p_freecolor1 ] = YELLOW;
                    if( i == factor * 80 )
                        BG_PALETTE[ p_freecolor1 ] = RED;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }
    void displayEP( u16 p_EPstart, u16 p_EP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR ) {
        u16 factor = std::max( 1, p_outerR / 15 );
        if( p_EPstart >= 100 || p_EP > 100 ) {
            BG_PALETTE[ p_freecolor1 ] = NORMAL_;
            for( u16 i = 0; i < factor * 100; ++i )
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                u16 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor1 ) << 8 ) | (u8)p_freecolor1;
                //printf("%i %i; ",nx,ny);
                }
        } else {
            BG_PALETTE[ p_freecolor2 ] = ICE;
            for( u16 i = p_EPstart*factor; i <= p_EP*factor; ++i ) {
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                    ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor2 ) << 8 ) | (u8)p_freecolor2;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }

    void initScreen( ) {
        cust_font.setColor( 0, 0 );
        cust_font.setColor( 251, 1 );
        cust_font.setColor( 252, 2 );
        cust_font2.setColor( 0, 0 );
        cust_font2.setColor( 253, 1 );
        cust_font2.setColor( 254, 2 );

        BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
        BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
        BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE[ 251 ] = RGB15( 15, 15, 15 );
        BG_PALETTE[ 252 ] = RGB15( 3, 3, 3 );
        BG_PALETTE[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE[ 254 ] = RGB15( 31, 31, 31 );
        FONT::putrec( 0, 0, 256, 63, true, false, 250 );
    }
    void clearScreen( ) {
        FONT::putrec( 0, 0, 256, 63, true, false, 250 );
    }
    void setTextColor( u16 p_color ) {
        BG_PALETTE_SUB[ 251 ] = BG_PALETTE[ 251 ] = p_color;
    }
    void setText2Color( u16 p_color ) {
        BG_PALETTE_SUB[ 253 ] = BG_PALETTE[ 253 ] = p_color;
    }
    void writeText( const std::wstring& p_message ) {
        cust_font.printMBString( p_message.c_str( ), 8, 8, true );
    }

    void waitForTouchUp( ) {
        while( 1 ) {
            swiWaitForVBlank( );
            updateTime( false );
            scanKeys( );
            auto t = touchReadXY( );
            if( t.px == 0 && t.py == 0 )
                break;
        }
    }
    void waitForKeyUp( int p_key ) {
        while( 1 ) {
            scanKeys( );
            swiWaitForVBlank( );
            updateTime( );
            if( keysUp( ) & p_key )
                break;
        }
    }

#define C2I(a) ((a) - L'0')
    std::wstring parseLogCmd( const battle& p_battle, const std::wstring& p_cmd ) {
        if( p_cmd == L"A" )
            return L"`";
        if( p_cmd == L"CLEAR" ) {
            clearScreen( );
            return L"";
        }
        if( p_cmd.substr( 0, 4 ) == L"COLR" ) {
            u8 r, g, b;

            r = 10 * C2I( p_cmd[ 5 ] ) + C2I( p_cmd[ 6 ] );
            g = 10 * C2I( p_cmd[ 8 ] ) + C2I( p_cmd[ 9 ] );
            b = 10 * C2I( p_cmd[ 11 ] ) + C2I( p_cmd[ 12 ] );

            setTextColor( RGB15( r, g, b ) );
            return L"";
        }

        POKEMON::pokemon& target = ACPKMN2( p_battle, 0, PLAYER );
        bool isPkmn = false;

        if( p_cmd.substr( 0, 4 ) == L"OWN1" ) {
            target = ACPKMN2( p_battle, 0, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OWN2" ) {
            target = ACPKMN2( p_battle, 1, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP1" ) {
            target = ACPKMN2( p_battle, 0, OPPONENT );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP2" ) {
            target = ACPKMN2( p_battle, 1, OPPONENT );
            isPkmn = true;
        }

        if( isPkmn && p_cmd.length( ) == 4 )
            return target.m_boxdata.m_name;

        if( isPkmn ) {
            auto specifier = p_cmd.substr( 5 );
            if( specifier == L"ABILITY" )
                return getWAbilityName( target.m_boxdata.m_ability );
            if( specifier.substr( 0, 4 ) == L"MOVE" ) {
                std::swprintf( wbuffer, 50, L"%s", AttackList[ target.m_boxdata.m_moves[ C2I( specifier[ 4 ] ) - 1 ] ]->m_moveName.c_str( ) );
                return std::wstring( wbuffer );
            }
            if( specifier == L"ITEM" ) {
                std::swprintf( wbuffer, 50, L"%s", ITEMS::ItemList[ target.m_boxdata.m_holdItem ].m_itemName.c_str( ) );
                return std::wstring( wbuffer );
            }
            if( specifier == L"LEVEL" )
                return ( L"" + target.m_Level );
        }

        return L"";
    }

    void battle::log( const std::wstring& p_message ) {
        std::wstring msg = L"";
        for( size_t i = 0; i < p_message.length( ); i++ ) {
            if( p_message[ i ] == L'[' ) {
                std::wstring accmd = L"";
                while( p_message[ ++i ] != L']' )
                    accmd += p_message[ i ];
                msg += parseLogCmd( *this, accmd );
            } else
                msg += p_message[ i ];
        }

        writeText( msg );
        clearScreen( );
    }

    /**
     *  @brief      Runs the battle.
     *  @returns    -1 if opponent won, 0 if the battle resulted in a tie, 1 otherwise
     */
    s8 battle::start( ) {
        battleEndReason battleEnd;

        initBattle( );

        while( _round++ < _maxRounds ) {
            if( canMove( 0 ) )
                declareBattleMove( 0 );
            if( m_battleMode == DOUBLE && canMove( 1 ) )
                declareBattleMove( 1 );

            doMoves( );

            if( endConditionHit( battleEnd ) ) {
                endBattle( battleEnd );
                break;
            }
            if( _round >= _maxRounds ) {
                endBattle( battleEnd = ROUND_LIMIT );
                break;
            }
            doWeather( );
        }

        return battleEnd;
    }

    /**
     *  @brief Initialize the battle.
     */
    void battle::initBattle( ) {
        //Some basic initialization stuff
        initScreen( );

        for( u8 i = 0; i < 6; ++i ) {
            ACPOS( i, PLAYER ) = ACPOS( i, OPPONENT ) = i;
            if( _player->m_pkmnTeam->size( ) > i ) {
                if( ACPKMN( i, PLAYER ).m_stats.m_acHP == 0 || ACPKMN( i, PLAYER ).m_boxdata.m_individualValues.m_isEgg )
                    ACPKMNSTS( i, PLAYER ) = KO;
                else if( ACPKMN( i, PLAYER ).m_statusint )
                    ACPKMNSTS( i, PLAYER ) = STS;
                else
                    ACPKMNSTS( i, PLAYER ) = OK;
            } else
                ACPKMNSTS( i, PLAYER ) = NA;
            if( _opponent->m_pkmnTeam->size( ) > i ) {
                if( ACPKMN( i, OPPONENT ).m_stats.m_acHP == 0 || ACPKMN( i, OPPONENT ).m_boxdata.m_individualValues.m_isEgg )
                    ACPKMNSTS( i, OPPONENT ) = KO;
                else if( ACPKMN( i, OPPONENT ).m_statusint )
                    ACPKMNSTS( i, OPPONENT ) = STS;
                else ACPKMNSTS( i, OPPONENT ) = OK;
            } else
                ACPKMNSTS( i, OPPONENT ) = NA;
        }

        for( u8 p = 0; p < 2; ++p ) {
            _battleSpotOccupied[ 0 ][ p ] = false;
            _battleSpotOccupied[ 1 ][ p ] = !( m_battleMode == DOUBLE );
        }

        trainerIntro( );

        refillBattleSpots( false );

        if( m_weather != NO_WEATHER ) {
            log( _weatherMessage[ m_weather ] );
        }

        doAbilities( ability::BEFORE_BATTLE );
    }

    /**
     *  @brief Shows Battle intro: Trainer mugshot and the "You're challenged by ..." message
     */
    void battle::trainerIntro( ) {
        //TODO
    }

    /**
     *  @brief send in PKMN for fainted ones, if possible
     *  @param p_choice: Specifies whether the player can choose the PKMN which is/are being sent
     */
    void battle::refillBattleSpots( bool p_choice ) {
        acStatus oldSts[ 2 ] = { };
        std::vector< std::pair<u16, u8> > inits;

        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < 2; ++j ) {
                if( !_battleSpotOccupied[ i ][ j ] ) {
                    u8 nextSpot;
                    if( !p_choice || j )
                        nextSpot = getNextPKMN( j );
                    else
                        nextSpot = choosePKMN( );

                    std::swap( ACPOS( i, j ), ACPOS( nextSpot, j ) );

                    if( !i ) {
                        oldSts[ j ] = ACPKMNSTS( 0, j );
                        ACPKMNSTS( 0, j ) = SELECTED;
                    }

                    u16 acSpd = ACPKMN( i, j ).m_stats.m_Spd;
                    inits.push_back( std::pair<u16, u8>( acSpd, 2u * i + ( 1u - j ) ) );
                }
            }
        }

        std::sort( inits.begin( ), inits.end( ), std::greater<std::pair<u16, u8>>( ) );

        for( auto i : inits ) {
            bool isOpp = i.second % 2,
                isSnd = i.second / 2;

            if( !isSnd )
                ACPKMNSTS( 0, isOpp ) = oldSts[ isOpp ];

            sendPKMN( isOpp, isSnd );
            _battleSpotOccupied[ isSnd ][ isOpp ];
        }
    }

    /**
     *  @brief Gets the index of the next PKMN that is not koed.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @returns Minimum i for which ACPKMNSTS(i,p_opponent) != KO and != SELECTED, 7 iff all PKMN fainted
     */
    u8 battle::getNextPKMN( bool p_opponent ) {
        u8 max = ( p_opponent ? _opponent->m_pkmnTeam->size( ) : _player->m_pkmnTeam->size( ) );

        for( u8 i = 0; i < max; ++i )
            if( ACPKMNSTS( i, p_opponent ) != KO
                && ACPKMNSTS( i, p_opponent ) != SELECTED )
                return i;
        return 7;
    }

    /**
     *  @brief Applies all possible abilities ordered by their PKMNs speed.
     *  @param p_situation: Current situation, on which an ability may be useable
     */
    void battle::doAbilities( ability::abilityType p_situation ) {

    }


























    //OLD STUFF -> deprcated

    POKEMON::PKMNDATA::pokemonData pd;
    void init( ) {
        for( int i = 5; i <= 8; ++i ) {
            Oam->oamBuffer[ i ].isHidden = true;
            swiWaitForVBlank( );
        }
        for( int i = 0; i < 4; ++i ) {
            Oam->oamBuffer[ 9 + 2 * i ].isHidden = true;
            swiWaitForVBlank( );
        }

        Oam->oamBuffer[ 20 ].isHidden = true;
        for( int i = 0; i < 20; ++i )
            Oam->oamBuffer[ 36 + i ].isHidden = true;
        //oamIndexS = 36;
        //nextAvailableTileIdxS = p_oam->oamBuffer[36].gfxIndex;
        //palcntS = p_oam->oamBuffer[36].palette;

        cust_font.setColor( 0, 0 );
        cust_font.setColor( 251, 1 );
        cust_font.setColor( 252, 2 );
        cust_font2.setColor( 0, 0 );
        cust_font2.setColor( 253, 1 );
        cust_font2.setColor( 254, 2 );

        BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
        BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
        BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE[ 251 ] = RGB15( 15, 15, 15 );
        BG_PALETTE[ 252 ] = RGB15( 3, 3, 3 );
        BG_PALETTE[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE[ 254 ] = RGB15( 31, 31, 31 );
        FONT::putrec( 0, 0, 256, 63, true, false, 250 );

        updateOAMSub( Oam );
    }
    void clear( ) {
        FONT::putrec( 0, 0, 256, 63, true, false, 250 );
    }
    void dinit( ) {

        drawSub( );

        for( int i = 5; i < 8; ++i )
            Oam->oamBuffer[ 31 + 2 * i ].isHidden = false;
        for( int i = 0; i < 4; ++i ) {
            Oam->oamBuffer[ 9 + 2 * i ].isHidden = false;
            swiWaitForVBlank( );
        }
        updateOAMSub( Oam );
    }

    battle::battle( battleTrainer* p_player, battleTrainer* p_opponent, int p_maxRounds, int p_AILevel, battleMode p_battleMode ) {
        _maxRounds = p_maxRounds;
        _AILevel = p_AILevel;
        _player = p_player;
        _opponent = p_opponent;
        m_battleMode = p_battleMode;

        m_distributeEXP = true;
    }

    u8 oamIndex, palcnt;
    u16 nextAvailableTileIdx;
    u8 oamIndexS, palcntS;
    u16 nextAvailableTileIdxS;

#define OWN_HP_1              1
#define OWN_HP_2              2
#define OPP_HP_1              3
#define OPP_HP_2              4
#define OWN_PB_START          5
#define OPP_PB_START         11
#define OPP_PKMN_1_START     17-1
#define OPP_PKMN_2_START     21-1
#define OWN_PKMN_1_START     25-1
#define OWN_PKMN_2_START     29-1

#define PB_ANIM             127
#define SHINY_ANIM          127

#define OPP_PKMN_1_PAL        0-1
#define OPP_PKMN_2_PAL        1-1
#define OWN_PKMN_1_PAL        2-1
#define OWN_PKMN_2_PAL        3-1

#define PB_PAL_START          4
#define HP_PAL                7

#define OPP_PKMN_1_TILE      44
#define OPP_PKMN_2_TILE     188
#define OWN_PKMN_1_TILE     332
#define OWN_PKMN_2_TILE     476

#define OWN1_EP_COL         160
#define OWN2_EP_COL         OWN1_EP_COL

#define OWN_HP_COL          150
#define OPP_HP_COL          155

#define HP_COL(a,b) (((a) == OPPONENT )? (OPP_HP_COL + (b)*2 ): (OWN_HP_COL + (b)*2 ))

    void initinitBattleScrnSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, int p_ownPok, int p_oppPok ) {
        oamIndex = palcnt = nextAvailableTileIdx = 0;

        SpriteInfo * type1Info = &p_spriteInfo[ oamIndex ];
        SpriteEntry * type1 = &p_oam->oamBuffer[ oamIndex ];
        type1Info->m_oamId = oamIndex;
        type1Info->m_width = 16;
        type1Info->m_height = 16;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 0;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = 0;
        type1->size = OBJSIZE_16;
        type1->gfxIndex = nextAvailableTileIdx;
        type1->priority = OBJPRIORITY_0;
        type1->palette = palcnt;

        for( int i = 0; i < 11; ++i ) {
            p_spriteInfo[ ++oamIndex ] = *type1Info;
            p_oam->oamBuffer[ oamIndex ] = *type1;
            p_oam->oamBuffer[ oamIndex ].x = i < 5 ? 16 + i * 16 : 256 - ( i - 4 ) * 16;
            p_oam->oamBuffer[ oamIndex ].isHidden = i < 5 ? p_oppPok < i : p_ownPok + 5 < i;
            p_oam->oamBuffer[ oamIndex ].y = i >= 5 ? 192 - 16 : 0;
        }

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Pal, &SPRITE_PALETTE[ ( palcnt++ ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall1TilesLen );
        nextAvailableTileIdx += BattleBall1TilesLen / BYTES_PER_16_COLOR_TILE;
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Pal, &SPRITE_PALETTE[ ( palcnt++ ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall2TilesLen );
        nextAvailableTileIdx += BattleBall2TilesLen / BYTES_PER_16_COLOR_TILE;
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Pal, &SPRITE_PALETTE[ ( palcnt++ ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall3TilesLen );
        nextAvailableTileIdx += BattleBall3TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo4Info = &p_spriteInfo[ ++oamIndex ];
        SpriteEntry * Bo4 = &p_oam->oamBuffer[ oamIndex ];
        Bo4Info->m_oamId = oamIndex;
        Bo4Info->m_width = 64;
        Bo4Info->m_height = 64;
        Bo4Info->m_angle = 0;
        Bo4Info->m_entry = Bo4;
        Bo4->y = 0;
        Bo4->isRotateScale = false;
        Bo4->blendMode = OBJMODE_NORMAL;
        Bo4->isMosaic = false;
        Bo4->colorMode = OBJCOLOR_16;
        Bo4->shape = OBJSHAPE_SQUARE;
        Bo4->isHidden = false;
        Bo4->x = 0;
        Bo4->size = OBJSIZE_64;
        Bo4->gfxIndex = nextAvailableTileIdx;
        Bo4->priority = OBJPRIORITY_2;
        Bo4->palette = palcnt;
        Bo4->vFlip = true;
        Bo4->hFlip = true;

        Bo4 = &p_oam->oamBuffer[ ++oamIndex ];
        Bo4->y = 128;
        Bo4->isRotateScale = false;
        Bo4->blendMode = OBJMODE_NORMAL;
        Bo4->isMosaic = false;
        Bo4->colorMode = OBJCOLOR_16;
        Bo4->shape = OBJSHAPE_SQUARE;
        Bo4->isHidden = false;
        Bo4->x = 192;
        Bo4->size = OBJSIZE_64;
        Bo4->gfxIndex = nextAvailableTileIdx;
        Bo4->priority = OBJPRIORITY_2;
        Bo4->palette = palcnt;
        Bo4->vFlip = false;
        Bo4->hFlip = false;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Border_4Pal, &SPRITE_PALETTE[ palcnt * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Border_4Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], Border_4TilesLen );
        nextAvailableTileIdx += Border_4TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo3Info = &p_spriteInfo[ ++oamIndex ];
        SpriteEntry * Bo3 = &p_oam->oamBuffer[ oamIndex ];
        Bo3Info->m_oamId = oamIndex;
        Bo3Info->m_width = 64;
        Bo3Info->m_height = 64;
        Bo3Info->m_angle = 0;
        Bo3Info->m_entry = Bo3;
        Bo3->y = 0;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = false;
        Bo3->x = 64;
        Bo3->size = OBJSIZE_64;
        Bo3->gfxIndex = nextAvailableTileIdx;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = palcnt;
        Bo3->vFlip = true;
        Bo3->hFlip = true;

        Bo3 = &p_oam->oamBuffer[ ++oamIndex ];
        Bo3->y = 128;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = false;
        Bo3->x = 128;
        Bo3->size = OBJSIZE_64;
        Bo3->gfxIndex = nextAvailableTileIdx;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = palcnt;
        Bo3->vFlip = false;
        Bo3->hFlip = false;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          Border_3Tiles,
                          &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ],
                          Border_3TilesLen );

        nextAvailableTileIdx += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo2Info = &p_spriteInfo[ ++oamIndex ];
        SpriteEntry * Bo2 = &p_oam->oamBuffer[ oamIndex ];
        Bo2Info->m_oamId = oamIndex;
        Bo2Info->m_width = 64;
        Bo2Info->m_height = 64;
        Bo2Info->m_angle = 0;
        Bo2Info->m_entry = Bo2;
        Bo2->y = 0;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = false;
        Bo2->x = 128;
        Bo2->size = OBJSIZE_64;
        Bo2->gfxIndex = nextAvailableTileIdx;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = palcnt;
        Bo2->vFlip = true;
        Bo2->hFlip = true;

        Bo2 = &p_oam->oamBuffer[ ++oamIndex ];
        Bo2->y = 128;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = false;
        Bo2->x = 64;
        Bo2->size = OBJSIZE_64;
        Bo2->gfxIndex = nextAvailableTileIdx;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = palcnt;
        Bo2->vFlip = false;
        Bo2->hFlip = false;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          Border_2Tiles,
                          &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ],
                          Border_2TilesLen );
        nextAvailableTileIdx += Border_2TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo1Info = &p_spriteInfo[ ++oamIndex ];
        SpriteEntry * Bo1 = &p_oam->oamBuffer[ oamIndex ];
        Bo1Info->m_oamId = oamIndex;
        Bo1Info->m_width = 64;
        Bo1Info->m_height = 64;
        Bo1Info->m_angle = 0;
        Bo1Info->m_entry = Bo1;
        Bo1->y = 0;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = false;
        Bo1->x = 192;
        Bo1->size = OBJSIZE_64;
        Bo1->gfxIndex = nextAvailableTileIdx;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = palcnt;
        Bo1->vFlip = true;
        Bo1->hFlip = true;

        Bo1 = &p_oam->oamBuffer[ ++oamIndex ];
        Bo1->y = 128;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = false;
        Bo1->x = 0;
        Bo1->size = OBJSIZE_64;
        Bo1->gfxIndex = nextAvailableTileIdx;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = palcnt;
        Bo1->vFlip = false;
        Bo1->hFlip = false;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          Border_1Tiles,
                          &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ],
                          Border_1TilesLen );
        nextAvailableTileIdx += Border_1TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo5Info = &p_spriteInfo[ ++oamIndex ];
        SpriteEntry * Bo5 = &p_oam->oamBuffer[ oamIndex ];
        Bo5Info->m_oamId = oamIndex;
        Bo5Info->m_width = 64;
        Bo5Info->m_height = 64;
        Bo5Info->m_angle = 0;
        Bo5Info->m_entry = Bo5;
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = false;
        Bo5->x = 0;
        Bo5->size = OBJSIZE_64;
        Bo5->gfxIndex = nextAvailableTileIdx;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = palcnt;
        Bo5->vFlip = true;
        Bo5->hFlip = true;

        Bo5 = &p_oam->oamBuffer[ ++oamIndex ];
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = false;
        Bo5->x = 192;
        Bo5->size = OBJSIZE_64;
        Bo5->gfxIndex = nextAvailableTileIdx;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = palcnt;
        Bo5->vFlip = false;
        Bo5->hFlip = false;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Border_5Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], Border_5TilesLen );
        nextAvailableTileIdx += Border_5TilesLen / BYTES_PER_16_COLOR_TILE;

        ++palcnt;
    }

    void drawTopBack( );

    void battle::initBattleScreen( ) {
        sprintf( buffer, "%i.raw", _opponent->m_trainerClass );

        FS::loadPicture( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "ClearD" );
        dmaCopy( BorderBitmap, bgGetGfxPtr( bg2sub ), 256 * 192 );
        dmaCopy( BorderPal, BG_PALETTE_SUB, 256 * 2 );
        drawTopBack( );
        initinitBattleScrnSprites( OamTop, spriteInfoTop, 6, 6 );

        for( int i = 0; i < 6; ++i )
            switch( ACPKMNSTS( i, OPPONENT ) ) {
                case NA:
                    OamTop->oamBuffer[ i ].isHidden = true;
                    break;
                case KO:
                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 32;
                    OamTop->oamBuffer[ i ].palette++;
                    break;
                case STS:
                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 16;
                    OamTop->oamBuffer[ i ].palette += 2;
                    break;
                default:
                    break;
        }
        for( int i = 6; i < 12; ++i )
            switch( _acPkmnStatus[ _acPkmnPosition[ i - 6 ][ PLAYER ] ][ PLAYER ] ) {
                case NA:
                    OamTop->oamBuffer[ i ].isHidden = true;
                    break;
                case KO:
                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 32;
                    OamTop->oamBuffer[ i ].palette++;
                    break;
                case STS:
                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 16;
                    OamTop->oamBuffer[ i ].palette += 2;
                    break;
                default:
                    break;
        }
        updateOAM( OamTop );


        consoleSetWindow( &Top, 0, 0, 32, 24 );
        consoleSelect( &Top );
        consoleClear( );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleClear( );
        consoleSetWindow( &Bottom, 2, 11, 32, 24 );
        consoleSelect( &Bottom );

        cust_font.setColor( 0, 0 );
        cust_font.setColor( 251, 1 );
        cust_font.setColor( 252, 2 );
        cust_font2.setColor( 0, 0 );
        cust_font2.setColor( 253, 1 );
        cust_font2.setColor( 254, 2 );

        BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE_SUB[ 251 ] = RGB15( 30, 30, 30 );
        BG_PALETTE_SUB[ 252 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
        sprintf( buffer, "Eine Herausforderung von\n %s %s!", trainerclassnames[ _opponent->m_trainerClass ], _opponent->m_battleTrainerName );
        cust_font.printString( buffer, 16, 80, true );

        FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n", 144, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
        updateOAM( OamTop );
        for( int i = 0; i < 8; ++i )
            swiWaitForVBlank( );
        oamIndex -= 4;
        --palcnt;
        --palcnt;
        nextAvailableTileIdx -= 144;
        FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n2", 144, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
        updateOAM( OamTop );
        for( int i = 0; i < 8; ++i )
            swiWaitForVBlank( );
        oamIndex -= 4;
        --palcnt;
        --palcnt;
        nextAvailableTileIdx -= 144;
        FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n3", 144, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
        updateOAM( OamTop );
        oamIndex -= 4;
        --palcnt;
        --palcnt;
        nextAvailableTileIdx -= 144;
        for( int i = 0; i < 20; ++i )
            swiWaitForVBlank( );

        for( int l = 0; l < 25; ++l ) {
            FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n3", 144 + 4 * l, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
            updateOAM( OamTop );
            oamIndex -= 4;
            --palcnt;
            --palcnt;
            nextAvailableTileIdx -= 144;
            for( int i = 0; i < 3; ++i )
                swiWaitForVBlank( );
        }

        FS::loadPicture( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "ClearD" );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleClear( );
    }

    void initBattleScreenSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo ) {
        oamIndex = palcnt = nextAvailableTileIdx = 0;

        SpriteInfo * type1Info = &p_spriteInfo[ OWN_HP_2 ];
        SpriteEntry * type1 = &p_oam->oamBuffer[ OWN_HP_2 ];
        type1Info->m_oamId = OWN_HP_2;
        type1Info->m_width = 32;
        type1Info->m_height = 32;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 192 - 32 - 8;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = 256 - 36;
        type1->size = OBJSIZE_32;
        type1->gfxIndex = nextAvailableTileIdx;
        type1->priority = OBJPRIORITY_2;
        type1->palette = HP_PAL;

        p_spriteInfo[ OWN_HP_1 ] = *type1Info;
        p_oam->oamBuffer[ OWN_HP_1 ] = *type1;
        p_oam->oamBuffer[ OWN_HP_1 ].x -= 88;
        p_oam->oamBuffer[ OWN_HP_1 ].y -= 32;
        p_oam->oamBuffer[ OWN_HP_1 ].priority = OBJPRIORITY_2;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle1Pal, &SPRITE_PALETTE[ (HP_PAL)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle1Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], Battle1TilesLen );
        nextAvailableTileIdx += Battle1TilesLen / BYTES_PER_16_COLOR_TILE;

        type1Info = &p_spriteInfo[ OPP_HP_2 ];
        type1 = &p_oam->oamBuffer[ OPP_HP_2 ];
        type1Info->m_oamId = OPP_HP_2;
        type1Info->m_width = 32;
        type1Info->m_height = 32;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 8;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = 0;
        type1->size = OBJSIZE_32;
        type1->gfxIndex = nextAvailableTileIdx;
        type1->priority = OBJPRIORITY_2;
        type1->palette = HP_PAL + 1;

        p_spriteInfo[ OPP_HP_1 ] = *type1Info;
        p_oam->oamBuffer[ OPP_HP_1 ] = *type1;
        p_oam->oamBuffer[ OPP_HP_1 ].x += 88;
        p_oam->oamBuffer[ OPP_HP_1 ].y += 24;
        p_oam->oamBuffer[ OPP_HP_1 ].priority = OBJPRIORITY_2;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle2Pal, &SPRITE_PALETTE[ ( HP_PAL + 1 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle2Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], Battle2TilesLen );
        nextAvailableTileIdx += Battle2TilesLen / BYTES_PER_16_COLOR_TILE;


        type1Info = &p_spriteInfo[ OWN_PB_START ];
        type1 = &p_oam->oamBuffer[ OWN_PB_START ];
        type1Info->m_oamId = OWN_PB_START;
        type1Info->m_width = 16;
        type1Info->m_height = 16;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 0;
        type1->isRotateScale = false;
        type1->isHidden = true;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = 0;
        type1->size = OBJSIZE_16;
        type1->gfxIndex = nextAvailableTileIdx;
        type1->priority = OBJPRIORITY_0;
        type1->palette = PB_PAL_START;

        for( int i = 1; i < 12; ++i ) {
            p_spriteInfo[ OWN_PB_START + i ] = *type1Info;
            p_oam->oamBuffer[ OWN_PB_START + i ] = *type1;
        }

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Pal, &SPRITE_PALETTE[ (PB_PAL_START)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall1TilesLen );
        nextAvailableTileIdx += BattleBall1TilesLen / BYTES_PER_16_COLOR_TILE;
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Pal, &SPRITE_PALETTE[ ( PB_PAL_START + 1 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall2TilesLen );
        nextAvailableTileIdx += BattleBall2TilesLen / BYTES_PER_16_COLOR_TILE;
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Pal, &SPRITE_PALETTE[ ( PB_PAL_START + 2 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall3TilesLen );
        nextAvailableTileIdx += BattleBall3TilesLen / BYTES_PER_16_COLOR_TILE;


    }

    void initBattleSubScreenSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, bool p_isWild, bool p_hasPokeNav ) {
        oamIndexS = palcntS = nextAvailableTileIdxS = 0;


        SpriteInfo * type1Info = &p_spriteInfo[ oamIndexS ];
        SpriteEntry * type1 = &p_oam->oamBuffer[ oamIndexS ];
        type1Info->m_oamId = oamIndexS;
        type1Info->m_width = 64;
        type1Info->m_height = 64;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 72;
        type1->isRotateScale = false;
        type1->isHidden = true;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = 64;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = nextAvailableTileIdxS;
        type1->priority = OBJPRIORITY_2;
        type1->palette = palcntS;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub1Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BattleSub1TilesLen );
        nextAvailableTileIdxS += BattleSub1TilesLen / BYTES_PER_16_COLOR_TILE;

        type1Info = &p_spriteInfo[ ++oamIndexS ];
        type1 = &p_oam->oamBuffer[ oamIndexS ];
        type1Info->m_oamId = oamIndexS;
        type1Info->m_width = 64;
        type1Info->m_height = 64;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 72;
        type1->isRotateScale = false;
        type1->isHidden = true;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = 128;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = nextAvailableTileIdxS;
        type1->priority = OBJPRIORITY_2;
        type1->palette = palcntS;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub2Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BattleSub2TilesLen );
        nextAvailableTileIdxS += BattleSub2TilesLen / BYTES_PER_16_COLOR_TILE;
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub1Pal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );

        type1Info = &p_spriteInfo[ ++oamIndexS ];
        type1 = &p_oam->oamBuffer[ oamIndexS ];
        type1Info->m_oamId = oamIndexS;
        type1Info->m_width = 64;
        type1Info->m_height = 32;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 152;
        type1->isRotateScale = false;
        type1->isHidden = true;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_WIDE;
        type1->x = 96;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = nextAvailableTileIdxS;
        type1->priority = OBJPRIORITY_2;
        type1->palette = palcntS;

        if( p_isWild ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub3Pal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub3Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BattleSub3TilesLen );
            nextAvailableTileIdxS += BattleSub3TilesLen / BYTES_PER_16_COLOR_TILE;
        } else if( p_hasPokeNav ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub6Pal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub6Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BattleSub6TilesLen );
            nextAvailableTileIdxS += BattleSub6TilesLen / BYTES_PER_16_COLOR_TILE;
        }

        type1Info = &p_spriteInfo[ ++oamIndexS ];
        type1 = &p_oam->oamBuffer[ oamIndexS ];
        type1Info->m_oamId = oamIndexS;
        type1Info->m_width = 64;
        type1Info->m_height = 32;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 144;
        type1->isRotateScale = false;
        type1->isHidden = true;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_WIDE;
        type1->x = 16;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = nextAvailableTileIdxS;
        type1->priority = OBJPRIORITY_1;
        type1->palette = palcntS;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub4Pal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub4Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BattleSub4TilesLen );
        nextAvailableTileIdxS += BattleSub4TilesLen / BYTES_PER_16_COLOR_TILE;

        type1Info = &p_spriteInfo[ ++oamIndexS ];
        type1 = &p_oam->oamBuffer[ oamIndexS ];
        type1Info->m_oamId = oamIndexS;
        type1Info->m_width = 64;
        type1Info->m_height = 32;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = 144;
        type1->isRotateScale = false;
        type1->isHidden = true;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_WIDE;
        type1->x = 176;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = nextAvailableTileIdxS;
        type1->priority = OBJPRIORITY_1;
        type1->palette = palcntS;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub5Pal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub5Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BattleSub5TilesLen );
        nextAvailableTileIdxS += BattleSub5TilesLen / BYTES_PER_16_COLOR_TILE;

        for( int i = 0; i < 4; ++i ) {
            SpriteInfo * MInfo = &p_spriteInfo[ ++oamIndexS ];
            SpriteEntry * M = &p_oam->oamBuffer[ oamIndexS ];
            MInfo->m_oamId = oamIndexS;
            MInfo->m_width = 64;
            MInfo->m_height = 64;
            MInfo->m_angle = 0;
            MInfo->m_entry = M;
            M->y = 0;
            M->isRotateScale = false;
            M->blendMode = OBJMODE_NORMAL;
            M->isMosaic = false;
            M->colorMode = OBJCOLOR_16;
            M->shape = OBJSHAPE_SQUARE;
            M->isHidden = true;
            M->x = (i)* 64;
            M->size = OBJSIZE_64;
            M->gfxIndex = nextAvailableTileIdxS;
            M->priority = OBJPRIORITY_2;
            M->palette = palcntS;
        }
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, MessagePal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, MessageTiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], MessageTilesLen );
        nextAvailableTileIdxS += MessageTilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo4Info = &p_spriteInfo[ ++oamIndexS ];
        SpriteEntry * Bo4 = &p_oam->oamBuffer[ oamIndexS ];
        Bo4Info->m_oamId = oamIndexS;
        Bo4Info->m_width = 64;
        Bo4Info->m_height = 64;
        Bo4Info->m_angle = 0;
        Bo4Info->m_entry = Bo4;
        Bo4->y = 0;
        Bo4->isRotateScale = false;
        Bo4->blendMode = OBJMODE_NORMAL;
        Bo4->isMosaic = false;
        Bo4->colorMode = OBJCOLOR_16;
        Bo4->shape = OBJSHAPE_SQUARE;
        Bo4->isHidden = true;
        Bo4->x = 0;
        Bo4->size = OBJSIZE_64;
        Bo4->gfxIndex = nextAvailableTileIdxS;
        Bo4->priority = OBJPRIORITY_2;
        Bo4->palette = palcntS;
        Bo4->vFlip = true;
        Bo4->hFlip = true;

        Bo4 = &p_oam->oamBuffer[ ++oamIndexS ];
        Bo4->y = 128;
        Bo4->isRotateScale = false;
        Bo4->blendMode = OBJMODE_NORMAL;
        Bo4->isMosaic = false;
        Bo4->colorMode = OBJCOLOR_16;
        Bo4->shape = OBJSHAPE_SQUARE;
        Bo4->isHidden = true;
        Bo4->x = 192;
        Bo4->size = OBJSIZE_64;
        Bo4->gfxIndex = nextAvailableTileIdxS;
        Bo4->priority = OBJPRIORITY_2;
        Bo4->palette = palcntS;
        Bo4->vFlip = false;
        Bo4->hFlip = false;

        memset( &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], 0, Border_4TilesLen );
        //dmaCopyHalfWords(SPRITE_DMA_CHANNEL, Border_4Pal, &SPRITE_PALETTE_SUB[palcntS * COLORS_PER_PALETTE], 32);
        //dmaCopyHalfWords(SPRITE_DMA_CHANNEL, Border_4Tiles, &SPRITE_GFX_SUB[nextAvailableTileIdxS * OFFSET_MULTIPLIER], Border_4TilesLen);
        nextAvailableTileIdxS += Border_4TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo3Info = &p_spriteInfo[ ++oamIndexS ];
        SpriteEntry * Bo3 = &p_oam->oamBuffer[ oamIndexS ];
        Bo3Info->m_oamId = oamIndexS;
        Bo3Info->m_width = 64;
        Bo3Info->m_height = 64;
        Bo3Info->m_angle = 0;
        Bo3Info->m_entry = Bo3;
        Bo3->y = 0;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = true;
        Bo3->x = 64;
        Bo3->size = OBJSIZE_64;
        Bo3->gfxIndex = nextAvailableTileIdxS;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = palcntS;
        Bo3->vFlip = true;
        Bo3->hFlip = true;

        Bo3 = &p_oam->oamBuffer[ ++oamIndexS ];
        Bo3->y = 128;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = true;
        Bo3->x = 128;
        Bo3->size = OBJSIZE_64;
        Bo3->gfxIndex = nextAvailableTileIdxS;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = palcntS;
        Bo3->vFlip = false;
        Bo3->hFlip = false;

        memset( &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], 0, Border_3TilesLen );
        /*dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_3Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdxS * OFFSET_MULTIPLIER],
        Border_3TilesLen);*/

        nextAvailableTileIdxS += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo2Info = &p_spriteInfo[ ++oamIndexS ];
        SpriteEntry * Bo2 = &p_oam->oamBuffer[ oamIndexS ];
        Bo2Info->m_oamId = oamIndexS;
        Bo2Info->m_width = 64;
        Bo2Info->m_height = 64;
        Bo2Info->m_angle = 0;
        Bo2Info->m_entry = Bo2;
        Bo2->y = 0;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = true;
        Bo2->x = 128;
        Bo2->size = OBJSIZE_64;
        Bo2->gfxIndex = nextAvailableTileIdxS;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = palcntS;
        Bo2->vFlip = true;
        Bo2->hFlip = true;

        Bo2 = &p_oam->oamBuffer[ ++oamIndexS ];
        Bo2->y = 128;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = true;
        Bo2->x = 64;
        Bo2->size = OBJSIZE_64;
        Bo2->gfxIndex = nextAvailableTileIdxS;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = palcntS;
        Bo2->vFlip = false;
        Bo2->hFlip = false;

        memset( &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], 0, Border_2TilesLen );
        /*dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_2Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdxS * OFFSET_MULTIPLIER],
        Border_2TilesLen);*/
        nextAvailableTileIdxS += Border_2TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo1Info = &p_spriteInfo[ ++oamIndexS ];
        SpriteEntry * Bo1 = &p_oam->oamBuffer[ oamIndexS ];
        Bo1Info->m_oamId = oamIndexS;
        Bo1Info->m_width = 64;
        Bo1Info->m_height = 64;
        Bo1Info->m_angle = 0;
        Bo1Info->m_entry = Bo1;
        Bo1->y = 0;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = true;
        Bo1->x = 192;
        Bo1->size = OBJSIZE_64;
        Bo1->gfxIndex = nextAvailableTileIdxS;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = palcntS;
        Bo1->vFlip = true;
        Bo1->hFlip = true;

        Bo1 = &p_oam->oamBuffer[ ++oamIndexS ];
        Bo1->y = 128;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = true;
        Bo1->x = 0;
        Bo1->size = OBJSIZE_64;
        Bo1->gfxIndex = nextAvailableTileIdxS;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = palcntS;
        Bo1->vFlip = false;
        Bo1->hFlip = false;

        memset( &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], 0, Border_1TilesLen );
        /*dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_1Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdxS * OFFSET_MULTIPLIER],
        Border_1TilesLen);*/
        nextAvailableTileIdxS += Border_1TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo5Info = &p_spriteInfo[ ++oamIndexS ];
        SpriteEntry * Bo5 = &p_oam->oamBuffer[ oamIndexS ];
        Bo5Info->m_oamId = oamIndexS;
        Bo5Info->m_width = 64;
        Bo5Info->m_height = 64;
        Bo5Info->m_angle = 0;
        Bo5Info->m_entry = Bo5;
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = true;
        Bo5->x = 0;
        Bo5->size = OBJSIZE_64;
        Bo5->gfxIndex = nextAvailableTileIdxS;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = palcntS;
        Bo5->vFlip = true;
        Bo5->hFlip = true;

        Bo5 = &p_oam->oamBuffer[ ++oamIndexS ];
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = false;
        Bo5->x = 192;
        Bo5->size = OBJSIZE_64;
        Bo5->gfxIndex = nextAvailableTileIdxS;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = palcntS;
        Bo5->vFlip = false;
        Bo5->hFlip = false;

        memset( &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], 0, Border_5TilesLen );
        //dmaCopyHalfWords(SPRITE_DMA_CHANNEL, Border_5Tiles, &SPRITE_GFX_SUB[nextAvailableTileIdxS * OFFSET_MULTIPLIER], Border_5TilesLen);
        nextAvailableTileIdxS += Border_5TilesLen / BYTES_PER_16_COLOR_TILE;

        ++palcntS;

        Bo5 = &p_oam->oamBuffer[ ++oamIndexS ];
        Bo5->y = 192;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = true;
        Bo5->x = 256;
        Bo5->size = OBJSIZE_64;
        Bo5->gfxIndex = nextAvailableTileIdxS;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = palcntS;
        Bo5->vFlip = false;
        Bo5->hFlip = false;
        SpriteInfo * backInfo = &p_spriteInfo[ ++oamIndexS ];
        SpriteEntry * back = &p_oam->oamBuffer[ oamIndexS ];
        backInfo->m_oamId = oamIndexS;
        backInfo->m_width = 32;
        backInfo->m_height = 32;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = SCREEN_HEIGHT - 28;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = true;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = SCREEN_WIDTH - 28;
        back->size = OBJSIZE_32;
        back->gfxIndex = nextAvailableTileIdxS;
        back->priority = OBJPRIORITY_1;
        back->palette = palcntS;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BackTiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BackTilesLen );
        nextAvailableTileIdxS += BackTilesLen / BYTES_PER_16_COLOR_TILE;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BackPal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );

        ++oamIndexS;
        int nextnext = nextAvailableTileIdxS + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
        for( int i = 0; i < 6; ++i ) {
            SpriteInfo * C1Info = &p_spriteInfo[ 2 * i + oamIndexS ];
            SpriteEntry * C1 = &p_oam->oamBuffer[ 2 * i + oamIndexS ];
            C1Info->m_oamId = oamIndexS;
            C1Info->m_width = 64;
            C1Info->m_height = 32;
            C1Info->m_angle = 0;
            C1Info->m_entry = C1;
            C1->y = 68 + ( i / 2 ) * 32;
            C1->isRotateScale = false;
            C1->blendMode = OBJMODE_NORMAL;
            C1->isMosaic = false;
            C1->colorMode = OBJCOLOR_16;
            C1->shape = OBJSHAPE_WIDE;
            C1->isHidden = true;
            C1->x = ( ( i % 2 ) ? 32 : 128 );
            C1->size = OBJSIZE_64;
            C1->gfxIndex = nextAvailableTileIdxS;

            C1->priority = OBJPRIORITY_2;
            C1->palette = palcntS;

            SpriteInfo * C3Info = &p_spriteInfo[ 2 * i + oamIndexS + 1 ];
            SpriteEntry * C3 = &p_oam->oamBuffer[ 2 * i + oamIndexS + 1 ];
            C3Info->m_oamId = oamIndexS;
            C3Info->m_width = 64;
            C3Info->m_height = 32;
            C3Info->m_angle = 0;
            C3Info->m_entry = C3;
            C3->y = 68 + ( i / 2 ) * 32;
            C3->isRotateScale = false;
            C3->blendMode = OBJMODE_NORMAL;
            C3->isMosaic = false;
            C3->colorMode = OBJCOLOR_16;
            C3->shape = OBJSHAPE_WIDE;
            C3->isHidden = true;
            C3->x = ( ( i % 2 ) ? 62 : 160 );
            C3->size = OBJSIZE_64;
            C3->gfxIndex = nextnext;
            C3->priority = OBJPRIORITY_2;
            C3->palette = palcntS;
        }
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_1Pal, &SPRITE_PALETTE_SUB[ palcntS * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_1Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], Choice_1TilesLen );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_3Tiles, &SPRITE_GFX_SUB[ nextnext * OFFSET_MULTIPLIER ], Choice_3TilesLen );
        nextAvailableTileIdxS = nextnext + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;

        for( int i = 0; i < 3; ++i ) {
            SpriteInfo * C2Info = &p_spriteInfo[ i + oamIndexS + 12 ];
            SpriteEntry * C2 = &p_oam->oamBuffer[ i + oamIndexS + 12 ];
            C2Info->m_oamId = oamIndexS;
            C2Info->m_width = 64;
            C2Info->m_height = 32;
            C2Info->m_angle = 0;
            C2Info->m_entry = C2;
            C2->y = 68 + (i)* 32;
            C2->isRotateScale = false;
            C2->blendMode = OBJMODE_NORMAL;
            C2->isMosaic = false;
            C2->colorMode = OBJCOLOR_16;
            C2->shape = OBJSHAPE_WIDE;
            C2->isHidden = true;
            C2->x = 96;
            C2->size = OBJSIZE_64;
            C2->gfxIndex = nextAvailableTileIdxS;
            C2->priority = OBJPRIORITY_2;
            C2->palette = palcntS;
        }
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_2Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], Choice_2TilesLen );
        nextAvailableTileIdxS += Choice_2TilesLen / BYTES_PER_16_COLOR_TILE;
        ++palcntS;
        oamIndexS += 15;
    }

#define PB_ANIM_TILES 700
    void animatePB( int p_x, int p_y ) {
        p_x += 8; p_y += 8;
        SpriteInfo * type1Info = &spriteInfoTop[ PB_ANIM ];
        SpriteEntry * type1 = &OamTop->oamBuffer[ PB_ANIM ];
        type1Info->m_oamId = PB_ANIM;
        type1Info->m_width = 16;
        type1Info->m_height = 16;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = p_y;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = p_x;
        type1->size = OBJSIZE_16;
        type1->gfxIndex = PB_ANIM_TILES;
        type1->priority = OBJPRIORITY_0;
        type1->palette = 15;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall3Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall3TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall4Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall4TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall5Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall5TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall6Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall6TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall7Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall7TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall8Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall8TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall9Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall9TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall10Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall10TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall11Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall11TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 3; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;

        type1Info->m_oamId = PB_ANIM;
        type1Info->m_width = 64;
        type1Info->m_height = 64;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = p_y - 22;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = p_x - 22;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = PB_ANIM_TILES;
        type1->priority = OBJPRIORITY_0;
        type1->palette = 15;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;
        updateOAM( OamTop );
    }

    void animateShiny( int p_x, int p_y ) {
        SpriteInfo * type1Info = &spriteInfoTop[ SHINY_ANIM ];
        SpriteEntry * type1 = &OamTop->oamBuffer[ SHINY_ANIM ];
        type1Info->m_oamId = SHINY_ANIM;
        type1Info->m_width = 64;
        type1Info->m_height = 64;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = p_y;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = p_x;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = PB_ANIM_TILES;
        type1->priority = OBJPRIORITY_0;
        type1->palette = 15;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;
        updateOAM( OamTop );
    }

#define GENDER(a) (a.m_boxdata.m_isFemale? 147 : (a.m_boxdata.m_isGenderless ? ' ' : 141))
    //
    //    void battle::switchOppPkmn( int p_newPok, int p_toSwitch ) {
    //        if( ACPKMN( p_newPok, OPPONENT ).m_stats.m_acHP == 0 )
    //            return;
    //
    //        init( );
    //
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //        if( ACPKMN( p_toSwitch, OPPONENT ).m_stats.m_acHP ) {
    //            clear( );
    //            sprintf( buffer, "%ls wird von\n%s %s\nauf die Bank geschickt. ", ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_Name,
    //                     trainerclassnames[ _opponent->m_trainerClass ], _opponent->m_battleTrainerName );
    //            cust_font.printString( buffer, 8, 8, true );
    //        }
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ); i < 6; ++i )
    //            if( i == p_newPok || i == p_toSwitch ) {
    //            switch( ACPKMNSTS( i, OPPONENT ) ) {
    //                case KO:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette--;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette -= 2;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //            }
    //
    //        std::swap( ACPOS( p_newPok, OPPONENT ), ACPOS( p_toSwitch, OPPONENT ) );
    //
    //        for( int i = 0; i < 150; ++i )
    //            swiWaitForVBlank( );
    //
    //        clear( );
    //        sprintf( buffer, "%s %s\nschickt %ls in den Kampf.",
    //                 trainerclassnames[ _opponent->m_trainerClass ],
    //                 _opponent->m_battleTrainerName,
    //                 ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //
    //        consoleSelect( &Top );
    //
    //        if( p_toSwitch == 0 ) {
    //            oamIndex = OPP_PKMN_1_START;
    //            palcnt = OPP_PKMN_1_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //
    //            OamTop->oamBuffer[ OPP_PB_START ].isHidden = true;
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_1_START + i ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 206, 50 );
    //
    //            OamTop->oamBuffer[ OPP_PB_START ].isHidden = false;
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop,
    //                "nitro:/PICS/SPRITES/PKMN/",
    //                ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //                176,
    //                20,
    //                oamIndex,
    //                palcnt,
    //                nextAvailableTileIdx,
    //                false,
    //                ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //                ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //                oamIndex = OPP_PKMN_1_START;
    //                palcnt = OPP_PKMN_1_PAL;
    //                nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //                loadPKMNSprite( OamTop,
    //                                spriteInfoTop,
    //                                "nitro:/PICS/SPRITES/PKMN/",
    //                                ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //                                176,
    //                                20,
    //                                oamIndex,
    //                                palcnt,
    //                                nextAvailableTileIdx,
    //                                false,
    //                                ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //                                !ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale );
    //            }
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_1_START + i ].isHidden = false;
    //            updateOAM( OamTop );
    //
    //            if( ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ) )
    //                animateShiny( 176 + 16, 36 );
    //
    //            displayHP( 100, 101, 88, 32, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP,
    //                       88, 32, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            OamTop->oamBuffer[ OPP_PB_START ].x = 96;
    //            OamTop->oamBuffer[ OPP_PB_START ].y = 41;
    //            consoleSetWindow( &Top, 0, 5, 20, 2 );
    //            consoleClear( );
    //
    //            printf( "%10ls%c\n",
    //                    ACPKMN( 0, OPPONENT ).m_boxdata.m_Name,
    //                    GENDER( ( *_player->m_pkmnTeam )[ ACPOS( 0, OPPONENT ) ] ) );
    //
    //            if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //                printf( " " );
    //            if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //                printf( " " );
    //            printf( "Lv%d%4dKP", ACPKMN( 0, OPPONENT ).m_Level,
    //                    ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //        } else {
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //            consoleSelect( &Top );
    //            oamIndex = OPP_PKMN_2_START;
    //            palcnt = OPP_PKMN_2_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_2_START + i ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 142, 34 );
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = false;
    //
    //            if( !loadPKMNSprite( OamTop,
    //                spriteInfoTop,
    //                "nitro:/PICS/SPRITES/PKMN/",
    //                ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC,
    //                112,
    //                4,
    //                oamIndex,
    //                palcnt,
    //                nextAvailableTileIdx,
    //                false,
    //                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ),
    //                ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //                oamIndex = OPP_PKMN_2_START;
    //                palcnt = OPP_PKMN_2_PAL;
    //                nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //                loadPKMNSprite( OamTop,
    //                                spriteInfoTop,
    //                                "nitro:/PICS/SPRITES/PKMN/",
    //                                ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC,
    //                                112,
    //                                4,
    //                                oamIndex,
    //                                palcnt,
    //                                nextAvailableTileIdx,
    //                                false,
    //                                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ),
    //                                !ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale );
    //            }
    //
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_2_START + i ].isHidden = false;
    //            updateOAM( OamTop );
    //
    //            if( ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ) )
    //                animateShiny( 112 + 16, 20 );
    //
    //            updateOAM( OamTop );
    //
    //            displayHP( 100, 101, 0, 8, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP, 0, 8, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].x = 8;
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].y = 17;
    //            consoleSetWindow( &Top, 4, 2, 20, 2 );
    //            consoleClear( );
    //            printf( "%ls%c\nLv%d%4dKP",
    //                    ACPKMN( 1, OPPONENT ).m_boxdata.m_Name,
    //                    GENDER( ACPKMN( 1, OPPONENT ) ),
    //                    ACPKMN( 1, OPPONENT ).m_Level,
    //                    ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //        }
    //
    //        consoleSelect( &Bottom );
    //
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ); i < 6; ++i )
    //            if( i == p_newPok || i == p_toSwitch ) {
    //            OamTop->oamBuffer[ OPP_PB_START + i ].x = -4 + 18 * i;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].y = -4;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, OPPONENT ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette++;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette += 2;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //            }
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 200; ++i )
    //            swiWaitForVBlank( );
    //
    //        if( abilities[ ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls (Gegn.) wirkt!\n",
    //                     abilities[ ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //    }
    //
    //    void battle::switchOwnPkmn( int p_newPok, int p_toSwitch ) {
    //        if( ACPKMN( p_newPok, PLAYER ).m_stats.m_acHP == 0 )
    //            return;
    //        init( );
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ); i < 6; ++i ) {
    //            if( i == p_newPok || i == p_toSwitch ) {
    //                OamTop->oamBuffer[ OWN_PB_START + i ].x = 236 - 18 * i;
    //                OamTop->oamBuffer[ OWN_PB_START + i ].y = 196 - 16;
    //                OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = false;
    //                switch( ACPKMNSTS( i, PLAYER ) ) {
    //                    case KO:
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].palette--;
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 32;
    //                        break;
    //                    case STS:
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].palette -= 2;
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 16;
    //                        break;
    //                    default:
    //                        break;
    //                }
    //            }
    //        }
    //        updateOAM( OamTop );
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //        if( ACPKMN( p_toSwitch, PLAYER ).m_stats.m_acHP ) {
    //            clear( );
    //            sprintf( buffer, "Auf die Bank,\n %ls!", ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //        }
    //        std::swap( ACPOS( p_newPok, PLAYER ), ACPOS( p_toSwitch, PLAYER ) );
    //
    //        for( int i = 0; i < 100; ++i )
    //            swiWaitForVBlank( );
    //
    //
    //        consoleSelect( &Bottom );
    //        clear( );
    //        sprintf( buffer, "Los %ls!", ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //
    //        consoleSelect( &Top );
    //        if( p_toSwitch == 0 ) {
    //            oamIndex = OWN_PKMN_1_START;
    //            palcnt = OWN_PKMN_1_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //
    //            OamTop->oamBuffer[ OWN_PB_START ].isHidden = true;
    //            for( int i = 0; i < 4; ++i )
    //                OamTop->oamBuffer[ OWN_PKMN_1_START + i + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 80, 170 );
    //
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale ) )
    //                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale );
    //
    //            OamTop->oamBuffer[ OWN_PB_START ].isHidden = false;
    //
    //            updateOAM( OamTop );
    //
    //            if( ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ) )
    //                animateShiny( 6, 116 );
    //
    //            displayHP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP,
    //                       256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayEP( 100, 100, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //
    //            POKEMON::PKMNDATA::getAll( ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //            displayEP( 0, ( ACPKMN( 0, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                       ( POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                       256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //            OamTop->oamBuffer[ OWN_PB_START ].x = 256 - 88 - 32 + 4;
    //            OamTop->oamBuffer[ OWN_PB_START ].y = 192 - 31 - 32;
    //            consoleSetWindow( &Top, 21, 16, 20, 4 );
    //            consoleClear( );
    //            printf( "%ls%c\nLv%d%4dKP",
    //                    ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                    GENDER( ACPKMN( 0, PLAYER ) ),
    //                    ACPKMN( 0, PLAYER ).m_Level,
    //                    ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //
    //            //OamTop->oamBuffer[OWN_PB_START + 1].isHidden = OamTop->oamBuffer[2].isHidden = false;
    //            updateOAM( OamTop );
    //        } else {
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //            consoleSelect( &Top );
    //            oamIndex = OWN_PKMN_2_START;
    //            palcnt = OWN_PKMN_2_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //
    //            for( int i = 0; i < 4; ++i ) //hide pokemon sprite
    //                OamTop->oamBuffer[ OWN_PKMN_2_START + i + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 142, 34 );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].isHidden = false;
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale ) )
    //                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale );
    //
    //            if( ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ) )
    //                animateShiny( 50 + 16, 136 );
    //
    //            updateOAM( OamTop );
    //
    //            displayHP( 100, 101, 256 - 36, 192 - 40, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                       256 - 36, 192 - 40, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayEP( 100, 100, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //            POKEMON::PKMNDATA::getAll( ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //            displayEP( 0,
    //                       ( ACPKMN( 1, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                       ( POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                       256 - 36,
    //                       192 - 40,
    //                       OWN2_EP_COL,
    //                       OWN2_EP_COL,
    //                       false );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].x = 256 - 32 + 4;
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].y = 192 - 31;
    //            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //            consoleClear( );
    //            printf( "%10ls%c\n",
    //                    ACPKMN( 1, PLAYER ).m_boxdata.m_Name,
    //                    GENDER( ACPKMN( 1, PLAYER ) ) );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                printf( " " );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                printf( " " );
    //            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //            //OamTop->oamBuffer[5].isHidden = OamTop->oamBuffer[1].isHidden = false;
    //            updateOAM( OamTop );
    //        }
    //
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ); i < 6; ++i )
    //            if( i == p_newPok || i == p_toSwitch ) {
    //            OamTop->oamBuffer[ OWN_PB_START + i ].x = 236 - 18 * i;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].y = 196 - 16;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, PLAYER ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette++;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette += 2;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //            }
    //
    //        for( int i = 0; i < 100; ++i )
    //            swiWaitForVBlank( );
    //        updateOAM( OamTop );
    //
    //        if( abilities[ ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls wirkt!\n",
    //                     abilities[ ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //    }
    //
    //    void setMainBattleVisibility( bool p_hidden ) {
    //        for( int i = 0; i <= 4; ++i )
    //            Oam->oamBuffer[ i ].isHidden = p_hidden;
    //        Oam->oamBuffer[ 2 ].isHidden |= !p_hidden & !SAV.m_activatedPNav;
    //        for( int i = 19; i <= 19; ++i )
    //            Oam->oamBuffer[ i ].isHidden = p_hidden;
    //        updateOAMSub( Oam );
    //    }
    //
    //
    //    void drawTopBack( ) {
    //        dmaCopy( TestBattleBackBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
    //        dmaCopy( TestBattleBackPal, BG_PALETTE, 128 * 2 );
    //    }
    //


    //    void battle::initBattleScene( int p_battleBack, weather p_weather ) {
    //        for( size_t i = 0; i < 6; ++i ) {
    //            ACPOS( i, PLAYER ) = ACPOS( i, OPPONENT ) = i;
    //            if( _player->m_pkmnTeam->size( ) > i ) {
    //                if( ACPKMN( i, PLAYER ).m_stats.m_acHP == 0 || ACPKMN( i, PLAYER ).m_boxdata.m_IV.m_isEgg )
    //                    ACPKMNSTS( i, PLAYER ) = KO;
    //                else if( ACPKMN( i, PLAYER ).m_statusint )
    //                    ACPKMNSTS( i, PLAYER ) = STS;
    //                else
    //                    ACPKMNSTS( i, PLAYER ) = OK;
    //            } else
    //                ACPKMNSTS( i, PLAYER ) = NA;
    //            if( _opponent->m_pkmnTeam->size( ) > i ) {
    //                if( ACPKMN( i, OPPONENT ).m_stats.m_acHP == 0 || ACPKMN( i, OPPONENT ).m_boxdata.m_IV.m_isEgg )
    //                    ACPKMNSTS( i, OPPONENT ) = KO;
    //                else if( ACPKMN( i, OPPONENT ).m_statusint )
    //                    ACPKMNSTS( i, OPPONENT ) = STS;
    //                else ACPKMNSTS( i, OPPONENT ) = OK;
    //            } else
    //                ACPKMNSTS( i, OPPONENT ) = NA;
    //        }
    //        if( ACPKMNSTS( 0, PLAYER ) == KO ) {
    //            for( size_t i = 1 + ( m_battleMode == DOUBLE ); i < _player->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                    std::swap( ACPOS( 0, PLAYER ), ACPOS( i, PLAYER ) );
    //                    break;
    //                }
    //            }
    //        }
    //        if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, PLAYER ) == KO ) {
    //            for( size_t i = 2; i < _player->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                    std::swap( ACPOS( 1, PLAYER ), ACPOS( i, PLAYER ) );
    //                    break;
    //                }
    //            }
    //        }
    //        if( ACPKMNSTS( 0, OPPONENT ) == KO ) {
    //            for( size_t i = 1 + ( m_battleMode == DOUBLE ); i < _opponent->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                    std::swap( ACPOS( 0, OPPONENT ), ACPOS( i, OPPONENT ) );
    //                    break;
    //                }
    //            }
    //        }
    //        if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, OPPONENT ) == KO ) {
    //            for( size_t i = 2; i < _opponent->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                    std::swap( ACPOS( 1, OPPONENT ), ACPOS( i, OPPONENT ) );
    //                    break;
    //                }
    //            }
    //        }
    //        vramSetup( );
    //        videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    //        drawTopBack( );
    //        Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    //        consoleSetFont( &Top, &cfont );
    //
    //        Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    //        consoleSetFont( &Bottom, &cfont );
    //
    //        //touchPosition t;
    //
    //        initOAMTableSub( Oam );
    //        initOAMTable( OamTop );
    //        _round = _maxRounds;
    //        initBattleScreen( );
    //        drawTopBack( );
    //
    //        drawSub( );
    //
    //        drawTopBack( );
    //
    //        initOAMTableSub( Oam );
    //        initOAMTable( OamTop );
    //        initBattleScreenSprites( OamTop, spriteInfoTop );
    //
    //        initBattleSubScreenSprites( Oam, spriteInfo, false, SAV.m_activatedPNav );
    //        for( int i = 5; i <= 19; ++i )
    //            Oam->oamBuffer[ i ].isHidden = false;
    //        updateOAMSub( Oam );
    //
    //        for( int i = 1; i <= 4; ++i )
    //            OamTop->oamBuffer[ i ].isHidden = true;
    //        updateOAM( OamTop );
    //
    //        init( );
    //        //Opp's Side
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //
    //        if( ( m_battleMode == DOUBLE ) && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO )
    //            sprintf( buffer, "%s %s\nschickt %ls ",
    //            trainerclassnames[ _opponent->m_trainerClass ],
    //            _opponent->m_battleTrainerName,
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //        else
    //            sprintf( buffer, "%s %s\nschickt %ls in den Kampf.",
    //            trainerclassnames[ _opponent->m_trainerClass ],
    //            _opponent->m_battleTrainerName,
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //        consoleSelect( &Top );
    //
    //        animatePB( 206, 50 );
    //
    //        OamTop->oamBuffer[ OPP_HP_1 ].isHidden = OamTop->oamBuffer[ OPP_PB_START ].isHidden = false;
    //        oamIndex = OPP_PKMN_1_START;
    //        palcnt = OPP_PKMN_1_PAL;
    //        nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //
    //        if( !loadPKMNSprite( OamTop,
    //            spriteInfoTop,
    //            "nitro:/PICS/SPRITES/PKMN/",
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //            176,
    //            20,
    //            oamIndex,
    //            palcnt,
    //            nextAvailableTileIdx,
    //            false,
    //            ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //
    //            oamIndex = OPP_PKMN_1_START;
    //            palcnt = OPP_PKMN_1_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //            loadPKMNSprite( OamTop,
    //                            spriteInfoTop,
    //                            "nitro:/PICS/SPRITES/PKMN/",
    //                            ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //                            176,
    //                            20,
    //                            oamIndex,
    //                            palcnt,
    //                            nextAvailableTileIdx,
    //                            false,
    //                            ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //                            !ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale );
    //        }
    //        if( ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ) )
    //            animateShiny( 176 + 16, 36 );
    //
    //        displayHP( 100, 101, 88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, false );
    //        displayHP( 100, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP, 88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, false );
    //        OamTop->oamBuffer[ OPP_PB_START ].x = 96;
    //        OamTop->oamBuffer[ OPP_PB_START ].y = 41;
    //        consoleSetWindow( &Top, 0, 5, 20, 5 );
    //
    //        printf( "%10ls%c\n", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name, GENDER( ( *_player->m_pkmnTeam )[ ACPOS( 0, OPPONENT ) ] ) );
    //        if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //            printf( " " );
    //        if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //            printf( " " );
    //        printf( "Lv%d%4dKP", ACPKMN( 0, OPPONENT ).m_Level,
    //                ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //
    //        if( ( m_battleMode == DOUBLE ) && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ) {
    //            OamTop->oamBuffer[ OPP_HP_2 ].isHidden = OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //            for( int i = 0; i < 80; ++i )
    //                swiWaitForVBlank( );
    //            consoleSelect( &Bottom );
    //            clear( );
    //            sprintf( buffer, "und %ls in den Kampf.", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            consoleSelect( &Top );
    //
    //            animatePB( 142, 34 );
    //            OamTop->oamBuffer[ OPP_HP_2 ].isHidden = OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = false;
    //            oamIndex = OPP_PKMN_2_START;
    //            palcnt = OPP_PKMN_2_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC, 112, 4, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ), ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //                oamIndex = OPP_PKMN_2_START;
    //                palcnt = OPP_PKMN_2_PAL;
    //                nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC, 112, 4, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ), !ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale );
    //            }
    //            if( ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ) )
    //                animateShiny( 112 + 16, 20 );
    //
    //
    //            displayHP( 100, 101, 0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP, 0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, false );
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].x = 8;
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].y = 17;
    //            consoleSetWindow( &Top, 4, 2, 20, 5 );
    //            printf( "%ls%c\nLv%d%4dKP", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name, GENDER( ( *_player->m_pkmnTeam )[ ACPOS( 1, OPPONENT ) ] ),
    //                    ACPKMN( 1, OPPONENT ).m_Level, ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //        }
    //
    //        consoleSelect( &Bottom );
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ); i < 6; ++i ) {
    //            OamTop->oamBuffer[ OPP_PB_START + i ].x = -4 + 18 * i;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].y = -4;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, OPPONENT ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette = PB_PAL_START + 1;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette = PB_PAL_START + 2;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //        }
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 80; ++i )
    //            swiWaitForVBlank( );
    //
    //        if( abilities[ ACPKMN( 0, OPPONENT ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls (Gegn.) wirkt!\n",
    //                     abilities[ ACPKMN( 0, OPPONENT ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //        if( ( m_battleMode == DOUBLE ) && abilities[ ACPKMN( 1, OPPONENT ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls (Gegn.) wirkt!\n",
    //                     abilities[ ACPKMN( 1, OPPONENT ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 1, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //
    //
    //        //Own Side
    //        consoleSelect( &Bottom );
    //        clear( );
    //        sprintf( buffer, "Los %ls! ", ACPKMN( 0, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //        consoleSelect( &Top );
    //
    //        animatePB( 20, 150 );
    //        oamIndex = OWN_PKMN_1_START;
    //        palcnt = OWN_PKMN_1_PAL;
    //        nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //        if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //            ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale ) ) {
    //            oamIndex = OWN_PKMN_1_START;
    //            palcnt = OWN_PKMN_1_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //            loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                            ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale );
    //        }
    //        if( ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ) )
    //            animateShiny( 6, 116 );
    //
    //        displayHP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //        displayHP( 100, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP,
    //                   256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //        displayEP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //
    //        POKEMON::PKMNDATA::getAll( ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, p );
    //        displayEP( 0, ( ACPKMN( 0, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                   ( POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                   256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //        OamTop->oamBuffer[ OWN_PB_START ].x = 256 - 88 - 32 + 4;
    //        OamTop->oamBuffer[ OWN_PB_START ].y = 192 - 31 - 32;
    //        consoleSetWindow( &Top, 21, 16, 20, 5 );
    //
    //        printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //
    //
    //        OamTop->oamBuffer[ OWN_PB_START ].isHidden = OamTop->oamBuffer[ OWN_HP_1 ].isHidden = false;
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 80; ++i )
    //            swiWaitForVBlank( );
    //
    //        if( ( m_battleMode == DOUBLE ) && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ) {
    //            consoleSelect( &Bottom );
    //            clear( );
    //            sprintf( buffer, "Auf in den Kampf %ls! ", ACPKMN( 1, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            consoleSelect( &Top );
    //
    //            animatePB( 80, 170 );
    //
    //            oamIndex = OWN_PKMN_2_START;
    //            palcnt = OWN_PKMN_2_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //
    //            if( !loadPKMNSprite( OamTop,
    //                spriteInfoTop,
    //                "nitro:/PICS/SPRITES/PKMNBACK/",
    //                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC,
    //                50,
    //                120,
    //                oamIndex,
    //                palcnt,
    //                nextAvailableTileIdx,
    //                false,
    //                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ),
    //                ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale ) ) {
    //
    //                oamIndex = OWN_PKMN_2_START;
    //                palcnt = OWN_PKMN_2_PAL;
    //                nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //                loadPKMNSprite( OamTop,
    //                                spriteInfoTop,
    //                                "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC,
    //                                50,
    //                                120,
    //                                oamIndex,
    //                                palcnt,
    //                                nextAvailableTileIdx,
    //                                false,
    //                                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ),
    //                                !ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale );
    //            }
    //            if( ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ) )
    //                animateShiny( 50 + 16, 136 );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].isHidden = OamTop->oamBuffer[ OWN_HP_2 ].isHidden = false;
    //            updateOAM( OamTop );
    //
    //            displayHP( 100, 101, 256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                       256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //            displayEP( 100, 100, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //            POKEMON::PKMNDATA::getAll( ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, p );
    //            displayEP( 0, ( ACPKMN( 1, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                       ( POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                       256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].x = 256 - 32 + 4;
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].y = 192 - 31;
    //            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //
    //            printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                printf( " " );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                printf( " " );
    //            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //        }
    //
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ); i < 6; ++i ) {
    //            OamTop->oamBuffer[ OWN_PB_START + i ].x = 236 - 18 * i;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].y = 196 - 16;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, PLAYER ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette = PB_PAL_START + 1;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette = PB_PAL_START + 2;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //        }
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 80; ++i )
    //            swiWaitForVBlank( );
    //
    //        consoleSelect( &Bottom );
    //        if( abilities[ ACPKMN( 0, PLAYER ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls wirkt!\n",
    //                     abilities[ ACPKMN( 0, PLAYER ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 0, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //        if( ( m_battleMode == DOUBLE ) && abilities[ ACPKMN( 1, PLAYER ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls wirkt!\n",
    //                     abilities[ ACPKMN( 1, PLAYER ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 1, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //
    //        //OamTop->oamBuffer[4].isHidden = OamTop->oamBuffer[12].isHidden = false;
    //
    //        updateOAM( OamTop );
    //
    //        for( int i = 0; i <= 4; ++i )
    //            Oam->oamBuffer[ i ].isHidden = false;
    //        Oam->oamBuffer[ 2 ].isHidden = !SAV.m_activatedPNav;
    //        updateOAMSub( Oam );
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //    }
    //
    //    float criticalChances[ 5 ] = { 0.0625, 0.125, 0.25, 0.3333, 0.5 };
    //    bool criticalOccured = false;
    //    float eff = 1;
    //    bool missed = false;
    //    int calcDamage( const move& p_move, const POKEMON::pokemon& p_attackingPkmn, const POKEMON::pokemon& p_defendingPkmn, int p_randomValue ) {
    //        if( p_move.m_moveHitType == move::moveHitTypes::STAT )
    //            return 0;
    //        eff = 1;
    //        missed = false;
    //        if( p_move.m_moveAccuracy && rand( ) * 1.0 / RAND_MAX > p_move.m_moveAccuracy / 100.0 ) {
    //            missed = true;
    //            return 0;
    //        }
    //
    //        if( p_defendingPkmn.m_stats.m_acHP == 0 ) {
    //            missed = true;
    //            return 0;
    //        }
    //
    //        int atkval = ( p_move.m_moveHitType == move::moveHitTypes::SPEC ? p_attackingPkmn.m_stats.m_SAtk : p_attackingPkmn.m_stats.m_Atk );
    //        int defval = ( p_move.m_moveHitType == move::moveHitTypes::SPEC ? p_defendingPkmn.m_stats.m_SDef : p_defendingPkmn.m_stats.m_Def );
    //
    //        int baseDmg = ( ( ( ( 2 * p_attackingPkmn.m_Level ) / 5 + 2 ) * p_move.m_moveBasePower * atkval ) / defval ) / 50 + 2;
    //
    //        POKEMON::PKMNDATA::pokemonData p1, p2;
    //        POKEMON::PKMNDATA::getAll( p_attackingPkmn.m_boxdata.m_speciesId, p1 );
    //        POKEMON::PKMNDATA::getAll( p_defendingPkmn.m_boxdata.m_speciesId, p2 );
    //
    //        int vs = 1;
    //        criticalOccured = false;
    //
    //        if( p_randomValue <= 15 )
    //            if( p_randomValue >= 0 && rand( ) * 1.0 / RAND_MAX <= criticalChances[ vs ] ) {
    //            baseDmg <<= 1;
    //            criticalOccured = true;
    //            }
    //
    //        if( p_randomValue == -1 )
    //            p_randomValue = 0;
    //        if( p_randomValue == -2 )
    //            p_randomValue = 15;
    //
    //        baseDmg = ( baseDmg * ( 100 - p_randomValue ) ) / 100;
    //
    //        if( p_move.m_moveType == p1.m_types[ 0 ] || p_move.m_moveType == p1.m_types[ 1 ] )
    //            baseDmg = ( baseDmg * 3 ) / 2;
    //
    //        eff = getEffectiveness( p_move.m_moveType, p2.m_types[ 0 ] );
    //        if( p2.m_types[ 1 ] != p2.m_types[ 0 ] )
    //            eff *= getEffectiveness( p_move.m_moveType, p2.m_types[ 1 ] );
    //        baseDmg = baseDmg * eff;
    //
    //        return std::max( 1, baseDmg );
    //    }
    //
    //    std::pair<int, int> ownAtk[ 2 ]; //AtkIdx; Target 1->opp[0]/2->opp[1]/3->both_opp/4->self/8->partner
    //    std::pair<int, int> oppAtk[ 2 ]; //AtkIdx; Target 1->own[0]/2->own[1]/3->both_opp/4->self/8->partner
    //    int switchWith[ 2 ][ 2 ] = { { 0 } };
    //
    //    void battle::printAttackChoiceScreen( int p_pkmnSlot, int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex ) {
    //        for( int i = 21; i < 29; i += 2 ) {
    //            if( !( ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ) )
    //                continue;
    //
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).y += 14 + 16 * ( ( i - 21 ) / 4 );
    //            ( Oam->oamBuffer[ i ] ).y += 14 + 16 * ( ( i - 21 ) / 4 );
    //            if( ( i / 2 ) % 2 )
    //                ( Oam->oamBuffer[ i ] ).x -= 16;
    //            else
    //                ( Oam->oamBuffer[ i + 1 ] ).x += 16;
    //            updateOAMSub( Oam );
    //
    //            consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 17, 5 );
    //            consoleClear( );
    //            drawTypeIcon( Oam, spriteInfo, p_oamIndex, p_paletteIndex, p_tileIndex,
    //                          AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveType,
    //                          ( Oam->oamBuffer[ i ] ).x + 4, ( Oam->oamBuffer[ i ] ).y - 10, true );
    //            printf( "    %s\n    AP %2hhu""/""%2hhu ",
    //                    &( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveName[ 0 ] ),
    //                    ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ ( i - 21 ) / 2 ],
    //                    AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_movePP *
    //                    ( ( 5 + ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.PPupget( ( i - 21 ) / 2 ) ) / 5 ) );
    //            switch( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveHitType ) {
    //                case move::PHYS:
    //                    printf( "PHS" );
    //                    break;
    //                case move::SPEC:
    //                    printf( "SPC" );
    //                    break;
    //                case move::STAT:
    //                    printf( "STS" );
    //                    break;
    //            }
    //            printf( "\n    S " );
    //            if( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveBasePower )
    //                printf( "%3i", AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveBasePower );
    //            else
    //                printf( "---" );
    //            printf( " G " );
    //            if( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveAccuracy )
    //                printf( "%3i", AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveAccuracy );
    //            else
    //                printf( "---" );
    //        }
    //    }
    //
    //    void battle::printTargetChoiceScreen( int p_pkmnSlot, int p_move, int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex ) {
    //        for( int i = 0; i < 5; ++i )
    //            Oam->oamBuffer[ p_oamIndex + i ].isHidden = true;
    //        for( int i = 21; i < 29; i += 2 ) {
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //        }
    //        updateOAMSub( Oam );
    //
    //        int poss = AttackList[ p_move ]->m_moveAffectsWhom;
    //        //Opp1, Opp2, ME, Partner, 
    //        bool validTrg[ 4 ] = {
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 16 || poss == 2 || poss == 4,
    //            poss == 0 || poss == 2 || poss == 32
    //        };
    //        if( !p_pkmnSlot )
    //            std::swap( validTrg[ 2 ], validTrg[ 3 ] );
    //        POKEMON::pokemon& p_attackingPkmn = ACPKMN( p_pkmnSlot, PLAYER );
    //
    //
    //        for( int i = 21; i < 29; i += 2 ) {
    //            int u = ( i - 21 ) / 2;
    //            if( m_battleMode != DOUBLE && ( u == 2 || u == 1 ) ) {
    //                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //                consoleClear( );
    //                continue;
    //            }
    //
    //            POKEMON::pokemon &acPK = ( u / 2 ? ACPKMN( 1 - u % 2, PLAYER ) : ACPKMN( u % 2, OPPONENT ) );
    //
    //            if( acPK.m_stats.m_acHP == 0 ) {
    //                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //                consoleClear( );
    //                continue;
    //            }
    //
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //            updateOAMSub( Oam );
    //
    //            consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //            consoleClear( );
    //
    //
    //            drawPKMNIcon( Oam, spriteInfo, acPK.m_boxdata.m_speciesId, ( Oam->oamBuffer[ i ] ).x - 4, ( Oam->oamBuffer[ i ] ).y - 16, p_oamIndex, p_paletteIndex, p_tileIndex, true );
    //
    //            updateOAMSub( Oam );
    //
    //            printf( "      %ls", acPK.m_boxdata.m_name );
    //            if( validTrg[ u ] ) {
    //                if( AttackList[ p_move ]->m_moveHitType != move::moveHitTypes::STAT )
    //                    printf( "\n      %3d-%2d KP\n        Schaden",
    //                    std::max( 1, calcDamage( *AttackList[ p_move ], p_attackingPkmn, acPK, -2 ) ),
    //                    std::max( 1, calcDamage( *AttackList[ p_move ], p_attackingPkmn, acPK, -1 ) ) );
    //                else
    //                    printf( "\n       Keinen\n        Schaden" );
    //            }
    //        }
    //    }
    //
    //    void battle::printPKMNSwitchScreen( int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex ) {
    //        setMainBattleVisibility( true );
    //        consoleClear( );
    //        dinit( );
    //        for( int i = 0; i <= 8; ++i )
    //            ( Oam->oamBuffer[ i ] ).isHidden = true;
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = false;
    //        updateOAMSub( Oam );
    //
    //        int num = (int)_player->m_pkmnTeam->size( );
    //        consoleSelect( &Bottom );
    //
    //        for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) ) {
    //            if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                break;
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).y -= 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //            ( Oam->oamBuffer[ i ] ).y -= 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //            updateOAMSub( Oam );
    //            consoleSetWindow( &Bottom, ( ( Oam->oamBuffer[ i ] ).x + 6 ) / 8, ( ( Oam->oamBuffer[ i ] ).y + 6 ) / 8, 12, 3 );
    //            if( !ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_IV.m_isEgg ) {
    //                printf( "   %3i/%3i\n ",
    //                        ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_stats.m_acHP,
    //                        ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_stats.m_maxHP );
    //                wprintf( ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_Name );
    //                printf( "\n" );
    //                printf( "%11s",
    //                        ItemList[ ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_Item ].getDisplayName( ).c_str( ) );
    //                drawPKMNIcon( Oam,
    //                              spriteInfo,
    //                              ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_SPEC,
    //                              ( Oam->oamBuffer[ i ] ).x - 4,
    //                              ( Oam->oamBuffer[ i ] ).y - 20,
    //                              p_oamIndex,
    //                              p_paletteIndex,
    //                              p_tileIndex,
    //                              true );
    //            } else {
    //                printf( "\n Ei" );
    //                drawEggIcon( Oam,
    //                             spriteInfo,
    //                             ( Oam->oamBuffer[ i ] ).x - 4,
    //                             ( Oam->oamBuffer[ i ] ).y - 20,
    //                             p_oamIndex,
    //                             p_paletteIndex,
    //                             p_tileIndex,
    //                             true );
    //            }
    //            updateOAMSub( Oam );
    //        }
    //    }
    //
    //    int battle::getSwitchPkmn( int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex, bool p_retA ) {
    //        int res = -1;
    //        int num = (int)_player->m_pkmnTeam->size( );
    //        touchPosition t;
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = !p_retA;
    //        while( 42 ) {
    //            swiWaitForVBlank( );
    //            updateOAMSub( Oam );
    //            updateTime( );
    //            touchRead( &t );
    //
    //            if( p_retA && t.px > 224 && t.py > 164 ) {
    //                waitForTouchUp( );
    //
    //                consoleSelect( &Bottom );
    //                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                consoleClear( );
    //                for( int i = 5; i <= 8; ++i )
    //                    ( Oam->oamBuffer[ i ] ).isHidden = false;
    //                for( int i = 20; i <= p_oamIndex; ++i )
    //                    ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                updateOAMSub( Oam );
    //
    //                for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) ) {
    //                    if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                        break;
    //                    ( Oam->oamBuffer[ i + 1 ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //                    ( Oam->oamBuffer[ i ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //                }
    //                init( );
    //                return -1;
    //            }
    //            for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) )
    //                if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                    break;
    //                else if( t.px > Oam->oamBuffer[ i ].x && t.py > Oam->oamBuffer[ i ].y && t.px - 64 < Oam->oamBuffer[ i + 1 ].x && t.py - 32 < Oam->oamBuffer[ i ].y ) {
    //                    if( ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_IV.m_isEgg ||
    //                        ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_stats.m_acHP == 0 ||
    //                        ( ( ( i - 21 ) / 2 ) ^ 1 ) < ( 1 + ( m_battleMode == DOUBLE ) ) )
    //                        continue;
    //
    //                    ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                    ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                    ( Oam->oamBuffer[ 3 + ( ( ( i - 21 ) / 2 ) ^ 1 ) ] ).isHidden = true;
    //                    updateOAMSub( Oam );
    //
    //                    waitForTouchUp( );
    //
    //                    res = ( ( ( i - 21 ) / 2 ) ^ 1 );
    //                    goto OUT2;
    //                }
    //        }
    //OUT2:
    //        consoleSelect( &Bottom );
    //        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //        consoleClear( );
    //        for( int i = 5; i <= 8; ++i )
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //        for( int i = 20; i <= p_oamIndex; ++i )
    //            ( Oam->oamBuffer[ i ] ).isHidden = true;
    //        for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) ) {
    //            if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                break;
    //            ( Oam->oamBuffer[ i + 1 ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //            ( Oam->oamBuffer[ i ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //        }
    //        updateOAMSub( Oam );
    //        init( );
    //
    //        return res;
    //    }
    //
    //#define BATTLE_END  -1
    //#define RETRY       +1
    //#define RETRY2       -2
    //#define SUCCESS     +2
    //    int battle::getChoice( int p_pkmnSlot ) {
    //        touchPosition t;
    //
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = !p_pkmnSlot;
    //        updateOAMSub( Oam );
    //
    //        setMainBattleVisibility( false );
    //        int oamIndex = oamIndexS, paletteIndex = palcntS, tileIndex = nextAvailableTileIdxS;
    //        drawPKMNIcon( Oam, spriteInfo, ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_SPEC, 112, 64, oamIndex, paletteIndex, tileIndex, true );
    //        clear( );
    //        sprintf( buffer, "Was soll %ls tun?", ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //        updateOAMSub( Oam );
    //
    //        int aprest = 0;
    //
    //        while( 42 ) {
    //            updateTime( );
    //            swiWaitForVBlank( );
    //            touchRead( &t );
    //            if( p_pkmnSlot && t.px > 224 && t.py > 164 ) {
    //                waitForTouchUp( );
    //
    //                return RETRY2;
    //            }
    //            //BEGIN FIGHT
    //            else if( t.px > 64 && t.px < 64 + 128 && t.py > 72 && t.py < 72 + 64 ) {
    //                waitForTouchUp( );
    //                setMainBattleVisibility( true );
    //                Oam->oamBuffer[ oamIndexS + 1 ].isHidden = true;
    //                consoleClear( );
    //
    //                aprest = 0;
    //                for( int i = 0; i < 4; ++i )
    //                    aprest += ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ i ];
    //                if( aprest == 0 ) {
    //                    clear( );
    //                    printf( "%ls hat keine\n restlichen Attacken...", ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //                    ownAtk[ p_pkmnSlot ] = std::pair<int, int>( 165, 1 | 2 | 4 | 8 );
    //                } else {
    //                    ( Oam->oamBuffer[ 20 ] ).isHidden = false;
    //
    //                    oamIndex = oamIndexS; paletteIndex = palcntS; tileIndex = nextAvailableTileIdxS;
    //                    printAttackChoiceScreen( p_pkmnSlot, oamIndex, paletteIndex, tileIndex );
    //
    //                    updateOAMSub( Oam );
    //                    consoleSetWindow( &Bottom, 0, 0, 32, 5 );
    //                    clear( );
    //                    sprintf( buffer, "Welchen Angriff?" );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    while( 42 ) {
    //                        updateTime( );
    //                        swiWaitForVBlank( );
    //                        touchRead( &t );
    //                        if( t.px > 224 && t.py > 164 ) { //Back  
    //                            waitForTouchUp( );
    //
    //                            consoleSelect( &Bottom );
    //                            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                            consoleClear( );
    //                            for( int i = 20; i <= oamIndex; ++i )
    //                                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                            setMainBattleVisibility( false );
    //
    //
    //                            for( int i = 21; i < 29; i += 2 ) {
    //                                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                                ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                                ( Oam->oamBuffer[ i + 1 ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //                                ( Oam->oamBuffer[ i ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //
    //                                if( ( i / 2 ) % 2 )
    //                                    ( Oam->oamBuffer[ i ] ).x += 16;
    //                                else
    //                                    ( Oam->oamBuffer[ i + 1 ] ).x -= 16;
    //                                updateOAMSub( Oam );
    //                            }
    //
    //                            return RETRY;
    //                        }
    //
    //                        for( int i = 21; i < 29; i += 2 )
    //                            if( t.px>( Oam->oamBuffer[ i ].x ) && t.px < ( Oam->oamBuffer[ i + 1 ].x + 64 ) &&
    //                                t.py>( Oam->oamBuffer[ i ] ).y && t.py < ( Oam->oamBuffer[ i ].y + 32 ) ) {
    //                            if( !ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] )
    //                                continue;
    //                            if( !ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ ( i - 21 ) / 2 ] )
    //                                continue;
    //
    //                            while( 1 ) {
    //                                swiWaitForVBlank( );
    //                                updateTime( false );
    //                                scanKeys( );
    //                                t = touchReadXY( );
    //                                if( t.px == 0 && t.py == 0 )
    //                                    break;
    //                            }
    //                            int trg = getTarget( p_pkmnSlot, ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] );
    //                            if( trg == 0 ) {
    //                                oamIndex = oamIndexS; paletteIndex = palcntS; tileIndex = nextAvailableTileIdxS;
    //                                for( int i = 21; i < 29; i += 2 ) {
    //                                    ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                                    ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                                    ( Oam->oamBuffer[ i + 1 ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //                                    ( Oam->oamBuffer[ i ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //
    //                                    if( ( i / 2 ) % 2 )
    //                                        ( Oam->oamBuffer[ i ] ).x += 16;
    //                                    else
    //                                        ( Oam->oamBuffer[ i + 1 ] ).x -= 16;
    //
    //                                    updateOAMSub( Oam );
    //                                }
    //                                printAttackChoiceScreen( p_pkmnSlot, oamIndex, paletteIndex, tileIndex );
    //
    //                                updateOAMSub( Oam );
    //                                consoleSetWindow( &Bottom, 0, 0, 32, 5 );
    //                                clear( );
    //                                sprintf( buffer, "Welchen Angriff?" );
    //                                cust_font.printString( buffer, 8, 8, true );
    //                                break;
    //                            }
    //
    //                            ownAtk[ p_pkmnSlot ] = std::pair<int, int>( ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ], trg );
    //                            ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ ( i - 21 ) / 2 ]--;
    //                            goto ATTACKCHOSEN;
    //                            }
    //                    }
    //ATTACKCHOSEN:
    //                    for( int i = 21; i < 29; i += 2 ) {
    //                        ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                        ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                        ( Oam->oamBuffer[ i + 1 ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //                        ( Oam->oamBuffer[ i ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //
    //                        if( ( i / 2 ) % 2 )
    //                            ( Oam->oamBuffer[ i ] ).x += 16;
    //                        else
    //                            ( Oam->oamBuffer[ i + 1 ] ).x -= 16;
    //
    //                        updateOAMSub( Oam );
    //                    }
    //                    for( int i = oamIndex; i >= oamIndex - 3; --i )
    //                        ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                    updateOAMSub( Oam );
    //                    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                    consoleClear( );
    //                    consoleSetWindow( &Bottom, 0, 0, 32, 5 );
    //                }
    //                return SUCCESS;
    //            }
    //            //END FIGHT
    //            //BEGIN BAG
    //            else if( t.px > 16 && t.px < 16 + 64 && t.py > 144 && t.py < 144 + 32 ) {
    //                waitForTouchUp( );
    //                return BATTLE_END;
    //            }
    //            //END BAG
    //            //BEGIN POKENAV
    //            else if( SAV.m_activatedPNav && t.px > 96 && t.px < 96 + 64 && t.py > 152 && t.py < 152 + 32 ) {
    //                waitForTouchUp( );
    //                printf( "TEST3" );
    //            }
    //            //END POKENAV
    //            //BEGIN pokemon
    //            else if( t.px > 176 && t.px < 176 + 64 && t.py > 144 && t.py < 144 + 32 ) {
    //                waitForTouchUp( );
    //
    //                oamIndex = oamIndexS; paletteIndex = palcntS; tileIndex = nextAvailableTileIdxS;
    //                printPKMNSwitchScreen( oamIndex, paletteIndex, tileIndex );
    //
    //                int res = getSwitchPkmn( oamIndex, paletteIndex, tileIndex, true );
    //
    //                if( res == -1 )
    //                    return RETRY;
    //
    //                switchWith[ p_pkmnSlot ][ PLAYER ] = res;
    //
    //                return SUCCESS;
    //            }
    //            //END pokemon
    //        }
    //    }
    //
    //#define OPP_1   1
    //#define OPP_2   2
    //#define ME      4
    //#define PARTNER 8
    //#define OWN_FIELD 16
    //#define OPP_FIELD 32
    //    int battle::getTarget( int p_pkmnSlot, int p_move ) {
    //        int ret = 0;
    //
    //        touchPosition t;
    //
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = false;
    //        updateOAMSub( Oam );
    //
    //        clear( );
    //        sprintf( buffer, "Wen soll %ls angreifen?", ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //
    //        int oamIndex = oamIndexS, paletteIndex = palcntS, tileIndex = nextAvailableTileIdxS;
    //        printTargetChoiceScreen( p_pkmnSlot, p_move, oamIndex, paletteIndex, tileIndex );
    //
    //        int poss = AttackList[ p_move ]->m_moveAffectsWhom;
    //        //Opp1, Opp2, ME, Partner
    //        bool validTrg[ 4 ] = {
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 16 || poss == 2 || poss == 4,
    //            poss == 0 || poss == 2 || poss == 32
    //        };
    //        if( !p_pkmnSlot )
    //            std::swap( validTrg[ 2 ], validTrg[ 3 ] );
    //
    //        int change = 0;
    //
    //        while( 42 ) {
    //            if( ++change == 60 ) {
    //                bool changeA[ 4 ] = { false };
    //
    //                if( poss & 2 )
    //                    changeA[ 2 ] = changeA[ 3 ] = true;
    //                if( ( poss & 8 ) || ( poss & 32 ) || ( poss & 64 ) )
    //                    changeA[ 0 ] = changeA[ 1 ] = true;
    //                if( poss & 32 )
    //                    changeA[ 2 + ( p_pkmnSlot ) ] = true;
    //
    //                for( int i = 21; i < 29; i += 2 ) {
    //                    if( m_battleMode == DOUBLE && changeA[ ( i - 21 ) / 2 ] ) {
    //                        int u = ( i - 21 ) / 2;
    //
    //                        POKEMON::pokemon &acPK = ( ( u / 2 ) ? ACPKMN( 1 - u % 2, PLAYER ) : ACPKMN( u % 2, OPPONENT ) );
    //
    //                        if( acPK.m_stats.m_acHP == 0 )
    //                            continue;
    //
    //                        Oam->oamBuffer[ i ].isHidden = !Oam->oamBuffer[ i ].isHidden;
    //                        Oam->oamBuffer[ i + 1 ].isHidden = !Oam->oamBuffer[ i + 1 ].isHidden;
    //                    }
    //                }
    //                updateOAMSub( Oam );
    //                change = 0;
    //            }
    //            updateTime( );
    //            swiWaitForVBlank( );
    //            touchRead( &t );
    //            if( t.px>224 && t.py > 164 ) { //Back  
    //                waitForTouchUp( );
    //                return 0;
    //            }
    //
    //            for( int i = 21; i < 29; i += 2 ) {
    //                if( t.px>( Oam->oamBuffer[ i ].x ) && t.px < ( Oam->oamBuffer[ i + 1 ].x + 64 ) &&
    //                    t.py>( Oam->oamBuffer[ i ] ).y && t.py < ( Oam->oamBuffer[ i ].y + 32 ) ) {
    //                    int u = ( i - 21 ) / 2;
    //                    if( m_battleMode != DOUBLE && ( u == 2 || u == 1 ) )
    //                        continue;
    //                    if( !validTrg[ u ] )
    //                        continue;
    //
    //                    POKEMON::pokemon &acPK = ( ( u / 2 ) ? ACPKMN( 1 - u % 2, PLAYER ) : ACPKMN( u % 2, OPPONENT ) );
    //
    //                    if( acPK.m_stats.m_acHP == 0 ) {
    //                        ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                        ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                        consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //                        consoleClear( );
    //                        continue;
    //                    }
    //
    //                    while( 1 ) {
    //                        swiWaitForVBlank( );
    //                        updateTime( false );
    //                        scanKeys( );
    //                        t = touchReadXY( );
    //                        if( t.px == 0 && t.py == 0 )
    //                            break;
    //                    }
    //                    ret = ( 1 << u );
    //                    if( u == 2 && p_pkmnSlot == 0 )
    //                        ret = PARTNER;
    //                    if( poss & 2 )
    //                        ret = OWN_FIELD;
    //                    if( poss & 4 )
    //                        ret = ( 2 << ( rand( ) % 3 ) );
    //                    if( ( poss & 8 ) || ( poss & 32 ) )
    //                        ret |= OPP_1 | OPP_2;
    //                    if( poss & 32 )
    //                        ret |= PARTNER;
    //                    if( poss & 64 )
    //                        ret = OPP_FIELD;
    //
    //                    return ret;
    //                }
    //            }
    //        }
    //    }
    //
    //    bool participated[ 6 ] = { false };
    //#define LUCKY_EGG_EFFEKT     42
    //    int calcEXP( const POKEMON::pokemon& p_move, int p_atkind, const POKEMON::pokemon& p_defendingPkmn, bool p_wild ) {
    //        if( !SAV.m_EXPShareEnabled && !participated[ p_atkind ] )
    //            return 0;
    //
    //        float a = p_wild ? 1 : 1.5;
    //        POKEMON::PKMNDATA::getAll( p_defendingPkmn.m_boxdata.m_speciesId, p );
    //        int b = p.m_EXPYield;
    //
    //        float e = ItemList[ p_move.m_boxdata.m_holdItem ].getEffectType( ) == LUCKY_EGG_EFFEKT ? 1.5 : 1;
    //
    //        int L = p_move.m_Level;
    //
    //        int s = 1;
    //        if( SAV.m_EXPShareEnabled && !participated[ p_atkind ] )
    //            s = 2;
    //
    //        float t = ( p_move.m_boxdata.m_oTId == SAV.m_Id && p_move.m_boxdata.m_oTSid == SAV.m_Sid ? 1 : 1.5 );
    //
    //        return int( ( a * t* b* e* L ) / ( 7 * s ) );
    //    }
    //
    //    void printEFFLOG( const POKEMON::pokemon& P, int p_move ) {
    //        if( AttackList[ p_move ]->m_moveHitType == move::moveHitTypes::STAT )
    //            return;
    //
    //        if( missed ) {
    //            clear( );
    //            if( P.m_stats.m_acHP )
    //                sprintf( buffer, "%ls wich der Attacke aus.", P.m_boxdata.m_name );
    //            else
    //                sprintf( buffer, "Die Attacke ging daneben..." );
    //            cust_font.printString( buffer, 8, 8, true );
    //            for( int i = 0; i < 75; ++i )
    //                swiWaitForVBlank( );
    //            return;
    //        }
    //        if( eff != 1 )
    //            clear( );
    //        if( eff > 3 )
    //            sprintf( buffer, "Das ist enorm effektiv\ngegen %ls!", P.m_boxdata.m_name );
    //        else if( eff > 1 )
    //            sprintf( buffer, "Das ist sehr effektiv\ngegen %ls!", P.m_boxdata.m_name );
    //        else if( eff == 0 )
    //            sprintf( buffer, "Hat die Attacke\n%lsgetroffen?", P.m_boxdata.m_name );
    //        else if( eff < 0.3 )
    //            sprintf( buffer, "Das ist nur enorm wenig\neffektiv gegen %ls...", P.m_boxdata.m_name );
    //        else if( eff < 1 )
    //            sprintf( buffer, "Das ist nicht sehr effektiv\ngegen %ls.", P.m_boxdata.m_name );
    //
    //        if( eff != 1 ) {
    //            cust_font.printString( buffer, 8, 8, true );
    //            for( int i = 0; i < 75; ++i )
    //                swiWaitForVBlank( );
    //        }
    //        if( criticalOccured ) {
    //            clear( );
    //            cust_font.printString( "Ein Volltreffer!", 8, 8, true );
    //            for( int i = 0; i < 75; ++i )
    //                swiWaitForVBlank( );
    //        }
    //    }
    //
    //    int battle::start( int p_battleBack, weather p_weather ) {
    //        drawSub( );
    //
    //        for( int i = 0; i < 6; ++i )
    //            participated[ i ] = false;
    //
    //        videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    //        drawTopBack( );
    //        initBattleScene( p_battleBack, p_weather );
    //        //touchPosition t;
    //
    //        while( ( _round-- ) != 0 ) {
    //            switchWith[ 0 ][ OPPONENT ] = switchWith[ 0 ][ PLAYER ] =
    //                switchWith[ 1 ][ OPPONENT ] = switchWith[ 1 ][ PLAYER ] = 0;
    //
    //            //Switch Out KOed pokemon
    //
    //            //Own
    //            if( ACPKMNSTS( 0, PLAYER ) == KO ) {
    //                int pcnt = 0, at = 0;
    //                for( int i = 1 + ( m_battleMode == DOUBLE ); i < 6; ++i )
    //                    if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                    //switchOwnPkmn(i,0);
    //                    pcnt++;
    //                    at = i;
    //                    }
    //                if( pcnt == 1 )
    //                    switchOwnPkmn( at, 0 );
    //                else if( pcnt > 1 ) {
    //                    int oamIndex = oamIndexS,
    //                        paletteIndex = palcntS,
    //                        tileIndex = nextAvailableTileIdxS;
    //                    printPKMNSwitchScreen( oamIndex, paletteIndex, tileIndex );
    //
    //                    int res = getSwitchPkmn( oamIndex, paletteIndex, tileIndex, false );
    //                    switchOwnPkmn( res, 0 );
    //                }
    //            }
    //            if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, PLAYER ) == KO ) {
    //                int pcnt = 0, at = 0;
    //                for( int i = 2; i < 6; ++i ) {
    //                    if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                        //switchOwnPkmn(i,0);
    //                        pcnt++;
    //                        at = i;
    //                    }
    //                }
    //                if( pcnt == 1 )
    //                    switchOwnPkmn( at, 1 );
    //                else if( pcnt > 1 ) {
    //                    int oamIndex = oamIndexS,
    //                        paletteIndex = palcntS,
    //                        tileIndex = nextAvailableTileIdxS;
    //                    printPKMNSwitchScreen( oamIndex, paletteIndex, tileIndex );
    //
    //                    int res = getSwitchPkmn( oamIndex, paletteIndex, tileIndex, false );
    //                    switchOwnPkmn( res, 1 );
    //                }
    //            }
    //
    //            if( ACPKMNSTS( 0, PLAYER ) == KO && ( m_battleMode != DOUBLE || ACPKMNSTS( 1, PLAYER ) == KO || ACPKMNSTS( 1, PLAYER ) == NA ) ) {
    //                //Player lost
    //                clear( );
    //                cust_font.printString( _opponent->getWinMsg( ), 8, 8, true );
    //                for( int i = 0; i < 75; ++i )
    //                    swiWaitForVBlank( );
    //
    //                dinit( );
    //                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                consoleSelect( &Bottom );
    //                consoleClear( );
    //                initOAMTableSub( Oam );
    //                initMainSprites( Oam, spriteInfo );
    //                setMainSpriteVisibility( false );
    //                Oam->oamBuffer[ 8 ].isHidden = true;
    //                Oam->oamBuffer[ 0 ].isHidden = true;
    //                Oam->oamBuffer[ 1 ].isHidden = false;
    //                return -1;
    //            }
    //
    //            //Opp
    //            if( ACPKMNSTS( 0, OPPONENT ) == KO )
    //                for( size_t i = 1 + ( m_battleMode == DOUBLE ); i < _opponent->m_pkmnTeam->size( ); ++i )
    //                    if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                switchOppPkmn( i, 0 );
    //                for( int i = 0; i < 6; ++i )
    //                    participated[ i ] = false;
    //                break;
    //                    }
    //            if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, OPPONENT ) == KO )
    //                for( size_t i = 2; i < _opponent->m_pkmnTeam->size( ); ++i )
    //                    if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                switchOppPkmn( i, 1 );
    //                for( int i = 0; i < 6; ++i )
    //                    participated[ i ] = false;
    //                break;
    //                    }
    //            if( ACPKMNSTS( 0, OPPONENT ) == KO && ( m_battleMode != DOUBLE || ACPKMNSTS( 1, OPPONENT ) == KO || ACPKMNSTS( 1, OPPONENT ) == NA ) ) {
    //                //Opp lost
    //                clear( );
    //                cust_font.printString( _opponent->getLooseMsg( ), 8, 8, true );
    //                for( int i = 0; i < 75; ++i )
    //                    swiWaitForVBlank( );
    //
    //                SAV.m_money += _opponent->getLooseMoney( );
    //                clear( );
    //                sprintf( buffer, "Du gewinnst %dP!", _opponent->getLooseMoney( ) );
    //                cust_font.printString( buffer, 8, 8, true );
    //                for( int i = 0; i < 75; ++i )
    //                    swiWaitForVBlank( );
    //
    //                dinit( );
    //                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                consoleSelect( &Bottom );
    //                consoleClear( );
    //                initOAMTableSub( Oam );
    //                initMainSprites( Oam, spriteInfo );
    //                setMainSpriteVisibility( false );
    //                Oam->oamBuffer[ 8 ].isHidden = true;
    //                Oam->oamBuffer[ 0 ].isHidden = true;
    //                Oam->oamBuffer[ 1 ].isHidden = false;
    //                return 1;
    //            }
    //            //End of Switch Out
    //
    //            participated[ ACPOS( 0, PLAYER ) ] = true;
    //            if( ( m_battleMode == DOUBLE ) )
    //                participated[ ACPOS( 1, PLAYER ) ] = true;
    //
    //            consoleSelect( &Bottom );
    //
    //BEFORE_0:
    //            if( ACPKMNSTS( 0, PLAYER ) != KO ) {
    //                switch( getChoice( 0 ) ) {
    //                    case SUCCESS:
    //                        goto BEFORE_1;
    //                    case RETRY:
    //                        switchWith[ 0 ][ PLAYER ] = 0;
    //                        goto BEFORE_0;
    //                    case BATTLE_END:
    //                        goto END;
    //                }
    //            }
    //BEFORE_1:
    //            if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, PLAYER ) != KO )
    //                switch( getChoice( 1 ) ) {
    //                    case RETRY:
    //                        switchWith[ 1 ][ PLAYER ] = 0;
    //                        goto BEFORE_1;
    //                    case RETRY2:
    //                        switchWith[ 0 ][ PLAYER ] = switchWith[ 1 ][ PLAYER ] = 0;
    //                        goto BEFORE_0;
    //                    case BATTLE_END:
    //                        goto END;
    //            }
    //
    //            ( Oam->oamBuffer[ 20 ] ).isHidden = true;
    //            updateOAMSub( Oam );
    //            //OPP'S ACTIONS
    //            switchWith[ 0 ][ OPPONENT ] = switchWith[ 0 ][ PLAYER ];
    //            switchWith[ 1 ][ OPPONENT ] = switchWith[ 1 ][ PLAYER ];
    //            oppAtk[ 0 ] = std::pair<int, int>( ACPKMN( 0, OPPONENT ).m_boxdata.m_Attack[ 0 ], 1 );
    //            oppAtk[ 1 ] = std::pair<int, int>( ACPKMN( 1, OPPONENT ).m_boxdata.m_Attack[ 0 ], 2 );
    //
    //
    //            consoleClear( );
    //
    //            int inits[ 4 ] = { 0 }, ranking[ 4 ] = { 0 };
    //
    //            int maxst = ( m_battleMode == DOUBLE ) ? 4 : 2;
    //
    //            for( int i = 0; i < maxst; ++i )
    //                if( i % 2 )
    //                    inits[ i ] = ( *_opponent->m_pkmnTeam )[ _acPkmnPosition[ i / 2 ][ OPPONENT ] ].m_stats.m_Spd;
    //                else
    //                    inits[ i ] = ( *_player->m_pkmnTeam )[ _acPkmnPosition[ i / 2 ][ PLAYER ] ].m_stats.m_Spd;
    //
    //            bool ko[ 2 ][ 2 ] = { { false } };
    //
    //            for( int i = 0; i < maxst; ++i ) {
    //                for( int j = 0, max = -1; j < maxst; ++j )
    //                    if( inits[ j ] > max && ( !i || ( ranking[ i - 1 ] != j && inits[ ranking[ i - 1 ] ] >= inits[ j ] ) ) ) {
    //                    max = inits[ j ];
    //                    ranking[ i ] = j;
    //                    }
    //                for( int i = 0; i < maxst; ++i ) {
    //                    int prio = 0;
    //                    if( i % 2 )
    //                        prio = AttackList[ oppAtk[ i / 2 ].first ]->m_movePriority;
    //                    else
    //                        prio = AttackList[ ownAtk[ i / 2 ].first ]->m_movePriority;
    //                    ranking[ i ] -= 4 * prio;
    //                }
    //            }
    //            for( int i = 0; i < maxst; ++i ) {
    //                int acin = 0;
    //                for( int j = 0, min = 100; j < maxst; ++j )
    //                    if( ranking[ j ] < min ) {
    //                    acin = j;
    //                    min = ranking[ j ];
    //                    }
    //                ranking[ acin ] = 42442;
    //
    //                bool opp = acin % 2;
    //
    //                if( opp ) {
    //                    if( ACPKMN( acin / 2, OPPONENT ).m_stats.m_acHP == 0 )
    //                        continue;
    //                } else
    //                    if( ACPKMN( acin / 2, PLAYER ).m_stats.m_acHP == 0 )
    //                        continue;
    //
    //                if( switchWith[ acin / 2 ][ opp ] ) {
    //                    if( opp )
    //                        switchOppPkmn( switchWith[ acin / 2 ][ opp ], acin / 2 );
    //                    else
    //                        switchOwnPkmn( switchWith[ acin / 2 ][ opp ], acin / 2 );
    //                    continue;
    //                }
    //
    //                ko[ 0 ][ PLAYER ] = ko[ 0 ][ OPPONENT ] = ko[ 1 ][ PLAYER ] = ko[ 1 ][ OPPONENT ] = false;
    //                consoleSelect( &Top );
    //
    //                if( opp )
    //                    sprintf( buffer,
    //                    "%ls (Gegner)\nsetzt %s ein!",
    //                    ACPKMN( acin / 2, OPPONENT ).m_boxdata.m_Name,
    //                    AttackList[ oppAtk[ acin / 2 ].first ]->m_moveName.c_str( ) );
    //                else
    //                    sprintf( buffer,
    //                    "%ls setzt\n%s ein!",
    //                    ACPKMN( acin / 2, PLAYER ).m_boxdata.m_Name,
    //                    AttackList[ ownAtk[ acin / 2 ].first ]->m_moveName.c_str( ) );
    //                clear( );
    //                cust_font.printString( buffer, 8, 8, true );
    //                for( int i = 0; i < 70; ++i )
    //                    swiWaitForVBlank( );
    //                if( opp ) {
    //                    int tg = oppAtk[ acin / 2 ].second;
    //                    int dmg1 = 0, dmg2 = 0, dmg3 = 0;
    //                    if( tg & 1 ) {
    //                        missed = false;
    //                        dmg1 = calcDamage( *AttackList[ oppAtk[ acin / 2 ].first ], ACPKMN( acin / 2, OPPONENT ), ACPKMN( 0, PLAYER ), rand( ) % 15 );
    //
    //                        if( !missed ) {
    //                            int old = ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP;
    //                            ACPKMN( 0, PLAYER ).m_stats.m_acHP = std::max( 0, ACPKMN( 0, PLAYER ).m_stats.m_acHP - dmg1 );
    //
    //                            ko[ 0 ][ PLAYER ] = ( ACPKMN( 0, PLAYER ).m_stats.m_acHP == 0 );
    //                            if( ko[ 0 ][ PLAYER ] )
    //                                ACPKMNSTS( 0, PLAYER ) = KO;
    //
    //                            displayHP( old, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP,
    //                                       256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, true );
    //                            consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                            consoleClear( );
    //                            printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                    GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                    ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //                        }
    //                        printEFFLOG( ( ACPKMN( 0, PLAYER ) ), oppAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 2 ) ) {
    //                        missed = false;
    //                        dmg2 = calcDamage( *AttackList[ oppAtk[ acin / 2 ].first ], ACPKMN( acin / 2, OPPONENT ), ACPKMN( 1, PLAYER ), rand( ) % 15 );
    //
    //                        if( !missed ) {
    //                            int old = ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP;
    //                            ACPKMN( 1, PLAYER ).m_stats.m_acHP = std::max( 0, ACPKMN( 1, PLAYER ).m_stats.m_acHP - dmg2 );
    //
    //                            ko[ 1 ][ PLAYER ] = ( ACPKMN( 1, PLAYER ).m_stats.m_acHP == 0 );
    //                            if( ko[ 1 ][ PLAYER ] )
    //                                ACPKMNSTS( 1, PLAYER ) = KO;
    //                            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                            consoleClear( );
    //                            printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                printf( " " );
    //                            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                printf( " " );
    //                            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //                            displayHP( old, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                                       256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, true );
    //
    //                        }
    //                        printEFFLOG( ( ACPKMN( 1, PLAYER ) ), oppAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 8 ) ) {
    //                        missed = false;
    //                        dmg3 = calcDamage( *AttackList[ oppAtk[ acin / 2 ].first ], ACPKMN( acin / 2, OPPONENT ),
    //                                           ACPKMN( 1 - acin / 2, OPPONENT ), rand( ) % 15 );
    //
    //                        if( !missed ) {
    //                            int old = 0;
    //                            if( acin / 2 == 0 )
    //                                old = ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP;
    //                            else
    //                                old = ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP;
    //
    //                            ACPKMN( 1 - acin / 2, OPPONENT ).m_stats.m_acHP = std::max( 0, ACPKMN( 1 - acin / 2, OPPONENT ).m_stats.m_acHP - dmg3 );
    //                            if( !ACPKMN( 1 - acin / 2, OPPONENT ).m_stats.m_acHP ) {
    //                                ko[ 1 - ( acin / 2 ) ][ OPPONENT ] = true;
    //                                ACPKMNSTS( 1 - ( acin / 2 ), OPPONENT ) = KO;
    //                            }
    //                            if( acin / 2 == 0 ) {
    //                                consoleSetWindow( &Top, 0, 5, 20, 2 );
    //                                consoleClear( );
    //                                printf( "%10ls%c\n", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name, GENDER( ACPKMN( 0, OPPONENT ) ) );
    //                                if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //                                    printf( " " );
    //                                if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //                                    printf( " " );
    //                                printf( "Lv%d%4dKP", ACPKMN( 0, OPPONENT ).m_Level,
    //                                        ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //                                displayHP( old, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP,
    //                                           88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, true );
    //                            } else {
    //                                consoleSetWindow( &Top, 4, 2, 20, 2 );
    //                                consoleClear( );
    //                                printf( "%ls%c\nLv%d%4dKP", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name, GENDER( ACPKMN( 1, OPPONENT ) ),
    //                                        ACPKMN( 1, OPPONENT ).m_Level, ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //                                displayHP( old, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP,
    //                                           0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, true );
    //                            }
    //                        }
    //                        printEFFLOG( ACPKMN( 1 - acin / 2, OPPONENT ), oppAtk[ acin / 2 ].first );
    //                    }
    //                } else {
    //                    int tg = ownAtk[ acin / 2 ].second;
    //                    int dmg1 = 0, dmg2 = 0, dmg3 = 0;
    //                    if( tg & 1 ) {
    //                        missed = false;
    //                        dmg1 = calcDamage( *AttackList[ ownAtk[ acin / 2 ].first ], ACPKMN( acin / 2, PLAYER ), ACPKMN( 0, OPPONENT ), rand( ) % 15 );
    //                        if( !missed ) {
    //                            int old = ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP;
    //                            ACPKMN( 0, OPPONENT ).m_stats.m_acHP = std::max( 0, ACPKMN( 0, OPPONENT ).m_stats.m_acHP - dmg1 );
    //                            if( !ACPKMN( 0, OPPONENT ).m_stats.m_acHP ) {
    //                                ko[ 0 ][ OPPONENT ] = true;
    //                                ACPKMNSTS( 0, OPPONENT ) = KO;
    //                            }
    //                            consoleSetWindow( &Top, 0, 5, 20, 2 );
    //                            consoleClear( );
    //                            printf( "%10ls%c\n", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name, GENDER( ACPKMN( 0, OPPONENT ) ) );
    //
    //                            if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //                                printf( " " );
    //                            if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //                                printf( " " );
    //
    //                            printf( "Lv%d%4dKP",
    //                                    ACPKMN( 0, OPPONENT ).m_Level,
    //                                    ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //
    //                            displayHP( old, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP,
    //                                       88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, true );
    //                        }
    //                        printEFFLOG( ACPKMN( 0, OPPONENT ), ownAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 2 ) ) {
    //                        missed = false;
    //                        dmg2 = calcDamage( *AttackList[ ownAtk[ acin / 2 ].first ],
    //                                           ACPKMN( acin / 2, PLAYER ),
    //                                           ACPKMN( 1, OPPONENT ),
    //                                           rand( ) % 15 );
    //                        if( !missed ) {
    //                            int old = ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP;
    //
    //                            ACPKMN( 1, OPPONENT ).m_stats.m_acHP = std::max( 0, ACPKMN( 1, OPPONENT ).m_stats.m_acHP - dmg2 );
    //
    //                            if( !ACPKMN( 1, OPPONENT ).m_stats.m_acHP ) {
    //                                ko[ 1 ][ OPPONENT ] = true;
    //                                ACPKMNSTS( 1, OPPONENT ) = KO;
    //                            }
    //
    //                            consoleSetWindow( &Top, 4, 2, 20, 2 );
    //                            consoleClear( );
    //                            printf( "%ls%c\nLv%d%4dKP",
    //                                    ACPKMN( 1, OPPONENT ).m_boxdata.m_Name,
    //                                    GENDER( ACPKMN( 1, OPPONENT ) ),
    //                                    ACPKMN( 1, OPPONENT ).m_Level,
    //                                    ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //                            displayHP( old, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP,
    //                                       0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, true );
    //
    //                        }
    //                        printEFFLOG( ACPKMN( 1, OPPONENT ), ownAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 8 ) ) {
    //                        missed = false;
    //                        dmg3 = calcDamage( *AttackList[ ownAtk[ acin / 2 ].first ],
    //                                           ACPKMN( acin / 2, PLAYER ),
    //                                           ACPKMN( 1 - acin / 2, PLAYER ),
    //                                           rand( ) % 15 );
    //                        if( !missed ) {
    //                            int old = 0;
    //                            if( acin / 2 == 1 )
    //                                old = ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP;
    //                            else
    //                                old = ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP;
    //
    //                            ACPKMN( 1 - acin / 2, PLAYER ).m_stats.m_acHP = std::max( 0, ACPKMN( 1 - acin / 2, PLAYER ).m_stats.m_acHP - dmg3 );
    //                            if( !ACPKMN( 1 - acin / 2, PLAYER ).m_stats.m_acHP ) {
    //                                ko[ 1 - ( acin / 2 ) ][ PLAYER ] = true;
    //                                ACPKMNSTS( 1 - ( acin / 2 ), PLAYER ) = KO;
    //                            }
    //                            if( acin / 2 == 1 ) {
    //                                displayHP( old, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 /
    //                                           ACPKMN( 0, PLAYER ).m_stats.m_maxHP, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, true );
    //                                consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                                consoleClear( );
    //                                printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                        GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                        ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //                            } else {
    //                                displayHP( old, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                                           256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, true );
    //                                consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                                consoleClear( );
    //                                printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                                if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                    printf( " " );
    //                                if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                    printf( " " );
    //                                printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                        ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //                            }
    //                        }
    //                        printEFFLOG( ACPKMN( 1 - acin / 2, PLAYER ), ownAtk[ acin / 2 ].first );
    //                    }
    //                }
    //
    //
    //                if( ko[ 0 ][ PLAYER ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls wurde besiegt.", ACPKMN( 0, PLAYER ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OWN_PKMN_1_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    consoleSelect( &Bottom );
    //
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //                }
    //                if( ko[ 1 ][ PLAYER ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls wurde besiegt.", ACPKMN( 1, PLAYER ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OWN_PKMN_2_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    consoleSelect( &Bottom );
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //                }
    //
    //                if( ko[ 0 ][ OPPONENT ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls (Gegner)\nwurde besiegt.", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //                    consoleSetWindow( &Top, 0, 5, 20, 2 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OPP_PKMN_1_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //
    //                }
    //                if( ko[ 1 ][ OPPONENT ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls (Gegner)\nwurde besiegt.", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    consoleSetWindow( &Top, 4, 2, 20, 2 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OPP_PKMN_2_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //                }
    //
    //
    //                consoleSelect( &Bottom );
    //
    //                if( ko[ 0 ][ OPPONENT ] || ko[ 1 ][ OPPONENT ] ) {
    //                    for( int i = 0; i < 6; ++i ) {
    //                        if( m_distributeEXP && ACPKMNSTS( i, PLAYER ) != acStatus::NA && ACPKMNSTS( i, PLAYER ) != acStatus::KO ) {
    //                            POKEMON::pokemon& acPK = ( *_player->m_pkmnTeam )[ ACPOS( i, PLAYER ) ];
    //                            POKEMON::pokemon& acDF = ACPKMN( 0, OPPONENT );
    //
    //                            POKEMON::PKMNDATA::getAll( ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC, p );
    //                            int exp = calcEXP( acPK, ACPOS( i, PLAYER ), acDF, false );
    //                            if( exp && acPK.m_Level < 100 ) {
    //                                int evsum = 0;
    //
    //                                for( int j = 0; j < 6; ++j ) {
    //                                    evsum += p.m_EVYield[ j ];
    //                                    acPK.m_boxdata.m_effortValues[ j ] += p.m_EVYield[ j ];
    //                                }
    //
    //                                clear( );
    //                                sprintf( buffer, "%ls erhält %d EV\nund %d E.-Punkte.", acPK.m_boxdata.m_name, evsum, exp );
    //                                cust_font.printString( buffer, 8, 8, true );
    //
    //                                POKEMON::PKMNDATA::getAll( ( *_player->m_pkmnTeam )[ ACPOS( i, PLAYER ) ].m_boxdata.m_SPEC, p );
    //                                int old = ( acPK.m_boxdata.m_experienceGained - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                    ( POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] );
    //
    //                                acPK.m_boxdata.m_experienceGained += exp;
    //
    //                                int nw = std::min( 100u, ( acPK.m_boxdata.m_experienceGained - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                   ( POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) );
    //
    //
    //                                if( i == 0 )
    //                                    displayEP( old, nw, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, true );
    //
    //                                if( ( m_battleMode == DOUBLE ) && i == 1 )
    //                                    displayEP( old, nw, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, true );
    //                                for( int i = 0; i < 75; ++i )
    //                                    swiWaitForVBlank( );
    //                                bool newLevel = acPK.m_Level < 100 && POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] <= acPK.m_boxdata.m_experienceGained;
    //                                bool nL = newLevel;
    //
    //                                u16 HPdif = acPK.m_stats.m_maxHP - acPK.m_stats.m_acHP;
    //
    //                                while( newLevel ) {
    //                                    acPK.m_Level++;
    //
    //                                    if( acPK.m_boxdata.m_speciesId != 292 )
    //                                        acPK.m_stats.m_maxHP = ( ( acPK.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ] + ( acPK.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPK.m_Level / 100 ) + 10;
    //                                    else
    //                                        acPK.m_stats.m_maxHP = 1;
    //                                    POKEMON::pkmnNatures nature = acPK.m_boxdata.getNature( );
    //                                    acPK.m_stats.m_Atk = ( ( ( acPK.m_boxdata.m_individualValues.m_moves + 2 * p.m_bases[ 1 ] + ( acPK.m_boxdata.m_effortValues[ 1 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ PLAYER ];
    //                                    acPK.m_stats.m_Def = ( ( ( acPK.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ 2 ] + ( acPK.m_boxdata.m_effortValues[ 2 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ OPPONENT ];
    //                                    acPK.m_stats.m_Spd = ( ( ( acPK.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ 3 ] + ( acPK.m_boxdata.m_effortValues[ 3 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ 2 ];
    //                                    acPK.m_stats.m_SAtk = ( ( ( acPK.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ 4 ] + ( acPK.m_boxdata.m_effortValues[ 4 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ 3 ];
    //                                    acPK.m_stats.m_SDef = ( ( ( acPK.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ 5 ] + ( acPK.m_boxdata.m_effortValues[ 5 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ 4 ];
    //
    //                                    acPK.m_stats.m_acHP = acPK.m_stats.m_maxHP - HPdif;
    //
    //                                    clear( );
    //                                    sprintf( buffer, "%ls erreicht Level %d.", acPK.m_boxdata.m_name, acPK.m_Level );
    //                                    cust_font.printString( buffer, 8, 8, true );
    //                                    for( int i = 0; i < 75; ++i )
    //                                        swiWaitForVBlank( );
    //
    //                                    nw = std::min( 100u, ( acPK.m_boxdata.m_experienceGained - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                   ( POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) );
    //
    //                                    if( i == 0 ) {
    //                                        displayEP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //                                        displayEP( 0, nw, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, true );
    //                                        consoleSelect( &Top );
    //                                        consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                                        consoleClear( );
    //                                        printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                                GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                                ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //                                    }
    //                                    if( ( m_battleMode == DOUBLE ) && i == 1 ) {
    //                                        displayEP( 100, 101, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //                                        displayEP( 0, nw, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, true );
    //                                        consoleSelect( &Top );
    //                                        consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                                        consoleClear( );
    //                                        printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                                        if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                            printf( " " );
    //                                        if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                            printf( " " );
    //                                        printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                                ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //                                    }
    //                                    newLevel = acPK.m_Level < 100 && (unsigned)POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] <= acPK.m_boxdata.m_experienceGained;
    //                                }
    //
    //                                if( nL && SAV.m_evolveInBattle ) {
    //                                    consoleSelect( &Top );
    //                                    if( acPK.canEvolve( ) ) {
    //                                        clear( );
    //                                        sprintf( buffer, "%ls entwickelt sich!", acPK.m_boxdata.m_name );
    //                                        cust_font.printString( buffer, 8, 8, true );
    //                                        for( int i = 0; i < 75; ++i )
    //                                            swiWaitForVBlank( );
    //
    //                                        acPK.evolve( );
    //
    //                                        if( i == 0 ) {
    //
    //                                            oamIndex = OWN_PKMN_1_START;
    //                                            palcnt = OWN_PKMN_1_PAL;
    //                                            nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //
    //                                            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale ) )
    //                                                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale );
    //
    //                                            updateOAM( OamTop );
    //
    //
    //                                            displayHP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //                                            displayHP( 100, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100
    //                                                       / ACPKMN( 0, PLAYER ).m_stats.m_maxHP, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //                                            displayEP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //
    //                                            POKEMON::PKMNDATA::getAll( ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //                                            displayEP( 0, ( ACPKMN( 0, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                       ( POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level ][ p.m_expType ] -
    //                                                       POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                                                       256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //                                            consoleSelect( &Top );
    //                                            consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                                            consoleClear( );
    //                                            printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                                    GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                                    ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //
    //                                        }
    //                                        if( ( m_battleMode == DOUBLE ) && i == 1 ) {
    //                                            oamIndex = OWN_PKMN_2_START;
    //                                            palcnt = OWN_PKMN_2_PAL;
    //                                            nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //
    //                                            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale ) )
    //                                                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale );
    //
    //                                            updateOAM( OamTop );
    //
    //                                            displayHP( 100, 101, 256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //                                            displayHP( 100, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 /
    //                                                       ACPKMN( 1, PLAYER ).m_stats.m_maxHP, 256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //                                            displayEP( 100, 100, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //                                            POKEMON::PKMNDATA::getAll( ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //                                            displayEP( 0, ( ACPKMN( 1, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                       ( POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level ][ p.m_expType ] -
    //                                                       POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                                                       256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //                                            consoleSelect( &Top );
    //                                            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                                            consoleClear( );
    //                                            printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                                            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                                printf( " " );
    //                                            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                                printf( " " );
    //                                            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //                                        }
    //
    //                                        clear( );
    //                                        sprintf( buffer, "Und wurde zu einem\n%ls.", acPK.m_boxdata.m_name );
    //                                        cust_font.printString( buffer, 8, 8, true );
    //
    //                                        for( int i = 0; i < 75; ++i )
    //                                            swiWaitForVBlank( );
    //                                    }
    //                                }
    //                            }
    //                        }
    //                    }
    //                }
    //
    //            }
    //
    //
    //
    //        }
    //END:
    //        dinit( );
    //        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //        initOAMTableSub( Oam );
    //        initMainSprites( Oam, spriteInfo );
    //        setMainSpriteVisibility( false );
    //        Oam->oamBuffer[ 8 ].isHidden = true;
    //        Oam->oamBuffer[ 0 ].isHidden = true;
    //        Oam->oamBuffer[ 1 ].isHidden = false;
    //        return 0;
    //    }
}