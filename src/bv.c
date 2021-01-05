#include "image_io.h"
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <math.h>
#include "bv.h"
#include "getchar.h"



/***********************************************************************************************************/
/*                      Sonstige Hilfsfunktionen                                                           */
/***********************************************************************************************************/
void init_cMatrix(unsigned char cMatrix[MAXXDIM][MAXYDIM], unsigned char val)
{
	memset(cMatrix, val, MAXXDIM*MAXYDIM * sizeof(unsigned char));
	return;
	/*
	unsigned char init = val > PIXEL_DEPTH - 1 ? 255 : val < 0 ? 0 : val;
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			cMatrix[i][j] = init;
	*/

}
void init_iMatrix(int iMatrix[MAXXDIM][MAXYDIM])
{
	memset(iMatrix, 0, MAXXDIM*MAXYDIM*sizeof(int));
	return;
	/*
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			iMatrix[i][j] = 0;
	*/

}
void init_fMatrix(float fMatrix[MAXXDIM][MAXYDIM])
{
	memset(fMatrix, 0, MAXXDIM*MAXYDIM*sizeof(float));
	return;
	/*
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			fMatrix[i][j] = 0.0;
	*/

}
//findet den vom Betrag gr��ten/kleinsten Wert der Matrix
int  find_abs_extremum_iMatrix(int min_max, int iMatrix[MAXXDIM][MAXYDIM])
{
	int extremum = min_max == MAX ? 0 : 0xFFFF;
	int abs = 0;
	if (min_max == MAX){
		for (int i = 0; i < MAXXDIM; i++){
			for (int j = 0; j < MAXYDIM; j++){
				abs = iMatrix[i][j] < 0 ? iMatrix[i][j] * -1 : iMatrix[i][j]; //sqrt(pow(iMatrix[i][j], 2));
				if (abs > extremum)
					extremum = abs;
			}
		}
	}else{
		for (int i = 0; i < MAXXDIM; i++){
			for (int j = 0; j < MAXYDIM; j++){
				abs = iMatrix[i][j] < 0 ? iMatrix[i][j] * -1 : iMatrix[i][j]; //sqrt(pow(iMatrix[i][j], 2));
				if (abs < extremum)
					extremum = abs;
			}
		}
	}

	return extremum;
}

//findet den vom Betrag gr��ten/kleinsten Wert der Matrix
float  find_abs_extremum_fMatrix(int min_max, float fMatrix[MAXXDIM][MAXYDIM])
{
	float extremum = min_max == MAX ? 0.0 : 0xFFFF;
	float abs = 0;
	if (min_max == MAX ){
		for (int i = 0; i < MAXXDIM; i++){
			for (int j = 0; j < MAXYDIM; j++){
				abs = fMatrix[i][j] < 0 ? fMatrix[i][j] * -1 : fMatrix[i][j]; // sqrt(pow(iMatrix[i][j], 2));
				if(abs > extremum)
					extremum = abs;
			}
		}
	} else {
		for (int i = 0; i < MAXXDIM; i++){
			for (int j = 0; j < MAXYDIM; j++){
				abs = fMatrix[i][j] < 0 ? fMatrix[i][j] * -1 : fMatrix[i][j]; // sqrt(pow(iMatrix[i][j], 2));
				if ( abs < extremum)
					extremum = abs;
			}
		}
	}
	return extremum;
}




// Bubble-Sort (Sortierverfahren), irgendwo aus dem internet kopiert :-)
// qsort(), w�re auch gegangen (Quicksort in ANSI c Bibliothek enthalten)
void bubblesort(int *array, int length)
{
	for (int i = 0; i < length - 1; ++i)
		for (int j = 0; j < length - i - 1; ++j)
			if (array[j] > array[j + 1]) {
				int tmp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = tmp;
			}
}

// Formel f�r die Fakult�t
double fakultaet(int n)
{
	double i, fak;
	for (i = 1, fak = 1; i <= n; i++)
		fak *= i;
	return fak;
}

// Binomialverteilung einer bestimmten stufe einholen
void get_bin_koeff(float bin_ver[50], int n, float normierung)
{
	if (n > 50)
		return;
	//float prev_gauss[50];

	for (int k = 0; k <= n; k++)		// n �ber k = n!/(n-k)!*k!
		bin_ver[k] = (float)fakultaet(n) / (float)(fakultaet(n - k) * fakultaet(k));
	// Summe aller Koeffizienten auf 100 normieren
	float sum = 0.0;
	for (int i = 0; i <= n; i++)
		sum += bin_ver[i];
	float faktor = normierung / sum;
	for (int i = 0; i <= n; i++)
		bin_ver[i] *= faktor;
}
void reset_blob_label(int iIMG[MAXXDIM][MAXYDIM], int oldLabel, int newLabel)
{
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			if (iIMG[x][y] == oldLabel)
				iIMG[x][y] = newLabel;
}


/***********************************************************************************************************/
/*                      Bin�re Bildverarbeitung                                                            */
/***********************************************************************************************************/
//Shrink Algorithmus
void shrink(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]) {
	int i,j;
	init_cMatrix(img2, 0);
	for (i = 1; i < MAXXDIM - 1; i++) 
		for (j = 1; j < MAXYDIM - 1; j++) 
			if (img[i][j] == 255 && img[i][j + 1] == 255 && img[i][j - 1] == 255 && img[i - 1][j] == 255 && img[i + 1][j] == 255) 
				img2[i][j] = 255;
}

// Blow Algorithmus
void blow(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]) {
	int i;
	int j;
	init_cMatrix(img2, 0);
	for (i = 1; i < MAXXDIM - 1; i++) 
		for (j = 1; j < MAXYDIM - 1; j++) 
			if (img[i][j] == 255) {
				img2[i][j] = 255;
				img2[i][j + 1] = 255;
				img2[i][j - 1] = 255;
				img2[i - 1][j] = 255;
				img2[i + 1][j] = 255;
			}
}


// Open Algorithmus
void funk_open(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]) {
	int x,y,i,j;
	printf("Geben Sie die Anzahl der Erosion- und Dilatation- Vorgaenge an\n");
	scanf("%d", &y);
	for (x = 0; x < y; x++) {
		shrink(img, img2);
		for (i = 0; i < MAXXDIM; i++) 
			for (j = 0; j < MAXYDIM; j++) 
				img[i][j] = img2[i][j];
	}
	for (x = 0; x < y; x++) {
		blow(img, img2);
		for(i = 0; i < MAXXDIM; i++) 
			for (j = 0; j < MAXYDIM; j++) 
				img[i][j] = img2[i][j];
	}
	system("cls");
}

// Close Algorithmus
void funk_close(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]) {
	int x,y,i,j;
	printf("Geben Sie die Anzahl der Dilatation- und Erosion- Vorgaenge an\n");
	scanf("%d", &y);
	for (x = 0; x < y; x++) {
		blow(img, img2);
		for (i = 0; i < MAXXDIM; i++) 
			for (j = 0; j < MAXYDIM; j++) 
				img[i][j] = img2[i][j];
	}
	for (x = 0; x < y; x++) {
		shrink(img, img2);
		for (i = 0; i < MAXXDIM; i++) 
			for (j = 0; j < MAXYDIM; j++) 
				img[i][j] = img2[i][j];
	}
	system("cls");
}

#define UNDEF 0
#define BURN 1
#define BURNED 2

// Z�hlen von Objekten in einem Bild
void grassfire(unsigned char img[MAXXDIM][MAXYDIM]) {
	char burn[MAXYDIM][MAXXDIM];
	//Initialisierung des hilfsarrays
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXXDIM; j++)
			burn[i][j] = UNDEF;

	// Objekt counter
	int object = 0;
	int burning = 0;
	for (int x = 1; x < MAXXDIM - 1; x++) {
		for (int y = 1; y < MAXYDIM - 1; y++) {
			if (img[x][y] == 255 && burn[x][y] == UNDEF)
			{
				object++;
				burning = 1;
				burn[x][y] = BURN;
				while (burning == 1)
				{
					burning = 0;
					for (int a = 1; a < MAXXDIM - 1; a++) {
						for (int b = 1; b < MAXYDIM - 1; b++) {
							if (burn[a][b] == BURN)
							{
								burning = 1;
								burn[a][b] = BURNED;
								if (img[a + 1][b] == 255 && burn[a + 1][b] == UNDEF)
									burn[a + 1][b] = BURN;

								if (img[a - 1][b] == 255 && burn[a - 1][b] == UNDEF)
									burn[a + 1][b] = BURN;

								if (img[a][b+1] == 255 && burn[a][b + 1] == UNDEF)
									burn[a][b + 1] = BURN;

								if (img[a][b - 1] == 255 && burn[a][b - 1] == UNDEF)
									burn[a][b - 1] = BURN;
							}
						}
					}
				}
			}
		}
	}
	printf("Das Bild enthaelt %i Objekte\n", object);
	printf("Druecken Sie eine beliebige Taste...");
	fflush(stdin);
	getch();
}

// Z�hlt wei�e Pixel im Bild
void count_white(unsigned char img[MAXXDIM][MAXYDIM]) {
	int i,j,grey = 0;
	for (i = 0; i < MAXXDIM; i++) 
		for (j = 0; j < MAXYDIM; j++) 
			if (img[i][j] != 0) 
				grey++;
	system("cls");
	printf("Die Anzahl der nicht schwarzen Pixel im Bild ist: %i\n", grey);
	printf("Druecken Sie eine beliebige Taste...");
	fflush(stdin);
	getch();
}



/***********************************************************************************************************/
/*                      Preprocessing                                                                      */
/***********************************************************************************************************/
// berechne normales Histogramm
void calc_absolut_histo(unsigned char img[MAXXDIM][MAXYDIM], int grey[PIXEL_DEPTH])
{
	for (int a = 0; a < PIXEL_DEPTH; a++)
		grey[a] = 0;
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXXDIM; j++)
			grey[img[i][j]]++;
}
void calc_rel_histo(unsigned char img[MAXXDIM][MAXYDIM], float grey[PIXEL_DEPTH])
{
	float div = (float)(MAXXDIM*MAXYDIM);
	for (int a = 0; a < PIXEL_DEPTH; a++)
		grey[a] = 0;
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXXDIM; j++)
			grey[img[i][j]]+= 1.0;
	for (int g = 0; g < PIXEL_DEPTH; g++)
		grey[g] /= div;
}

// Berechne das komulative Histogramm (monoton steigend)
void calc_kumulativ_histo(unsigned char img[MAXXDIM][MAXYDIM], int grey[PIXEL_DEPTH])
{
	calc_absolut_histo(img, grey);
	for (int i = 1; i < PIXEL_DEPTH; i++)
		grey[i] = grey[i - 1] + grey[i];
}

