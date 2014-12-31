#pragma once
#include <nds/ndstypes.h>
#include <map>
#include <vector>

#include "map2d.h"

namespace map2d {
    enum MoveMode {
        WALK,
        SURF,
        BIKE
    };
    enum FaceDirection {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    struct map2devent {
        Map::pos m_position;
        u8       m_eventType;
    };

    struct map2dperson : public map2devent {
        u8      m_id;
        u16     m_spriteIdx;
        u8      m_moveType;
        u8      m_movement;
        u8      m_isTrainer;
        u16     m_range;
        u16     m_scriptId;
    };

    struct map2dwarp : public map2devent {
        u8      m_targetWarp;
        char    m_targetMap[ 100 ];
    };

    struct map2dscript : public map2devent {
        u16     m_activationCheckId;
        u16     m_activatedValue;
        u16     m_scriptId;
    };

    struct map2dsign : public map2devent {
        u8      m_signType;
        u16     m_scriptId; //Also itemId
        u8      m_itemTakenId;
        u8      m_itemCount; //Also SecretBaseId
    };

    class map2devents {
    public:
        u8      m_personCnt;
        u8      m_warpCnt;
        u8      m_scriptCnt;
        u8      m_signCnt;

        Map*    m_map;

        std::map<Map::pos, std::vector<map2devent> > m_events;

        map2devents( Map* p_map, const char* p_path, const char* p_name );

        void    triggerStepOn( Map::pos p_pos, MoveMode p_moveMode );
        void    triggerInteract( Map::pos p_pos, FaceDirection p_faceDirection );
    };
}