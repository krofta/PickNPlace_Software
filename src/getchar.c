/*
 * getchar.c
 *
 *  Created on: 06.12.2020
 *      Author: niko
 */
#include "getchar.h"
#include "cursor.h"



static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  current = old; /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      current.c_lflag |= ECHO; /* set echo mode */
  } else {
      current.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
int getch(void)
{
	/*
	int c;
	if((c=getch_(0))==27){
		getch_(0);
		c = getch_(0);
		if(c==65)
			c =  KEY_ARROW_UP;
		if(c==66)
			c =  KEY_ARROW_DOWN;
		if(c==68)
			c = KEY_ARROW_LEFT;
		if(c==67)
			c = KEY_ARROW_RIGHT;
	}
	//printf("val: %i\n",c);
	return c;
	*/
	return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}