//berechnet und schreibt ein normales/kumulatives Histogramm
void histogramm(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int ART) {
	int min = 255, max = 0, min_index = 0, max_index = 0;
	int grey[PIXEL_DEPTH];
	if (ART == HISTO_NORMAL)
		calc_absolut_histo(img, grey);
	else if (ART == HISTO_KUMULATIV)
		calc_kumulativ_histo(img, grey);
	//minimale Anzahl eines Farbwertes (au�er der Anzahl 0)
	for (int b = 0; b < PIXEL_DEPTH; b++)
		if (grey[b] < min && grey[b] > 0) {
			min = grey[b];
			min_index = b;
		}
	//maximale Anzahl eines Farbwertes 
	for (int c = 0; c < PIXEL_DEPTH; c++)
		if (grey[c] > max) {
			max = grey[c];
			max_index = c;
		}
	system("cls");
	printf("Der Maximale Grauwert ist %i mit einer Anzahl von %i\n", max_index, max);
	printf("Der Minimale Grauwert ist %i mit einer Anzahl von %i\n", min_index, min);
	// Faktor berechnen f�r 256 Farbwerte
	float faktor = (float)PIXEL_DEPTH / (float)max;
	//initialisierungdes histogramms
	float skaliert[PIXEL_DEPTH];
	for (int a = 0; a < PIXEL_DEPTH; a++)
		skaliert[a] = 0;
	for (int i = 0; i < PIXEL_DEPTH; i++)
		skaliert[i] = (float)grey[i] * faktor;
	init_cMatrix(img2, 255);
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 255; y > 0; y--)
		{
			if (y >(255 - (int)skaliert[x]))
				img2[y][x] = 0;
			if (skaliert[x] < 1 && skaliert[x] > 0)
				img2[255][x] = 0;
		}
	printf("Druecken Sie eine beliebige Taste um das Histogramm abzuspechern...");
	fflush(stdin);
	getch();
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}


//Grauwerte des Bildes werden auf den vollen Umfang an Grauwerten gedehnt
void grauwert_dehnung(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]) {
	system("cls");
	// kleinster Grauwert
	int kl_grau = 0, gr_grau = 0;
	int grey[PIXEL_DEPTH];
	calc_absolut_histo(img, grey);
	// In Abbildung des Histogramms in den y-Zellen[255] nach pixeln suchen
	// kleinster Grau-Wert
	for (int a = 0; a < MAXXDIM; a++)
		if (grey[a] != 0){
			kl_grau = a;
			break;
		}
	//gr��ter Grau-Wert
	for (int a = 255; a > 0; a--)
		if (grey[a] != 0){
			gr_grau = a;
			break;
		}
	// Aufweiten der Grauwerte auf die maximale Breite
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img2[x][y] = (int)((float)255 / (float)(gr_grau - kl_grau) * (float)(img[x][y] - kl_grau));
	printf("Druecken Sie eine beliebige Taste um das bearbeitete Bild abzuspeichern...");
	fflush(stdin);
	getch();
	system("cls");
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}


//linearer Hisotgrammauslgleich 
// Quelle: Digitale Bildverarbeitung - Eine algorithmische Einf�hrung mit Java
// Autor: Wilhelm Burger, Mark James Burge
void linearer_histo_ausgleich(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int anzGrauWerte)
{
	int grey[PIXEL_DEPTH];
	calc_kumulativ_histo(img, grey);
	// linearer Ausgleich des Bildes
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
		{
			// Formel aus Quelle ( �berf�hrt von Java nach c++)
			int new_px = grey[img[x][y]] * (anzGrauWerte - 1) / (MAXXDIM * MAXYDIM);
			img2[x][y] = new_px > 255 ? 255 : new_px;
		}
	// Aufweiten der Grauwerte auf die maximale Breite
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img2[x][y] = (int)((float)(PIXEL_DEPTH-1) / (float)(anzGrauWerte - 1) * (float)(img2[x][y]));
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}


