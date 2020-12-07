/*
 * getch.h
 *
 *  Created on: 06.12.2020
 *      Author: niko
 */

#ifndef GETCH_H_
#define GETCH_H_

#include <termios.h>
#include <stdio.h>

static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo);

/* Restore old terminal i/o settings */
void resetTermios(void);

/* Read 1 character - echo defines echo mode */
char getch_(int echo) ;

/* Read 1 character without echo */
int getch(void) ;

/* Read 1 character with echo */
char getche(void) ;

#endif /* GETCH_H_ */
