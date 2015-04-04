#include "dexUI.h"
#include "dex.h"
#include "uio.h"
#include "sprite.h"
#include "fs.h"
#include "pokemon.h"
#include "defines.h"

#include "Back.h"
#include "BagSpr.h"
#include "BigCirc1.h"
#include "memo.h"
#include "PKMN.h"
#include "time_icon.h"

#include "DexTop2.h"
#include <vector>
#include <cstdio>

namespace DEX {
#define PKMN_SPRITE_START(a) (4*(a))

#define BAG_SPR_SUB(a) (1 + (a))
#define BIG_CIRC_START 9
#define PKMN_ICON_SUB(a) (13 + (a))
#define PAGE_ICON_START 19

#define PKMN_ICON_SUB_PAL(a) (3 + (a))
#define PKMN_SPRITE_SUB_PAL 8
#define PAGE_ICON_PAL_START 9


    void dexUI::init( ) {
        IO::vramSetup( );
        videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

        IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &IO::Top, IO::consoleFont );

        IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
        consoleSetFont( &IO::Bottom, IO::consoleFont );


        //Initialize the top screen
        IO::initOAMTable( false );

        u16 tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", _currPkmn, 80, 64, PKMN_SPRITE_START( 0 ), 0, 0, false );
        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", _currPkmn, 80, 64, PKMN_SPRITE_START( 1 ), 1, tileCnt, false );

