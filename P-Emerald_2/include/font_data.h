#pragma once
#include <nds.h>
 
namespace font{
    const u8 NUM_CHARS = 513;
    const u8 FONT_WIDTH = 16;
    const u8 FONT_HEIGHT = 16;
    extern u8 font_widths[513];
    extern u8 font_data[513 * FONT_WIDTH * FONT_HEIGHT];
}