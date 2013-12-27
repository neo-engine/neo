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

std::string sav_nam = "./p_smaragd_2.sav";/*"nitro:/SAV";*/ //as nitro:/SAV doesn't seem to work :( ...
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
    fscanf(fd,"%i %i %hi %i %i %lld",&this->KANTO_Badges,&this->JOHTO_Badges,&this->Orden,&this->Dex,&this->Money,&this->PLAYTIME);
    fscanf(fd,"%i",&booltemp);
    this->hasPKMN = bool(booltemp);
    Func(30);
    
    fscanf(fd,"%i %i %i %i %s ",&acposx,&acposy,&acposz,&acMapIdx,acMapName);

    int a; fscanf(fd,"%i ",&a);
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
        fscanf(fd,"%i ",&booltemp);
        inDex[i] = bool(booltemp);
        Func(40 + (30*(i/649)));
    }
    
    fscanf(fd,"%i ",&booltemp);
    hasGDex = bool(booltemp);
    Func(100);
    fclose(fd);
    good = true;
}

bool savgm::save(void Func(int))
{
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
    fprintf(fd,"%i %lld %i ",this->Money,this->PLAYTIME,booltemp);
    Func(30);
    
    fprintf(fd,"%i %i %i %i %s ",acposx,acposy,acposz,acMapIdx,acMapName);

    fprintf(fd,"%i ",(this->PKMN_team).size());
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
        fprintf(fd,"%i ",booltemp);
    }
    Func(50);

    
    booltemp = hasGDex;
    fprintf(fd,"%i ",booltemp);
    Func(100);
    fclose(fd);
    return true;
}