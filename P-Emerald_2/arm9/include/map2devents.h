#pragma once
#include <nds/ndstypes.h>

namespace map2d {
    struct map2dperson {
        u8      m_id;
        u16     m_spriteIdx;
        u16     m_posX;
        u16     m_posY;
        u8      m_posZ;
        u8      m_moveType;
        u8      m_movement;
        u8      m_isTrainer;
        u16     m_range;
        u16     m_scriptId;
    };

    struct map2dwarp {
        u16     m_posX;
        u16     m_posY;
        u8      m_posZ;
        u8      m_targetWarp;
        char    m_targetMap[ 100 ];
    };

    struct map2dscript {
        u16     m_posX;
        u16     m_posY;
        u8      m_posZ;
        u16     m_activationCheckId;
        u16     m_activatedValue;
        u16     m_scriptId;
    };

    struct map2dsign {
        u16     m_posX;
        u16     m_posY;
        u8      m_posZ;
        u8      m_signType;
        u16     m_scriptId; //Also itemId
        u8      m_itemTakenId;
        u8      m_itemCount; //Also SecretBaseId
    };

    struct map2devents {
        u8      m_personCnt;
        u8      m_warpCnt;
        u8      m_scriptCnt;
        u8      m_signCnt;
    };
}