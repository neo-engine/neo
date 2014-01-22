#include "Gen.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include "PKMN.h"
#include "savgm.h"

extern savgm SAV;

namespace gen3{
#define MAX_TRAINER_NAME_LEN 7
#define MAX_NICKNAME_LEN 10

    char text[] = {"                                                                                                                                                                 0123456789!?.- ?\"\"''?? , /ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz                 "};

    int getNItemIdx(int gen3Idx){
        return gen3Idx;
    }
    int getNLocation(int gen3Idx){
        if(gen3Idx < 88)
            return gen3Idx + 235;
        if(gen3Idx < 99)
            return gen3Idx - 88 + 138;
        if(gen3Idx == 99)
            return 152;
        if(gen3Idx == 100)
            return 158;
        if(gen3Idx < 126)
            return gen3Idx - 101 + 149;

        return 322 + gen3Idx;
    }
    static void smemcpy(u8* dest, const u8* src, int size) {
        while(size--) 
            *dest++ = *src++;
    }
    SaveParser* SaveParser::spInstance = NULL;

    int SaveParser::get_newest_save(block *blocks[NUM_BLOCKS_TOTAL]) {
        int i, newestSave = 0;

        for (i = 0; i < NUM_BLOCKS_TOTAL; i++)
            if (blocks[i]->footer.savenumber > newestSave)
                newestSave = blocks[i]->footer.savenumber;
        return newestSave;
    }

    u16 SaveParser::get_block_checksum(block* b) {
        int checksum = 0;
        int i;
        for (i = 0; i < BLOCK_DATA_LEN; i+=4) 
            checksum += *((int*)b+i/4);
        checksum += checksum >> 16;
        checksum &= 0xFFFF;

        return (u16)checksum;
    }

    char* SaveParser::parse_save(block *blocks[NUM_BLOCKS_TOTAL])
    {
        char *data;
        int i, newestSave;

        for (i = 0; i < NUM_BLOCKS_TOTAL; i++){
            blocks[i] = new block();
            smemcpy((u8*)(blocks[i]),SRAM + i*(sizeof(block)),sizeof(block));
        }

        newestSave = get_newest_save(blocks);

        data = (char*)malloc(SAVESLOT_LEN);
        for (i = 0; i < NUM_BLOCKS_TOTAL; i++)
            if (blocks[i]->footer.savenumber == newestSave)
                memcpy(data + BLOCK_DATA_LEN * blocks[i]->footer.blocknum, blocks[i]->data, BLOCK_DATA_LEN);
        return data;
    }

    int SaveParser::pack_save(char *unpackeddata, block *blocks[NUM_BLOCKS_TOTAL], char savefile[SAVEFILE_LEN])
    {
        FILE *f;
        int i, newestSave;
        int tempCount = 0;

        newestSave = get_newest_save(blocks);

        // Re-split into blocks and place over buffer
        for(i = 0; i < NUM_BLOCKS_TOTAL; i++)
            if (blocks[i]->footer.savenumber == newestSave) {
                memcpy(blocks[i]->data, unpackeddata + BLOCK_DATA_LEN * blocks[i]->footer.blocknum, BLOCK_DATA_LEN);
                // Re-calculate and set this block's checksum
                blocks[i]->footer.checksum = get_block_checksum(blocks[i]);
                tempCount++;
            }
            free(unpackeddata);

            // Save back to file
            if ((f = fopen((char*) SRAM, "wb")) == NULL)
                return -1;

            if (fwrite(savefile, SAVEFILE_LEN, 1, f) != 1) {
                fclose(f);

                return -1;
            }
            fclose(f);

            return 0;
    }

    char* SaveParser::get_text(u8* raw, bool is_nickname)
    {
        char* actual_text;
        int len;

        if(is_nickname)
        {
            actual_text = new char[MAX_NICKNAME_LEN];
            len = MAX_NICKNAME_LEN;
        }
        else
        {
            actual_text = new char[MAX_TRAINER_NAME_LEN];
            len = MAX_TRAINER_NAME_LEN;
        }

        for(int i = 0; i < len; i++)
        {
            if(int(raw[i]) != 255) actual_text[i] = text[int(raw[i])];
            else actual_text[i] = '\0';
        }

        return actual_text;
    }

