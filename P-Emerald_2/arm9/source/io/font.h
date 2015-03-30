#pragma once
#include <nds.h>

namespace IO {
#define NUMFONTS 2

    typedef u16 color;

    namespace REGULAR_FONT {
#define NUM_CHARS 490
        void shiftchar( u16& val );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    }
    namespace BOLD_FONT {
#define NUM_CHARS 490
        void shiftchar( u16& val );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    }

    class font {
    public:
        // Constructors
        font( u8 *p_fontData, u8 *p_characterWidths, void( *p_shiftchar )( u16& val ) );

        // Accessors
        void setColor( color p_newColor, int p_num ) {
            _color[ p_num ] = p_newColor;
        }
        color getColor( int p_num ) const {
            return _color[ p_num ];
        }

        // Methods
        void printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom );
        void printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance = 16 );
        void printStringCenter( const char *p_string, bool p_bottom );
        void printStringD( const char *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void printStringCenterD( const char *p_string, bool p_bottom );
        void printNumber( s32 p_num, s16 p_x, s16 p_y, bool p_bottom );
        u32 stringWidth( const char *p_string ) const;

        void printString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance = 16 );
        void printMBString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
        void printMBString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
        void printMBStringD( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
        void printMBStringD( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
        void printStringCenter( const wchar_t *p_string, bool p_bottom );
        void printStringD( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void printStringCenterD( const wchar_t *p_string, bool p_bottom );
        u32 stringWidth( const wchar_t *p_string ) const;

    private:
        u8 *_data;
        u8 *_widths;
        void( *_shiftchar )( u16& val );
        color _color[ 5 ];
    };
}