/***********************************************************************************************************/
/*                      Filterfunktionen                                                                   */
/***********************************************************************************************************/
//Medianilter mit quadratischer Dimension der Form n*n, wobei n GERADE UND UNGERADE sein darf!
void median_filter(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iDIMxy)
{
	// Sicherheitsabfrage, ob Eingabe ok
	if (iDIMxy < 2 || iDIMxy > 11)
		return;
	//init_cMatrix(img2,255);
	for (int x = 0; x < MAXXDIM ; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img2[x][y] = img[x][y];
	int MedianSort[11 * 11];
	// Anfangswerte setzen je nach gerader/ungerader Filtermatrix und Gr��e der Matrix
	for (int i = iDIMxy / 2 - (iDIMxy % 2 == 0 ? 1 : 0); i < MAXXDIM - iDIMxy / 2; i++) {
		for (int j = iDIMxy / 2 - (iDIMxy % 2 == 0 ? 1 : 0); j < MAXYDIM - iDIMxy / 2; j++)
		{
			//Werte f�r die Sortierung holen
			for (int z = 0; z < pow(iDIMxy, 2); z++)
				MedianSort[z] = 0;
			// Anfangswerte setzen, Randproblem beachten, da sonst Zugriff auf nicht indizierte Zellen.
			for (int x = i - ((iDIMxy / 2) - (iDIMxy % 2 == 0 ? 1 : 0)), counter = 0; x <= i + (iDIMxy / 2); ++x)
				for (int y = j - ((iDIMxy / 2) - (iDIMxy % 2 == 0 ? 1 : 0)); y <= j + (iDIMxy / 2); ++y)
					MedianSort[counter++] = img[x][y];
			bubblesort(MedianSort, pow(iDIMxy, 2));
			// Wenn gerade Matrix, dann Mittelwert aus den mittleren Beiden werten als Median bilden
			img2[i][j] = iDIMxy % 2 == 0 ? (MedianSort[iDIMxy*iDIMxy / 2] + MedianSort[iDIMxy*iDIMxy / 2 - 1]) / 2 : MedianSort[iDIMxy*iDIMxy / 2];
		}
	}
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}

//Mittelwertfiler f�r Filter-Matrizen der Form n*n, wobei n GERADE UND UNGERADE sein darf!
void mittelwert_filter(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iDIMxy, int Gewichtung)
{
	// Sicherheitsabfrage, ob Eingabe ok
	if (iDIMxy < 2 || iDIMxy > 11)
		return;
	//init_cMatrix(img2,255);
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img2[x][y] = img[x][y];
	int zw;
	// Anfangswerte setzen je nach gerader/ungerader Filtermatrix und Gr��e der Matrix
	for (int i = iDIMxy / 2 - (iDIMxy % 2 == 0 ? 1 : 0); i < MAXXDIM - iDIMxy / 2; i++) {
		for (int j = iDIMxy / 2 - (iDIMxy % 2 == 0 ? 1 : 0); j < MAXYDIM - iDIMxy / 2; j++)
		{
			// Anfangswerte setzen je nach gerader/ungerader Filtermatrix, Randproblem beachten
			for (int x = i - ((iDIMxy / 2) - (iDIMxy % 2 == 0 ? 1 : 0)), a = zw = 0; x <= i + (iDIMxy / 2); ++x, ++a) {
				for (int y = j - ((iDIMxy / 2) - (iDIMxy % 2 == 0 ? 1 : 0)), b = 0; y <= j + (iDIMxy / 2); ++y, ++b) {
					if (iDIMxy % 2 == 0)
						// Wenn a == b und iDIMxy/2-1 erreicht ist, ist dies der Mittelpunkt pixel f�r gerade Matrix
						zw += a == b && a == (iDIMxy / 2 - 1) ? img[x][y] * Gewichtung : img[x][y];
					else
						//  Wenn a == b und iDIMxy/2 erreicht ist, ist dies der Mittelpunkt pixel f�r ungerade Matrix
						zw += a == b && a == (iDIMxy / 2) ? img[x][y] * Gewichtung : img[x][y];
				}
			}
			// Mittelwert bilden
			zw /= pow(iDIMxy, 2) + (Gewichtung - 1);
			img2[i][j] = (unsigned char)(zw > 255 ? 255 : zw < 0 ? 0 : zw);
		}
	}
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}
// Gaussfilter: img - Eingabebild, img2 - Ausgabebild, scale - Gr��e des Filters
void gauss_filter(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int scale)
{
	// Sicherheitsabfrage
	if (scale > 50 || scale < 3 || scale % 2 == 0)
		return;
	float bin_ver[50];
	for (int i = 0; i < 50; i++)
		bin_ver[i] = 0;
	// Hole binomialkoeffizienten (Summe normiert auf 100)
	get_bin_koeff(bin_ver, scale - 1, 1.0);
	// dynamisch Speicher reservieren, direkt mit 0 initialisiert
	float **gauss_filter = (float**)calloc(scale + 3, sizeof(float*));
	for (int i = 0; i < scale + 3; i++)
		gauss_filter[i] = (float*)calloc(scale + 3, sizeof(float));
	// 2D-Gauss-Filtermatrix berechnen
	for (int x = 0, a = 0; x < scale; x++)
		for (int y = 0; y < scale; y++, a++)
			gauss_filter[x][y] = bin_ver[x] * bin_ver[y];
	//Filter auf Bild anwenden
	int iIMG[MAXXDIM][MAXYDIM];
	init_iMatrix(iIMG);
	// Anfangswerte setzen je nach gerader/ungerader Filtermatrix und Gr��e der Matrix
	for (int i = scale / 2; i < MAXXDIM - (scale / 2); i++) {
		for (int j = scale / 2; j < MAXYDIM - (scale / 2); j++)
		{
			// zwischenwert der Filterposition
			float zw = 0.0;
			// Anfangswerte setzen je nach gerader/ungerader Filtermatrix, Randproblem beachten
			for (int x = i - (scale / 2), a = 1; x <= i + (scale / 2); x++, a++)
				for (int y = j - (scale / 2), b = 1; y <= j + (scale / 2); y++, b++)
					zw += gauss_filter[a][b] * (float)img[x][y];
			iIMG[i][j] = (int)zw;
		}
	}
	// Speicher freigeben
	for (int i = 0; i < scale; i++)
		free(gauss_filter[i]);
	free(gauss_filter);
	//init_cMatrix(img2, 0);
	memcpy(&img2, &img, sizeof(img));
	/*
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img2[x][y] = img[x][y];
	*/
	for (int x = (scale / 2); x < MAXXDIM - (scale / 2); x++)
		for (int y = (scale / 2); y < MAXYDIM - (scale / 2); y++)
			img2[x][y] = (unsigned char)(iIMG[x][y] > 255 ? 255 : iIMG[x][y] < 0 ? 0 : iIMG[x][y]);
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}


/***********************************************************************************************************/
/*                      Kantendetektion                                                                    */
/***********************************************************************************************************/
// Erste Ableitung in x Richtung, Sobeloperator
void sobelx(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int sobelx[MAXXDIM][MAXYDIM])
{
	init_cMatrix(img2, (PIXEL_DEPTH / 2));
	init_iMatrix(sobelx);
	int summe[8] = { 0,0,0,0,0,0,0,0 };
	for (int i = 1; i < MAXXDIM - 1; i++) {
		for (int j = 1; j < MAXYDIM - 1; j++) {

			summe[0] = img[i][j + 1] * -2;
			summe[1] = img[i][j - 1] * 2;
			summe[2] = img[i + 1][j + 1] * -1;
			summe[3] = img[i - 1][j - 1];
			summe[4] = img[i - 1][j + 1] * -1;
			summe[5] = img[i + 1][j - 1];
			for (int z = 0; z < 6; z++)
				sobelx[i][j] += summe[z];
		}
	}
	// Pixelwerte skalieren und Bild schreiben
	int max = find_abs_extremum_iMatrix(MAX, sobelx);
	// Skalierung 0<127<255
	float faktor;
	faktor = (float)(PIXEL_DEPTH / 2) / (float)max;
	for (int x = 1; x < MAXXDIM - 1; x++)
		for (int y = 1; y < MAXYDIM - 1; y++)
			img2[x][y] += (unsigned char)((float)sobelx[x][y] * faktor);
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}
// Erste Ableitung in y-Richtung, Sobeloperator
void sobely(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int sobely[MAXXDIM][MAXYDIM]) {
	init_cMatrix(img2, (PIXEL_DEPTH / 2));
	init_iMatrix(sobely);
	int summe[8] = { 0,0,0,0,0,0,0,0 };
	for (int i = 1; i < MAXXDIM - 1; i++) {
		for (int j = 1; j < MAXYDIM - 1; j++) {
			summe[0] = img[i + 1][j] * -2;
			summe[1] = img[i - 1][j] * 2;
			summe[2] = img[i + 1][j + 1] * -1;
			summe[3] = img[i - 1][j - 1];
			summe[4] = img[i - 1][j + 1];
			summe[5] = img[i + 1][j - 1] * -1;
			for (int z = 0; z < 6; z++)
				sobely[i][j] += summe[z];
		}
	}
	// Pixelwerte skalieren und Bild schreiben
	int max = find_abs_extremum_iMatrix(MAX, sobely);
	// Skalierung 0<127<255
	float faktor;
	faktor = (float)(PIXEL_DEPTH / 2) / (float)max;
	for (int x = 1; x < MAXXDIM - 1; x++)
		for (int y = 1; y < MAXYDIM - 1; y++)
			img2[x][y] += (unsigned char)((float)sobely[x][y] * faktor);
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}

// Kobination der Operatoren Sobel in x und y Richtung
void sobelxy(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int sobelx[MAXXDIM][MAXYDIM], int sobely[MAXXDIM][MAXYDIM]) {
	init_cMatrix(img2, (PIXEL_DEPTH / 2));
	int iIMG[MAXXDIM][MAXYDIM];
	init_iMatrix(iIMG);
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			iIMG[i][j] = sqrt(pow(sobely[i][j], 2) + pow(sobelx[i][j], 2));
	// Pixelwerte skalieren und Bild schreiben
	int max = find_abs_extremum_iMatrix(MAX, iIMG);
	// Skalierung 0<127<255
	float faktor;
	faktor = (float)(PIXEL_DEPTH - 1) / (float)max;
	for (int x = 1; x < MAXXDIM - 1; x++)
		for (int y = 1; y < MAXYDIM - 1; y++)
			img2[x][y] = (unsigned char)((float)iIMG[x][y] * faktor);
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}

// zweite Ableitung durch Laplace Operator
void laplace(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int Umgebung) {
	init_cMatrix(img2, (PIXEL_DEPTH / 2));
	int summe[3] = { 0,0,0 };
	int zw = 0;
	// Matrix der Laplace Operation berechnen
	//Filter auf Bild anwenden
	int iIMG[MAXXDIM][MAXYDIM];
	init_iMatrix(iIMG);
	for (int i = 1; i < MAXXDIM - 1; i++) {
		for (int j = 1; j < MAXYDIM - 1; j++) {
			if (Umgebung == LAPLACE_4) {
				summe[0] = img[i - 1][j];
				summe[1] = img[i][j - 1] + (img[i][j] * (-4)) + img[i][j + 1];
				summe[2] = img[i + 1][j];
			}
			else if (Umgebung == LAPLACE_8)
			{
				summe[0] = img[i - 1][j - 1] + img[i - 1][j] + img[i - 1][j + 1];
				summe[1] = img[i][j - 1] + (img[i][j] * (-8)) + img[i][j + 1];
				summe[2] = img[i + 1][j - 1] + img[i + 1][j] + img[i + 1][j + 1];
			}
			zw = 0;
			for (int z = 0; z < 3; z++) {
				zw += summe[z];
			}
			// Dieser Wert kann positiv und negativ sein
			iIMG[i][j] = zw;
		}
	}
	// Pixelwerte skalieren und Bild schreiben
	int max = find_abs_extremum_iMatrix(MAX, iIMG);
	// Skalierung 0<127<255
	float faktor;
	faktor = (float)(PIXEL_DEPTH / 2) / (float)max;
	for (int x = 1; x < MAXXDIM - 1; x++)
		for (int y = 1; y < MAXYDIM - 1; y++)
		{
			img2[x][y] += (unsigned char)((float)iIMG[x][y] * faktor);
		}
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}
//Difference of Gaussian 
void difference_of_gaussian(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int scale, int grundton)
{
	// Sicherheitsabfrage
	if (scale > 50 || scale < 3 || scale % 2 == 0)
		return;
	float bin_ver[2][50], diff[50];
	for (int i = 0; i < 50; i++)
		bin_ver[0][i] = bin_ver[1][i] = diff[i] = 0;
	// Hole binomialkoeffizienten (Summe normiert auf 100)
	get_bin_koeff(bin_ver[0], scale - 1, 100.0);
	get_bin_koeff(bin_ver[1], scale - 3, 100.0);
	// Differenz der Koeffizienten in 1D
	for (int i = 0; i < scale - 1; i++)
	{
		if (i == 0 || i == scale - 1)
		{
			diff[i] = bin_ver[0][i];
			continue;
		}
		diff[i] = bin_ver[0][i + 1] - bin_ver[1][i];
	}
	// dynamisch Speicher reservieren, direkt mit 0 initialisiert
	float **diff_of_gauss = (float**)calloc(scale + 3, sizeof(float*));
	for (int i = 0; i < scale + 3; i++)
		diff_of_gauss[i] = (float*)calloc(scale + 3, sizeof(float));
	// 2D-Gauss-Filtermatrix berechnen
	for (int x = 0, a = 0; x < scale; x++)
		for (int y = 0; y < scale; y++, a++)
			diff_of_gauss[x][y] = diff[x] * diff[y];
	//Filter auf Bild anwenden
	int iIMG[MAXXDIM][MAXYDIM];
	init_iMatrix(iIMG);
	// Anfangswerte setzen je nach gerader/ungerader Filtermatrix und Gr��e der Matrix
	for (int i = scale / 2; i < MAXXDIM - (scale / 2); i++) {
		for (int j = scale / 2; j < MAXYDIM - (scale / 2); j++)
		{
			// zwischenwert der Filterposition
			float zw = 0.0;
			// Anfangswerte setzen je nach gerader/ungerader Filtermatrix, Randproblem beachten
			for (int x = i - (scale / 2), a = 1; x <= i + (scale / 2); x++, a++) {
				for (int y = j - (scale / 2), b = 1; y <= j + (scale / 2); y++, b++) {
					 zw += diff_of_gauss[a][b] * (float)img[x][y];
				}
			}
			iIMG[i][j] = (int)zw;
		}
	}
	// Speicher freigeben
	for (int i = 0; i < scale; i++)
		free(diff_of_gauss[i]);
	free(diff_of_gauss);
	grundton = grundton < 0 ? 0 : grundton > 255 ? 255 : grundton;
	init_cMatrix(img2, grundton);
	// Pixelwerte skalieren und Bild schreiben
	int max = find_abs_extremum_iMatrix(MAX, iIMG);
	// Skalierung 0<127<255
	float faktor;
	faktor = grundton == 0 || grundton == 255 ? (float)255 / (float)max : (float)grundton / (float)max;
	for (int x = 1; x < MAXXDIM - 1; x++)
		for (int y = 1; y < MAXYDIM - 1; y++)
		{
			if (grundton != 0 && grundton != 255)
				img2[x][y] += (unsigned char)((float)iIMG[x][y] * faktor);
			else if (grundton == 0)
				img2[x][y] = (unsigned char)sqrt(pow((int)((float)iIMG[x][y] * faktor), 2));
			else if (grundton == 255)
				img2[x][y] = 255 - (unsigned char)sqrt(pow((int)((float)iIMG[x][y] * faktor), 2));
		}
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}

/***********************************************************************************************************/
/*                      Texturenerkennung                                                                  */
/***********************************************************************************************************/
// Texturenerkennung mit den Laws-Masken
void laws_textur(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{
	init_cMatrix(img2,255);
	float fArray[MAXXDIM][MAXYDIM];
	init_fMatrix(fArray);

	int masken[9][9] = {	 
		{-1,0,1,-2,0,2,-1,0,1},		// L3E3
		{-1,2,-1,-2,4,-2,-1,2,-1},	// L3S3
		{-1,2,-1,0,0,0,1,-2,1},		// E3L3
		{1,0,-1,0,0,0,-1,0,1},		// E3E3
		{1,-2,1,0,0,0,-1,2,-1},		// E3S3
		{-1,-2,-1,2,4,2,-1,-2,-1},	// S3L3
		{1,0,-1,-2,0,2,1,0,-1},		// S3E3
		{1,-2,1,-2,4,-2,1,-2,1},	// S3S3
		{1,2,1,2,4,2,1,2,1 }		// L3L3
	};
	for (int x = 1; x < MAXXDIM - 1; x++) {
		for (int y = 1; y < MAXYDIM - 1; y++) {
			// Werte aus dem Bild holen
			float zw[9] = { 0,0,0,0,0,0,0,0,0 };
			for (int mask = 0; mask < 8; mask++) // L3L3 wird nicht angewandt
				for (int i = x - 1,mask_val = 0; i <= x + 1; i++) 
					for (int j = y - 1; j <= y + 1; j++,mask_val++) 
						zw[mask] += ((float)masken[mask][mask_val] * (float)img[i][j]);
			for (int zw_mask = 0; zw_mask < 8; zw_mask++)		// zw_mask = zwischenwert der Maske
				fArray[x][y] += zw[zw_mask]* zw[zw_mask];		// Alle quadratisch addieren
			fArray[x][y] = sqrt(fArray[x][y]);	// Qurzel ziehen //Pytagoras
		}
	}
	float max = find_abs_extremum_fMatrix(MAX, fArray);			// vom Betrag her das Max finden
	float faktor = (float)PIXEL_DEPTH / max;					// Skalierung 0-255
	for (int x = 1; x < MAXXDIM - 1; x++)
		for (int y = 1; y < MAXYDIM - 1; y++)
			img2[x][y] = (unsigned char)(fArray[x][y] * faktor);
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}

// Statistik 2. Ordnung
void cooccurence_matrix(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], float fIMG[MAXXDIM][MAXYDIM], int direction, int save)
{
	init_cMatrix(img2, 255);
	if(save == 1)
		init_fMatrix(fIMG);
	int begin_x = direction != 135 ? 0 : 1, begin_y = direction == 135 ? 1 : 0;
	int end_x = direction == 0 || direction == 45 ? MAXXDIM - 1 : MAXXDIM, end_y = direction != 0 ? MAXYDIM - 1 : MAXYDIM;

	for (int x = begin_x; x < end_x; x++)
		for (int y = begin_y; y < end_y; y++)
			if (direction == 0) {
				fIMG[img[x + 1][y]][img[x][y]] += 1.0;
				fIMG[img[x][y]][img[x + 1][y]] += 1.0;
			}
			else if (direction == 45) {
				fIMG[img[x + 1][y + 1]][img[x][y]] += 1.0;
				fIMG[img[x][y]][img[x + 1][y + 1]] += 1.0;
			}
			else if (direction == 90) {
				fIMG[img[x][y + 1]][img[x][y]] += 1.0;
				fIMG[img[x][y]][img[x][y + 1]] += 1.0;
			}	
			else if (direction == 135) {
				fIMG[img[x - 1][y + 1]][img[x][y]] += 1.0;
				fIMG[img[x][y]][img[x - 1][y + 1]] += 1.0;
			}	
	if (save == 1)
	{
		int max = find_abs_extremum_fMatrix(MAX, fIMG);
		float faktor = (float)(PIXEL_DEPTH / 16) / (float)max;
		for (int x = 0; x < MAXXDIM; x++)
			for (int y = 0; y < MAXYDIM; y++) {
				float scale = (fIMG[x][y] * faktor);
				if (scale > 0.0 && scale < 1.0)
					scale = 1.0;
				int val = (int)scale * (PIXEL_DEPTH / 16);
				img2[x][y] = 255 - (unsigned char)(val < 0 ? 0 : val > (PIXEL_DEPTH - 1) ? (PIXEL_DEPTH - 1) : val);
			}
		writeImage_ppm(img2, MAXXDIM, MAXYDIM);
	}
	float sum = 0.0;
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			sum += fIMG[i][j];
}

void cooc_matrix_kombi_asm(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], float fIMG[MAXXDIM][MAXYDIM], int graustufen)
{
	init_cMatrix(img2, 255);
	init_fMatrix(fIMG);
	for (int dir = 0; dir < 4; dir++)
		cooccurence_matrix(img, img2, fIMG, dir * 45, 0);
	// Mittelwert bilden
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			fIMG[x][y] /= 4.0;

	int max = find_abs_extremum_fMatrix(MAX, fIMG);
	float faktor = (float)(PIXEL_DEPTH / graustufen) / (float)max;
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++) {
			float scale = (fIMG[x][y] * faktor);
			if (scale > 0.0 && scale < 1.0)
				scale = 1.0;
			int val = (int)scale * (PIXEL_DEPTH / graustufen);
			img2[x][y] = 255 - (unsigned char)(val < 0 ? 0 : val >(PIXEL_DEPTH - 1) ? (PIXEL_DEPTH - 1) : val);
		}
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
	calc_asm_energie(fIMG);
}