    //void SaveParser::print_pokemon(box_pokemon_t* pokemon)
    //{
    //    PKMN::pokemon_attacks_t *pa;
    //    PKMN::pokemon_effort_t *pe;
    //    PKMN::pokemon_growth_t *pg;
    //    PKMN::pokemon_misc_t *pm;
    //    int o, totalIVs, totalEVs;
    //    char* nickname = get_text(pokemon->name, true);
    //
    //    // Figure out the order
    //    o = pokemon->personality % 24;
    //    pa = (PKMN::pokemon_attacks_t *)(pokemon->data + DataOrderTable[o][0] * sizeof(PKMN::pokemon_attacks_t));
    //    pe = (PKMN::pokemon_effort_t *)(pokemon->data + DataOrderTable[o][1] * sizeof(PKMN::pokemon_effort_t));
    //    pg = (PKMN::pokemon_growth_t *)(pokemon->data + DataOrderTable[o][2] * sizeof(PKMN::pokemon_growth_t));
    //    pm = (PKMN::pokemon_misc_t *)(pokemon->data + DataOrderTable[o][3] * sizeof(PKMN::pokemon_misc_t));
    //
    //    totalIVs = pm->IVs.hp + pm->IVs.atk + pm->IVs.def + pm->IVs.spatk + pm->IVs.spdef + pm->IVs.spd;
    //    totalEVs = pe->hp + pe->attack + pe->defense + pe->spatk + pe->spdef + pe->speed;
    //    fprintf(stdout, "Species: %s, Nickname: %s, held: %s, Nature: %s\n", pokemon_species[pg->species], nickname, items[pg->held], natures[pokemon->personality % 25]);
    //    fprintf(stdout, "Attacks: 1:%s, 2:%s, 3:%s, 4:%s\n", attacks[pa->atk1], attacks[pa->atk2], attacks[pa->atk3], attacks[pa->atk4] );
    //    fprintf(stdout, "IVs:\tHP:%d\tAtk:%d\tDef:%d\tSpA:%d\tSpD:%d\tSpe:%d\tTotal:%d\n", pm->IVs.hp, pm->IVs.atk, pm->IVs.def, pm->IVs.spatk, pm->IVs.spdef, pm->IVs.spd, totalIVs );
    //    fprintf(stdout, "EVs:\tHP:%d\tAtk:%d\tDef:%d\tSpA:%d\tSpD:%d\tSpe:%d\tTotal:%d\n", pe->hp, pe->attack, pe->defense, pe->spatk, pe->spdef, pe->speed, totalEVs );
    //
    //    delete[] nickname;
    //}

    int SaveParser::parse_pokemon(char* buf, int offset, void** pokemon, PKMN::pokemon_attacks_t** pa, PKMN::pokemon_effort_t** pe,
        PKMN::pokemon_growth_t** pg, PKMN::pokemon_misc_t** pm, int num, int size)
    {
        int i;
        if (size != sizeof(belt_pokemon_t) && size != sizeof(box_pokemon_t))
            return -1;

        // Parse pokemon
        for(i = 0; i < num; i++)
        {
            int o;

            // Read data on pokemon
            pokemon[i] = (buf + offset + (i * size));

            // Unencrypt pokemon's data
            // box and belt pokemon have these struct members all at
            // the same offset so we can cast to either type
            SaveParser::Instance()->encrypt(((box_pokemon_t*)pokemon[i])->data, ((box_pokemon_t*)pokemon[i])->personality, ((box_pokemon_t*)pokemon[i])->otid);

            // Figure out the order
            o = ((box_pokemon_t*)pokemon[i])->personality % 24;
            pa[i] = (PKMN::pokemon_attacks_t *)(((box_pokemon_t*)pokemon[i])->data + DataOrderTable[o][0] * sizeof(PKMN::pokemon_attacks_t));
            pe[i] = (PKMN::pokemon_effort_t *)(((box_pokemon_t*)pokemon[i])->data + DataOrderTable[o][1] * sizeof(PKMN::pokemon_effort_t));
            pg[i] = (PKMN::pokemon_growth_t *)(((box_pokemon_t*)pokemon[i])->data + DataOrderTable[o][2] * sizeof(PKMN::pokemon_growth_t));
            pm[i] = (PKMN::pokemon_misc_t *)(((box_pokemon_t*)pokemon[i])->data + DataOrderTable[o][3] * sizeof(PKMN::pokemon_misc_t));
        }

        return 0;
    }

    SaveParser* SaveParser::Instance()
    {
        if (!spInstance) // If an instance hasn't already been created, so do
            spInstance = new SaveParser();

        return spInstance;
    }

    int SaveParser::load(int game) {
        // Parse save
        unpackeddata = parse_save(blocks);
        if (unpackeddata == NULL)
            return -1;
        // Decode belt part
        parse_pokemon(unpackeddata, belt_offsets[game], (void**)pokemon, pokemon_attacks,
            pokemon_effort, pokemon_growth, pokemon_misc, NUM_BELT_POKEMON, sizeof(belt_pokemon_t));

        return 0;
    }

    int SaveParser::save() {
        // Re encrypt and set checksum
        for(int i = 0; i < NUM_BELT_POKEMON; i++)
            pokemon[i]->checksum = encrypt(pokemon[i]->data, pokemon[i]->personality, pokemon[i]->otid);

        if (pack_save(unpackeddata, blocks, (char*)SRAM))
            return -1;
        return 0;
    }

    /*
    *	Encrypts/decrypts the 48 byte data buffer based on the xored pv and otid values
    *  TODO: Make it encrypt its own data/not need arguments
    */
    u16 SaveParser::encrypt(u8 *data, u32 pv, u32 otid) {
        u32 xorkey = pv ^ otid;
        u16 checksum = 0;
        u32 i;

        for (i = 0; i < POKEMON_DATA_LENGTH; i+=4)
        {
            checksum += data[i+1]<<8 | data[i];
            checksum += data[i+3]<<8 | data[i+2];

            data[i] ^= (xorkey >> 0)&0xFF;
            data[i+1] ^= (xorkey >> 8)&0xFF;
            data[i+2] ^= (xorkey >> 16)&0xFF;
            data[i+3] ^= (xorkey >> 24)&0xFF;
        }

        return checksum;

    }

}