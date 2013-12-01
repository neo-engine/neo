#include <nds.h>
#include <string.h>
#include "font_data.h"
#include "print.h"
#include "mbox.h"

namespace font{
    void putrec(int x1,int y1,int x2,int y2, bool bottom,bool striped,int color){
        for(int x = x1; x <= x2; ++x) for(int y = y1; y < y2; ++y)
            if(bottom)
                ((Color *)BG_BMP_RAM_SUB(1))[(x + y * SCREEN_WIDTH)/2] = !striped ? (((u8)color) << 8 ) | ((u8)color) : color;
            else
                ((Color *)BG_BMP_RAM(1))[(x + y * SCREEN_WIDTH)/2] = !striped ? (((u8)color) << 8 ) | ((u8)color) : color;
    }
    Font::Font(u8 *the_data, u8 *the_widths) {
        data = the_data;
        widths = the_widths;
        color[0] = color[1] = color[2] = RGB(31, 31, 31);
    }

    Font::~Font() {
    }

    void Font::print_char(char ch, s16 x, s16 y,bool bottom) {
        s16 put_x, put_y;
        u8 get_x, get_y;
        u32 offset = (u8)ch * FONT_WIDTH * FONT_HEIGHT;
    
        for (put_y = y,get_y = 0; put_y < y + FONT_HEIGHT; ++put_y,++get_y) 
            for (put_x = x, get_x = 0; put_x < x + widths[(u8)ch]; put_x+= 2, get_x+=2)
                if (data[offset + (get_x + get_y * FONT_WIDTH)] && data[offset + (1 + get_x + get_y * FONT_WIDTH)]) {
                    if (put_x >= 0 && put_x < SCREEN_WIDTH && put_y >= 0 && put_y < SCREEN_HEIGHT)
                         if(!bottom) 
                             top_screen_plot(put_x, put_y, ((u8)(color[data[1+offset + (get_x + get_y * FONT_WIDTH)]]) << 8) | 
                             (u8) (color[data[offset + ( get_x + get_y * FONT_WIDTH)]] ));
                         else 
                             btm_screen_plot(put_x, put_y, color[data[offset + (get_x + get_y * FONT_WIDTH)]]);  
                }
                else if (data[offset + (1+get_x + get_y * FONT_WIDTH)]) 
                    if (put_x >= 0 && put_x < SCREEN_WIDTH && put_y >= 0 && put_y < SCREEN_HEIGHT)
                         if(!bottom) 
                             top_screen_plot(put_x, put_y, ((u8)(color[data[1+offset + (get_x + get_y * FONT_WIDTH)]]) << 8));
                         else 
                             btm_screen_plot(put_x, put_y, color[data[offset + (get_x + get_y * FONT_WIDTH)]]);  
                else if (data[offset + (get_x + get_y * FONT_WIDTH)]) 
                    if (put_x >= 0 && put_x < SCREEN_WIDTH && put_y >= 0 && put_y < SCREEN_HEIGHT)
                         if(!bottom) 
                             top_screen_plot(put_x, put_y, ((u8)(color[data[offset + (get_x + get_y * FONT_WIDTH)]])));
                         else 
                             btm_screen_plot(put_x, put_y, color[data[offset + (get_x + get_y * FONT_WIDTH)]]);  
    }
    
    void Font::print_string(const char *string, s16 x, s16 y,bool bottom) {
        u32 current_char = 0;
        s16 put_x = x, put_y = y;
    
        while (string[current_char]) {
            print_char(string[current_char], put_x, put_y,bottom);
            put_x += widths[(u8)string[current_char]] + 1;
        
            current_char++;
        }
    }
    void Font::print_string_d(const char *string, s16 x, s16 y,bool bottom) {
        u32 current_char = 0;
        s16 put_x = x, put_y = y;
    
        while (string[current_char]) {
            print_char(string[current_char], put_x, put_y,bottom);
            put_x += widths[(u8)string[current_char]] + 1;
            
            for(int i= 0; i < 80/TEXTSPEED; ++i)
                swiWaitForVBlank();
            current_char++;
        }
    }
    
    void Font::print_string_center(const char *string,bool bottom) {
        s16 x = SCREEN_WIDTH / 2 - string_width(string) / 2;
        s16 y = SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2;
    
        print_string(string, x, y,bottom);
    }
    void Font::print_string_center_d(const char *string,bool bottom) {
        s16 x = (SCREEN_WIDTH / 2 - string_width(string) / 2);
        s16 y = (SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2);
    
        print_string_d(string, x, y,bottom);
    }

    void Font::print_number(s32 num, s16 x, s16 y,bool bottom) {
        char numstring[10] = "";
        u32 number = num, quotient = 1, remainder = 0;
        char remainder_str[3] = "";
        u32 current_char = 0, current_char2 = 0;
        static char string[100];
    
        if (number == 0) {
            strcpy(string, "0");
        } else {
            while (quotient != 0) {
                remainder = number % 10;
                quotient = number / 10;
                remainder_str[0] = remainder+48;
                remainder_str[1] = '\0';
                strcat(numstring, remainder_str);
                number = quotient;
            }
        
            current_char = strlen(numstring)-1;
            while (current_char != 0) {
                string[current_char2] = numstring[current_char];
                current_char--;
                current_char2++;
            }
            string[current_char2] = numstring[current_char];
            string[current_char2 + 1] = '\0';
        }
    
        print_string(string, x, y,bottom);
    }


    u32 Font::string_width(const char *string) const {
        u32 current_char = 0;
        u32 width = 0;
    
        while (string[current_char]) {
            width += widths[(u8)string[current_char]] + 1;
        
            current_char++;
        }
    
        return width - 1;
    }
}

void top_screen_darken() {
    u16 i;
    Color pixel;
    
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        pixel = ((Color *)BG_BMP_RAM(1))[i];
        ((Color *)BG_BMP_RAM(1))[i] = ((pixel & 0x1F) >> 1) |
                                  ((((pixel >> 5) & 0x1F) >> 1) << 5) |
                                  ((((pixel >> 10) & 0x1F) >> 1) << 10) |
                                  (1 << 15);
    }
}
void top_screen_plot(u8 x, u8 y, Color color) {
    if((color >> 8) != 0 && (color % (1 <<8)) != 0)
        ((Color *)BG_BMP_RAM(1))[(x + y * SCREEN_WIDTH)/2] = color ; 
    else if((color >> 8) != 0)
        ((Color *)BG_BMP_RAM(1))[(x + y * SCREEN_WIDTH)/2] = color | (((Color *)BG_BMP_RAM(1))[(x + y * SCREEN_WIDTH)/2] % (1<<8)); 
    else if((color % (1 <<8)) != 0)
        ((Color *)BG_BMP_RAM(1))[(x + y * SCREEN_WIDTH)/2] = color | (((Color *)BG_BMP_RAM(1))[(x + y * SCREEN_WIDTH)/2] >> 8); 
}
void btm_screen_darken() {
    u16 i;
    Color pixel;
    
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        pixel = ((Color *)BG_BMP_RAM_SUB(1))[i];
        ((Color *)BG_BMP_RAM_SUB(1))[i] = ((pixel & 0x1F) >> 1) |
                                  ((((pixel >> 5) & 0x1F) >> 1) << 5) |
                                  ((((pixel >> 10) & 0x1F) >> 1) << 10) |
                                  (1 << 15);
    }
}
void btm_screen_plot(u8 x, u8 y, Color color) {((Color *)BG_BMP_RAM_SUB(1))[(x + y * SCREEN_WIDTH)/2] = color; }