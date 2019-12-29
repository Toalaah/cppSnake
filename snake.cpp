#include <iostream>
#include <ncurses.h>
#include <unistd.h>
using namespace std;

const int width = 60; const int height = 20; int playing = 1;
int score = 0;
int fruitX;
int fruitY;
int x;
int prevX; int prevY; int prevX1; int prevY1;
int y;
int keyInput;
int body[100][2];
int drawn;
int xdir = 0; 
int ydir = 0;
enum direction {STOP = 0, LEFT, RIGHT, UP, DOWN};
direction dir = STOP;
direction dirPrev;
void setup() {
	srand(time(NULL));
	fruitX = rand() % (width+1) + 1;
	fruitY = rand() % (height+1) + 1;
	x = width / 2;
	y = height /2;

}


void draw() {
	wclear(stdscr);
	wrefresh(stdscr);
	for(int i = 0; i < width; i++){
		printw("#");
	}
	printw("\n");
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			drawn = 0;
			for(int b = 0; b < (score); b++) {
				if(body[b][0] == j && body[b][1] == i) {
					drawn = 1;
					printw("o");
				} 
			}
			if(j == 0) {
				printw("#");
			}
			else if(j == width-1) {
				printw("#\n");
			}
			else if(j == x && i == y){
				printw("0");
			}
			else if (j == fruitX && i == fruitY) {
				printw("*");
			}
			else if(drawn == 0){
				printw(" ");
			}
			else {
				;
			}
		}	
	}
	for(int i = 0; i < width; i++) {
		printw("#");
	}		
	printw("\nScore: %d\n", score);
	wrefresh(stdscr);
}

int kbhit() {
	keyInput = getch();
	
	if(keyInput != ERR) {
		ungetch(keyInput);
		return 1;
	} else {
		return 0;
	}
}
void newF() {
		srand(time(NULL));
		fruitX = rand() % (width+1) + 1;
		fruitY = rand() % (height+1) + 1;
}

void input() {

	if(kbhit()) {	
		keyInput = getch();
		switch (keyInput) {
			case 'w': dirPrev = dir; dir = UP; break;				
			case 's': dirPrev = dir; dir = DOWN; break;
			case 'a': dirPrev = dir; dir = LEFT; break;
			case 'd': dirPrev = dir; dir = RIGHT; break;
			case 'f': dirPrev = dir; dir = STOP; break;
			case 't': newF(); break;
			default: dir = dir; break;
		}

		switch(dir) {
			case STOP: xdir = 0; ydir = 0; break;
			case LEFT: if(dirPrev != RIGHT && dirPrev != dir) {
				xdir = -1; ydir = 0;} break;
			case RIGHT: if(dirPrev != LEFT && dirPrev != dir) {
				xdir = 1; ydir = 0;} break;
			case UP: if(dirPrev != DOWN && dirPrev != dir) {
				xdir = 0; ydir = -1;} break;
			case DOWN: if(dirPrev != UP && dirPrev != dir) {
				xdir = 0; ydir = 1;} break;
		}
	}
}

void logic() {
	if(x > width || x < 0 || y > height || y < 0){
		playing = 0;
	}
	if(x == fruitX && y == fruitY) {
		score ++;		
		newF();
	}
	prevX = x;
	prevY = y; 
	x += xdir;
	y += ydir;
	for(int b = 0; b < score; b++) {
		if(x == body[b][0] && y == body[b][1])
			playing = 0;
		prevX1 = body[b][0];
		prevY1 = body[b][1];
		body[b][0] = prevX;
		body[b][1] = prevY;
		prevX = prevX1;
		prevY = prevY1;
	}
}
 


int main() {
	initscr();	
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	scrollok(stdscr, TRUE);
	setup();
	while(playing) {
		draw();
		input();
		logic();
		usleep(1000*100);
	} 
	endwin();
}



