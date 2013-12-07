#include <nds.h>
#include <string>
#include <map>

extern PrintConsole Bottom;
#define MAXKEYBOARDS 3
extern void updateTime();
class keyboard
{
public:
    keyboard():ind(0){}

    std::wstring getText(int length,const char* msg = 0,bool time = true);

private:
    int ind;
    std::map<std::pair<int,int>,char> chars;

    void draw();
    void undraw();
};
