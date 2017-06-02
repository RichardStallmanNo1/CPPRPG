#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <limits.h>
#include <unistd.h>   //_getch*/
#include <termios.h>  //_getch*/
#include "mhead.h"

struct Location {
	int x;
	int y;
};

struct QuestStruct {
	int numId;
	std::string name;
	unsigned char type;
	int monItemX;
	int amtY;
	unsigned char completed;
	unsigned char qRequired;
	unsigned char mapStart;
};

struct PlayerStruct {
    std::string username;
	Location pLoc; //will call by Player.pLoc.X
	unsigned char curMap;
	int bHp;
	int bAtk;
	int bDef; 
	int bLuk;
	int Hp;
	int Atk;
	int Def;
	int Luk;
	long xp;
	long neededXpl
	long level;
	unsigned char invSize;
	unsigned char resInvSize;
	unsigned char direction;
	unsigned char isMoving;
	QuestStruct quest;
};

struct MapStruct {
	std::string tile;
	unsigned char isCollidable;
	unsigned char isInteractable;
};

struct ModifierStruct{
	std::string name;
	int numId;
	double HpPerc;
	double AtkPerc;
	double DefPerc;
	double LukPerc;
};

struct ItemStruct { //moving chance to drop item into monster i think
	std::string name;
	int numId;
	int bHp;
	int bAtk;
	int bDef;
	int bLuk;
	int Hp;
	int Atk;
	int Def;
	int Luk;
	int Set;
	ModifierStruct modifier;
	unsigned char type;
};

struct SetStruct {
	std::string flavorTxt;
	int numId;
	int Hp;
	int Atk;
	int Def;
	int Luk;
};

struct MonsterStruct {
	std::string name;
	int Hp;
	int Atk;
	int Def;
	int Luk;
	double spawnChance;
	unsigned char alive;
	long xp;
	std::vector<int> ItemDrops;
	std::vector<double> ItemDropChances;
};

static const unsigned char UP = 1;
static const unsigned char DOWN = 2;
static const unsigned char LEFT = 3;
static const unsigned char RIGHT = 4;
static const unsigned char notMoving = 0;
static const unsigned char isMoving = 1;
static const int MapH = 50;
static const int MapW = 50;
static const std::string cwd = getcwd(NULL, 0);
static const std::string pdir = cwd + "/content/stats/player/";

std::vector<ItemStruct> ItemList;
std::vector<ItemStruct> Inventory;
std::vector<ResItemStruct> ResInventory;
std::vector<ItemStruct> EquipList (7);
std::vector<QuestStruct> QuestList;

MapStruct Map[MapH][MapW];
PlayerStruct Player;



int main() {
    std::string username;
    std::cout << "Username [Case Sensitive]: " << std::endl;
    std::cin >> Player.username;
    
    if (!checkPlayerExists()) {
        makePlayer();
        //std::cout << cwd + "/content/stats/player/" + ////Player.username + "inv.txt";
        std::cout << "PLayer NOT existed!" << std::endl;
    }
	setupGame();
	//gameLoop();
	return 0;
}

bool checkPlayerExists() {
   //return honestlyfuckthis;
    std::ifstream myFile;
    myFile.open(pdir + Player.username + "inv.txt");
    return (bool)myFile;
}

void makePlayer() {
    makeItemLists(); //inv, resinv, eqitem
    makeQuest(); //curquest, qlist
    makeStats(); //map(current map + loc), sizes, stats
}

void makeItemLists() {
    std::ofstream inv(pdir +Player.username + "inv.txt");
    inv << "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0" << std::endl;
    inv.close();
    std::ofstream rinv(pdir +Player.username + "rinv.txt");
    rinv << "0" << std::endl;
    rinv.close();
    std::ofstream eqitem(pdir +Player.username + "eqitem.txt");
    eqitem << "0,0,0,0,0,0,0" << std::endl;
    eqitem.close();
}

void makeQuest() {
    std::ofstream cq(pdir +Player.username + "curquest.txt");
    cq << "0,0" << std::endl;
    cq.close();
    std::ofstream ql(pdir +Player.username + "qlist.txt");
    ql << "0" << std::endl; //change to for totalquest
    ql.close();
}

