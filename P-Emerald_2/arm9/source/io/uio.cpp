#include <nds.h>

#include "uio.h"
#include "../defines.h"
#include "../ds/type.h"

namespace IO {
    font* regularFont = new font( REGULAR_FONT::fontData, REGULAR_FONT::fontWidths, REGULAR_FONT::shiftchar );
    font* boldFont = new font( BOLD_FONT::fontData, BOLD_FONT::fontWidths, BOLD_FONT::shiftchar );
    ConsoleFont* consoleFont = new ConsoleFont( );

    u8 ASpriteOamIndex = 0;

    OAMTable *Oam = new OAMTable( );
    SpriteInfo spriteInfo[ SPRITE_COUNT ];

    OAMTable *OamTop = new OAMTable( );
    SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

    u8 TEXTSPEED = 35;

    PrintConsole Top, Bottom;

    int bg3sub;
    int bg2sub;
    int bg3;
    int bg2;

#define IN_RANGE( p_touch, p_input ) ( (p_touch).px >= (p_input).m_targetX1 && (p_touch).py >= (p_input).m_targetY1 \
                                    && (p_touch).px <= (p_input).m_targetX2 && (p_touch).py <= (p_input).m_targetY2 )

    bool waitForTouchUp( bool p_updateTime, bool p_timeParameter, u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 ) {
        return waitForTouchUp( p_updateTime, p_timeParameter, inputTarget( p_targetX1, p_targetY1, p_targetX2, p_targetY2 ) );
    }
    bool waitForTouchUp( bool p_updateTime, bool p_timeParameter, inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::TOUCH ) {
            while( 1 ) {
                swiWaitForVBlank( );
                scanKeys( );
                if( p_updateTime )
                    updateTime( p_timeParameter );
                auto touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    return true;
                if( !IN_RANGE( touch, p_inputTarget ) )
                    return false;
            }
        }
        return false;
    }


    void waitForKeysUp( bool p_updateTime, bool p_timeParameter, KEYPAD_BITS p_keys ) {
        return waitForKeysUp( p_updateTime, p_timeParameter, inputTarget( p_keys ) );
    }
    void waitForKeysUp( bool p_updateTime, bool p_timeParameter, inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON ) {
            while( 1 ) {
                swiWaitForVBlank( );
                scanKeys( );
                if( p_updateTime )
                    updateTime( p_timeParameter );
                auto touch = touchReadXY( );
                if( ( keysUp( ) & p_inputTarget.m_keys ) == p_inputTarget.m_keys )
                    return;
            }
        }
    }

    bool waitForInput( bool p_updateTime, bool p_timeParameter, inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON ) {
            waitForKeysUp( p_updateTime, p_timeParameter, p_inputTarget );
            return true;
        }
        return waitForTouchUp( p_updateTime, p_timeParameter, p_inputTarget );
    }


    void initTextField( ) {
        regularFont->setColor( 0, 0 );
        regularFont->setColor( BLACK_IDX, 1 );
        regularFont->setColor( GRAY_IDX, 2 );
        boldFont->setColor( 0, 0 );
        boldFont->setColor( GRAY_IDX, 1 );
        boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = STEEL;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
        BG_PALETTE_SUB[ BLUE_IDX ] = BLUE;
        printRectangle( (u8)0, (u8)0, (u8)255, (u8)63, true, false, WHITE_IDX );
    }

    void putrec( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, bool p_striped, u8 p_color ) {
        printRectangle( p_x1, p_y1, p_x2, p_y2, p_bottom, p_striped, p_color );
    }
    void printRectangle( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, bool p_striped, u8 p_color ) {
        for( u16 x = p_x1; x <= p_x2; ++x ) for( u16 y = p_y1; y < p_y2; ++y )
            if( p_bottom )
                ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( x + y * (u16)SCREEN_WIDTH ) / 2 ] = !p_striped ? ( ( (u8)p_color ) << 8 ) | ( (u8)p_color ) : p_color;
            else
                ( (color *)BG_BMP_RAM( 1 ) )[ ( x + y * (u16)SCREEN_WIDTH ) / 2 ] = !p_striped ? ( ( (u8)p_color ) << 8 ) | ( (u8)p_color ) : p_color;
    }


    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_colorIdx, bool p_pressed ) {
        if( !p_pressed ) {
            printRectangle( p_x1 + 1, p_y1 + 1, p_x2, p_y2,
                            true, false, BLACK_IDX );
            printRectangle( p_x1, p_y1, p_x2 - 2, p_y2 - 1,
                            true, false, p_colorIdx );
            printRectangle( p_x1 + 1 + p_borderWidth, p_y1 + p_borderWidth - 1, p_x2 - p_borderWidth, p_y2 - p_borderWidth + 4,
                            true, false, BLACK_IDX );
            printRectangle( p_x1 + p_borderWidth, p_y1 + p_borderWidth - 2, p_x2 - p_borderWidth - 2, p_y2 - p_borderWidth + 3,
                            true, false, WHITE_IDX );
        } else {
            printRectangle( p_x1, p_y1, p_x2 - 1, p_y2 - 1,
                            true, false, BLACK_IDX );
            printRectangle( p_x1 + 2, p_y1 + 1, p_x2, p_y2,
                            true, false, p_colorIdx );

            printRectangle( p_x1 + p_borderWidth, p_y1 + p_borderWidth - 2, p_x2 - p_borderWidth - 1, p_y2 - p_borderWidth + 3,
                            true, false, BLACK_IDX );

            printRectangle( p_x1 + 2 + p_borderWidth, p_y1 + p_borderWidth - 1, p_x2 - p_borderWidth, p_y2 - p_borderWidth + 4,
                            true, false, WHITE_IDX );
        }
    }


    void printChar( font* p_font, u16 p_ch, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printChar( p_ch, p_x, p_y, p_bottom );
    }
    void printString( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance ) {
        p_font->printString( p_string, p_x, p_y, p_bottom, p_yDistance );
    }
    void printStringCenter( font* p_font, const char *p_string, bool p_bottom ) {
        p_font->printStringCenter( p_string, p_bottom );
    }
    void printStringD( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printStringD( p_string, p_x, p_y, p_bottom );
    }
    void printStringCenterD( font* p_font, const char *p_string, bool p_bottom ) {
        p_font->printStringCenterD( p_string, p_bottom );
    }
    void printNumber( font* p_font, s32 p_num, s16 p_x, s16 p_y, bool p_bottom );

    void printString( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance ) {
        p_font->printString( p_string, p_x, p_y, p_bottom, p_yDistance );
    }
    void printMBString( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime, s8 p_updateTimePar ) {
        p_font->printMBString( p_string, p_x, p_y, p_bottom, p_updateTime, p_updateTimePar );
    }
    void printMBString( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime, s8 p_updateTimePar ) {
        p_font->printMBString( p_string, p_x, p_y, p_bottom, p_updateTime, p_updateTimePar );
    }
    void printMBStringD( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime, s8 p_updateTimePar ) {
        p_font->printMBStringD( p_string, p_x, p_y, p_bottom, p_updateTime, p_updateTimePar );
    }
    void printMBStringD( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime, s8 p_updateTimePar ) {
        p_font->printMBStringD( p_string, p_x, p_y, p_bottom, p_updateTime, p_updateTimePar );
    }
    void printStringCenter( font* p_font, const wchar_t *p_string, bool p_bottom ) {
        p_font->printStringCenter( p_string, p_bottom );
    }
    void printStringD( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printStringD( p_string, p_x, p_y, p_bottom );
    }
    void printStringCenterD( font* p_font, const wchar_t *p_string, bool p_bottom ) {
        p_font->printStringCenterD( p_string, p_bottom );
    }

    void topScreenDarken( ) {
        u16 i;
        color pixel;

        for( i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ ) {
            pixel = ( (color *)BG_BMP_RAM( 1 ) )[ i ];
            ( (color *)BG_BMP_RAM( 1 ) )[ i ] = ( ( pixel & 0x1F ) >> 1 ) |
                ( ( ( ( pixel >> 5 ) & 0x1F ) >> 1 ) << 5 ) |
                ( ( ( ( pixel >> 10 ) & 0x1F ) >> 1 ) << 10 ) |
                ( 1 << 15 );
        }
    }
    void topScreenPlot( u8 p_x, u8 p_y, color p_color ) {
        if( ( p_color >> 8 ) != 0 && ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color;
        else if( ( p_color >> 8 ) != 0 )
            ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] % ( 1 << 8 ) );
        else if( ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] << 8 );
    }
    void btmScreenDarken( ) {
        u16 i;
        color pixel;

        for( i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ ) {
            pixel = ( (color *)BG_BMP_RAM_SUB( 1 ) )[ i ];
            ( (color *)BG_BMP_RAM_SUB( 1 ) )[ i ] = ( ( pixel & 0x1F ) >> 1 ) |
                ( ( ( ( pixel >> 5 ) & 0x1F ) >> 1 ) << 5 ) |
                ( ( ( ( pixel >> 10 ) & 0x1F ) >> 1 ) << 10 ) |
                ( 1 << 15 );
        }
    }
    void btmScreenPlot( u8 p_x, u8 p_y, color p_color ) {
        if( ( p_color >> 8 ) != 0 && ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color;
        else if( ( p_color >> 8 ) != 0 )
            ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] % ( 1 << 8 ) );
        else if( ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] << 8 );
    }

    u16 getColor( Type p_type ) {
        switch( p_type ) {
            case NORMAL:
                return NORMAL_;
                break;
            case KAMPF:
                return RED;
                break;
            case FLUG:
                return TURQOISE;
                break;
            case GIFT:
                return POISON;
                break;
            case BODEN:
                return GROUND;
                break;
            case GESTEIN:
                return ROCK;
                break;
            case KAEFER:
                return BUG;
                break;
            case GEIST:
                return GHOST;
                break;
            case STAHL:
                return STEEL;
                break;
            case UNBEKANNT:
                return UNKNOWN;
                break;
            case WASSER:
                return BLUE;
                break;
            case FEUER:
                return ORANGE;
                break;
            case PFLANZE:
                return GREEN;
                break;
            case ELEKTRO:
                return YELLOW;
                break;
            case PSYCHO:
                return PURPLE;
                break;
            case EIS:
                return ICE;
                break;
            case DRACHE:
                return DRAGON;
                break;
            case UNLICHT:
                return BLACK;
                break;
            case FEE:
                return FAIRY;
                break;
            default:
                return DRAGON;
                break;
        }
        return WHITE;
    }
}