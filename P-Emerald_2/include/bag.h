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
private:
    std::vector<std::pair<int,int> > bags[8];
};