void calc_asm_energie(float fIMG[MAXXDIM][MAXYDIM])
{
	float fASM = 0,sum = 0;
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			sum += fIMG[x][y];
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++) 
			fASM += ((float)pow(fIMG[x][y] / sum, 2));
			
	system("cls");
	printf(" ������������������������������������������������������������ͻ\n");
	printf(" �  Anzahl der Eintraege in der Matrix:%20f   �\n", sum);
	printf(" �  Der ASM Wert des Bildes betraegt:%22f   �\n",fASM);
	printf(" �  Druecken Sie eine Taste um zurueck zum Menu zu gelangen.  �\n");
	printf(" ������������������������������������������������������������ͼ\n");
	printf("\n");
	getch();
}
/***********************************************************************************************************/
/*                      Segmentierung                                                                      */
/***********************************************************************************************************/

void segmentierung_von_otsu(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM])
{

	// Verfahren von Otsu
	// optimaler Schwellwert
	int iSchwelle = 0;
	float dim_xy = (float)(MAXXDIM*MAXYDIM);
	float grey[PIXEL_DEPTH];
	calc_rel_histo(img, grey);
	// zwischenspeicher f�r Maximalen Quotienten
	float quotient = 0.0;
	// jeden Grauwert ausprobieren
	for (int g = 0; g < PIXEL_DEPTH; g++)
	{
		// arithmeithscer Mittelwert gesamt, Klasse 0, Klasse 1, Auftrittswahrscheinlichkeit Klasse 0, Auftrittswahrscheinlicheit Klasse 1
		float gx = 0, g0 = 0, g1 = 0, p0 = 0, p1 = 0;
		int counter = 0;
		// arithmetische Mittelwerte berechnen
		for (int x = 0; x < MAXXDIM; x++)
			for (int y = 0; y < MAXYDIM; y++)
			{
				gx += (float)img[x][y];
				if (img[x][y] <= g) {
					g0 += (float)img[x][y];
					counter++;
				}
				else
					g1 += (float)img[x][y];
			}
		gx /= (float)(dim_xy);				// arithmetischer Mittelwert aller Grauwerte
		g0 /= (float)counter;				// arithmeithscer Mittelwert der Grauwerte der Klasse 0
		g1 /= (float)(dim_xy - counter);	// arithmetischer Mittelwert der Grauwerte der Klasse 1

		for (int p = 0; p < g; p++)
			p0 += grey[p];					// Auftrittswahrscheinlichkeit der Klasse 0					
		p1 = 1.0 - p0;						// Auftrittswahrscheinlichkeit der Klasse 1

		float var0 = 0, var1 = 0;
		for (int i = 0; i < g; i++)
			var0 += pow(i - g0, 2) * grey[i];	// Varianz innerhalb der Klasse 0
		for (int i = g + 1; i < PIXEL_DEPTH; i++)
			var1 += pow(i - g1, 2) * grey[i]; // Varianz innerhalb der Klasse 1

		float varianz_zw = p0 * pow(g0 - gx, 2) + p1 * pow(g1 - gx, 2);	// Varainz zwischen den Klassen
		float varianz_in = p0 * var0 + p1 * var1;						// Varianz in den Klassen

		// Maximalen Qutienten zwischen Varianz in den Klassen zu Varianz zwischen den klassen suchen
		if ((varianz_zw / (varianz_in == 0.0 ? 1.0 : varianz_in)) > quotient) {
			quotient = (varianz_zw / (varianz_in == 0.0 ? 1.0 : varianz_in));
			iSchwelle = g;
		}
	}
	printf("Der Schwellwert nach Otsu: %3i\n", iSchwelle);
	printf("Druecken Sie eine beliebige Taste, um die Operation durchzufuehren.\n");
	getch();
	fflush(stdin);
	segmentierung_binaer(img, img2, iSchwelle);
}

void segmentierung_binaer(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int threshold)
{
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img2[x][y] = img[x][y] <= threshold ? 0 : 255;
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
}

// Blob-Coloring mit L�sung der Ausfransungen, mit Iterationsverfahren zum Verbessern der Segmentierung
void blob_coloring_imagesensitiv(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM],
		int intervall, int keine_fransen, int writeImage, int iterationen)
{
	int bereich = 5, max = 0, null_labels = 0;
	init_cMatrix(img2, 0);
	init_iMatrix(iIMG);

	int dy = 0, dx = 0, dy2 = 0, dx2 = 0;
	int l_extend = 0;

	float ink_bereich = (float)intervall / (float)iterationen;
	for (int iter = 0; iter < iterationen; iter++){
		cls();
		printf("Bitte warten: Iteration %3i von %3i", iter + 1, iterationen);
		bereich = (int)((float)(iter + 1)*ink_bereich);
		unsigned int blob = 0;
		for (int x = 1; x < MAXXDIM; x++){
			for (int y = 1; y < MAXYDIM; y++) {
				//<----- Diff( x_c, x_l) > bereich------------------------------- && Diff( x_c, x_u) > bereich    -> x_c = neues Label
				dy = img[x][y]-img[x][y-1];
				dy = dy < 0 ? dy * -1 : dy;
				dx = img[x][y]-img[x-1][y];
				dx = dx < 0 ? dx * -1 : dx;

				if ((dy > bereich) && (dx > bereich) && (iter == 0))
					iIMG[x][y] = ++blob;
				else if ((dy > bereich) && (dx > bereich) && (iter != 0))
					iIMG[x][y] = ++blob;
				//<---- Diff( center, links) <= bereich------------------------------- && Diff( center, unten) <= bereich   -> x_c = x_u
				else if ((dy <= bereich) && (dx <= bereich) && (iter == 0)) {
					l_extend = (bereich / 2) < 1 ? 1 : (bereich / 2);
					dy2 = img[x][y] - img[x][y - l_extend];
					dy2 = dy2 < 0 ? dy2 * -1 : dy2;
					dx2 = img[x][y] - img[x - l_extend][y];
					dx2 = dx2 < 0 ? dx2 * -1 : dx2;
					if (iIMG[x - 1][y] != iIMG[x][y - 1] &&	// Label stimmen nicht �berein -> Selbe Region?
						dy2 <= bereich && // L-Maske verbreitern -> immernoch im Bereich?
						dx2 <= bereich && // L-Maske verl�ngern -> immernoch im Bereich?
						x > l_extend && y > l_extend) {
						int old_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x - 1][y] : iIMG[x][y - 1];
						int new_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x][y - 1] : iIMG[x - 1][y];
						reset_blob_label(iIMG, old_label, new_label);
					}
					iIMG[x][y] = iIMG[x - 1][y];
				}
				else if (dy <= bereich && dx <= bereich && iter != 0) {
					l_extend = (bereich / 2) < 1 ? 1 : (bereich / 2);
					dy2 = img[x][y] - img[x][y - l_extend];
					dy2 = dy2 < 0 ? dy2 * -1 : dy2;
					dx2 = img[x][y] - img[x - l_extend][y];
					dx2 = dx2 < 0 ? dx2 * -1 : dx2;
					if (iIMG[x - 1][y] != iIMG[x][y - 1] &&	// Label stimmen nicht �berein -> Selbe Region?
						dy2 <= bereich && // L-Maske verbreitern -> immernoch im Bereich?
						dx2 <= bereich && // L-Maske verlängern -> immernoch im Bereich?
						x > l_extend && y > l_extend && keine_fransen == 1) {
						int old_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x - 1][y] : iIMG[x][y - 1];
						int new_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x][y - 1] : iIMG[x - 1][y];
						reset_blob_label(iIMG, old_label, new_label);
					}
					iIMG[x][y] = iIMG[x - 1][y];
				}
				//<---- Diff( x_c, x_l) <= bereich------------------------------- && Diff( x_c, x_u) > bereich
				else if (dy <= bereich && dx > bereich && iter == 0) {
					if (y == 1)
						iIMG[x][y] = ++blob;
					else
						iIMG[x][y] = iIMG[x][y - 1];
				}
				else if (dy <= bereich && dx > bereich && iter != 0) {
					if (y == 1)
						iIMG[x][y] = ++blob;
					else
						iIMG[x][y] = iIMG[x][y - 1];
				}
				//<---- Diff( x_c, x_l) > bereich------------------------------- && Diff( x_c, x_u) <= bereich
				else if (dy > bereich && dx <= bereich && iter == 0) {
					iIMG[x][y] = iIMG[x - 1][y];
				}
				else if (dy > bereich && dx <= bereich && iter != 0) {
					iIMG[x][y] = iIMG[x - 1][y];
				}
			}
		}
		// Ränder der Merkermatrix markieren
		for (int x = 0; x < MAXXDIM; x++)
			for (int y = 0; y < MAXYDIM; y++)
				if (!(x == 0 || y == 0))
					continue;
				else if (x == 0)
					iIMG[x][y] = iIMG[x + 1][y];
				else if (y == 0)
					iIMG[x][y] = iIMG[x][y + 1];

		// Ausgabebild nach dem Eingabebild einf�rben
		if (writeImage == 1) {
			// anzahl der Blobs herausfinden
			max = find_abs_extremum_iMatrix(MAX, iIMG);
			null_labels = 0;
			for (int i = 0; i <= max; i++) {
				float counter = 0, mittelwert = 0;
				for (int x = 0; x < MAXXDIM; x++)
					for (int y = 0; y < MAXYDIM; y++)
						if (iIMG[x][y] == i) {
							mittelwert += (iter == 0 ? (float)img[x][y] : (float)img2[x][y]);
							counter += 1.0;
						}
				if (counter > 0)// keine div durch 0 zulassen
					mittelwert /= counter;
				if (mittelwert != 0.0) {
					for (int x = 0; x < MAXXDIM; x++)
						for (int y = 0; y < MAXYDIM; y++)
							if (iIMG[x][y] == i)
								img2[x][y] = (unsigned char)mittelwert;
				}
				else
					null_labels++;
			}
		}
	}
	writeImage_ppm(img2, MAXXDIM, MAXYDIM);
	cls();
	printf("Anzahl der Blobs: %i\n", (max - null_labels));
	printf("Druecken Sie eine beliebige Taste zum fortfahren.\n");
	getch();
	fflush(stdin);
}

