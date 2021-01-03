/*
 ============================================================================
 Name        : ImageProcessing.c
 Author      : Niko Krofta
 Version     :
 Copyright   : Bla
 Description : ImageProcessing in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getchar.h"
#include "cursor.h"
#include "bv.h"
#include "image_io.h"

void printline(){
	printf(" __________________________________________________\n");
}

void grauwert_menu_help()
{
	cls();
	cursorOff();
	setColor(FOREGROUND_YELLOW);
	printline();
	printf("   Hilfe Menu Grauwert-Aufloesung                  \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	printf("  Grauwert-Aufloesung: \n");
	printf("  Waehlen Sie einen Wert fuer die Aufloesung der \n");
	printf("  Grauwerte (absolute Anzahl der Graustufen).\n");
	printf("  Es wird versucht eine Keilform im kumulativen Histogramm \n");
	printf("  zu erreichen. Dadurch wird eine Gleichverteilung des \n");
	printf("  normalen Histogramms angenaehert. \n");
	printf("  Nach der linearen Anpassung wird noch eine Dehnung \n");
	printf("  der Grauwerte vorgenommen. \n");
	printf("  Eine geringe Aufloesung erzielt eine bessere Gleichverteilung \n");
	printf("  als groessere Aufloesungen.\n");
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();
}
void print_grauwert_menu()
{
	cls();
	setColor(FOREGROUND_YELLOW);
	printline();
	printf("   Grauwertaufloesung lin. Histogrammausgleich     \n");
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  [16 ]     [32 ]     [64 ]     [128]     [256]\n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
}



int grauwert_menu(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	int wahl = 0;
	int cursorColumn = 0;
	print_grauwert_menu();
	while (wahl != KEY_ESC)
	{
		setCursor(START_CURSOR_LINE, cursorColumn + 3);
		wahl = getch();
		switch (wahl)
		{
		case KEY_ARROW_LEFT:
			if (cursorColumn > 3)
				cursorColumn -= 10;
			break;
		case KEY_ARROW_RIGHT:
			if (cursorColumn < 33)
				cursorColumn += 10;
			break;
		case KEY_ENTER:
			fflush(stdin);
			cls();
			switch (cursorColumn)
			{
			case 0:		linearer_histo_ausgleich(img, img2, 16);	break;
			case 10:	linearer_histo_ausgleich(img, img2, 32);	break;
			case 20:	linearer_histo_ausgleich(img, img2, 64);	break;
			case 30:	linearer_histo_ausgleich(img, img2, 128);	break;
			case 40:	linearer_histo_ausgleich(img, img2, 256);	break;
			default:	break;
			}
			return 0;
			break;
		case KEY_F1:
			grauwert_menu_help();
			print_grauwert_menu();
			break;

		}

	}
	return 1;

}
void laplace_menu_help() {
	cls();
	cursorOff();
	setColor(FOREGROUND_YELLOW);
	printline();
	printf("   Hilfe Menu Laplace Operation                    \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	printf("  Laplace-Operation: \n");
	printf("  Das Ausgabebild hat nur Pixel mit den Werten 0 und 255.\n");
	printf("  4er Laplace:          8er Laplace:\n");
	printf("               0  1  0               1  1  1\n");
	printf("               1 -4  1               1 -8  1\n");
	printf("               0  1  0               1  1  1\n");
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();
}
void print_laplace_menu()
{
	cls();
	setColor(FOREGROUND_YELLOW);
	printline();
	printf("   Auswahl der Laplace-Umgebung                    \n");
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  Binaere Grauwerte, 4er Umgebung: [ ]\n");
	printf("  Binaere Grauwerte, 8er Umgebung: [ ]\n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
}

int laplace_menu(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	int wahl = 0;
	int cursorLine = 0;
	print_laplace_menu();
	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 36);
		wahl = getch();
		switch (wahl)
		{
		case KEY_ARROW_UP:
			if (cursorLine>0)
				cursorLine--;
			break;
		case KEY_ARROW_DOWN:
			if (cursorLine<1)
				cursorLine++;
			break;
		case KEY_ENTER:
			fflush(stdin);
			cls();
			if( cursorLine == 0)
				laplace(img, img2, LAPLACE_4);
			if ( cursorLine == 1)
				laplace(img, img2, LAPLACE_8);
			return 0;
			break;
		case KEY_F1:
			laplace_menu_help();
			print_laplace_menu();
			break;
		default:
			break;
		}

	}
	return 1;
}
#define MITTELWERT_NORMAL 0
#define MITTELWERT_MEDIAN 1
#define GAUSS_FILTER 2

void mittelwert_menu_help(int Art)
{
	cls();
	cursorOff();
	setColor(FOREGROUND_YELLOW);
	printline();
	switch (Art)
	{
	case MITTELWERT_NORMAL:
	printf("   Hilfe Menu normales Filter                      \n");
		break;
	case MITTELWERT_MEDIAN:
	printf("   Hilfe Menu median Filter                        \n");
		break;
	}
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	if (Art == MITTELWERT_NORMAL) {
		printf("  Mittelwert-Filter: \n");
		printf("  Die Gewichtung wirkt auf den zentralen Pixel.\n");
		printf("  Der zentrale Pixel ist in den Beispielen mit z gekennzeichnet.\n");
		printf("  n - gerade:                 n - ungerade:\n");
		printf("             x x x x                       x x x\n");
		printf("             x z x x                       x z x\n");
		printf("             x x x x                       x x x\n");
		printf("             x x x x \n");
		printf("  n gibt die Dimension fuer das Filter n*n an.");
	}
	else if (Art == GAUSS_FILTER) {
		printf("  Gauss-Filter: \n");
		printf("  Gaussförmige Verteilung der Gewichtung.\n");
		printf("  Beispiel fuer einen Gaussfilter: .\n");
		printf("             1 2 1  \n");
		printf("             2 4 2  \n");
		printf("             1 2 1  \n");
		printf("  n gibt die Dimension fuer das Filter n*n an.");
	}
	else {
		printf("  Median-Filter: \n");
		printf("  Das Medianfilter nimmt den Median aus der Umgebung n*n \n");
		printf("  als neuen Wert fuer den zentralen Pixel z. \n");
		printf("  Der zentrale Pixel ist in den Beispielen mit z gekennzeichnet.\n");
		printf("  n - gerade:                 n - ungerade:\n");
		printf("             x x x x                       x x x\n");
		printf("             x z x x                       x z x\n");
		printf("             x x x x                       x x x\n");
		printf("             x x x x \n");
		printf("  n gibt die Dimension fuer das Filter n*n an.");
	}
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();
}
void print_mittelwert_menu(int Art, int iDIMxy, int gew) {
	cls();
	setColor(FOREGROUND_YELLOW);
	printline();
	switch (Art)
	{
	case MITTELWERT_NORMAL:
	printf("   Eingabe der Filtergroesse (normal)              \n");
		break;
	case GAUSS_FILTER:
	printf("   Eingabe der Filtergroesse (Gauss-Filter)        \n");
		break;
	case MITTELWERT_MEDIAN:
	printf("   Eingabe der Filtergroesse (median)              \n");
		break;
	}
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  Eingabe Dimension n*n:   \n");
	if( Art == MITTELWERT_NORMAL)
	printf("  Eingabe Gewichtung:      \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  Pfeiltaste    a/d       = Inkrement/Dekrement    \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
	char buf[3];
	setCursor(START_CURSOR_LINE, 25);
	sprintf(buf,"%i",iDIMxy);
	//itoa(, buf, 10);
	fputs(buf, stdout);
	if (Art == MITTELWERT_NORMAL)
	{
		setCursor(START_CURSOR_LINE + 1, 25);
		//itoa(gew, buf, 10);
		sprintf(buf,"%i",gew);
		fputs(buf, stdout);
	}
}

void filter_menu(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int Art)
{
	int wahl = 0;
	int cursorLine = 0;
	int iDIMxy = 3, gew = 1;
	char buf[3];
	print_mittelwert_menu(Art,iDIMxy,gew);

	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 25);
		wahl = getch();
		switch (wahl)
		{
		case KEY_ARROW_UP:
			if (cursorLine>0)
				cursorLine--;
			break;
		case KEY_ARROW_DOWN:
			if (cursorLine<1 && Art == MITTELWERT_NORMAL)
				cursorLine++;
			break;
		case KEY_ARROW_LEFT:
			if (cursorLine == 0 && iDIMxy > 2 && (Art == MITTELWERT_MEDIAN || Art == MITTELWERT_NORMAL)){
				iDIMxy--;
				sprintf(buf,"%i",iDIMxy);
				//itoa(iDIMxy, buf, 10);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			else if (cursorLine == 0 && iDIMxy > 3 && Art == GAUSS_FILTER){
				iDIMxy-=2;
				//itoa(iDIMxy, buf, 10);
				sprintf(buf,"%i",iDIMxy);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			else if (cursorLine == 1 && Art == MITTELWERT_NORMAL && gew > 1){
				gew--;
				//itoa(gew, buf, 10);
				sprintf(buf,"%i",iDIMxy);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			break;
		case KEY_ARROW_RIGHT:
			if (cursorLine == 0 && iDIMxy < 11 && (Art == MITTELWERT_MEDIAN || Art == MITTELWERT_NORMAL)){
				iDIMxy++;
				//itoa(iDIMxy, buf, 10);
				sprintf(buf,"%i",iDIMxy);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			else if (cursorLine == 0 && iDIMxy < 49 && Art == GAUSS_FILTER) {
				iDIMxy += 2;
				//itoa(iDIMxy, buf, 10);
				sprintf(buf,"%i",iDIMxy);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			else if (cursorLine == 1 && Art == MITTELWERT_NORMAL && gew < 11){
				gew ++;
				//itoa(gew, buf, 10);
				sprintf(buf,"%i",gew);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			break;
		case KEY_ENTER:
			if (Art == MITTELWERT_NORMAL)
			{
				system("cls");
				mittelwert_filter(img, img2, iDIMxy, gew);
			}
			else if (Art == MITTELWERT_MEDIAN)
			{
				system("cls");
				median_filter(img, img2, iDIMxy);
			}
			else if (Art == GAUSS_FILTER)
			{
				system("cls");
				gauss_filter(img, img2, iDIMxy);
			}
			print_mittelwert_menu(Art,iDIMxy,gew);
			break;
		case KEY_F1:
			mittelwert_menu_help(Art);
			print_mittelwert_menu(Art,iDIMxy,gew);
			break;
		default:
			break;
		}
	}

}

void print_binaer_bv_menu()
{
	cls();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Funktionen Binaere Bildverarbeitung             \n");
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("   1. Shrink:                [ ]  \n");
	printf("   2. Blow:                  [ ]  \n");
	printf("   3. Pixel zaehlen:         [ ]  \n");
	printf("   4. Oeffnen:               [ ]  \n");
	printf("   5. Schliessen:            [ ]  \n");
	printf("   6. Grassfire:             [ ]  \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
}
void menu_binaer_bv_help(int HelpFunc)
{
	cls();
	cursorOff();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Hilfe Menu Binaere Bildverarbeitung             \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	switch (HelpFunc)
	{
	case 0:
		printf("  1. Shrink:\n");
		printf("  Duennt das Bild aus.\n");
		printf("  Diese Funktion kann benutzt werden, \n");
		printf("  um Salz- und Pfeffer-Rauschen zu eliminieren.\n");
		printf("  Objekte verkleinern sich. \n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 1:
		printf("  2. Blow:\n");
		printf("  Dickt das Bild an.  \n");
		printf("  Objekte vergroessern sich. \n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 2:
		printf("  3. Pixel zaehlen:  \n");
		printf("  Zaehlt alle Pixel, die nicht schwarz sind.  \n");
		break;
	case 3:
		printf("  4. Oeffnen:\n");
		printf("  Unterdrueckung lokaler Stoerungen und Filtern kleiner Strukturen\n");
		printf("  Es werden weisse Pixel hinzugefuegt\n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 4:
		printf("  5. Schliessen:\n");
		printf("  Unterdruecken von lokalen dunklen Stoerungen.\n");
		printf("  Es werden weisse Pixel hinzugefuegt.\n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 5:
		printf("  6. Grassfire:\n");
		printf("  Dieser Algorithmus zaehlt Objekte, die nur aus weissen\n");
		printf("  Pixeln bestehen.\n");
		break;
	default: break;
	}
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();
}

void menu_binaer_bv(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	int wahl = 0;
	int cursorLine = 0;

	print_binaer_bv_menu();
	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 30);
		wahl = getch();
		switch (wahl)
		{
		case KEY_ARROW_UP:
			if (cursorLine>0)
				cursorLine--;
			break;

		case KEY_ARROW_DOWN:
			if (cursorLine<5)
				cursorLine++;
			break;

		case KEY_ENTER:
			fflush(stdin);
			switch (cursorLine)
			{
			case 0:
				cls();
				shrink(img, img2);
				writeImage_ppm(img2, MAXXDIM, MAXYDIM);
				break;
			case 1:
				cls();
				blow(img, img2);
				writeImage_ppm(img2, MAXXDIM, MAXYDIM);
				break;
			case 2:
				cls();
				count_white(img);
				break;
			case 3:
				cls();
				funk_open(img, img2);
				writeImage_ppm(img2, MAXXDIM, MAXYDIM);
				break;
			case 4:
				cls();
				funk_close(img, img2);
				writeImage_ppm(img2, MAXXDIM, MAXYDIM);
				break;
			case 5:
				cls();
				grassfire(img);
				break;
			}
			print_binaer_bv_menu();
			break;
		case KEY_F1:
			menu_binaer_bv_help(cursorLine);
			print_binaer_bv_menu();
			break;
		}
	}
}

void print_menu_preprocessing()
{
	cls();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Funktionen Preprocessing                        \n");
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("   1. Histogramm (normal)    [ ]  \n");
	printf("   2. Histogramm (kumulativ) [ ]  \n");
	printf("   3. Grauwert-Dehnung       [ ]  \n");
	printf("   4. Histogramm-Ebnung      [ ]  \n");
	printf("   5. Mittelwert-Filter      [ ]  \n");
	printf("   6. Median-Filter          [ ]  \n");
	printf("   7. Gauss-Filter           [ ]  \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
}
void menu_preprocessing_help(int HelpFunc)
{
	cls();
	cursorOff();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Hilfe Menu Preprocessing                        \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	switch (HelpFunc)
	{
	case 0:
		printf("  1. Histogramm (normal): \n");
		printf("  Es wird ein normales Histogramm berechnet. \n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 1:
		printf("  2. Histogramm (kumulativ): \n");
		printf("  Es wird ein kumulatives Histogramm berechnet. \n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 2:
		printf("  3. Histogramm-Dehnung:  \n");
		printf("  Die Grauwerte des Bildes werden auf den maximalen \n");
		printf("  Bereich aller Grauwerte gedehnt. \n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 3:
		printf("  4. lineare Histogramm-Anpassung:\n");
		printf("  Es wird versucht eine Gleichverteilung der\n");
		printf("  Grauwerte zu erreichen.\n");
		break;
	case 4:
		printf("  5. Normales Mittelwert-Filter:\n");
		printf("  Es wird ein Menu zur Auswahl der Dimension aufgerufen.\n");
		break;
	case 5:
		printf("  6. Median-Filter:\n");
		printf("  Es wird ein Menu zur Auswahl der Dimension aufgerufen.\n");
		break;
	case 6:
		printf("  6. Gauss-Filter :\n");
		printf("  Es wird ein Menu zur Auswahl der Dimension aufgerufen.\n");
		break;
	default: break;
	}
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();
}
void menu_preprocessing(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	int wahl = 0;
	int cursorLine = 0;

	print_menu_preprocessing();
	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 30);
		wahl = getch();
		switch (wahl)
		{
		case KEY_ARROW_UP:
			if (cursorLine>0)
				cursorLine--;
			break;

		case KEY_ARROW_DOWN:
			if (cursorLine<6)
				cursorLine++;
			break;
		case KEY_ENTER:
			fflush(stdin);
			switch (cursorLine)
			{
			case 0:
				cls();
				histogramm(img, img2, HISTO_NORMAL);
				break;
			case 1:
				cls();
				histogramm(img, img2, HISTO_KUMULATIV);
				break;
			case 2:
				cls();
				grauwert_dehnung(img, img2);
				break;
			case 3:
				cls();
				grauwert_menu(img, img2);
				break;
			case 4:
				cls();
				filter_menu(img, img2, MITTELWERT_NORMAL);
				break;
			case 5:
				cls();
				filter_menu(img, img2, MITTELWERT_MEDIAN);
				break;
			case 6:
				cls();
				filter_menu(img, img2, GAUSS_FILTER);
				break;
			default:break;
			}
			print_menu_preprocessing();
			break;
		case KEY_F1:
			menu_preprocessing_help(cursorLine);
			print_menu_preprocessing();
			break;
		}
	}
}

void DoG_menu_help()
{
	cls();
	cursorOff();
	setColor(FOREGROUND_YELLOW);
	printline();
	printf("   Hilfe Menu Difference of Gaussians              \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	printf("  Difference of Gaussians: \n");
	printf("  Zwei normalisierete Gauss-Verteilungen werden \n");
	printf("  voneinander abgezogen.\n");
	printf("  n gibt die groessere Gauss-Verteilung an\n");
	printf("  Grundton gibt den initialen Wert des Ergebnisbildes an.\n");
	printf("  Mit dem Grundton 127, koennen positive und negative\n");
	printf("  Kanten erkannt werden.\n");
	printf("  Mit den Grundton 0, werden Kanten hell dargestellt.\n");
	printf("  Mit den Grundton 255, werden Kanten dunkel dargestellt.\n");
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();
}
void print_DoG_menu(int iDIMxy, int Grundton) {
	cls();
	setColor(FOREGROUND_YELLOW);
	printline();
	printf("   Eingabe der Difference of Gaussians Groesse     \n");
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  Eingabe Dimension n*n:   \n");
	printf("  Eingabe Grundton:        \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  Pfeiltaste    a/d       = Inkrement/Dekrement    \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
	char buf[3];
	setCursor(START_CURSOR_LINE, 25);
	//itoa(iDIMxy, buf, 10);
	sprintf(buf,"%i",iDIMxy);
	fputs(buf, stdout);
	setCursor(START_CURSOR_LINE+1, 25);
	//itoa(Grundton, buf, 10);
	sprintf(buf,"%i",Grundton);
	fputs(buf, stdout);
}

void DoG_menu(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	int wahl = 0;
	int cursorLine = 0;
	int scale = 3,Grundton = 127;
	char buf[3];
	print_DoG_menu(scale,Grundton);

	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 25);
		wahl = getch();

		switch (wahl)
		{
		case KEY_ARROW_UP:
			if (cursorLine>0)
				cursorLine--;
			break;
		case KEY_ARROW_DOWN:
			if (cursorLine<1)
				cursorLine++;
			break;
		case KEY_ARROW_LEFT:
			if (scale > 3 && cursorLine == 0)
			{
				scale-=2;
				//itoa(scale, buf, 10);
				sprintf(buf,"%i",scale);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			else if (cursorLine == 1)
			{
				Grundton = Grundton == 255 ? 127 : Grundton == 127 ? 0 : 0;
				//itoa(Grundton, buf, 10);
				sprintf(buf,"%i",Grundton);
				strcat(buf, "  ");
				fputs(buf, stdout);
			}
			break;
		case KEY_ARROW_RIGHT:
			if (scale < 49 && cursorLine == 0)
			{
				scale+=2;
				//itoa(scale, buf, 10);
				sprintf(buf,"%i",scale);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			else if ( cursorLine == 1)
			{
				Grundton = Grundton == 0 ? 127 : Grundton == 127 ? 255 : 255;
				//itoa(Grundton, buf, 10);
				sprintf(buf,"%i",Grundton);
				strcat(buf, "  ");
				fputs(buf, stdout);
			}
			break;
		case KEY_ENTER:
			difference_of_gaussian(img, img2, scale,Grundton);
			print_DoG_menu(scale,Grundton);
			break;
		case KEY_F1:
			DoG_menu_help();
			print_DoG_menu(scale,Grundton);
			break;
		default:
			break;
		}
	}
}

void print_menu_kanten()
{
	cls();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Funktionen Kantenerkennung                      \n");
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  1. Sobel-x-Operation       [ ]  \n");
	printf("  2. Sobel-y-Operation       [ ]  \n");
	printf("  3. Sobel-xy-Operation      [ ]  \n");
	printf("  4. Laplace-Operator        [ ]  \n");
	printf("  5. Difference of Gaussians [ ]  \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
}
void menu_kanten_help(int HelpFunc)
{
	cls();
	cursorOff();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Hilfe Menu Kantenerkennung                      \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	switch (HelpFunc)
	{
	case 0:
		printf("  1. Sobel-x-Operation:  \n");
		printf("  Kantenerkennung in x-Richtuing durch den \n");
		printf("  Sobeloperator:\n");
		printf("  1    0    -1\n");
		printf("  2    0    -2\n");
		printf("  1    0    -1\n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 1:
		printf("  2. Sobel-y-Operation:  \n");
		printf("  Kantenerkennung in y-Richtuing durch den \n");
		printf("  Sobeloperator:\n");
		printf("   1     2     1\n");
		printf("   0     0     0\n");
		printf("  -1    -2     1\n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 2:
		printf("  3. Sobel-xy-Operation:  \n");
		printf("  Zusammenf�gen von Sobeloperationen in x und y Richtung \n");
		printf("  Sie muessen zunaechst die Operationen in x und y Richtung  \n");
		printf("  durchfuehren. Dabei werden interne Matrizen fuer die   \n");
		printf("  Weiterverarbeitung gefuellt.   \n");
		printf("  Die Ausgabe wird nach der folgenden Formel berechnet: \n");
		printf("  A(x,y)=sqrt(sq(E(x)) + sq(E(y))) \n");
		printf("\n");
		printf("  Das Ergebnis wird als *.mpp gespeichert \n");
		break;
	case 3:
		printf("  4. Laplace-Operator:\n");
		printf("  Es wird das Menu zur Wahl des 4er/8er Laplace aufgerufen.\n");
		break;
	case 4:
		printf("  5. Menu DoG: Difference of Gaussians:\n");
		printf("  Filter und Kantenerkennung in einem Schritt\n");
		break;
	default: break;
	}
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();

}

void menu_kanten(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	int wahl = 0;
	int cursorLine = 0;
	// Matrizen f�r Interfunktionale Zusammenh�nge(Sobel_x, Sobel_y
	int iMatrix_x[MAXXDIM][MAXYDIM], iMatrix_y[MAXXDIM][MAXYDIM];
	print_menu_kanten();
	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 30);
		wahl = getch();
		switch (wahl)
		{
		case KEY_ARROW_UP:
			if (cursorLine>0)
				cursorLine--;
			break;

		case KEY_ARROW_DOWN:
			if (cursorLine<4)
				cursorLine++;
			break;
		case KEY_ENTER:
			fflush(stdin);
			switch (cursorLine)
			{
			case 0:
				cls();
				sobelx(img, img2, iMatrix_x);
				break;
			case 1:
				cls();
				sobely(img, img2, iMatrix_y);
				break;
			case 2:
				cls();
				sobelxy(img, img2, iMatrix_x, iMatrix_y);
				break;
			case 3:
				cls();
				laplace_menu(img, img2);
				break;
			case 4:
				cls();
				DoG_menu(img, img2);
				break;
			default:break;
			}
			print_menu_kanten();
			break;
		case KEY_F1:
			menu_kanten_help(cursorLine);
			print_menu_kanten();
			break;
		}
	}
}

void print_menu_textur(int winkel, int graustufen)
{
	cls();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Funktionen Textur                               \n");
	printf(" \n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  1. Laws Texturerkennung              [ ] \n");
	printf("                                           \n");
	printf("  2. Cooccurence-Matrix (Richtung)     :   \n");
	printf("     ->Wert: 0,45,90,135 Grad              \n");
	printf("  3. Cooc.-Mat. (Kombi+ASM)            :   \n");
	printf("     ->Wert: Anzahl der Graustufen         \n");
	printf("                                           \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  Pfeiltaste    a/d       = Inkrement/Dekrement    \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
	char buf[10];
	setCursor(START_CURSOR_LINE + 2, 41);
	//itoa(winkel, buf, 10);
	sprintf(buf,"%i",winkel);
	fputs(buf, stdout);
	setCursor(START_CURSOR_LINE + 4, 41);
	//itoa(graustufen, buf, 10);
	sprintf(buf,"%i",graustufen);
	fputs(buf, stdout);
}
void menu_textur_help(int HelpFunc)
{
	cls();
	cursorOff();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Hilfe Menu Texturen                             \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	switch (HelpFunc)
	{
	case 0:
		printf("  1. Laws Texturerkennung  \n");
		printf("  Texturerkennung durch 3x3 Faltungsmasken\n");
		break;
	case 2:
		printf("  2. Cooccurence Matrix (Richtung) \n");
		printf("  Statistik 2. Ordnung.\n");
		printf("  Die Richtung gibt den Winkel an, in welcher \n");
		printf("  Lagebeziehung die Statistik erstellt wird\n");
		printf("  In die Matrix wird die Haeufigkeit, mit der die\n");
		printf("  Kombination Grauwert_1/Grauwert_2 auftritt, eingetragen.\n");
		break;
	case 4:
		printf("  3. Cooccurence Matrix (Kombi) \n");
		printf("  Statistik 2. Ordnung.\n");
		printf("  Es werden die Cooccurence Matrizen der vier Richtungen \n");
		printf("  (0,45,90,135)Grad berechnet und anschliessend zusammengefasst.\n");
		printf("  Der ASM Wert wird berechnet und ausgegeben.\n");
		break;
	default:
		break;
	}
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	fflush(stdin);
	cursorOn();
}
void menu_textur(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	float fIMG[MAXXDIM][MAXYDIM];
	int wahl = 0, winkel = 0, graustufen = 16;
	char buf[10];
	int cursorLine = 0;
	print_menu_textur(winkel, graustufen);
	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 41);
		wahl = getch();
		switch (wahl)
		{
		case KEY_ARROW_UP:
			if (cursorLine>0)
				cursorLine -= 2;
			break;

		case KEY_ARROW_DOWN:
			if (cursorLine<4)
				cursorLine += 2;
			break;
		case KEY_ARROW_LEFT:
			if ((winkel > 0) && (cursorLine == 2))
			{
				winkel -= 45;
				//itoa(winkel, buf, 10);
				sprintf(buf,"%i",winkel);
				strcat(buf, "  ");
				fputs(buf, stdout);
			}
			else if ((graustufen > 4) && (cursorLine == 4))
			{
				graustufen -= 4;
				//itoa(graustufen, buf, 10);
				sprintf(buf,"%i",graustufen);
				strcat(buf, "  ");
				fputs(buf, stdout);
			}

			break;
		case KEY_ARROW_RIGHT:
			if (winkel < 135 && cursorLine == 2)
			{
				winkel += 45;
				//itoa(winkel, buf, 10);
				sprintf(buf,"%i",winkel);
				strcat(buf, " ");
				fputs(buf, stdout);
			}
			else if (graustufen < 256 && cursorLine == 4)
			{
				graustufen += 4;
				//itoa(graustufen, buf, 10);
				sprintf(buf,"%i",graustufen);
				strcat(buf, "  ");
				fputs(buf, stdout);
			}
			break;
		case KEY_ENTER:
			fflush(stdin);
			switch (cursorLine)
			{
			case 0:
				cls();
				laws_textur(img, img2);
				break;
			case 2:
				cls();
				cooccurence_matrix(img, img2, fIMG, winkel, 1);
				break;
			case 4:
				cls();
				cooc_matrix_kombi_asm(img, img2, fIMG, graustufen);
				break;
			default:break;
			}
			print_menu_textur(winkel, graustufen);
			break;
		case KEY_F1:
			menu_textur_help(cursorLine);
			print_menu_textur(winkel, graustufen);
			break;
		}
	}

}

void print_menu_segmentierung(int threshold, int intervall[3], int blob_label, int background, int blob_label2)
{
	cls();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Funktionen Segmentierung                       \n");
	printf("\n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  1. Thresholding (manuell)                    :  \n");
	printf("  2. Thresholding (von Otsu)                   [ ]\n");
	printf("  3. Blob-Coloring 1 GW:Marker (Bereich)       :  \n");
	printf("  4. Blob-Coloring 2 GW:Image  (Bereich)       :  \n");
	printf("  5. Blob-Coloring 3 GW:Image  (Bereich)       :  \n");
	printf("  6. Schwerpunkt                     Blob Label:  \n");
	printf("  7. Groesster Blob                 Hintergrund:  \n");
	printf("  8. Rotation                        Blob Label:  \n");
	printf("  8. Invertieren                               [ ]  \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  Pfeiltaste    a/d       = Inkrement/Dekrement    \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
	char buf[10];
	setCursor(START_CURSOR_LINE, 49);
	//itoa(threshold, buf, 10);
	sprintf(buf,"%i",threshold);
	fputs(buf, stdout);
	for (int i = 0; i < 3; i++) {
		setCursor(START_CURSOR_LINE + i + 2, 49);
		//itoa(intervall[i], buf, 10);
		sprintf(buf,"%i",intervall[i]);
		fputs(buf, stdout);
	}
	setCursor(START_CURSOR_LINE + 5, 49);
	sprintf(buf,"%i",blob_label);
	fputs(buf, stdout);
	setCursor(START_CURSOR_LINE + 6, 49);
	sprintf(buf,"%i",background);
	fputs(buf, stdout);
	setCursor(START_CURSOR_LINE + 7, 49);
	sprintf(buf,"%i",blob_label2);
	fputs(buf, stdout);
}
void menu_segmentierung_help(int HelpFunc)
{
	cls();
	cursorOff();
	setColor(FOREGROUND_RED);
	printline();
	printf("   Hilfe Menu Segmentierung                        \n");
	printf(" \n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	switch (HelpFunc)
	{
	case 0:
		printf("  1. binaere Segmentierung (manuell)  \n");
		printf("  Segmentierung des Bildes anhand eines Schwellwertes. \n");
		break;
	case 1:
		printf("  2. Segmentierung nach Otsu  \n");
		printf("  Segmentierung des Bildes anhand eines Schwellwertes. \n");
		printf("  Der Schwellwert wird durch das Verfahren von Otsu berechnet. \n");
		break;
	case 2:
		printf("  3. Blob-Coloring  (Farbgebung aus Markiermatrix)\n");
		printf("  Ein Blob ist eine zusammenhaengende\n");
		printf("  Menge von Punkten im Vordergrund eines\n");
		printf("  Bildes. \n");
		printf("  Die Farbe des Blobs wird OHNE Zusammenhang \n");
		printf("  mit dem Originalbild gew�hlt. \n");
		break;
	case 3:
		printf("  4. Blob-Coloring (Farbgebung aus dem Originalbild)\n");
		printf("  **MIT AUSFRANSUNGEN**\n");
		printf("  Ein Blob ist eine zusammenhaengende\n");
		printf("  Menge von Punkten im Vordergrund eines\n");
		printf("  Bildes. \n");
		printf("  Die Farbe des Blobs wird MIT Zusammenhang \n");
		printf("  des Originalbildes gew�hlt. \n");
		break;
	case 4:
		printf("  5. Blob-Coloring (Farbgebung aus dem Originalbild)\n");
		printf("  **OHNE AUSFRANSUNGEN**\n");
		printf("  **In Problemzonen wird L-Maske vergroessert.**\n");
		printf("  **5 Iterationen bis zum endgueltogen Schwellwert**\n");
		printf("  Ein Blob ist eine zusammenhaengende\n");
		printf("  Menge von Punkten im Vordergrund eines\n");
		printf("  Bildes. \n");
		printf("  Die Farbe des Blobs wird MIT Zusammenhang \n");
		printf("  des Originalbildes gew�hlt. \n");
		break;
	case 5:
		printf("  6. Schwerpunktberechnung  \n");
		printf("  Berechnet den Flächenschwerpunkt eines Blobs\n");
		printf("  Markiert die BoundaryBox des Blobs\n");
		break;
	case 6:
		printf("  7. Groessten Blob suchen  \n");
		printf("  Sucht den groessten Blob unter bruecksichtung\n");
		printf("  des zu ignorierenden Hintergrunds\n");
		break;
	case 7:
		printf("  7. Rotation  \n");
		printf("  Errechnet die Ausrichtugn einer Pixelmenge\n");
		printf("  mit dem gegebenen Grauwert.\n");
		break;
	case 8:
		printf("  8. Invert  \n");
		printf("  Invertiert jeden Pixel\n");
		break;
	default:
		return;
	}
	printf("\n");
	printline();
	printf("  q                     = Zurueck                \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	fflush(stdin);
	cursorOn();
}
void menu_segmentierung(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	int iIMG[MAXXDIM][MAXYDIM];
	int wahl = 0;
	int cursorLine = 0;
	int threshold = 127;
	int blob_label = 0;
	int background = 255;
	int blob_label2 = 0;
	int intervall[3] = { 10,10,10 };
	char buf[20];
	print_menu_segmentierung(threshold, intervall, blob_label,background, blob_label2);
	while (wahl != KEY_ESC)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 49);
			wahl = getch();
			switch (wahl)
			{
			case KEY_ARROW_UP:
				if (cursorLine>0)
					cursorLine--;
				break;

			case KEY_ARROW_DOWN:
				if (cursorLine<8)
					cursorLine++;
				break;
			case KEY_ARROW_LEFT:
				if (threshold > 0 && cursorLine == 0){
					//itoa(--threshold, buf, 10);
					sprintf(buf,"%i",--threshold);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (intervall[0] > 0 && cursorLine == 2) {
					//itoa(--intervall[0], buf, 10);
					sprintf(buf,"%i",--intervall[0]);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (intervall[1] > 0 && cursorLine == 3) {
					//itoa(--intervall[1], buf, 10);
					sprintf(buf,"%i",--intervall[1]);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (intervall[2] > 0 && cursorLine == 4) {
					//itoa(--intervall[2], buf, 10);
					sprintf(buf,"%i",--intervall[2]);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (blob_label > 0 && cursorLine == 5) {
					//itoa(--intervall[2], buf, 10);
					sprintf(buf,"%i",--blob_label);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (background > 0 && cursorLine == 6) {
					//itoa(--intervall[2], buf, 10);
					sprintf(buf,"%i",--background);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (blob_label2 > 0 && cursorLine == 7) {
					//itoa(--intervall[2], buf, 10);
					sprintf(buf,"%i",--blob_label2);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				break;
			case KEY_ARROW_RIGHT:
				if (threshold < 255 && cursorLine == 0){
					sprintf(buf,"%i",++threshold);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (intervall[0] < 100 && cursorLine == 2) {
					sprintf(buf,"%i",++intervall[0]);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (intervall[1] < 100 && cursorLine == 3) {
					sprintf(buf,"%i",++intervall[1]);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (intervall[2] < 100 && cursorLine == 4) {
					sprintf(buf,"%i",++intervall[2]);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (blob_label < 255 && cursorLine == 5) {
					//itoa(--intervall[2], buf, 10);
					sprintf(buf,"%i",++blob_label);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (background < 255 && cursorLine == 6) {
					//itoa(--intervall[2], buf, 10);
					sprintf(buf,"%i",++background);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				if (blob_label2 < 255 && cursorLine == 7) {
					//itoa(--intervall[2], buf, 10);
					sprintf(buf,"%i",++blob_label2);
					strcat(buf, "    ");
					fputs(buf, stdout);
				}
				break;
			case KEY_ENTER:
				fflush(stdin);
				switch (cursorLine)
				{
				case 0:
					cls();
					segmentierung_binaer(img, img2, threshold);
					break;
				case 1:
					cls();
					segmentierung_von_otsu(img, img2);
					break;
				case 2:
					cls();
					blob_coloring_markersensitiv(img, img2,iIMG, intervall[0], 1);
					break;
				case 3:
					cls();
					//blob_coloring_imagesensitiv(img, img2, iIMG, intervall[1],1, 1,1);
					fast_blob_coloring(img, intervall[1]);
					break;
				case 4:
					cls();
					blob_coloring_imagesensitiv(img, img2, iIMG, intervall[2],1,1,1);
					break;
				case 5:
					cls();
					zeige_schwerpunkt(img, (unsigned int)blob_label);
					break;
				case 6:
					cls();
					biggestBlob(img,iIMG, background,500);
					break;
				case 7:
					cls();
					zeige_rotation(img,(unsigned int) blob_label2);
					break;
				case 8:
					cls();
					invert(img);
					break;
				default:break;
				}
				print_menu_segmentierung(threshold, intervall, blob_label, background, blob_label2);
				break;
			case KEY_F1:
				menu_segmentierung_help(cursorLine);
				print_menu_segmentierung(threshold, intervall, blob_label,background, blob_label2);
				break;
			}
		}
}

void menu_help(int HelpFunc)
{
	cls();
	cursorOff();
	setColor(FOREGROUND_VIOLET);
	printline();
	printf("   Hilfe Hauptmenu                                \n");
	printf("\n");
	printline();
	printf("\n");
	setColor(FOREGROUND_WHITE);
	switch (HelpFunc)
	{
	case 0:
		printf("  Bild einlesen:\n");
		printf("  Um dieses Programm zu benutzen muss zunaechst ein Bild, \n");
		printf("  mit der Funktion \"Bild einlesen\" ,in den Arbeitsspeicher, \n");
		printf("  geladen werden. \n");
		printf("  Daraufhin koennen Sie alle Funktionen des Programms \n");
		printf("  auf das eingelesene Bild anwenden. \n");
		break;
	case 1:
		printf("  Bild anzeigen:\n");
		printf("  Mit dieser Funktion koennen Sie lediglich ein  \n");
		printf("  Bild waehlen, um dieses anzuzeigen. \n");
		break;
	case 2:
		printf("  Funktionen binaere Bildverarbeitung: \n");
		printf("  Menu mit allen Funktionen zur binaeren Bildverarbeitung. \n");
		break;
	case 3:
		printf("  Funktionen Preprocessing: \n");
		printf("  Menu mit allen Funktionen zur Bildvorverarbeitung. \n");
		break;
	case 4:
		printf("  Funktionen Kantenerkennung: \n");
		printf("  Menu mit allen Funktionen zur Kantenerkennung. \n");
		break;
	case 5:
		printf("  Funktionen Textur: \n");
		printf("  Menu mit allen Funktionen zu Texturen. \n");
		break;
	case 6:
		printf("  Bild-Segmentierung:\n");
		printf("  Menu mit allen Funktionen zur Segmentierung\n");
		break;
	case 7:
		printf("  Beenden:\n");
		printf("  Beendet das Programm\n");
		break;
	default: break;
	}
	printf("\n");
	printline();
	printf("  q                     = Zurueck               \n");
	printline();
	int wahl = 0;
	while (wahl != KEY_ESC) {
		wahl = getch();
	}
	cursorOn();
}
void print_menu()
{
	cls();
	setColor(FOREGROUND_VIOLET);
	printline();
	printf("   Bildverarbeitung Hauptmenu                     \n");
	printf("\n");
	printline();
	setColor(FOREGROUND_WHITE);
	printf("\n");
	printf("  1. Bild einlesen           [ ]  \n");
	printf("  2. Bild anzeigen           [ ]  \n");
	printf("  3. Binaere Bildverarbeitung[ ]  \n");
	printf("  4. Preprocessing           [ ]  \n");
	printf("  5. Kantenerkennung         [ ]  \n");
	printf("  6. Texturen                [ ]  \n");
	printf("  7. Segmentierung           [ ]  \n");
	printf("  8. Beenden                 [ ]  \n");
	printf("\n");
	printline();
	printf("  Pfeiltaste    w/s       = Auswahl                \n");
	printf("  e                       = Bestaetigen            \n");
	printf("  q                       = Zurueck                \n");
	printf("  h                       = Info zur Funktion      \n");
	printline();
}
void menu()
{
	int wahl = 0;
	int cursorLine = 0;
	unsigned char image[MAXXDIM][MAXYDIM];
	unsigned char image2[MAXXDIM][MAXYDIM];
	memset(&image, 0, sizeof(image));
	/*
	image[10][100] = 255;
	writeImage_ppm(image,MAXXDIM, MAXYDIM);
	*/
	cursorSize(100);
	print_menu();
	int beenden = 0;
	while (wahl != 1)
	{
		setCursor(cursorLine + START_CURSOR_LINE, 31);
		wahl = getch();


			switch (wahl)
			{
			case KEY_ARROW_UP:
				if (cursorLine>0)
					cursorLine--;
				break;

			case KEY_ARROW_DOWN:
				if (cursorLine<7)
					cursorLine++;
				break;
			case KEY_ENTER:
				switch (cursorLine)
				{
				case 0:
					cls();
					readImage_ppm(image);
					break;
				case 1:
					cls();
					viewImage_ppm();
					wahl = 0;
					break;
				case 2:
					cls();
					menu_binaer_bv(image, image2);
					break;
				case 3:
					cls();
					menu_preprocessing(image, image2);
					break;
				case 4:
					cls();
					menu_kanten(image, image2);
					break;
				case 5:
					cls();
					menu_textur(image, image2);
					break;
				case 6:
					cls();
					menu_segmentierung(image, image2);
					break;
				case 7:
					beenden = 1;
					cls();
					return;
				default:break;
				}
				printf("print menu\n");
				print_menu();
				break;
			case KEY_F1:
				menu_help(cursorLine);
				print_menu();
				break;
			}
	}
}

