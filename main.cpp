#include <iostream>
#include <ncurses.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <utility>
#include <iomanip>
#include <string.h>

static int UPS = 30;
static int WIDTH = 30;
static int HEIGHT = 30;
const static std::vector<std::string> validArgs {"--help", "-h", "-r", "-c", "u"};


bool inArgs(std::string s, std::vector<std::string> v)
{

  return true;
}

enum direction { NONE, LEFT, RIGHT, UP, DOWN };

struct snake_t
{
  int x;
  int y;
  int dx;
  int dy;
  int prev_x {0};
  int prev_y {0};
  std::vector< std::pair<int,int> > body;
};

void init_colors()
{
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK); // snake
  init_pair(2, COLOR_RED, COLOR_BLACK); // fruit 
}

void init_snake(snake_t *snake, int w, int h) { snake->x = w/2; snake->y = h/2; }

void putBlock(WINDOW *w, int color) { waddch(w, ' ' | A_REVERSE | COLOR_PAIR(color)); waddch(w, ' ' | A_REVERSE | COLOR_PAIR(color)); }

void putNothing(WINDOW *w) { waddch(w, ' '); waddch(w, ' '); }

bool tick_game(direction dir, direction dirPrev, snake_t *snake, std::pair<int,int> &fruit)
{
  if(snake->x > WIDTH || snake->x < 0 || snake->y > HEIGHT || snake->y < 0)
    return false;

  if(snake->x == fruit.first && snake->y == fruit.second) 
  {
    // grow body
    snake->body.push_back(std::make_pair(snake->x,snake->y));
    // we ate a fruit, time to spawn a 'new' one
    fruit.first = rand() % WIDTH;
    fruit.second = rand() % HEIGHT;
  }

  // update snake
  int tmpx;
  int tmpy;
  int lastx = snake->x;
  int lasty = snake->y;

  // move head
  snake->x += snake->dx;
  snake->y += snake->dy;

  // move body
  for(auto &p : snake->body)
  {
    if(snake->x == p.first && snake->y == p.second)
      return false;
    tmpx = p.first;
    tmpy = p.second;
    p.first = lastx;
    p.second = lasty;
    lastx = tmpx;
    lasty = tmpy;
  }

  // update direction
  switch (dir)
  {
    case LEFT : if(dirPrev != RIGHT) { snake->dx = -1; snake->dy = 0;} break;
    case RIGHT: if(dirPrev != LEFT) { snake->dx = 1; snake->dy = 0;}   break;
    case UP   : if(dirPrev != DOWN) { snake->dx = 0; snake->dy = -1;}  break;
    case DOWN : if(dirPrev != UP) { snake->dx = 0; snake->dy = 1;}     break;
    case NONE : snake->dx = 0; snake->dy = 0;                          break;
  }

  return true;
}

void display_board(WINDOW *w, snake_t *snake, std::pair<int, int> &fruit)
{
  bool occupied;
  box(w, 0, 0);
  for(int i = 0; i < HEIGHT; i++) // h
  {
    wmove(w, 1 + i, 1);
    for(int j = 0; j < WIDTH; j++) // w
    {
      occupied = false;
      for(auto & p : snake->body) 
      {
        if(p.first == j && p.second == i)
        {
          occupied = true;
          putBlock(w, 1);
          break;
        }
      }

      if(j == snake->x && i == snake->y)
        putBlock(w, 1);
      else if(j == fruit.first && i == fruit.second)
        putBlock(w, 2);
      else if(!occupied)
        putNothing(w);
    }
  }
  wnoutrefresh(w);
}

void display_score(snake_t *snake, WINDOW *w)
{
  wclear(w);
  box(w, 0, 0);
  wmove(w, 1, 1);
  wprintw(w, "Score: %d", snake->body.size());
  wnoutrefresh(w);
}

void printInvalidArgs(char *msg) { std::cout << "Usage: " << msg << " [-r <value>] [-c <value>] [-u <value>]" << std::endl; exit(0); }

void printHelp(char *msg)
{
  std::cout << "Usage: " << msg << " [-r <value>] [-c <value>] [-u <value>]" << std::endl;
  std::cout << std::left << std::setw(12) << "-r(ows) " << std::setw(10) << ' ' << "specify number of rows" << std::endl;
  std::cout << std::left << std::setw(12) << "-c(olumns) " << std::setw(10) << ' ' << "specify number of columns" << std::endl;
  std::cout << std::left << std::setw(12) << "-u(ps) " << std::setw(10) << ' ' << "specify number of updates per second (update rate)" << std::endl;
  std::cout << std::left << std::setw(12) << "example " << std::setw(10) << ' ' << msg << " -r 25 -c 30 -u 25" << std::endl;
  exit(0);
}

int main(int argc, char **argv)
{
  // argument parsing 
  if(argc != 1)
  {
    int i = 1; 
    while(i < argc)
    {
      if(!(std::strcmp(argv[i],"--help")) || !(std::strcmp(argv[i],"-h")))
        printHelp(argv[0]);
      else if(!(std::strcmp(argv[i], "-c")))
        WIDTH = atoi(argv[(i++)+1]);
      else if(!(std::strcmp(argv[i], "-r")))
        HEIGHT = atoi(argv[(i++)+1]);
      else if(!(std::strcmp(argv[i], "-u")))
        UPS = atoi(argv[(i++)+1]);
      else
        printInvalidArgs(argv[0]);
      i++;
    }
  }
  
  WINDOW *board;
  WINDOW *score;
  snake_t *snake = new snake_t;
  bool playing = true;
  direction dir = NONE;
  direction dirPrev;
  init_snake(snake, WIDTH, HEIGHT);

  // ncurses initialization
  initscr();             
  cbreak();              
  noecho();              
  keypad(stdscr, TRUE);  
  timeout(0);            
  curs_set(0);           
  init_colors();         

  srand(time(NULL));
  std::pair<int, int> fruit = std::make_pair(rand()%WIDTH, rand()%HEIGHT);

  board = newwin(HEIGHT + 2, 2*WIDTH + 2, 0, 0);
  score = newwin(3, 2*WIDTH + 2, HEIGHT + 2, 0);

  while(playing) 
  {
    playing = tick_game(dir, dirPrev, snake, fruit);
    display_board(board, snake, fruit);
    display_score(snake, score);
    doupdate();

    switch(getch())
    {
      case KEY_UP   : case 'w': dirPrev = dir; dir = UP;    break;
      case KEY_DOWN : case 's': dirPrev = dir; dir = DOWN;  break;
      case KEY_LEFT : case 'a': dirPrev = dir; dir = LEFT;  break;
      case KEY_RIGHT: case 'd': dirPrev = dir; dir = RIGHT; break;
      case 'p'                : timeout(-1); 
                                getch(); timeout(0);        break;
      case '0'                : playing = false;            break;
      default:                                              break;
    }
    usleep(33320 * 30 / UPS);
  }

  // cleanup 
  wclear(stdscr);
  endwin();
  delete snake;

  return 0;
}