        tileCnt = IO::loadPKMNIcon( _currPkmn, 0, 8, PKMN_SPRITE_START( 2 ), 2, tileCnt, false );
        tileCnt = IO::loadTypeIcon( Type( 0 ), 33, 35, PKMN_SPRITE_START( 2 ) + 1, 3, tileCnt, false );
        tileCnt = IO::loadTypeIcon( Type( 0 ), 33, 35, PKMN_SPRITE_START( 2 ) + 2, 4, tileCnt, false );
        tileCnt = IO::loadPKMNIcon( _currPkmn, 0, 8, PKMN_SPRITE_START( 3 ), 5, tileCnt, false );
        for( u8 i = PKMN_SPRITE_START( 0 ); i <= PKMN_SPRITE_START( 3 ); ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;

        IO::updateOAM( false );

        //Initialize the subScreen
        IO::initOAMTable( true );

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, 0, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

        for( u8 i = 0; i < 5; ++i ) {
            tileCnt = IO::loadSprite( BAG_SPR_SUB( i ), 1, tileCnt, dexsppos[ 0 ][ i ], dexsppos[ 1 ][ i ], 32, 32, BagSprPal,
                                      BagSprTiles, BagSprTilesLen, false, false, !!_inDex, OBJPRIORITY_2, true );
            tileCnt = IO::loadPKMNIcon( 0, dexsppos[ 0 ][ i ], dexsppos[ 1 ][ i ], PKMN_ICON_SUB( i ), PKMN_ICON_SUB_PAL( i ), tileCnt, true );
            IO::Oam->oamBuffer[ PKMN_ICON_SUB( i ) ].isHidden = !!_inDex;
        }

        tileCnt = IO::loadSprite( BIG_CIRC_START, 2, tileCnt, 64, 48, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, false, false, true, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( BIG_CIRC_START + 1, 2, tileCnt, 128, 48, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, true, false, true, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( BIG_CIRC_START + 2, 2, tileCnt, 64, 112, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, false, true, true, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( BIG_CIRC_START + 3, 2, tileCnt, 128, 112, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, true, true, true, OBJPRIORITY_2, true );

        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", _currPkmn, dexsppos[ 0 ][ 8 ] + 16,
                                      dexsppos[ 1 ][ 8 ] + 16, PKMN_ICON_SUB( 5 ), PKMN_SPRITE_SUB_PAL, tileCnt, false );

        tileCnt = IO::loadSprite( PAGE_ICON_START, PAGE_ICON_PAL_START, tileCnt, dexsppos[ 0 ][ 5 ], dexsppos[ 1 ][ 5 ], 32, 32, memoPal,
                                  memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( PAGE_ICON_START + 1, PAGE_ICON_PAL_START + 1, tileCnt, dexsppos[ 0 ][ 6 ], dexsppos[ 1 ][ 6 ], 32, 32, time_iconPal,
                                  time_iconTiles, time_iconTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( PAGE_ICON_START + 2, PAGE_ICON_PAL_START + 2, tileCnt, dexsppos[ 0 ][ 7 ], dexsppos[ 1 ][ 7 ], 32, 32, PKMNPal,
                                  PKMNTiles, PKMNTilesLen, false, false, false, OBJPRIORITY_0, true );

        IO::updateOAM( true );
    }

    void dexUI::drawFormes( u16 p_formeIdx, bool p_hasGenderDifference, const std::string& p_formeName ) {
        IO::boldFont->setColor( BLACK_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        if( p_formeIdx == -1 ) {
            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", 0, 80, 64, PKMN_SPRITE_START( 0 ), 0, 0, false );
            IO::boldFont->printString( "Keine Daten.", 90, 167, false );
            for( u8 i = PKMN_SPRITE_START( 1 ); i < PKMN_SPRITE_START( 2 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            for( u8 i = PKMN_SPRITE_START( 3 ); i <= PKMN_SPRITE_START( 4 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
        }

        pokemonData data; getAll( _currPkmn, data );
        char buffer[ 50 ];

        u16 tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_formeIdx, 30, 64,
                                          PKMN_SPRITE_START( 0 ), 0, 0, false, false, p_hasGenderDifference, true );
        if( _currForme && !p_hasGenderDifference )
            IO::boldFont->printString( p_formeName.c_str( ), 30, 167, false );
        else if( _currForme % 2 )
            IO::boldFont->printString( "weiblich", 30, 167, false );
        else if( p_hasGenderDifference )
            IO::boldFont->printString( "männlich", 30, 167, false );
        else
            IO::boldFont->printString( getDisplayName( _currPkmn ), 30, 167, false );

        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_formeIdx, 110, 64,
                                      PKMN_SPRITE_START( 1 ), 1, tileCnt, false, true, p_hasGenderDifference );
        if( _currForme )
            IO::boldFont->printString( p_formeName.c_str( ), 110, 160, false );
        else if( _currForme % 2 )
            IO::boldFont->printString( "weiblich", 110, 160, false );
        else if( p_hasGenderDifference )
            IO::boldFont->printString( "männlich", 110, 160, false );
        else
            IO::boldFont->printString( getDisplayName( _currPkmn ), 110, 160, false );
        IO::boldFont->printString( "(schillernd)", 110, 176, false );

        //Load Icons of the other formes ( max 5 )
        auto formes = getAllFormes( _currPkmn );
        if( formes.empty( ) ) {
            for( u8 i = PKMN_SPRITE_START( 3 ); i <= PKMN_SPRITE_START( 4 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            return;
        }
        u8 currpos = ( _currForme / ( 1 + p_hasGenderDifference ) ) % formes.size( );
        tileCnt = IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 3 ) ].gfxIndex;
        for( u8 i = 0; i < std::min( 5u, formes.size( ) ); ++i ) {
            tileCnt = IO::loadPKMNIcon( formes[ currpos ], 210, 150 - 35 * i, PKMN_SPRITE_START( 3 ) + i, 5 + i, tileCnt, false );
            currpos = ( currpos + 1 ) % formes.size( );
        }
    }

#define IN_DEX(pidx) ( _inDex[ pidx / 8 ] & ( 1 << ( pidx % 8 ) ) )
    void dexUI::drawPage( bool p_newPok ) {
        //Redraw the subscreen iff p_newPok
        if( p_newPok ) {
            IO::drawSub( );

            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", _currPkmn, dexsppos[ 0 ][ 8 ] + 16,
                                dexsppos[ 1 ][ 8 ] + 16, PKMN_ICON_SUB( 5 ), PKMN_SPRITE_SUB_PAL,
                                IO::Oam->oamBuffer[ PKMN_ICON_SUB( 5 ) ].gfxIndex, false );
            if( _inDex ) {
                u16 pidx = ( _currPkmn + _maxPkmn - 3 ) % _maxPkmn;
                for( u8 i = 0; i < 5; ++i ) {
                    if( ( ( ++pidx ) %= _maxPkmn ) == _currPkmn - 1 )
                        pidx = ( pidx + 1 ) % _maxPkmn;
                    IO::loadPKMNIcon( IN_DEX( pidx + 1 ) ? ( pidx + 1 ) : 0, dexsppos[ 0 ][ i ],
                                      dexsppos[ 1 ][ i ], PKMN_ICON_SUB( i ), PKMN_ICON_SUB_PAL( i ),
                                      IO::Oam->oamBuffer[ PKMN_ICON_SUB( i ) ].gfxIndex, true );
                }
            }
        }
        for( u8 i = 0; i < 3; ++i )
            IO::Oam->oamBuffer[ i + PAGE_ICON_START ].isHidden = ( i == _currPage );

        IO::updateOAM( true );

        pokemonData data; getAll( _currPkmn, data );

        //Draw top screen's background
        if( _currPage == 0 )
            FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "DexTop" );
        else if( _currPage == 1 ) {
            if( _currForme % 3 == 0 )
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "BottomScreen2" );
            else if( _currForme % 3 == 0 )
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "BottomScreen3" );
            else
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "BottomScreen2_BG3_KJ" );
        } else {
            dmaCopy( DexTop2Bitmap, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
            dmaCopy( DexTop2Pal, BG_PALETTE, 256 * 2 );
        }
        //Init some colors
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( 251, 1 );
        IO::regularFont->setColor( 252, 2 );
        BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );
        BG_PALETTE[ 252 ] = RGB15( 31, 31, 31 );

        pkmnGenderType acG = GENDERLESS;
        u16 newformepkmn = _currPkmn;
        std::string formeName;
        bool isFixed;
        char buffer[ 50 ];
        if( _currPage != 1 ) {
            if( !_inDex || IN_DEX( _currPkmn ) ) {
                BG_PALETTE[ 0 ] = IO::getColor( data.m_types[ 0 ] );

                isFixed = ( data.m_gender == GENDERLESS ) || ( data.m_gender == MALE ) || ( data.m_gender == FEMALE );

                isFixed &= !!IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", _currPkmn, 30, 64,
                                                 PKMN_SPRITE_START( 0 ), 0, 0, false, false, true, true );
                for( u8 i = PKMN_SPRITE_START( 0 ); i < PKMN_SPRITE_START( 1 ); ++i )
                    IO::OamTop->oamBuffer[ i ].isHidden = true;

                _currForme %= data.m_formecnt ? ( ( isFixed ? 1 : 2 ) * data.m_formecnt ) : 2;

                newformepkmn = data.m_formecnt ? getForme( _currPkmn, _currForme / ( isFixed ? 1 : 2 ), formeName ) : _currPkmn;
                acG = data.m_gender;
                if( data.m_formecnt )
                    getAll( newformepkmn, data );

                //BG_PALETTE[ 1 ] = IO::getColor( data.m_types[ 0 ] );
                IO::loadPKMNIcon( ( _currPkmn == 493 || _currPkmn == 649 ) ? _currPkmn : newformepkmn, 0, 8,
                                  PKMN_SPRITE_START( 2 ), 2, IO::Oam->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );

                IO::loadTypeIcon( data.m_types[ 0 ], 33, 35, PKMN_SPRITE_START( 2 ) + 1, 3,
                                  IO::Oam->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].gfxIndex, false );
                if( data.m_types[ 0 ] != data.m_types[ 1 ] ) {
                    IO::loadTypeIcon( data.m_types[ 1 ], 65, 35, PKMN_SPRITE_START( 2 ) + 2, 3,
                                      IO::Oam->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].gfxIndex, false );
                }
                printf( "\n    Du hast ?? dieser Pok\x82""mon.\n\n" );
                sprintf( buffer, "%s", getDisplayName( _currPkmn ) );
                sprintf( buffer, "%s - %s", buffer, getSpecies( _currPkmn ) );
                IO::regularFont->printString( buffer, 36, 20, false );
                printf( "\n\n %03i", _currPkmn );
            } else {
                printf( "\n    Keine Daten vorhanden.\n\n" );
                sprintf( buffer, "???????????? - %s", getSpecies( 0 ) );
                IO::regularFont->printString( buffer, 36, 20, false );
                printf( "\n\n %03i", _currPkmn );
            }
        }

