#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <string>

using namespace std;
  unsigned int buff1[12300] = {0};
  unsigned short int buff2[300] = {0};
const int NUM_TILES = 128;//12288; //9216; 96x96 sprites //128;// for sprites
const int NUM_COLORS = 16;//256; //16;//for sprites
int main(){
  string path;
  getline(cin,path);
  DIR* dir;
  struct dirent* entry;
  if(dir= opendir(path.c_str())){
    while(entry = readdir(dir)){
      if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") != 0){
	string name = path +"/"+ entry->d_name;
	FILE* f = fopen(&name[0],"r");
	if(f){
	  cout << entry->d_name << endl;
	  int lstentry = 0;
	  for(int i= 0; i< NUM_TILES; ++i){
	    fscanf(f,"%x",&(buff1[i]));
	    if(buff1[i])
	      lstentry = i;
	  }
	  for(int i = 0; i< NUM_COLORS; ++i){
	    fscanf(f,"%hx",&(buff2[i]));
	  }
	  f = fopen(&name[0],"wb");
	  fwrite(&buff1[0],sizeof(unsigned int),NUM_TILES,f);
	  fwrite(&buff2[0],sizeof(unsigned short int),NUM_COLORS,f);
	  fclose(f);
	}
      }
    }
    closedir(dir);
  }
}