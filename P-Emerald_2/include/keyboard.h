#include <nds.h>
#include <string>
#include <map>

#include "_keyboard.h"

extern PrintConsole Bottom;
#define MAXKEYBOARDS 3
extern void updateTime();
class keyboard
{
public:
	keyboard():ind(0){}

	std::wstring getText(int length,const char* msg = 0,bool time = true)
	{
		dmaCopy(_keyboardBitmap, bgGetGfxPtr(bg3sub), 256*256);
		dmaCopy(_keyboardPal, BG_PALETTE_SUB, 256*2); 
		if(msg != 0){
			consoleSelect(&Bottom);
			consoleSetWindow(&Bottom,5,3,20,2);
			printf(msg);
		}
		draw();
		std::wstring out;
		consoleSelect(&Bottom);
		touchPosition t;
		int i = 0;
		while(1)
		{
			scanKeys();
			touchRead(&t);  
			if(time)
				updateTime();
			if(i < length && chars.count(std::pair<int,int>(t.px/8,t.py/8)))
			{
				while(1)
				{
					scanKeys();
					if(time)
						updateTime();
					if(keysUp() & KEY_TOUCH)
						break;
				}
				++i;
				consoleSetWindow(&Bottom,13 - (i/3),5,length + 1,1);
				consoleClear();
				out += chars[std::pair<int,int>(t.px/8,t.py/8)];
				printf("%ls",&(out[0]));
				swiWaitForVBlank();
			}
			else if(t.px>224 && t.py>164)
			{
				while(1)
				{
					scanKeys();
					if(time)
						updateTime();
					if(keysUp() & KEY_TOUCH)
						break;
				}
				ind = (ind +1) % MAXKEYBOARDS;
				undraw(); 
				if(msg != 0){

					consoleSelect(&Bottom);
					consoleSetWindow(&Bottom,5,3,20,2);
					printf(msg);
				}
				draw(); 
				if(time)
					updateTime();
				consoleSetWindow(&Bottom,13 - (i/3),5,length + 1,1);
				consoleClear();
				printf("%ls",&(out[0]));
			}
			else if(t.px && t.px <= 26 && t.py <= 30)
			{
				while(1)
				{
					scanKeys();
					if(time)
						updateTime();
					if(keysUp() & KEY_TOUCH)
						break;
				}
				undraw();
				return out;
			}
			else if(i && t.py <= 20 && t.px > 26 && t.px < 48)
			{
				while(1)
				{
					scanKeys();
					if(time)
						updateTime();
					if(keysUp() & KEY_TOUCH)
						break;
				}
				out.erase((out.end() - 1));
				consoleSetWindow(&Bottom,13 - (i/3),5,length + 1,1);
				consoleClear();
				--i;
				consoleSetWindow(&Bottom,13 - (i/3),5,length + 1,1);
				printf("%ls",&(out[0]));
			}
			swiWaitForVBlank();
		}
	}

private:
	int ind;
	std::map<std::pair<int,int>,char> chars;

	void draw()
	{
		consoleSelect(&Bottom);
		consoleSetWindow(&Bottom,4,11,26,13);

		switch (ind)
		{
		case 0://UPPER
			{
				int cnt = 0;
				int rw_cnt = 11,cl_cnt = 4;
				chars.clear();
				for (char i = 'A'; i <= 'Z'; i++)
				{
					if(cnt++ == 13)
					{
						cnt = 0;
						cl_cnt = 4;
						printf("\n");
						printf("\n");
						rw_cnt += 3;
					}
					printf("%c ",i);
					chars[std::pair<int,int>(cl_cnt,rw_cnt)] = i;
					cl_cnt+=2;
				}
				printf("\n");printf("\n");
				printf("Ä Ö Ü\n");
				chars[std::pair<int,int>(4,rw_cnt+3)] = 'Ä';
				chars[std::pair<int,int>(6,rw_cnt+3)] = 'Ö';
				chars[std::pair<int,int>(8,rw_cnt+3)] = 'Ü';
				break;
			}
		case 1://LOWER
			{
				int cnt = 0;
				int rw_cnt = 11,cl_cnt = 4;
				for (char i = 'a'; i <= 'z'; i++)
				{
					if(cnt++ == 13)
					{
						cnt = 0;
						cl_cnt = 4;
						printf("\n");
						printf("\n");
						rw_cnt += 3;
					}
					printf("%c ",i);
					chars[std::pair<int,int>(cl_cnt,rw_cnt)] = i;
					cl_cnt+=2;
				}
				printf("\n");printf("\n");
				printf("\x84 \x94 \x81 ß\n");
				chars[std::pair<int,int>(4,rw_cnt+3)] = '\x84';
				chars[std::pair<int,int>(6,rw_cnt+3)] = '\x94';
				chars[std::pair<int,int>(8,rw_cnt+3)] = '\x81';
				chars[std::pair<int,int>(10,rw_cnt+3)] = 'ß';
				break;
			}
		case 2://NUM
			{
				int rw_cnt = 11,cl_cnt = 4;

				for (char i = '0'; i <= '9'; i++)
				{
					printf("%c ",i);
					chars[std::pair<int,int>(cl_cnt,rw_cnt)] = i;
					cl_cnt+=2;
				}
				printf("\n");printf("\n");printf("\n");
				printf(", . : ! ? + - * / ""\\  %c\n",147);
				chars[std::pair<int,int>(4,rw_cnt+3)] = ',';
				chars[std::pair<int,int>(6,rw_cnt+3)] = '.';
				chars[std::pair<int,int>(8,rw_cnt+3)] = ':';
				chars[std::pair<int,int>(10,rw_cnt+3)] = '!';
				chars[std::pair<int,int>(12,rw_cnt+3)] = '?';
				chars[std::pair<int,int>(14,rw_cnt+3)] = '+';
				chars[std::pair<int,int>(16,rw_cnt+3)] = '-';
				chars[std::pair<int,int>(18,rw_cnt+3)] = '*';
				chars[std::pair<int,int>(20,rw_cnt+3)] = '/';
				chars[std::pair<int,int>(22,rw_cnt+3)] = '\\';
				chars[std::pair<int,int>(25,rw_cnt+3)] = 147;
				printf("\n");printf("\n");
				printf("[ ] ( ) < > { } $ &  %c\n",141);
				chars[std::pair<int,int>(4,rw_cnt+6)] = '[';
				chars[std::pair<int,int>(6,rw_cnt+6)] = ']';
				chars[std::pair<int,int>(8,rw_cnt+6)] = '(';
				chars[std::pair<int,int>(10,rw_cnt+6)] = ')';
				chars[std::pair<int,int>(12,rw_cnt+6)] = '<';
				chars[std::pair<int,int>(14,rw_cnt+6)] = '>';
				chars[std::pair<int,int>(16,rw_cnt+6)] = '{';
				chars[std::pair<int,int>(18,rw_cnt+6)] = '}';
				chars[std::pair<int,int>(20,rw_cnt+6)] = '$';
				chars[std::pair<int,int>(22,rw_cnt+6)] = '&';
				chars[std::pair<int,int>(25,rw_cnt+6)] = 141;
				break;
			}
		}
	}
	void undraw()
	{
		consoleSelect(&Bottom);
		consoleSetWindow(&Bottom,0,0,32,24);
		consoleClear();
	}
};