extern void fast_blob_coloring(unsigned char img[MAXYDIM][MAXXDIM], int bereich);
extern void blob_coloring_imagesensitiv(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM],
		int intervall, int keine_fransen, int writeImage, int iterationen);
int main() {


	unsigned char image[MAXXDIM][MAXYDIM];
	unsigned char image2[MAXXDIM][MAXYDIM];
	int iIMG[MAXXDIM][MAXYDIM];
	readImage_ppm(image);
	//blob_coloring_imagesensitiv(image, image2, iIMG, 10, 1, 1, 1);
	fast_blob_coloring(image,10);
	return 0;


	menu();
}

/*

int main(void) {

	//CSI n ; m H
	//puts("\033[00;31mtest\033[00m pisse");
	printf("_______________________\n");
	printf("_______________________\n");
	printf("_______________________\n");
	printf("_______________________\n");
	printf("_______________________\n");
	printf("_______________________\n");
	setCursor(2,4);
	int x = getch();
	printf("du hast %c gedrückt", x);
	//printf("\033[25h");
	printf("\033[4m");
	printf("\e[?16;0;200c");
	sleep(3);
	printf("\1\e[6 q\2");

	puts(";test");
	//printf("\033[8m");
	sleep(3);
	cls();
	return EXIT_SUCCESS;
}
*/
