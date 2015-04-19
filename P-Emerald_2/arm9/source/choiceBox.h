#pragma once

#include <nds.h>
#include "pokemon.h"

namespace IO {
#define MAX_CHOICES_PER_PAGE 3
#define MAX_CHOICES_PER_SMALL_PAGE 6
    class choiceBox {
    public:
        choiceBox( int p_num, const char** p_choices, const char* p_name, bool p_big );
        choiceBox( pokemon p_pokemon, u16 p_moveToLearn );

        void draw( u8 p_pressedIdx );
        int getResult( const char* p_text = 0, bool p_backButton = false );
        void kill( );
    private:
        const char** _choices;
        bool _big;
        u8 _num;
        const char* _name;
        const char* _text;
        u8 _acPage;

        bool _drawSub;
    };
}