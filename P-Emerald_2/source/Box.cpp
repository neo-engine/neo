#include "scrnloader.h"
#include "PKMN.h"
#include <vector>

class berry;

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
extern savgm SAV;

extern PrintConsole Top,Bottom;

POKEMON::PKMN::BOX_PKMN stored_pkmn[MAXSTOREDPKMN];
std::vector<int> box_of_st_pkmn[MAXPKMN];
std::vector<int> free_spaces;

extern PrintConsole Top,Bottom;
void dPage(int Page)
{

}

extern OAMTable *oam;
extern SpriteInfo* spriteInfo;
const int MAXPKMNINBOXLIST = 7;
int drawBox(short PkmnInd)
{
    --PkmnInd;
    oam->oamBuffer[A_ID].isHidden = true;
    oam->oamBuffer[BACK_ID].isHidden = true;
    for(int i= BORDER_ID; i< BORDER_ID + 10; ++i)
        oam->oamBuffer[i].isHidden = true;
    updateOAMSub(oam);
    
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","PKMNInfoScreen");

    consoleSetWindow(&Top,0,0,SCREEN_WIDTH/8,SCREEN_HEIGHT/8);
    consoleSelect(&Top);	
    consoleClear();	
    // Load bitmap to bottom background
    if(!POKEMON::drawInfoSub(bgGetGfxPtr(bg3sub), PkmnInd+1))
    {
        dmaCopy( PKMNInfoScreenBottomBitmap, bgGetGfxPtr(bg3sub), 256*256 );
        dmaCopy( PKMNInfoScreenBottomPal, BG_PALETTE_SUB, 256*2); 
    }
    consoleSelect(&Bottom);
    consoleClear();
    
    consoleSelect(&Bottom);
    consoleSetWindow(&Bottom,1,1,32,24);
    printf("Pok\x82""mon in dieser Box:\n\n>");
    consoleSetWindow(&Bottom,2,3,30,20);

    int page = 0,pagemax = 3;

    int acin = 0, max_ = std::min(MAXPKMNINBOXLIST,(int)box_of_st_pkmn[PkmnInd].size());
    std::string status_[] = {"   ","BSG","BRT","GIF","SLF","PAR","GFR"};
    for (int i = 0; i < max_; i++)
    {
        POKEMON::PKMN::BOX_PKMN& acPKMN = stored_pkmn[box_of_st_pkmn[PkmnInd][i]];
        wprintf(&acPKMN.Name[0]);
        if(acPKMN.PKRUS % 16)
            printf(" PKRS");
        else if(acPKMN.PKRUS)
            printf(" *");
        //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
        //printf(" %s\n",&status_[acPKMN._status][0]);
    }
    dPage(page);
    
    touchPosition touch;
    while(1)
    {
        touchRead(&touch);
        swiWaitForVBlank();
        scanKeys();
        int pressed = keysDown();

        
        if((pressed & KEY_DOWN) || ((touch.py>178)&&(touch.px <= 220)&&(touch.px > 200)))
        {
            while(1)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH)
                    break;
                if(keysUp() & KEY_DOWN)
                    break;
            }
            DOWN:
            
            consoleSelect(&Bottom);
            consoleClear();
            if((int)box_of_st_pkmn[PkmnInd].size()-acin-1)
                ++acin;
            max_ = acin + std::min(MAXPKMNINBOXLIST,(int)box_of_st_pkmn[PkmnInd].size());
            for (int i = acin; i < max_; i++)
            {
                POKEMON::PKMN::BOX_PKMN& acPKMN = stored_pkmn[box_of_st_pkmn[PkmnInd][i]];
                wprintf(&acPKMN.Name[0]);
                if(acPKMN.PKRUS % 16)
                    printf(" PKRS");
                else if(acPKMN.PKRUS)
                    printf(" *");
                //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
                //printf(" %s\n",&status_[acPKMN._status][0]);
            }
        }
        else if((pressed & KEY_UP)|| ((touch.py>178)&&(touch.px <= 240)&&(touch.px > 220)))
        {
            while(1)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH)
                    break;
                if(keysUp() & KEY_UP)
                    break;
            }
            UP:
            
            consoleSelect(&Bottom);
            consoleClear();
            if((int)box_of_st_pkmn[PkmnInd].size()-acin-1)
                --acin;
            max_ = acin + std::min(MAXPKMNINBOXLIST,(int)box_of_st_pkmn[PkmnInd].size());
            for (int i = acin; i < max_; i++)
            {
                POKEMON::PKMN::BOX_PKMN& acPKMN = stored_pkmn[box_of_st_pkmn[PkmnInd][i]];
                wprintf(&acPKMN.Name[0]);
                if(acPKMN.PKRUS % 16)
                    printf(" PKRS");
                else if(acPKMN.PKRUS)
                    printf(" *");
                //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
                //printf(" %s\n",&status_[acPKMN._status][0]);
            }
        }
        else if((pressed & KEY_B)||((touch.py>178)&&(touch.px > 240)))
        {
            while(1)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH)
                    break;
                if(keysUp() & KEY_B)
                    break;
            }
            consoleSelect(&Bottom);
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            swiWaitForVBlank();		
            return pressed | KEY_B;
        }
        else if (pressed & KEY_RIGHT|| ((touch.py>178)&&(touch.px > 20)&&(touch.px <= 40)))
        {
            if (++page == pagemax)
                page = 0;
            while(1)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH)
                    break;
                if(keysUp() & KEY_RIGHT)
                    break;
            }
            dPage(page);
        }
        else if (pressed & KEY_LEFT|| ((touch.py>178)&&(touch.px <= 20)))
        {
            if (--page == -1)
                page = pagemax-1;
            while(1)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH)
                    break;
                if(keysUp() & KEY_LEFT)
                    break;
            }
            dPage(page);
        }
        else if(pressed & KEY_L)
        {
            consoleSelect(&Bottom);
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            return KEY_L;
        }
        else if(pressed & KEY_R)
        {
            consoleSelect(&Bottom);
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            return KEY_R;
        }
        else if(pressed & KEY_A)
        {
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            int ret = stored_pkmn[box_of_st_pkmn[PkmnInd][acin]].draw();
            /*if(ret & KEY_UP)
                goto UP;
            else if(ret & KEY_DOWN)
                goto DOWN;*/
            consoleSetWindow(&Bottom,1,1,32,24);
            consoleSelect(&Bottom);
            printf("Pok\x82""mon in dieser Box:\n\n>");
            consoleSetWindow(&Bottom,2,3,30,20);
            for (int i = acin; i < max_; i++)
            {
                POKEMON::PKMN::BOX_PKMN& acPKMN = stored_pkmn[box_of_st_pkmn[PkmnInd][i]];
                wprintf(&acPKMN.Name[0]);
                if(acPKMN.PKRUS % 16)
                    printf(" PKRS");
                else if(acPKMN.PKRUS)
                    printf(" *");
                //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
                //printf(" %s\n",&status_[acPKMN._status][0]);
            }
        }
    }
}