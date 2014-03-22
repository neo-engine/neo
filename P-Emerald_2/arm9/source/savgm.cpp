/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : savgm.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for parsing game save files

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



#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fat.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <nds.h>
#include <fcntl.h>
#include <unistd.h>

#include "PKMN.h"
#include "savgm.h"
#include "Gen.h"

std::string sav_nam = "./p_smaragd_2.sav";/*"nitro:/SAV";*/ //as nitro:/SAV doesn't seem to work :( ...
std::string sav_nam_2 = "./p_smaragd_2.gba.sav";/*"nitro:/SAV";*/ //as nitro:/SAV doesn't seem to work :( ...
extern SavMod savMod;
#define PKMN_DATALENGTH 128
extern const short POKEMON::OTLENGTH;
extern const short POKEMON::PKMN_NAMELENGTH;

extern POKEMON::PKMN::BOX_PKMN stored_pkmn[MAXSTOREDPKMN];
extern std::vector<int> box_of_st_pkmn[MAXPKMN];
extern std::vector<int> free_spaces;

extern "C"{    extern long hexdec(unsigned const char *hex); }

savgm::savgm(void Func(int))
{
    int booltemp = true;
    FILE* fd = fopen(&sav_nam[0],"r");
    if(fd == 0)
    {
        good = false;
        this->SavTyp = 0;
        return;
    }
    fscanf(fd,"%i ",&this->SavTyp);
    if(this->SavTyp == 0){
        good = true;
        return;
    }
    wchar_t buf[POKEMON::OTLENGTH];
    fscanf(fd,"%ls",buf);
    this->setName(std::wstring(buf));
    fscanf(fd,"%i %i",&this->ID,&this->SID);
    Func(10);
    fscanf(fd,"%i %i",&booltemp,&this->HOENN_Badges);
    this->IsMale = bool(booltemp);
    Func(20);
    fscanf(fd,"%i %i %hi %i %i %u",&this->KANTO_Badges,&this->JOHTO_Badges,&this->Orden,&this->Dex,&this->Money,&this->PLAYTIME);
    fscanf(fd,"%i",&booltemp);
    this->hasPKMN = bool(booltemp);
    Func(30);

    fscanf(fd,"%i %i %i %i %s %i ",&acposx,&acposy,&acposz,&acMapIdx,acMapName,&acMoveMode);
    fscanf(fd,"%hu",&this->owIdx);

    int a;
    for(int i= 0; i < 8; ++i){
        fscanf(fd," %i ",&a);
        this->Bag.bags[i].assign(a,std::pair<int,int>(0,0));
        fread(&this->Bag.bags[i][0],sizeof(std::pair<int,int>),a,fd);
    }

    fscanf(fd," %i ",&a);
    this->PKMN_team = std::vector<POKEMON::PKMN>(a);
    for(int i= 0; i< a; ++i)
        fread(&this->PKMN_team[i],sizeof(POKEMON::PKMN),1,fd);

    free_spaces.clear();
    for (int i = 0; i < MAXSTOREDPKMN; i++){
        //stored_pkmn[i] = PKMN(fd);
        fread(&stored_pkmn[i],sizeof(POKEMON::PKMN::BOX_PKMN),1,fd);
        if(stored_pkmn[i].SPEC == 0 || stored_pkmn[i].SPEC > MAXPKMN)
            free_spaces.push_back(i);
        else
            box_of_st_pkmn[stored_pkmn[i].SPEC].push_back(i);
    }
    Func(40);

    for (int i = 0; i < 649; i++)
    {
        fscanf(fd," %i ",&booltemp);
        inDex[i] = bool(booltemp);
        Func(40 + (30*(i/649)));
    }

    fscanf(fd," %i ",&booltemp);
    hasGDex = bool(booltemp);
    fread(this->flags,1,1000,fd);
    Func(100);
    fclose(fd);
    good = true;

    //todo
    EXPShareEnabled = false;
    EvolveInBattle = true;
}

bool saveGBA(void Func(int)){
    FILE* fd = fopen(&sav_nam_2[0],"wb");
    if(fd == 0)
        return false;
    fwrite(gen3::SaveParser::Instance()->unpackeddata,sizeof(u8),14 * sizeof(gen3::block),fd);
    fclose(fd);
    return true;
}

bool savgm::save(void Func(int))
{
    if(savMod == _GBA)
        return saveGBA(Func);

    int booltemp;
    FILE* fd = fopen(&sav_nam[0],"w");
    if(fd == 0)
        return false;
    fprintf(fd,"%i ",this->SavTyp);
    for(size_t i = 0; i < this->PLAYERNAME.size(); ++i)
        fprintf(fd,"%lc",this->PLAYERNAME[i]);
    for(size_t i = this->PLAYERNAME.size(); i < POKEMON::OTLENGTH; ++i)
        fprintf(fd,"%lc",0);
    fprintf(fd," ");
    fprintf(fd,"%i %i ",this->ID,this->SID);
    Func(10);
    booltemp = this->IsMale;
    fprintf(fd,"%i %i ",booltemp,this->HOENN_Badges);
    Func(20);
    fprintf(fd,"%i %i %hi %i ",this->KANTO_Badges,this->JOHTO_Badges,this->Orden,this->Dex);
    booltemp = this->hasPKMN;
    fprintf(fd,"%i %u %i ",this->Money,this->PLAYTIME,booltemp);
    Func(30);

    fprintf(fd,"%i %i %i %i %s %i ",acposx,acposy,acposz,acMapIdx,acMapName,acMoveMode);
    fprintf(fd,"%hu",this->owIdx);

    for(int i= 0; i < 8; ++i){
        fprintf(fd," %i ",this->Bag.bags[i].size());
        fwrite(this->Bag.bags[i].data(),sizeof(std::pair<int,int>),this->Bag.bags[i].size(),fd);
    }

    fprintf(fd," %i ",(this->PKMN_team).size());
    for(size_t i = 0; i < this->PKMN_team.size(); ++i)
        //I->save(fd);
            fwrite(&(PKMN_team[i]),1,sizeof(POKEMON::PKMN),fd);
    for (int i = 0; i < MAXSTOREDPKMN; i++){
        //stored_pkmn[i].save(fd);
        fwrite(&(stored_pkmn[i]),sizeof(POKEMON::PKMN::BOX_PKMN),1,fd);
    }

    Func(40);
    for (int i = 0; i < 649; i++)
    {
        booltemp = (inDex[i]?1:0);
        fprintf(fd," %i",booltemp);
    }
    Func(50);


    booltemp = hasGDex;
    fprintf(fd," %i ",booltemp);

    fwrite(this->flags,1,1000,fd);
    Func(100);
    fclose(fd);
    return true;
}