unsigned int find_blobs(unsigned char img[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM], int bereich){
	init_iMatrix(iIMG);
	unsigned int blob = 0;
	int dy = 0, dx = 0;

	for (int x = 1; x < MAXXDIM; x++){
		for (int y = 1; y < MAXYDIM; y++)
		{
			dy = img[x][y]-img[x][y-1];
			dy = dy < 0 ? dy * -1 : dy;
			dx = img[x][y]-img[x-1][y];
			dx = dx < 0 ? dx * -1 : dx;
			//       Diff( x_c, x_l) > bereich                                && Diff( x_c, x_u) > bereich    -> x_c = neues Label
			if (dy > bereich && dx > bereich)
				iIMG[x][y] = ++blob;
			//      Diff( x_c, x_l) <= bereich                                && Diff( x_c, x_u) <= bereich   -> x_c = x_u
			else if (dy <= bereich && dx <= bereich) {
				//Grauwerte sind im Intervall, aber die labels im Merker sind nicht identisch -> falsches label
				if (iIMG[x - 1][y] != iIMG[x][y - 1]){
					// Label überscheiben! Höherwertiges Label wird überschrieben, da dies falsch inkrementiert wurde
					int old_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x - 1][y] : iIMG[x][y - 1];
					int new_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x][y - 1] : iIMG[x - 1][y];
					--blob;
					reset_blob_label(iIMG, old_label, new_label);
				}
				// nun gewönlich x_u, x_c zuweisen
				iIMG[x][y] = iIMG[x - 1][y];
			}
			//      Diff( x_c, x_l) <= bereich                                && Diff( x_c, x_u) > bereich
			else if (dy <= bereich && dx > bereich)
					iIMG[x][y] = iIMG[x][y - 1];
			//      Diff( x_c, x_l) > bereich                                && Diff( x_c, x_u) <= bereich
			else if (dy > bereich && dx <= bereich)
				iIMG[x][y] = iIMG[x - 1][y];
		}
	}
	return blob;
}

unsigned short ** allocateTwoDimenArray(unsigned short row, unsigned short col){
    unsigned short ** ptr = (unsigned short **) malloc(sizeof(unsigned short *)*row);
    for(int i = 0; i < row; i++)
        ptr[i] = (unsigned short *) malloc(sizeof(unsigned short)*col);
    return ptr;
}

void destroyTwoDimenArray(unsigned short ** ptr, unsigned short row,unsigned short col){
    for(int i = 0; i < row; i++)
        free(ptr[i]);
    free(ptr);
}



#define L_EXTEND 0
#if 0
unsigned int fast_blob_coloring(unsigned char img[MAXYDIM][MAXXDIM], int bereich){
    //unsigned short ** iIMG = allocateTwoDimenArray(MAXYDIM, MAXXDIM);
	printf("sizeof unsigned short %i\n", sizeof(unsigned short));
    unsigned short iIMG[MAXYDIM][MAXXDIM];
    // start code for stm32
    unsigned short blob_alias[10000];
    //for(int i = 0; i<10000;){
    //	blob_alias[i]=i++;
    //}
    unsigned short max_blob = 10000;
    for(int i = 0; i< 10000; i++){
    	blob_alias[i] = i;
    }
	for (int x = 0; x < MAXXDIM; x++){
		for (int y = 0; y < MAXYDIM; y++){
			iIMG[y][x] = 0;
		}
	}
	unsigned short blob = 0;
	int dy = 0, dx = 0, dy2 = 0, dx2 = 0;
	int l_extend = 0;
	printf("arrays initialized\n");
	for (int y = 1; y < MAXYDIM; y++){
		for (int x = 1; x < MAXXDIM; x++){
			dx = img[y][x]-img[y][x-1];
			dx = dx < 0 ? dx * -1 : dx;
			dy = img[y][x]-img[y-1][x];
			dy = dy < 0 ? dy * -1 : dy;
			//       Diff( x_c, x_l) > bereich                                && Diff( x_c, x_u) > bereich    -> x_c = neues Label
			if (dy > bereich && dx > bereich){
				if(blob == (max_blob-1)){
					goto func_error;
				}
				iIMG[y][x] = (unsigned short)++blob;
				blob_alias[blob] = (unsigned short)blob;
			//      Diff( x_c, x_l) <= bereich                                && Diff( x_c, x_u) <= bereich   -> x_c = x_u
			}else if (dy <= bereich && dx <= bereich) {
				iIMG[y][x] = iIMG[y][x - 1];
				continue;
#if L_EXTEND == 1
				l_extend = (bereich / 2) < 1 ? 1 : (bereich / 2);
				dx2 = img[y][x] - img[y][x - l_extend];
				dx2 = dx2 < 0 ? dx2 * -1 : dx2;
				dy2 = img[y][x] - img[y - l_extend][x];
				dy2 = dy2 < 0 ? dy2 * -1 : dy2;
				if ((iIMG[y - 1][x] != iIMG[y][x - 1]) &&	// Label stimmen nicht �berein -> Selbe Region?
					(dy2 <= bereich) && // L-Maske verbreitern -> immernoch im Bereich?
					(dx2 <= bereich) && // L-Maske verlängern -> immernoch im Bereich?
					(x > l_extend) && (y > l_extend)) {

					//Grauwerte sind im Intervall, aber die labels im Merker sind nicht identisch -> falsches label
					unsigned short higher_label = iIMG[y-1][x] > iIMG[y][x-1] ? iIMG[y-1][x] : iIMG[y][x-1];
					unsigned short lower_label = iIMG[y-1][x] > iIMG[y][x-1] ? iIMG[y][x-1] : iIMG[y-1][x];

					printf("blob collision\n");
					// Blob alias liste verwalten
					// TODO: write function to create alias table
					// check ob höheres label ein alias von dem kleineren ist
					for (int x = 0; x < MAXXDIM; x++)
						for (int y = 0; y < MAXYDIM; y++)
							if (iIMG[y][x] == higher_label)
								iIMG[x][y] = lower_label;
					/*
					if(blob_alias[higher_label] != blob_alias[lower_label]){
						solve_alias(blob_alias, lower_label, higher_label);
					}
					*/
					iIMG[y][x] = lower_label;
					continue;
				}
				iIMG[y][x] = iIMG[y][x- 1];
#elif L_EXTEND == 0
				//Grauwerte sind im Intervall, aber die labels im Merker sind nicht identisch -> falsches label
				unsigned short higher_label = iIMG[y-1][x] > iIMG[y][x-1] ? iIMG[y-1][x] : iIMG[y][x-1];
				unsigned short lower_label = iIMG[y-1][x] > iIMG[y][x-1] ? iIMG[y][x-1] : iIMG[y-1][x];
				if (iIMG[y - 1][x] != iIMG[y][x - 1]){
					//printf("blob collision\n");
					// Blob alias liste verwalten
					// TODO: write function to create alias table
					// check ob höheres label ein alias von dem kleineren ist
					if(blob_alias[higher_label] != blob_alias[lower_label]){
						solve_alias(blob_alias, higher_label ,lower_label);
					}
				}
				// das kleinere Label vergeben, da dieser blob als erstes erkannt wurde
				iIMG[y][x] = lower_label;
#endif
			//      Diff( x_c, x_l) <= bereich                                && Diff( x_c, x_u) > bereich
			}else if (dy <= bereich && dx > bereich){
					iIMG[y][x] = iIMG[y][x - 1];
			//      Diff( x_c, x_l) > bereich                                && Diff( x_c, x_u) <= bereich
			}else if (dy > bereich && dx <= bereich){
				iIMG[y][x] = iIMG[y - 1][x];
			}
		}
	}

	for(int i = 0; i < blob; i++)
		printf("aliases %u to %u\n", i, blob_alias[i]);

	printf("override aliases\n");
	for(int x = 0; x < MAXXDIM; x++){
		for(int y = 0; y < MAXYDIM; y++){
			iIMG[y][x] = blob_alias[iIMG[y][x]];
		}
	}

	// find max in label_matrix
	printf("find max in label matrix\n");
	unsigned short extremum =0;
	for (int x = 0; x < MAXXDIM; x++){
		for (int y = 0; y < MAXYDIM; y++){
			if (iIMG[y][x] > extremum){
				printf("new extremum %u, x%i, y%i \n", iIMG[y][x], x,y);
				extremum = iIMG[y][x];
			}
		}
	}
	printf("extremum found %u\n", extremum);
	printf("scale labels for image\n");
	float faktor = (float)(PIXEL_DEPTH - 1) / (float)extremum;
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img[y][x] = (unsigned char)((float)iIMG[y][x] * faktor);
	writeImage_ppm(img, MAXXDIM, MAXYDIM);
	//system("cls");
	printf("Anzahl der Blobs: %i", blob);
	getch();
	fflush(stdin);
    // end code for stm32

    //destroyTwoDimenArray(iIMG, MAXYDIM, MAXXDIM);
    return blob;
func_error:
	//destroyTwoDimenArray(iIMG, MAXYDIM, MAXXDIM);
	return 0;
}
#endif

void solve_alias(unsigned short alias_arr[10000], unsigned short fist_alias, unsigned short second_alias, unsigned short blobcount){
	//alias_arr[second_alias] = fist_alias;
	//printf("enter solve_alias first %hu second %hu\n", fist_alias, second_alias);
	if( blobcount > 10000 -1)
		return;

	for(int i = 0; i< blobcount; i++){
		if(alias_arr[i] == second_alias)
			alias_arr[i] = fist_alias;
	}
	return;
	unsigned short tmp = alias_arr[fist_alias];
	while(alias_arr[tmp] != tmp){
		tmp = alias_arr[tmp];
	}
	alias_arr[second_alias] = tmp;
}

