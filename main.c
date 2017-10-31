#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <poll.h>
#include <fcntl.h>

#define SIZE_X 65
#define SIZE_Y 40

#define clear_console() printf("\033[H\033[J")
#define ngetc(c) read(0, c, 1)

int inpX = 0;
int inpY = 0;

int area[SIZE_X][SIZE_Y];

int direction = 0;
int counter = 1;
int size = 1;

int x = SIZE_X/2, y = SIZE_Y/2;

void clear()
{
  for (int i = 0; i < SIZE_X; i++)
    for (int o = 0; o < SIZE_Y; o++)
      area[i][o] = -1;
}

void spawn_food()
{
  int tx = rand() % SIZE_X;
  int ty = rand() % SIZE_Y;

  //BUG BUG BUG
  area[ty][tx] = -2;
}

int tick()
{

  switch(direction)
  {
  case 0:
    x++;
    break;
  case 1:
    y++;
    break;
  case 2:
    x--;
    break;
  case 3:
    y--;
    break;
  }

  counter++;

  if (x < 0)
    x = SIZE_X - 1;
  if (x >= SIZE_X)
    x = 0;
  if (y < 0)
    y = SIZE_Y - 1;
  if (y >= SIZE_Y)
    y = 0;

  if (area[x][y] > 0 && area[x][y] >= counter - size)
    return 0;

  if (area[x][y] == -2)
  {
    spawn_food();
    size++;
  }

  area[x][y] = counter;

  return 1;
}

void draw()
{
  clear_console();
  for (int i = -1; i <= SIZE_Y; i++)
  {
    for (int o = -1; o <= SIZE_X; o++)
    {
      if (i < 0 || i >= SIZE_Y || o < 0 || o >= SIZE_X)
        printf("#");
      else if (area[o][i] == -2)
        printf("*");
      else if (area[o][i] <= counter - size)
        printf(" ");
      else
        printf("@");
    }
    printf("\n");
  }
}

void handle_input()
{
  if (inpX == 1 && direction != 2)
    direction = 0;
  else if (inpX == -1 && direction != 0)
    direction = 2;
  else if (inpY == 1 && direction != 1)
    direction = 3;
  else if (inpY == -1 && direction != 3)
    direction = 1;
}

void input_thread()
{
  fcntl(0, F_SETFL, O_NONBLOCK);
  char c = 0;
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_iflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ngetc (&c);
  system("/bin/stty raw");
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  //pthread_mutex_lock(&lock);
  switch (c)
    {
    case 'w':
    case 'W':
      inpX = 0;
      inpY = 1;
      break;
    case 's':
    case 'S':
      inpX = 0;
      inpY = -1;
      break;
    case 'd':
    case 'D':
      inpX = 1;
      inpY = 0;
      break;
    case 'a':
    case 'A':
      inpX = -1;
      inpY = 0;
      break;
    }
  printf("%c", c);
  c = 0;
}
int main()
{

  srand(time(NULL));

  clear();

  spawn_food();
  spawn_food();
  spawn_food();
  spawn_food();

  while (1)
  {
    input_thread();
    handle_input();
    if (!tick())
      break;
    draw();
    usleep(100*1000);
  }

  return 0;
}

