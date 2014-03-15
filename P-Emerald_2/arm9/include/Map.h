//#pragma once
//
//using namespace std;
//#include <string>
//#include <n3d.h>
//#include <n3d\nds\arm9\n3ddevice.h>
//#include "n3dsampleframework.h"
//
//#include "mbox.h"
//
//struct Point{ int x,y,z; Point(int X = 0,int Y= 0,int Z=0): x(X),y(Y),z(Z){} };
//
//class Map
//{
//public:
//	enum Movetype{
//		SURF,
//		WALK,
//		BIKE,
//		ROD
//	} movetype;
//	Point acPos;
//	N3DFLOAT rotation;
//	string name;
//	char* moveData;	/*	MoveData:
//	00 - immer begehbar
//	01 - fest
//	02 - Gehen E0
//	03 - Surfen E0
//	04 - Fest E0
//	05 - Warp E0
//	06 - Nur mit Rad befahrbar E0
//	07 - Gehen oder fahren E0
//	...
//	0A - Gehen E1
//	0B - Surfen E1
//	0C - Fest E1
//	0D - Warp E1
//	...
//
//	Normal: E2
//	*/ 
//	pair<int,Point>* warpData;
//	int index;
//	int SX;
//	int SY;
//	int SZ;
//	short pkmn_grass[10];
//	short pkmn_rod[10];
//	short pkmn_water[10];
//
//	Map(int ind, string N, int sx = 16,int sy = 16,int sz = 16) 
//		:name(N),index(ind)
//	{
//		warpData = new pair<int,Point>[sx*sy];
//		moveData = new char[sx*sy];
//		for (int i = 0; i < sx*sy; i++)
//		{
//			/*if(i%sy < 2)
//			{
//				warpData[i] = pair<int,Point>(ind,Point(i/sy,SY-2,0));
//				moveData[i] =5;
//			}
//			else if(i%sy > SY-2 )
//			{
//				warpData[i] = pair<int,Point>(ind,Point(i/sy,2,0));
//				moveData[i] =5;
//			}
//			else if(i/sy < 2)
//			{
//				warpData[i] = pair<int,Point>(ind,Point(SX-2,i%sy,0));
//				moveData[i] =5;
//			}
//			else if(i/sy > SX -2)
//			{
//				warpData[i] = pair<int,Point>(ind,Point(2,i%sy,0));
//				moveData[i] = 5;
//			}
//			else*/
//			{
//				warpData[i] = pair<int,Point>(0,Point());
//				moveData[i] = 0;
//			}
//		}
//		rotation = 0;
//		acPos = Point(4,4,0);
//		SX = sx;
//		SY = sy;
//		SZ = sz;
//	}
//	Map(int ind,string N, pair<int,Point>* wD,char* mD,int sx = 16,int sy = 16,int sz = 16)
//		:name(N),index(ind)
//	{
//		warpData = new pair<int,Point>[sx*sy];
//		moveData = new char[sx*sy];
//		for (int i = 0; i < sx*sy; i++)
//		{
//			warpData[i] = wD[i];
//			moveData[i] = mD[i];
//		}
//		rotation = 0;
//		acPos = Point();
//		acPos.x = 0;
//		acPos.y = 0;
//		acPos.z = 0;
//		SX = sx;
//		SY = sy;
//		SZ = sz;
//	}
//	Map(): index(0),SX(1),SY(1),SZ(1)
//{
//		warpData = new pair<int,Point>[1];
//		moveData = new char[1];
//	}
//	~Map(){
//		delete warpData;
//		delete moveData;
//	}
//
//	short getPKMN(Movetype M){
//		int r = rand() % 55;
//		short* List;
//		switch (M)
//		{
//		case Map::SURF:
//			List = pkmn_water;
//			break;
//		case Map::WALK:
//			List = pkmn_grass;
//			break;
//		case Map::BIKE:
//			List = pkmn_grass;
//			break;
//		case Map::ROD:
//			List = pkmn_rod;
//			break;
//		}
//		for (int i = 1, j = 1; i < 11; i++,j+= i)
//			if(r < j)
//				return List[10 - i];
//		return 0;
//	}
//
//	void render();
//	void load(Point);
//
//	void setPos(Point);
//	void setRot(N3DFLOAT);
//};