void fast_blob_coloring(unsigned char img[MAXYDIM][MAXXDIM], int bereich){
	int  max = 0, null_labels = 0;
	int iIMG[MAXYDIM][MAXXDIM];
	init_iMatrix(iIMG);

    unsigned short blob_alias[10000];
    unsigned short max_blob = 10000;
    for(int i = 0; i< 10000; i++){
    	blob_alias[i] = i;
    }

	int dy = 0, dx = 0, dy2 = 0, dx2 = 0;
	int l_extend = 0;

	unsigned int blob = 0;
	for (int x = 1; x < MAXXDIM; x++){
		for (int y = 1; y < MAXYDIM; y++) {
			//<----- Diff( x_c, x_l) > bereich------------------------------- && Diff( x_c, x_u) > bereich    -> x_c = neues Label
			dy = img[y][x]-img[y][x-1];
			dy = dy < 0 ? dy * -1 : dy;
			dx = img[y][x]-img[y-1][x];
			dx = dx < 0 ? dx * -1 : dx;

			if ((dy > bereich) && (dx > bereich)){
				if(blob == max_blob - 1)
					return;// nicht gut genug vorverarbeitet, zu viele pixelregionen
				iIMG[y][x] = ++blob;
				//blob_alias[blob] = (unsigned short)blob;
			//<---- Diff( center, links) <= bereich------------------------------- && Diff( center, unten) <= bereich   -> x_c = x_u
			}else if ((dy <= bereich) && (dx <= bereich)) {
				/*
				l_extend = (bereich / 2) < 1 ? 1 : (bereich / 2);
				dy2 = img[x][y] - img[x][y - l_extend];
				dy2 = dy2 < 0 ? dy2 * -1 : dy2;
				dx2 = img[x][y] - img[x - l_extend][y];
				dx2 = dx2 < 0 ? dx2 * -1 : dx2;
				if (iIMG[x - 1][y] != iIMG[x][y - 1] &&	// Label stimmen nicht �berein -> Selbe Region?
					dy2 <= bereich && // L-Maske verbreitern -> immernoch im Bereich?
					dx2 <= bereich && // L-Maske verl�ngern -> immernoch im Bereich?
					x > l_extend && y > l_extend) {
					int first_alias = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x - 1][y] : iIMG[x][y - 1];
					int second_alias = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x][y - 1] : iIMG[x - 1][y];
					//reset_blob_label(iIMG, old_label, new_label);

					if(blob_alias[first_alias] != blob_alias[second_alias]){
						solve_alias(blob_alias,first_alias,second_alias);
					}
				}
				*/
				if (iIMG[y - 1][x] != iIMG[y][x - 1]){
					img[y][x] = 127;
					printf("alias y-1 %u, x-y %u, label y-1 %u, x-1 %u \n",blob_alias[iIMG[y - 1][x]],blob_alias[iIMG[y][x-1]],iIMG[y - 1][x],iIMG[y][x-1] );
					int second_alias = iIMG[y - 1][x] > iIMG[y][x - 1] ? iIMG[y - 1][x] : iIMG[y][x - 1];
					int first_alias = iIMG[y - 1][x] > iIMG[y][x - 1] ? iIMG[y][x - 1] : iIMG[y - 1][x];
					if(blob_alias[second_alias]!= blob_alias[first_alias])
						blob_alias[second_alias]= blob_alias[first_alias];
					//reset_blob_label(iIMG, second_alias, first_alias);
					//if(blob_alias[first_alias] != blob_alias[second_alias]){
						//solve_alias(blob_alias,first_alias,second_alias, (unsigned short)blob);
					//}
				}
				iIMG[y][x] = iIMG[y][x-1];
			}
			//<---- Diff( x_c, x_l) <= bereich------------------------------- && Diff( x_c, x_u) > bereich
			else if (dy <= bereich && dx > bereich) {
				if (x == 1){
					iIMG[y][x] = ++blob;
					//blob_alias[blob] = (unsigned short)blob;
				}else
					iIMG[y][x] = iIMG[y][x - 1];
			}
			//<---- Diff( x_c, x_l) > bereich------------------------------- && Diff( x_c, x_u) <= bereich
			else if (dy > bereich && dx <= bereich) {
				iIMG[y][x] = iIMG[y - 1][x];
			}
		}
	}
	writeImage_ppm(img, MAXXDIM, MAXYDIM);
	return;
	// Ränder der Merkermatrix markieren
	for (int x = 0; x < MAXXDIM; x++){
		iIMG[0][x] = iIMG[1][x];
	}
	for (int y = 0; y < MAXYDIM; y++){
		iIMG[y][0] = iIMG[y][1];
	}

	if(1){
		printf("override aliases\n");
		for(int x = 0; x < MAXXDIM; x++){
			for(int y = 0; y < MAXYDIM; y++){
				iIMG[y][x] = blob_alias[iIMG[y][x]];
			}
		}
		for(int i = 0; i < blob; i++)
			printf("aliases %u to %u\n", i, blob_alias[i]);
	}
	// Ausgabebild nach dem Eingabebild einf�rben

	// anzahl der Blobs herausfinden
if(0){
	max = find_abs_extremum_iMatrix(MAX, iIMG);
	null_labels = 0;
	for (int i = 0; i <= max; i++) {
		float counter = 0, mittelwert = 0;
		for (int x = 0; x < MAXXDIM; x++)
			for (int y = 0; y < MAXYDIM; y++)
				if (iIMG[x][y] == i) {
					mittelwert += (float)img[x][y];
					counter += 1.0;
				}
		if (counter > 0)// keine div durch 0 zulassen
			mittelwert /= counter;
		if (mittelwert != 0.0) {
			for (int x = 0; x < MAXXDIM; x++)
				for (int y = 0; y < MAXYDIM; y++)
					if (iIMG[x][y] == i)
						img[x][y] = (unsigned char)mittelwert;
		}
		else
			null_labels++;
	}


}else{
	max = find_abs_extremum_iMatrix(MAX, iIMG);
	float faktor = (float)(PIXEL_DEPTH - 1) / (float)max;
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img[y][x] = (unsigned char)((float)iIMG[y][x] * faktor);
}
	writeImage_ppm(img, MAXXDIM, MAXYDIM);
	system("cls");
	printf("Anzahl der Blobs: %i", max - null_labels);
	getch();
	fflush(stdin);
}


//Labling connected components in an image, where non-zero pixels are 
// deemed as foreground, and will be labeled with an positive integer
// while background pixels will be labled with zeros.
//Input and output are 2D matrices of size h-by-w.
//Return maxLabel. Output labels are continuously ranged between [0,maxLabel).
//Assume each pixel has 4 neighbors.
//yuxianguo, 2018/3/27

int bwLabel(unsigned char img[MAXXDIM][MAXYDIM])
{
	unsigned int label[MAXYDIM][MAXXDIM];
	memset(label, 0, MAXXDIM*MAXYDIM*sizeof(int));
	//link[i]:
	//(1) link label value "i" to its connected component (another label value);
	//(2) if link[i] == i, then it is a root.
	int maxComponents = (MAXXDIM * MAXYDIM >> 1) + 1; //max possible connected components (38400 @ 320x240px)
	int link[maxComponents];
	int lb = 1, x, y, a, b, t;
	int h = MAXYDIM;
	int w = MAXXDIM;
	link[0] = 0;
	//first row
	if(img[0][0]) {
		label[0][0] = lb;
		link[lb] = lb;
		lb++;
	}
	for(x = 1; x < w; x++)
		if(img[0][x]) {
			if(label[0][x - 1])
				label[0][x] = label[0][x - 1];
		else {
			label[0][x] = lb;
			link[lb] = lb;
			lb++;
		}
	}
	//bw += w, p += w;
	//rest rows
	for(y = 1; y < h; y++) {
		if(img[y][0]) {
			if(label[y-1][x])
				label[y][0] = label[y-1][0];
			else {
				label[y][0] = lb;
				link[lb] = lb;
				lb++;
			}
		}
		for(x = 1; x < w; x++){
			if(img[y][x]) {
				a = label[y][x - 1];
				b = label[y-1][x]; //left & top
				if(a) {
					if(a == b)
						label[y][x] = a;
					else {
						//find root of a
						t = a;
						while(a != link[a])
							a = link[a];
						label[y][x] = link[t] = a;
						if(b) {
							//find root of b
							t = b;
							while(b != link[b])
								b = link[b];
							link[t] = b;
							//link b to a or link a to b, both fine
							if(a < b) link[b] = a; else link[a] = b;
						}
					}
				}
				else if(b) {
					//find root of b
					t = b;
					while(b != link[b])
						b = link[b];
					label[y][x] = link[t] = b;
				}
				else {
					//generate a new component
					label[y][x] = lb;
					link[lb] = lb;
					lb++;
				}
			}
		}
	}
 
	//Rearrange the labels with continuous numbers
	t = 1;
	for(x = 1; x < lb; x++)
		if(x == link[x]) {
			link[x] = -t; //using negative values to denote roots
			t++;
		}
	for(x = 1; x < lb; x++) {
		//find the root of x
		y = x;
		while(link[y] >= 0)
			y = link[y];
		//set the value of label x
		link[x] = link[y];
	}
	//Negative to positive
	for(x = 1; x < lb; x++)
		link[x] = -link[x];
 
	//Replace existing label values by the corresponding root label values
	//p = label;
	for(y = 0; y < h; y++)
		for(x = 0; x < w; x++)
			label[y][x] = link[label[y][x]];

	float faktor = (float)(PIXEL_DEPTH - 1) / (float)t;
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img[y][x] = (unsigned char)((float)label[y][x] * faktor);

	writeImage_ppm(img,MAXXDIM,MAXYDIM);
	return t; //num components (maxLabel + 1)
}
int bwLabelThresholding(unsigned char img[MAXXDIM][MAXYDIM], int thresholdSteps, int minBlobSize)
{
	if(thresholdSteps < 2)	// 0: error division durch 0, 1: keine sinnvollen ergebnisse
		return 0;
	unsigned int label[MAXYDIM][MAXXDIM];
	memset(label, 0, MAXXDIM*MAXYDIM*sizeof(int));
	//link[i]:
	//(1) link label value "i" to its connected component (another label value);
	//(2) if link[i] == i, then it is a root.
	int maxComponents = (MAXXDIM * MAXYDIM >> 1) + 1; //max possible connected components (38400 @ 320x240px)
	int link[maxComponents];
	int lb = 1, x, y, a, b, t, i, max;
	int h = MAXYDIM;
	int w = MAXXDIM;
	int range = PIXEL_DEPTH /thresholdSteps;
	int th_bot, th_top, last_lb;
	link[0] = 0;
	last_lb = 0;
	//first row
	/*
	for(i = 0; i < thresholdSteps+1; i++){
		if(i == thresholdSteps){
			th_bot = PIXEL_DEPTH - (PIXEL_DEPTH % thresholdSteps);
			if(th_bot == PIXEL_DEPTH)	// keine restpixel übrig zum segmentieren
				break;
			th_top = PIXEL_DEPTH - 1;
		}else{
			th_bot = i * (PIXEL_DEPTH /thresholdSteps);
			th_top = th_bot + (PIXEL_DEPTH/thresholdSteps);
		}
		*/
		if(img[0][0]>= th_bot && img[0][0] < th_top) {
			label[0][0] = lb;
			link[lb] = lb;
			lb++;
		}
		for(x = 1; x < w; x++)
			if(img[0][x]>=th_bot && img[0][x] < th_top) {
				if(label[0][x - 1]>last_lb)
					label[0][x] = label[0][x - 1];
			else {
				label[0][x] = lb;
				link[lb] = lb;
				lb++;
			}
		}
		for(y = 1; y < h; y++) {
			if(img[y][0] >= th_bot && img[y][0] < th_top) {
				if(label[y-1][x]>last_lb)
					label[y][0] = label[y-1][0];
				else {
					label[y][0] = lb;
					link[lb] = lb;
					lb++;
				}
			}
			for(x = 1; x < w; x++){
				if(img[y][x] >= th_bot && img[y][x] < th_top) {
					a = label[y][x - 1];
					b = label[y-1][x]; //left & top
					if(a>last_lb) {
						if(a == b)
							label[y][x] = a;
						else {
							//find root of a
							t = a;
							while(a != link[a])
								a = link[a];
							label[y][x] = link[t] = a;
							if(b) {
								//find root of b
								t = b;
								while(b != link[b])
									b = link[b];
								link[t] = b;
								//link b to a or link a to b, both fine
								if(a < b) link[b] = a; else link[a] = b;
							}
						}
					}
					else if(b>last_lb) {
						//find root of b
						t = b;
						while(b != link[b])
							b = link[b];
						label[y][x] = link[t] = b;
					}
					else {
						//generate a new component
						label[y][x] = lb;
						link[lb] = lb;
						lb++;
					}
				}
			}
		}
		last_lb = lb;
	}

	//Rearrange the labels with continuous numbers
	t = 1;
	for(x = 1; x < lb; x++)
		if(x == link[x]) {
			link[x] = -t; //using negative values to denote roots
			t++;
		}
	for(x = 1; x < lb; x++) {
		//find the root of x
		y = x;
		while(link[y] >= 0)
			y = link[y];
		//set the value of label x
		link[x] = link[y];
	}
	//Negative to positive
	for(x = 1; x < lb; x++)
		link[x] = -link[x];

	//Replace existing label values by the corresponding root label values
	//p = label;
	for(y = 0; y < h; y++)
		for(x = 0; x < w; x++)
			label[y][x] = link[label[y][x]];
	// override link table with zeros

	memset(link, 0, sizeof(int) * maxComponents);
	// calculate blobsizes
	for(x = 0; x < w; x++){
		for(y=0;y<h;y++){
			link[label[y][x]]++;
		}
	}
	// find biggest blob -> must be background
	max = 0;
	int max_index = 0;
	for(i = 0; i < lb; i++){
		if(link[i] > max){
			max = link[i];
			max_index = i;
		}
	}
	// set labels to background if they are smaller then accepted, 1:background, 0: blob
	for(i = 0; i < lb; i++){
			link[i] = link[i] < minBlobSize ? 1 : 0;
	}
	// override to small blobs
	for(x = 0; x < w; x++){
		for(y=0;y<h;y++){
			if(link[label[y][x]])
			label[y][x] = max_index;
		}
	}
	max = 0;
	for(x = 0; x < w; x++){
		for(y=0;y<h;y++){
			if(label[y][x]> 0)
				max = label[y][x];
		}
	}

	float faktor = (float)(PIXEL_DEPTH - 1) / (float)t;//max;
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			//img[y][x] = label[y][x] != 0 ? 255 :0;
			img[y][x] = (unsigned char)((float)label[y][x] * faktor);

	writeImage_ppm(img,MAXXDIM,MAXYDIM);
	return t; //num components (maxLabel + 1)
}

