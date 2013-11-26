#pragma once
#include <nds.h>

typedef u16 Color;
namespace font{
    #define RGB(r, g, b) (RGB15((r), (g), (b)) | BIT(15))
    const u8 SCREEN_TOP = 1;
    const u8 SCREEN_BOTTOM = 0;
    void putrec(int x1,int y1,int x2,int y2, bool bottom = true, bool striped = false,int color = 42);

    class Font{
    public:
        // Constructors
        Font(u8 *the_data, u8 *the_widths);
        ~Font();
        
        // Accessors
        void set_color(Color new_color,int num) { color[num] = new_color; }
        Color get_color(int num) const { return color[num]; }
        
        // Methods
        void print_char(char ch, s16 x, s16 y,int scale,bool bottom);
        void print_string(const char *string, s16 x, s16 y,int scale,bool bottom);
        void print_string_center(const char *string,int scale,bool bottom);
        void print_string_d(const char *string, s16 x, s16 y,int scale,bool bottom);
        void print_string_center_d(const char *string,int scale,bool bottom);
        void print_number(s32 num, s16 x, s16 y,int scale,bool bottom);
        u32 string_width(const char *string) const;
        
    private:
        u8 *data;
        u8 *widths;
        Color color[3];
    };
}

void top_screen_darken();
void btm_screen_darken();
void top_screen_plot(u8 x, u8 y, Color color,int scale);
void btm_screen_plot(u8 x, u8 y, Color color,int scale);