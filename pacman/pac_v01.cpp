#include <iostream>
#include <fstream>
#include <allegro.h>
#include <time.h>
using namespace std;

#define DIM 10
#define HALL 'X'
#define PAC 'P'
#define DIAMOND '-'
#define DEMON 'D'
#define EMPTY 'Y'
#define SPRITE_SIZE 50
#define UP 3
#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define BORD 10

bool load_map(char* map);
void stamp_map();
void doppiobuffering();
void init_map(int n, int dir, int score);
void sfondo(int color);
int set_direction(int old_dir);
void find_pac(int& x, int& y);
void move(int dir, int& posx, int& posy);
bool can_move(int dir, int posx, int posy);
int gnam(int posx, int posy);
void find_demon(int& x, int& y);
void move_demon(int dir, int& posx, int& posy, int& prox);
bool dead(int pac_x, int pac_y, int demon_x, int demon_y);
int rand_dir();

char mappa[DIM][DIM];

BITMAP* buff; //PRIMA SCRIVO SUL BUFF, POI A SCHERMO

BITMAP* hall;
BITMAP* pac[2];
BITMAP* diamond;
BITMAP* demon;
BITMAP* bk;

int main(){
	allegro_init();
	install_keyboard();
	install_timer();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,DIM*SPRITE_SIZE,DIM*SPRITE_SIZE+BORD,0,0);

	char map[9] = "map0.map";
	load_map(map);

	hall = load_bitmap("hall.bmp",NULL);
	pac[0] = load_bitmap("pac02.bmp",NULL);
	pac[1] = load_bitmap("pac04.bmp",NULL);
	diamond = load_bitmap("diamond.bmp",NULL);
	demon = load_bitmap("demon.bmp",NULL);
	bk= load_bitmap("backscreen.bmp",NULL);
	
	buff = create_bitmap(screen->w,screen->h);	//SETTO GRANDEZZA BUFF
	clear(buff);	//PULISCO BUFF

	draw_sprite(screen,bk,0,5);

	int pac_dir = RIGHT;
	int pac_x,pac_y;
	find_pac(pac_x,pac_y);
	int n=0;
	bool died = false;

	int demon_dir = LEFT;
	int demon_x,demon_y;
	find_demon(demon_x,demon_y);
	int prox = EMPTY;
	int points = 0;

	srand(time(0));
	rest(5000);
	while(!key[KEY_ESC]){	//INIZIO CICLO DI GIOCO
		doppiobuffering();
		rest(100);
		init_map(n,pac_dir,points);
		pac_dir=set_direction(pac_dir);
		demon_dir=rand_dir();
		move_demon(demon_dir,demon_x,demon_y,prox);
		if(!died){
			move(pac_dir,pac_x,pac_y);
			points += gnam(pac_x,pac_y);
			died = dead(pac_x,pac_y,demon_x,demon_y);
		}
		if(n==0)
			n=1;
		else
			n=0;
	}

	destroy_bitmap(buff);
	destroy_bitmap(hall);
	destroy_bitmap(pac[0]);
	destroy_bitmap(pac[1]);
	destroy_bitmap(diamond);
	destroy_bitmap(bk);

	return 0;
}
END_OF_MAIN();

bool load_map(char* map){
	ifstream in;
	in.open(map);
	for(int i=0; i<DIM; i++){
		for(int j=0; j<DIM; j++){
			in >> mappa[i][j];
		}
	}
	in.close();
	return true;	
}

void stamp_map(){
	for(int i=0; i<DIM; i++){
		for(int j=0; j<DIM; j++){
			cout << mappa[i][j];
		}
		cout << endl;
	}
}

void doppiobuffering(){
	vsync();
	blit(buff,screen,0,0,0,0,screen->w,screen->h);
	clear(buff);
}

void init_map(int n, int dir, int score){
	sfondo(makecol(50,50,50));
	for(int i=0; i<DIM; i++){
		for(int j=0; j<DIM; j++){
			if(mappa[i][j]==HALL)
				draw_sprite(buff,hall,j*SPRITE_SIZE,i*SPRITE_SIZE);
			else if(mappa[i][j]==DIAMOND)
				draw_sprite(buff,diamond,j*SPRITE_SIZE,i*SPRITE_SIZE);
			else if(mappa[i][j]==PAC){
				//draw_sprite(buff,pac[n],j*SPRITE_SIZE,i*SPRITE_SIZE);
				rotate_sprite(buff,pac[n],j*SPRITE_SIZE,i*SPRITE_SIZE,itofix(dir*64));
			}
			else if(mappa[i][j]==DEMON)
				draw_sprite(buff,demon,j*SPRITE_SIZE,i*SPRITE_SIZE);
		}
	}
	textprintf_ex(buff, font, 10, screen->h-BORD, makecol(255, 255, 255), -1, "SCORE: %d", score);
}

void sfondo(int color){
	clear_to_color(buff,color);
}

int set_direction(int old_dir){
	int dir = old_dir;
	if(key[KEY_RIGHT]){
		dir = RIGHT;
	}
	else if(key[KEY_LEFT]){
		dir = LEFT;
	}
	else if(key[KEY_DOWN]){
		dir = DOWN;
	}
	else if(key[KEY_UP]){
		dir = UP;
	}
	return dir;
}

bool can_move(int dir, int posx, int posy){
	bool can_m = true;
	if(dir == RIGHT)
		posy++;
	else if(dir == LEFT)
		posy--;
	else if(dir == UP)
		posx--;
	else if (dir == DOWN)
		posx++;
	if(mappa[posx][posy] == HALL)
		can_m = false;
	return can_m;
}

void find_pac(int& x, int& y){
	for(int i=0; i<DIM; i++){
		for(int j=0; j<DIM; j++){
			if(mappa[i][j]==PAC){
				x=i;
				y=j;
			}
		}
	}
}

void move(int dir, int& posx, int& posy){
	if(can_move(dir,posx,posy)){
		if(mappa[posx][posy] != DEMON)
			mappa[posx][posy] = EMPTY;
		if(dir == RIGHT)
			posy++;
		else if(dir == LEFT)
			posy--;
		else if(dir == UP)
			posx--;
		else if (dir == DOWN)
			posx++;
	}
}

int gnam(int posx, int posy){
	int score = 0;
	if(mappa[posx][posy] == DIAMOND)
		score += 100;
	mappa[posx][posy] = PAC;
	return score;
}


void find_demon(int& x, int& y){
	for(int i=0; i<DIM; i++){
		for(int j=0; j<DIM; j++){
			if(mappa[i][j]==DEMON){
				x=i;
				y=j;
			}
		}
	}
}

void move_demon(int dir, int& posx, int& posy, int& prox){
	if(can_move(dir,posx,posy)){
		mappa[posx][posy] = prox;
		if(dir == RIGHT)
			posy++;
		else if(dir == LEFT)
			posy--;
		else if(dir == UP)
			posx--;
		else if (dir == DOWN)
			posx++;
	}
	if(mappa[posx][posy] != DEMON && mappa[posx][posy] != PAC)
		prox = mappa[posx][posy];
	if(mappa[posx][posy] == PAC)
		prox = EMPTY;
	mappa[posx][posy] = DEMON;
}

bool dead(int pac_x, int pac_y, int demon_x, int demon_y){
	bool dead = false;
	if((pac_x == demon_x) && (pac_y == demon_y)){
		mappa[pac_x][pac_y] = DEMON;
		dead = true;
	}
	return dead;
}

int rand_dir(){
	int dir = rand()%4;
	return dir;
}