/*
	//Replace existing label values by the corresponding root label values
	//p = label;
	for(y = 0; y < h; y++)
		for(x = 0; x < w; x++)
			label[y][x] = link[label[y][x]];
	// override link table with zeros

	memset(link, 0, sizeof(int) * maxComponents);
	// calculate blobsizes
	for(x = 0; x < w; x++){
		for(y=0;y<h;y++){
			link[label[y][x]]++;
		}
	}
	// find biggest blob -> must be background
	max = 0;
	int max_index = 0;
	for(i = 0; i < lb; i++){
		if(link[i] > max){
			max = link[i];
			max_index = i;
		}
	}
	// set labels to background if they are smaller then accepted, 1:background, 0: blob
	for(i = 0; i < lb; i++){
			link[i] = link[i] < minBlobSize ? 1 : 0;
	}
	// override to small blobs
	for(x = 0; x < w; x++){
		for(y=0;y<h;y++){
			if(link[label[y][x]])
			label[y][x] = max_index;
		}
	}
	max = 0;
	for(x = 0; x < w; x++){
		for(y=0;y<h;y++){
			if(label[y][x]> 0)
				max = label[y][x];
		}
	}

*/
// Blob-Coloring mit Lösung der Ausfransungen, ohne Iterationsverafahren: zum segmentieren von einfachen Objekten ( evtl binaerisiert )
void blob_coloring_markersensitiv(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM], int bereich, int writeImage)
{
	find_blobs(img,iIMG, bereich);
	// Blob-Coloring nur als Bild schreiben, wenn es direkt aus dem Menu aufgreufen wird
	// handelt es sich um ein Binaerbild ?
	init_cMatrix(img2, 0);
	if (writeImage == 1) {
		int max = find_abs_extremum_iMatrix(MAX, iIMG);
		float faktor = (float)(PIXEL_DEPTH - 1) / (float)max;
		for (int x = 0; x < MAXXDIM; x++)
			for (int y = 0; y < MAXYDIM; y++)
				img2[x][y] = (unsigned char)((float)iIMG[x][y] * faktor);
		writeImage_ppm(img2, MAXXDIM, MAXYDIM);
		system("cls");
		printf("Anzahl der Blobs: %i", max);
		getch();
		fflush(stdin);
	}
}

// Annahme: Der groesste Blob ist der Hintergrund
// Der zweitgroesste Blob ist das Hauptobjekt des Bildes
// Alle anderen werden dem Hintergrund gleich gemacht
// Der Hintergrund wird mit 255 markiert
// Das Objekt mit 0



void bubblesort_blob(Blob *blobs, int length)
{
	for (int i = 0; i < length - 1; ++i)
		for (int j = 0; j < length - i - 1; ++j)
			if (blobs[j].blob_size > blobs[j + 1].blob_size) {
				Blob tmp;
				tmp.blob_size= blobs[j].blob_size;
				tmp.blob_label= blobs[j].blob_label;
				blobs[j].blob_size = blobs[j + 1].blob_size;
				blobs[j].blob_label = blobs[j + 1].blob_label;
				blobs[j + 1].blob_size = tmp.blob_size;
				blobs[j + 1].blob_label = tmp.blob_label;
			}
}

void biggestBlob(unsigned char img[MAXXDIM][MAXYDIM],unsigned int iIMG[MAXXDIM][MAXYDIM], int background_threshold, int min_blobsize){
	printf("min blobsize %i\n", min_blobsize);
	// Marker Matrix mit 0 initialisieren
	memset(iIMG,0,sizeof(iIMG));
	// Als erstes Oberhalb des Schwellwertes alle Pixel auf 255 schreiben
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			if((img[x][y] >= background_threshold))
				img[x][y] = 255;
	// markiere alle blobs (farbabstufungbeim erreichnen 10)
	unsigned int max_blob = find_blobs(img,iIMG,5);
	printf("blob count %i\n",max_blob);

	// dynamisch Speicher reservieren, direkt mit 0 initialisiert
	//Blob *blobs = (Blob*)calloc(max_blob+1, sizeof(Blob));
	Blob *blobs = (Blob*)malloc((max_blob*  sizeof(Blob)) +1);
	memset(blobs, 0, max_blob);
	// Blobgroessen und das entsprechende Label ermitteln
	for (int x = 0; x < MAXXDIM; x++){
		for (int y = 0; y < MAXYDIM; y++){
			blobs[iIMG[x][y]].blob_size++;
			blobs[iIMG[x][y]].blob_label = iIMG[x][y];
		}
	}
	// den groessten Blob finden
	Blob biggest_blob;
	memset(&biggest_blob,0,sizeof(Blob));
	for(int i = 0; i < (max_blob+1); i++){
		if(blobs[i].blob_size > biggest_blob.blob_size){
			biggest_blob.blob_size = blobs[i].blob_size;
			biggest_blob.blob_label = blobs[i].blob_label;
		}
	}
	// Alle Blobs löschen ausser den Hintergrund und das Objekt
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			if(iIMG[x][y] == biggest_blob.blob_label)// Hintergrund
				img[x][y] = 255;
			else if(blobs[iIMG[x][y]].blob_size > min_blobsize)	// Objekt
				img[x][y] = 0;
			else
				img[x][y] = 255;	// Unwichtige Blobs-> Hintergrund
	free(blobs);
	writeImage_ppm(img, MAXXDIM, MAXYDIM);

}


// Funktion zur erstellen eines Berichtes �ber Tablettenblister f�r Pr�sentation
void blister_blob(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM])
{
	int iOben = 1380, iUnten = 1310;
	// Blob-Coloring ohne schreiben des Bildes aufrufen.
	blob_coloring_markersensitiv(img, img2, iIMG, 20,0);
	int max_blob = find_abs_extremum_iMatrix(MAX,iIMG);

	// dynamisch Speicher reservieren, direkt mit 0 initialisiert
	int *groesse_blobs = (int*)calloc(max_blob+1, sizeof(int));
	int *ist_tablette = (int*)calloc(max_blob+1, sizeof(int));

	for (int i = 0; i <= max_blob; i++)
		for (int x = 0; x < MAXXDIM; x++)
			for (int y = 0; y < MAXYDIM; y++)
				if (iIMG[x][y] == i)
					groesse_blobs[i]++;
	for (int ok = 0; ok <= max_blob; ok++)
		ist_tablette[ok] = (groesse_blobs[ok] > iUnten && groesse_blobs[ok] < iOben) ? 1 : 0;


	FILE *fp;
	char line[100];
	fp = fopen("c:\\bv\\report.txt", "w");
	if (fp == NULL) 
		printf("Datei konnte nicht geoeffnet werden.\n");
	else {
		sprintf(line, "Anzahl der Blobs: %10i\n", max_blob);
		fputs(line, fp);
		sprintf(line, "Tablette definiert durch die Groesse g: [%5i < g <%5i]\n", iUnten, iOben);
		fputs(line, fp);
		fputs("\n", fp);
		for (int i = 0; i<=max_blob; i++) {
			sprintf(line, "Blob %3i Flaecheninhalt: %10i\n", i, groesse_blobs[i]);
			fputs(line, fp);
			if (ist_tablette[i] == 1)
				sprintf(line, "Blob %3i:Tablette\n", i);
			else
				sprintf(line, "Blob %3i:keine Tablette \n", i);
			fputs(line, fp);
		}
		int anz_zab = 0;
		for (int i = 0; i <= max_blob; i++)
		{
			if (ist_tablette[i] == 1)
				anz_zab++;
		}
		fputs("\n", fp);
		sprintf(line, "Die Anzahl der Tabletten betraegt: %i", anz_zab);
		fputs(line, fp);
		fclose(fp);
	}
	free(groesse_blobs);
	free(ist_tablette);
	printf("Report wurde erstellt.\n");
	printf("Druecken Sie eine Taste.");
	getch();
}

