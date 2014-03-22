/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : Bag.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Bag engine.

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

#include "bag.h"

#include <algorithm>

void bag::addItem(BAGTYPE bag_type,int item_id,int cnt){
    for(auto I = this->bags[bag_type].begin();
        I != this->bags[bag_type].end(); ++I)
        if(I->first == item_id){
            if(cnt + I->second <=MAXITEMCOUNT){
                I->second += cnt;
                return;
            }
            I->second = MAXITEMCOUNT;
            cnt -= MAXITEMCOUNT - I->second;
        }
        this->bags[bag_type].push_back(std::pair<int,int>(item_id,cnt));
        std::sort(this->bags[bag_type].begin(),this->bags[bag_type].end());
}
void bag::removeItem(BAGTYPE bag_type,int item_id,int cnt){
    for(auto I = this->bags[bag_type].begin();
        I != this->bags[bag_type].end(); ++I)
        if(I->first == item_id){
            int num = I->second;
            if(cnt && ((I->second - cnt) > 0)){
                I->second -= cnt;
                std::sort(this->bags[bag_type].begin(),this->bags[bag_type].end());
                return;
            }
            else{
                this->bags[bag_type].erase(I);
                cnt -= num;
                break;
            }
        }
        std::sort(this->bags[bag_type].begin(),this->bags[bag_type].end());
}
int bag::countItem(BAGTYPE bag_type,int item_id){
    int cnt = 0;
    for(auto I = this->bags[bag_type].begin();
        I != this->bags[bag_type].end(); ++I)
        if(I->first == item_id)
            cnt += I->second;
    return cnt;
}

bool bag::empty(BAGTYPE bag_type){ return this->bags[bag_type].empty(); }
std::size_t bag::size(BAGTYPE bag_type){ return this->bags[bag_type].size(); }

std::pair<int,int> bag::element_at(BAGTYPE bag_type,int index){return this->bags[int(bag_type)][index]; }

void bag::clear(BAGTYPE bag_type){this->bags[bag_type].clear(); }
