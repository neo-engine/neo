//#include <string>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <fat.h>
//#include <nds.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
//
//#include "scrnloader.h"
//#include "mbox.h"
//
//struct YNB_set;
//extern YNB_set YNBs[2];
//
//struct BG_set;
//extern BG_set BGs[2];
//extern int BG_ind;
//
//vector<std::string> poss;
//
//bool initPoss()
//{
//	FILE* fd = fopen("nitro:/PICS/NAV/info","r");
//
//	if(fd == 0)
//		return false;
//	int num;
//	fscanf(fd,"%i",&num);
//	poss.clear();
//	for (int i = 0; i < num; i++)
//	{
//		char buf[20];
//		fscanf(fd,"%s",buf);
//		poss.push_back(string(buf));
//	}
//	fclose(fd);
//	return true;
//}
//
//bool loadDefault(scrnloader& scrn,const int& ind)
//{
//	BG_ind = 1;
//	scrn.draw(ind);
//	return true;
//}