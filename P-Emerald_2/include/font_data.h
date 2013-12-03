#pragma once
#include <nds.h>
 
namespace font{
    namespace font1{
#define NUM_CHARS 490
        void shiftchar(u16& val);
        extern u8 font_widths[NUM_CHARS];
        extern u8 font_data[NUM_CHARS * 256];
    }
    namespace font2{
#define NUM_CHARS 490
        void shiftchar(u16& val);
        extern u8 font_widths[NUM_CHARS];
        extern u8 font_data[NUM_CHARS * 256];
    }
}