// -----------------------------------------------------------------------
// Datei : console.h


#include <inttypes.h>



#define BACKGROUND_RED		"0;41"
#define BACKGROUND_VIOLET 	"0;45"
#define BACKGROUND_CYAN   	"0;46"
#define BACKGROUND_YELLOW 	"0;43"
#define BACKGROUND_WHITE  	"0;47"
#define BACKGROUND_BLACK 	"0;40"

#define FOREGROUND_RED 		"0;31"
#define FOREGROUND_VIOLET 	"0;35"
#define FOREGROUND_CYAN   	"0;36"
#define FOREGROUND_YELLOW 	"0;33"
#define FOREGROUND_WHITE  	"1;37"
#define FOREGROUND_BLACK 	"0;30"



// Prototypen der Funktionen aus console.c

 int cls();
 int setCursor( int zeile, int spalte);
 int setColor( char* color );

void cursorOff();
void cursorOn();

int cursorSize(uint32_t  size);
void stringInput(char value[], int max);
int intInput(char val[], int max);



#define KEY_ENTER 'e'
#define KEY_ESC 'q'
#define KEY_ARROW_UP 'w'
#define KEY_ARROW_DOWN 's'
#define KEY_ARROW_LEFT 'a'
#define KEY_ARROW_RIGHT 'd'
#define KEY_F1	'h'

#define START_CURSOR_LINE 6
