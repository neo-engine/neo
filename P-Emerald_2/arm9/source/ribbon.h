#pragma once

#include <string>
#include <vector>

#include <nds.h>
#include "pokemon.h"

struct ribbon {
    std::string m_name;
    std::string m_description;

    static std::vector<u8> getRibbons( const pokemon& p_pokemon );
};

#define MAX_RIBBONS (12*8)
extern ribbon RibbonList[ MAX_RIBBONS ];
