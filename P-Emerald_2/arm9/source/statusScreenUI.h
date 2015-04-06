#pragma once

#include "pokemon.h"
#include <vector>

namespace STS {
    class statusScreenUI {
    protected:
        friend class statusScreen;
        friend class regStsScreen;

        std::vector<pokemon>* _pokemon;

        bool _showTakeItem;
        u8 _showMoveCnt;

        virtual void init( u8 p_current, bool p_initTop = true ) = 0;
        virtual bool draw( u8 p_current, u8 p_moveIdx ) = 0;
        virtual void draw( u8 p_current, u8 p_page, bool p_newpok ) = 0;
        virtual void draw( u8 p_current ) = 0;
    public:
        u8 m_pagemax;
    };

    class regStsScreenUI : public statusScreenUI {
        u8 _current;
        void initTop( );
        void initSub( u16 p_pkmIdx );

    public:

        void init( u8 p_current, bool p_initTop = true ) override;
        bool draw( u8 p_current, u8 p_moveIdx ) override;
        void draw( u8 p_current, u8 p_page, bool p_newpok ) override;
        void draw( u8 p_current ) override;

        regStsScreenUI( std::vector<pokemon>* p_pokemon, u8 p_pageMax = 3 );
    };

    //class battleStsScreenUI : public statusScreenUI {
    //public:
    //    void init( u8 p_current ) override;
    //    void draw( u8 p_current, u8 p_page, bool p_newpok ) override;
    //    void draw( u8 p_current ) override;
    //};
}