void makeStats() {
    std::ofstream map(pdir +Player.username + "map.txt");
    map << "1,5,5" << std::endl;
    map.close();
    std::ofstream sizes(pdir +Player.username + "sizes.txt");
    sizes << "15,0" << std::endl;
    sizes.close();
    std::ofstream stats(pdir +Player.username + "stats.txt");
    stats << "10,1,1,1,1,0,1" << std::endl;
    stats.close();
}


void setupGame() {
	setupMap();
	setupLists();
	setupPlayer();
}

void setupMap() {
	for(int i = 0; i < MapH; i++) {
		for(int k = 0; k < MapW; k++) {
			Map[i][k].tile = "*";
			Map[i][k].isCollidable = 0;
			Map[i][k].isInteractable = 0;
		}
	}
}

void setupPlayer() {
	getStats();
	postStats();
	getItemLists();
	getQuests();
	setupItems();
}

void getItemLists() {
	std::ifstream inv(pdir + Player.username + "inv.txt");
	for (int i = 0; i < Inventory.size(); i++) {
		getline(inv, Inventory.at(i).numId, ',');
	}
	inv.close();
	std::ifstream rinv(pdir + Player.username + "rinv.txt");
	for (int i = 0; i < ResInventory.size(); i++) {
		getline(rinv, ResInventory.at(i).numId, ',');
	}
	rinv.close();
	std::ifstream eq(pdir + Player.username + "eqitem.txt");
	for (int i = 0; i < EquipList.size(); i++) {
		getline(eq, EquipList.at(i).numId, ',');
	}
	eq.close();
}
void getQuests() {
	int questList = 0;
	std::string st = "";
	std::ifstream cq(pdir + Player.username + "curquest.txt");
		cq >> Player.quest.monItemX >> Player.quest.amtY;
	cq.close();
	std::ifstream ql(pdir + Player.username + "qlist.txt"); //have to go through this twice...
	while (ql.good) {
		getline(ql, st, ',');
		questList++;
	}
	ql.close();
	QuestList.resize(questList);
	std::ifstream ql(pdir + Player.username + "qlist.txt");
	for (int i = 0; i < QuestList.size(); i++){
		getline(ql, QuestList.at(i).numId, ',');
	}
	ql.close();
}
void getStats() {
	std::ifstream stats(pdir + Player.username + "stats.txt");
	stats >> Player.bHp >> Player.bAtk >> Player.bDef >> Player.bLuk >> Player.direction >> Player.xp >> Player.level;
	stats.close();
	std::ifstream sizes(pdir + Player.username + "sizes.txt");
	sizes >> Player.invSize >> Player.resInvSize;
	sizes.close();
	std::ifstream map(pdir + Player.username + "map.txt");
	map >> Player.curMap >> Player.pLoc.x >> Player.pLoc.y;
	map.close();
}
void postStats() {
	Inventory.resize(Player.invSize);
	ResInventory.resize(Player.resInvSize);
}

void gameLoop() {

	char in;
	
	do {
		in = getch();
		in = tolower(in);
		
		switch(in) {
			case 'w':
				updateDir(UP, isMoving);
				break;
			case 's':
				updateDir(DOWN, isMoving);
				break;
			case 'a':
				updateDir(LEFT, isMoving);
				break;
			case 'd':
				updateDir(RIGHT, isMoving);
				break;
		}

		std::cout << "Player X: " << Player.pLoc.x << '\n' << "Player Y: " << Player.pLoc.y << '\n';
	//updateWorld();
	updatePlayer();
	//displayWorld();
	//displayPlayer();
	} while (in != 'q' && in != 'Q');
}

void updateDir(unsigned char dir, unsigned char mv) {
	Player.direction = dir;
	Player.isMoving = mv;
}

void updatePlayer() {
	if (Player.isMoving == isMoving) {movePlayer(Player.direction);}
}

void movePlayer(unsigned char dir) {
switch(dir) {
case UP:
	if (Player.pLoc.y - 1 >= 0) {Player.pLoc.y--;}
	break;
case DOWN:
	if (Player.pLoc.y + 1 <= MapH) {Player.pLoc.y++;}
	break;
case LEFT:
	if (Player.pLoc.x - 1 >= 0) {Player.pLoc.x--;}
	break;
case RIGHT:
	if (Player.pLoc.x + 1 <= MapW) {Player.pLoc.x++;}
	break;
}
}


char getch(){
    
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    return buf;
 }

std::string getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
}