        switch( _currPage ) {
            case 0:{
                printf( "\x1b[37m" );
                if( !_inDex || IN_DEX( _currPkmn ) ) {
                    BG_PALETTE[ COLOR_IDX ] = IO::getColor( data.m_types[ 1 ] );
                    for( u8 i = 0; i < 6; ++i ) {
                        IO::printRectangle( u8( 19 + 40 * i ), u8( std::max( 56, 102 - data.m_bases[ i ] / 3 ) ),
                                            u8( 37 + 40 * i ), (u8)102, false, true );
                    }
                    printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                    sprintf( buffer, "GW.  %5.1fkg", data.m_weight / 10.0 );
                    IO::regularFont->printString( buffer, 10, 109, false );
                    sprintf( buffer, "GR.  %6.1fm", data.m_size / 10.0 );
                    IO::regularFont->printString( buffer, 100, 109, false );
                    consoleSetWindow( &IO::Top, 1, 16, 30, 24 );
                    printf( getDexEntry( _currPkmn ) );
                } else {
                    printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                    sprintf( buffer, "GW.  ???.?kg" );
                    IO::regularFont->printString( buffer, 10, 109, false );
                    sprintf( buffer, "GR.  ???.?m" );
                    IO::regularFont->printString( buffer, 100, 109, false );
                    consoleSetWindow( &IO::Top, 1, 16, 30, 24 );
                    printf( getDexEntry( 0 ) );
                }
                break;
            }
            case 2: {
                if( !_inDex || IN_DEX( _currPkmn ) ) {
                    drawFormes( newformepkmn, !isFixed && ( _currForme % 2 ), formeName );
                } else {
                    drawFormes( -1, 0, formeName );
                }
            }
        }

        IO::updateOAM( false );
    }
}