Schwerpunkt schwerpunkt(unsigned char img[MAXXDIM][MAXYDIM], unsigned int bloblabel){
	printf("blob label %u\n", bloblabel);
	Schwerpunkt s;
	memset(&s,0,sizeof(Schwerpunkt));
	s.boundary_box.x1 = MAXXDIM;
	s.boundary_box.y1 = MAXYDIM;
	double sx = 0, sy = 0;
	for (int x = 0; x < MAXXDIM; x++){
		for (int y = 0; y < MAXYDIM; y++){
			if((unsigned int)img[y][x] == bloblabel){
				++s.A;
				// für boundary box
				s.boundary_box.x1 = x < s.boundary_box.x1 ? x : s.boundary_box.x1;
				s.boundary_box.x2 = x > s.boundary_box.x2 ? x : s.boundary_box.x2;
				s.boundary_box.y1 = y < s.boundary_box.y1 ? y : s.boundary_box.y1;
				s.boundary_box.y2 = y > s.boundary_box.y2 ? y : s.boundary_box.y2;
				// für flächenschwerpunkt
				// +0,5 weil der 0. Pixel einen abstand zu 0 von 0,5 hat.
				sx += (double)x+0.5;
				sy += (double)y+0.5;
			}
		}
	}
	if(s.A > 0){
		sx /= (double)s.A;
		sy /= (double)s.A;
	}
	s.x = (unsigned int)sx;
	s.y = (unsigned int)sy;
	return s;
}

void zeige_schwerpunkt(unsigned char img[MAXXDIM][MAXYDIM],unsigned int bloblabel){
	printf("blob label %u\n", bloblabel);
	Schwerpunkt s = schwerpunkt(img, bloblabel);
	if(s.A > 0){
		// Test direkt momente Berechnen
		Momente m = widerstandsmomente(img, s, bloblabel);
		printf("Ix: %ld\n",m.Ix);
		printf("Iy: %ld\n",m.Iy);
		printf("Ixy: %ld\n",m.Ixy);

		//Markiere Schwerpunkt im Bild
		for(int x = 0; x< MAXXDIM; x++)
			img[s.y][x] = (char)bloblabel - (PIXEL_DEPTH/2);
		for(int y = 0; y< MAXXDIM; y++)
			img[y][s.x] = (char)bloblabel - (PIXEL_DEPTH/2);

		// Boundary Box im Bild Markieren
		for(int x = s.boundary_box.x1; x <= s.boundary_box.x2; x++){
			img[s.boundary_box.y1][x] = (char)bloblabel - (PIXEL_DEPTH/2);
			img[s.boundary_box.y2][x] = (char)bloblabel - (PIXEL_DEPTH/2);
		}
		for(int y = s.boundary_box.y1; y <= s.boundary_box.y2; y++){
			img[y][s.boundary_box.x1] = (char)bloblabel - (PIXEL_DEPTH/2);
			img[y][s.boundary_box.x2] = (char)bloblabel - (PIXEL_DEPTH/2);
		}
		printf("Schwerpunkt x: %i\n", s.x);
		printf("Schwerpunkt y: %i\n", s.y);
		printf("P1xy(%i,%i)P2xy(%i,%i)P3xy(%i,%i)P4xy(%i,%i)\n", s.boundary_box.x1, s.boundary_box.y1, s.boundary_box.x2, s.boundary_box.y1, s.boundary_box.y2, s.boundary_box.x1, s.boundary_box.x2, s.boundary_box.y2);
		printf("Fläche       : %i\n", s.A);
		printf("Press key to save result\n");
		writeImage_ppm(img, MAXXDIM, MAXYDIM);
	}
	else{
		printf("No Blob found\n");
		printf("Press key to continue\n");
		getch_(0);
	}
}

Momente widerstandsmomente(unsigned char img[MAXXDIM][MAXYDIM],Schwerpunkt s, unsigned int object_label){
	// Widerstandsmoment I_x: Summe(x^2*dA)
	Momente m;
	//memset(&m,0,sizeof(Momente));
	m.Ix = m.Iy = m.Ixy = 0;
	//double Ix = 0, Iy = 0, Ixy = 0;
	long int tmp = 0;
	for(int x = s.boundary_box.x1; x <= s.boundary_box.x2; x++){
		for(int y = s.boundary_box.y1; y <= s.boundary_box.y2; y++){
			// dA ist immer 1, da ein Pixel ein dA darstellt
			if((unsigned int)img[y][x] == object_label){
				// Schwerpunkt ist der Bezugspunkt
				// Abstand zum schwerpunkt y

				//tmp = ((x*10 + 5) -  (s->boundary_box->x1*10));
				tmp = ((int)s.x - x);
				tmp *= tmp;
				//printf("tmp %ld\n", tmp);
				m.Ix += tmp;
				//tmp = ((y*10 + 5) -  (s->boundary_box->y1*10));
				tmp = ((int)s.y - y);
				m.Iy += (tmp * tmp);
				//m.Ixy += (((x*10) + 5) -  (s->boundary_box->x1*10)) * (((y*10) + 0.5) -  (s->boundary_box->y1*10));
				m.Ixy += ((int)s.x - x) * ((int)s.y - y);
				//img[x][y] = 127;
				/*
				Ix += pow((((double)x + 0.5) -  (double)boundary_box->x1),2);
				Iy += pow((((double)y + 0.5) -  (double)boundary_box->y1),2);
				Ixy += (((double)x + 0.5) -  (double)boundary_box->x1) * (((double)y + 0.5) -  (double)boundary_box->y1);
				*/
			}
		}
	}
	m.Ixy *= -1;
	/*
	m.Ix /= 10;
	m.Iy /= 10;
	m.Ixy /= 10;
	*/
	return m;
}

double orientierung(Momente m){
	// Drehung der Hauptachsen tan(2a)=(2*Ixy)/(Iy-Ix)
	double x = ((double)m.Ixy*2)/(((double)m.Iy) - ((double)m.Ix));
	double erg = atan(x)/2.0;
	erg *= 180 / M_PI;
	return erg;
}

void zeige_rotation(unsigned char img[MAXXDIM][MAXYDIM], unsigned int object_label){
	Schwerpunkt s = schwerpunkt(img, object_label);
	printf("Schwerpunkt x/y: %u %u \n", s.x, s.y);
	Momente m = widerstandsmomente(img, s, object_label);
	double r = orientierung(m);
	printf("Orientierung: ----------------\n");
	printf("Ix %li\n",m.Ix);
	printf("Iy %li\n",m.Iy);
	printf("Ixy %li°\n",m.Ixy);
	printf("Orientierung des Körpers %2.3lf°\n",r);

	double w = winkel_rechteck(img, s, object_label);
	printf("Winkel: ----------------------\n");
	printf("Winkel des Körpers %2.3lf°\n",w);
	printf("Press key...\n");
	writeImage_ppm(img,MAXXDIM, MAXYDIM);
	//getch_(0);
}

double winkel_rechteck(unsigned char img[MAXXDIM][MAXYDIM],Schwerpunkt s, unsigned int bloblabel){
	if(s.x == 0 || s.y == 0)
		return -100;
	unsigned int sy = 0, sx = 0;
	// Schauen o
	if((s.boundary_box.y2 - s.boundary_box.y1) < 25 )
		return -101;
	if((s.boundary_box.x2 - s.boundary_box.x1) < 25 )
		return -102;

	printf("winkel bloblabel %u\n", bloblabel);
	// Suche in der Boundary Box in y-Richtung nach Schnittpunkt
	for(int y = s.boundary_box.y1; y < s.boundary_box.y2; y++){
		if(img[y][s.boundary_box.x1] == bloblabel){
			sy = y;
			break;
		}
	}
	// Suche in der Boundary Box in x-Richtung nach Schnittpunkten
	for(int x = s.boundary_box.x1; x < s.boundary_box.x2; x++){
		if(img[s.boundary_box.y1][x] == bloblabel){
			sx = x;
			break;
		}
	}
	if( sy == 0 || sx == 0)
		return -101;
/*y1   B
 * 	|''''/
 * 	|   /
 * A|  /  C
 * 	| /
 * 	|/
 *sy
 * 	alpha= arctan(B/A)
 */
	// A > B : Von oben nach unten iterieren
	unsigned int a = 0, b = 0, count = 0;
	double alpha = 0, beta = 0, tmp = 0;
	unsigned  int dy = (sy - s.boundary_box.y1) ;
	unsigned  int dx = (sx - s.boundary_box.x1) ;
	printf("x1 %u y1 %u\n", s.boundary_box.x1,s.boundary_box.y1);
	printf("x2 %u y2 %u\n", s.boundary_box.x2,s.boundary_box.y2);
	printf("sx %u sy %u\n", sx, sy);
	printf("dx %u dy %u\n", dx, dy);
	if(dy > dx){
		unsigned int y_end = sy - ((sy - s.boundary_box.y1) / 2);
		for(unsigned int y = s.boundary_box.y1; y < y_end; y++){
			for(unsigned int x = s.boundary_box.x1; x <= s.boundary_box.x2; x++){
				if(img[y][x] == bloblabel){
					a = sy - y;
					b = x - s.boundary_box.x1;
					if(a == 0)
						break;
					tmp = atan(((double)b/(double)a));
					printf("a=%3u b=%3u w=%lf\n",a, b,(tmp* 180 / M_PI));
					alpha += tmp;
					count++;
					break;
				}
				img[y][x] = 127;
			}
			//printf("y++ %u\n",y);
		}
		alpha /= (double)count; // IC nach links drehen
		alpha *= 180 / M_PI;
		return alpha;

	}
	// B > A : von links nach rechts iterieren
	else{
		unsigned int x_end = sx - ((sx - s.boundary_box.x1) / 2);
		printf("x_end %u\n", x_end);
		for(unsigned int x = s.boundary_box.x1; x < x_end; x++){
			for(unsigned int y = s.boundary_box.y1; y < s.boundary_box.y2; y++){
				//printf("x%u y%u\n", x,y);
				if(img[y][x] == bloblabel){
					b = sx - x;
					a = y - s.boundary_box.y1;
					if(a == 0)
						break;
					tmp = atan(((double)b/(double)a));
					printf("a=%3u b=%3u w=%lf\n",a, b,90.0 - (tmp* 180 / M_PI));
					alpha += tmp;
					count++;
					break;
				}
				img[y][x] = 127;
			}
		}
		alpha /= (double)count; // IC nach links drehen
		alpha *= 180 / M_PI;
		alpha -= 90.0;
		return alpha;
	}
}


void invert(unsigned char img[MAXXDIM][MAXYDIM]){
	for(int x = 0 ; x < MAXXDIM; x++)
		for(int y = 0; y < MAXYDIM; y++){

			img[x][y] = (255) - img[x][y];
		}
	writeImage_ppm(img,MAXXDIM, MAXYDIM);
}


/*
void biggestBlob(unsigned char img[MAXXDIM][MAXYDIM], int background_threshold){
	int biggest_blob = 0;
	int greyscale = 0;
	int abs_px[PIXEL_DEPTH];
	printf("calc histo\n");
	calc_absolut_histo(img,abs_px);
	printf("calc color of biggest blobs\n");
	for(int i = 0; i < PIXEL_DEPTH; i++){
		if(i >= background_threshold)
			continue;
		if(abs_px[i] > biggest_blob){
			biggest_blob = abs_px[i];
			greyscale = i;
		}
	}
	printf("Goesster Blob:         %i\n",biggest_blob);
	printf("Goesster Blob Farbe:   %i\n",greyscale);
	// Alle blobs loeschen, die nicht zu den beiden groessten gehoeren
	printf("delete other blobs\n");
	for (int x = 0; x < MAXXDIM; x++){
		for (int y = 0; y < MAXYDIM; y++){
			if((img[x][y] >= background_threshold) || (img[x][y] != greyscale)){
				img[x][y] = 255;
			}
		}
	}
	writeImage_ppm(img, MAXXDIM, MAXYDIM);
}
*/



