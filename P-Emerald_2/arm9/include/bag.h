/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : bag.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
        Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        is required.

    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
        distribution.
*/

#pragma once
#include <vector>
#include "item.h"

#define MAXITEMCOUNT 999
class bag
{
public:
    
    //Goods,Keys,TMs,Mails,Medicine,Berries,PokéBalls,BattleItems
    typedef item::ITEM_TYPE BAGTYPE;
    bag(){
        for(int i= 0; i< 8; ++i)
            this->bags[i].clear();
    }
    ~bag(){}

    /** Adds cnt items with mo. item_id to the bag.
    */
    void addItem(BAGTYPE,int item_id,int cnt);
    /** Removes cnt items with no. item_id from the bag.
      * cnt == -1: removes all items of desired kind
      */
    void removeItem(BAGTYPE,int item_id,int cnt = -1);
    /** Returnes the number of items with no. item_id in the bag.
    */
    int countItem(BAGTYPE,int item_id);

    /** Returnes true if the specified bag is empty.
    */
    bool empty(BAGTYPE);

    /** Clears the specified bag.
    */
    void clear(BAGTYPE);
    
    /** Returnes the number of items in the specified bag.
    */
    std::size_t size(BAGTYPE);

    std::pair<int,int> element_at(BAGTYPE,int);
    void draw();

    std::vector<std::pair<int,int> > bags[8];
};