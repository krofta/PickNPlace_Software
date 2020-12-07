// -----------------------------------------------------------------------
// Datei : cursor.c
// Zweck : Einige Funktionen zur Steuerung der Ausgabe im Consolen-Fenster
// -----------------------------------------------------------------------

#include "cursor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


////////////////////////////////////////////////////////////////////////
// Funktion setCursor() : Cursor auf die angegebene Position setzen.

int setCursor( int zeile, int spalte)
{
    return printf("\033[%i;%iH", zeile, spalte) != 0;
}

////////////////////////////////////////////////////////////////////////
// Funktion setColor() : Vorder- und Hintergrundfarbe setzen.

int setColor( char* color)
{
	printf("\033[0%sm",color);

}

////////////////////////////////////////////////////////////////////////
// Funktion cls() : Consolen-Puffer mit aktuellem Attribut l�schen und
//                  Cursor auf die Home-Position setzen.

int cls()
{
	int res = printf("\033[2J");
	res |= setCursor(0,0);
    return res;

}

////////////////////////////////////////////////////////////////////////
// Funktion cursorOff() : Cursor unsichtbar machen.

void cursorOff()
{
	return;
}

////////////////////////////////////////////////////////////////////////
// Funktion cursorOn() : Cursor wieder sichtbar machen.

void cursorOn()
{
return;
}

////////////////////////////////////////////////////////////////////////
// Funktion cursorSize(DWORD  size) : CursorSize setzen (in Prozent)

int cursorSize(uint32_t  size)
{
return 0;
}


// Eingabe direkt in der Console
void stringInput(char value[], int max)
{
	char buffer[100];
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = '\0';
	strncpy(value, buffer, max);
}
// Zahl einlesen
// gibt -1 wieder, wenn etwas anderes als eine Zahl gedr�ckt wird
// �bersch�ssige Zeichen buflen>max-1 werden abgeschnitten
int intInput(char val[], int max)
{
	char buffer[100];
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = '\0';
	// Eingabe pr�fen
	int buflen = strlen(buffer);
	for (int i = 0; i < buflen; i++)
	{
		// Kein akzeptiertes Zeichen f�r einen Integer (ASCII - Code), oder zu viele eingegebene Zeichen
		if (!(buffer[i] > 47 && buffer[i] < 58))
			return -1;
		// Zeichen, die zu viel sind abschneiden
		if (i > max - 1)
			buffer[i] = '\0';
	}
	int iVal = atoi(buffer);
	return iVal;
}


