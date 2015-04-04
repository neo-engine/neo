#pragma once

#include <nds.h>

namespace IO {
#define MAX_CHOICES_PER_PAGE 3
#define MAX_CHOICES_PER_SMALL_PAGE 6
    class choiceBox {
    public:
        choiceBox( int p_num, const char** p_choices, const char* p_name, bool p_big );

        void draw( u8 p_pressedIdx );
        int getResult( const char* p_text = 0, bool p_time = true, bool p_backButton = false );
        void kill( );
    private:
        const char** _choices;
        bool _big;
        u8 _num;
        const char* _name;
        const char* _text;
        u8 _acPage;
    };
}