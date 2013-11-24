#pragma once
#include <nds.h>
 
namespace font{
    const u8 NUM_CHARS = 128;
    const u8 FONT_WIDTH = 7;
    const u8 FONT_HEIGHT = 5;
    const u8 FONT_MULTIPLY = 2; 
    extern u8 font_widths[NUM_CHARS];
    extern u8 font_data[NUM_CHARS * FONT_WIDTH * FONT_HEIGHT];
}