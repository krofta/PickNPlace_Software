#include "image_io.h"
#include <stdio.h>
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
	unsigned char init = val > PIXEL_DEPTH - 1 ? 255 : val < 0 ? 0 : val;
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			cMatrix[i][j] = init;
}
void init_iMatrix(int iMatrix[MAXXDIM][MAXYDIM])
{
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			iMatrix[i][j] = 0;
}
void init_fMatrix(float fMatrix[MAXXDIM][MAXYDIM])
{
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
			fMatrix[i][j] = 0.0;
}
//findet den vom Betrag gr��ten/kleinsten Wert der Matrix
int  find_abs_extremum_iMatrix(int min_max, int iMatrix[MAXXDIM][MAXYDIM])
{
	int extremum = min_max == MAX ? 0 : 0xFFFF;
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
		{
			int abs = sqrt(pow(iMatrix[i][j], 2));
			if (min_max == MAX && abs > extremum)
				extremum = abs;
			else if (min_max == MIN && abs < extremum)
				extremum = abs;
		}
	return extremum;
}

//findet den vom Betrag gr��ten/kleinsten Wert der Matrix
float  find_abs_extremum_fMatrix(int min_max, float iMatrix[MAXXDIM][MAXYDIM])
{
	float extremum = min_max == MAX ? 0.0 : 0xFFFF;
	for (int i = 0; i < MAXXDIM; i++)
		for (int j = 0; j < MAXYDIM; j++)
		{
			float abs = sqrt(pow(iMatrix[i][j], 2));
			if (min_max == MAX && abs > extremum)
				extremum = abs;
			else if (min_max == MIN && abs < extremum)
				extremum = abs;
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
	float prev_gauss[50];

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
	char burn[256][256];
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
	for (int x = 0; x < MAXXDIM; x++)
		for (int y = 0; y < MAXYDIM; y++)
			img2[x][y] = img[x][y];
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
			fArray[x][y] = (float)sqrt((double)fArray[x][y]);	// Qurzel ziehen //Pytagoras
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
void blob_coloring_imagesensitiv(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM], int intervall, int keine_fransen, int writeImage)
{
	int bereich = 5, max = 0, null_labels = 0;
	init_cMatrix(img2, 0);
	init_iMatrix(iIMG);
	int iterationen = 5;
	float ink_bereich = (float)intervall / (float)iterationen;
	for (int iter = 0; iter < iterationen; iter++){
		cls();
		printf("Bitte warten: Iteration %3i von %3i", iter + 1, iterationen);
		bereich = (int)((float)(iter + 1)*ink_bereich);
		unsigned int blob = 0;
		for (int x = 1; x < MAXXDIM; x++){
			for (int y = 1; y < MAXYDIM; y++) {
				//<----- Diff( x_c, x_l) > bereich------------------------------- && Diff( x_c, x_u) > bereich    -> x_c = neues Label
				if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) > bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) > bereich && iter == 0)
					iIMG[x][y] = ++blob;
				else if ((int)sqrt(pow(img2[x][y] - img2[x][y - 1], 2)) > bereich && (int)sqrt(pow(img2[x][y] - img2[x - 1][y], 2)) > bereich && iter != 0)
					iIMG[x][y] = ++blob;
				//<---- Diff( x_c, x_l) <= bereich------------------------------- && Diff( x_c, x_u) <= bereich   -> x_c = x_u
				else if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) <= bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) <= bereich && iter == 0) {
					int l_extend = (bereich / 2) < 1 ? 1 : (bereich / 2);
					if (iIMG[x - 1][y] != iIMG[x][y - 1] &&	// Label stimmen nicht �berein -> Selbe Region?
						(int)sqrt(pow(img[x][y] - img[x][y - l_extend], 2)) <= bereich && // L-Maske verbreitern -> immernoch im Bereich?
						(int)sqrt(pow(img[x][y] - img[x - l_extend][y], 2)) <= bereich && // L-Maske verl�ngern -> immernoch im Bereich?
						x > l_extend && y > l_extend && keine_fransen == 1) {
						int old_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x - 1][y] : iIMG[x][y - 1];
						int new_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x][y - 1] : iIMG[x - 1][y];
						reset_blob_label(iIMG, old_label, new_label);
					}
					iIMG[x][y] = iIMG[x - 1][y];
				}
				else if ((int)sqrt(pow(img2[x][y] - img2[x][y - 1], 2)) <= bereich && (int)sqrt(pow(img2[x][y] - img2[x - 1][y], 2)) <= bereich && iter != 0) {
					int l_extend = (bereich / 2) < 1 ? 1 : (bereich / 2);
					if (iIMG[x - 1][y] != iIMG[x][y - 1] &&	// Label stimmen nicht �berein -> Selbe Region?
						(int)sqrt(pow(img2[x][y] - img2[x][y - l_extend], 2)) <= bereich && // L-Maske verbreitern -> immernoch im Bereich?
						(int)sqrt(pow(img2[x][y] - img2[x - l_extend][y], 2)) <= bereich && // L-Maske verlängern -> immernoch im Bereich?
						x > l_extend && y > l_extend && keine_fransen == 1) {
						int old_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x - 1][y] : iIMG[x][y - 1];
						int new_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x][y - 1] : iIMG[x - 1][y];
						reset_blob_label(iIMG, old_label, new_label);
					}
					iIMG[x][y] = iIMG[x - 1][y];
				}
				//<---- Diff( x_c, x_l) <= bereich------------------------------- && Diff( x_c, x_u) > bereich
				else if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) <= bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) > bereich && iter == 0) {
					if (y == 1)
						iIMG[x][y] = ++blob;
					else
						iIMG[x][y] = iIMG[x][y - 1];
				}
				else if ((int)sqrt(pow(img2[x][y] - img2[x][y - 1], 2)) <= bereich && (int)sqrt(pow(img2[x][y] - img2[x - 1][y], 2)) > bereich && iter != 0) {
					if (y == 1)
						iIMG[x][y] = ++blob;
					else
						iIMG[x][y] = iIMG[x][y - 1];
				}
				//<---- Diff( x_c, x_l) > bereich------------------------------- && Diff( x_c, x_u) <= bereich
				else if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) > bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) <= bereich && iter == 0) {
					iIMG[x][y] = iIMG[x - 1][y];
				}
				else if ((int)sqrt(pow(img2[x][y] - img2[x][y - 1], 2)) > bereich && (int)sqrt(pow(img2[x][y] - img2[x - 1][y], 2)) <= bereich && iter != 0) {
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
	for (int x = 1; x < MAXXDIM; x++){
		for (int y = 1; y < MAXYDIM; y++)
		{
			//       Diff( x_c, x_l) > bereich                                && Diff( x_c, x_u) > bereich    -> x_c = neues Label
			if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) > bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) > bereich)
				iIMG[x][y] = ++blob;
			//      Diff( x_c, x_l) <= bereich                                && Diff( x_c, x_u) <= bereich   -> x_c = x_u
			else if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) <= bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) <= bereich) {
				//Grauwerte sind im Intervall, aber die labels im Merker sind nicht identisch -> falsches label
				if (iIMG[x - 1][y] != iIMG[x][y - 1]){
					// Label überscheiben! Höherwertiges Label wird überschrieben, da dies falsch inkrementiert wurde
					--blob;
					int old_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x - 1][y] : iIMG[x][y - 1];
					int new_label = iIMG[x - 1][y] > iIMG[x][y - 1] ? iIMG[x][y - 1] : iIMG[x - 1][y];
					reset_blob_label(iIMG, old_label, new_label);
				}
				// nun gewönlich x_u, x_c zuweisen
				iIMG[x][y] = iIMG[x - 1][y];
			}
			//      Diff( x_c, x_l) <= bereich                                && Diff( x_c, x_u) > bereich
			else if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) <= bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) > bereich) 
					iIMG[x][y] = iIMG[x][y - 1];
			//      Diff( x_c, x_l) > bereich                                && Diff( x_c, x_u) <= bereich
			else if ((int)sqrt(pow(img[x][y] - img[x][y - 1], 2)) > bereich && (int)sqrt(pow(img[x][y] - img[x - 1][y], 2)) <= bereich) 
				iIMG[x][y] = iIMG[x - 1][y];
		}
	}
	return blob;
}

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

void schwerpunkt(unsigned char img[MAXXDIM][MAXYDIM], int bloblabel){
	unsigned int px_count = 0;
	unsigned long int schwerpunkt_x = 0, schwerpunkt_y = 0;
	int min_x = 255, max_x = 0, min_y = 255, max_y = 0;
	for (int x = 0; x < MAXXDIM; x++){
		for (int y = 0; y < MAXYDIM; y++){
			if(img[x][y] == bloblabel){
				++px_count;
				// für boundary box
				min_x = x < min_x ? x : min_x;
				max_x = x > max_x ? x : max_x;
				min_y = y < min_y ? y : min_y;
				max_y = y > max_y ? y : max_y;
				// für flächenschwerpunkt
				schwerpunkt_x += x;
				schwerpunkt_y += y;
			}
		}
	}
	if(px_count > 0){
		schwerpunkt_x /= px_count;
		schwerpunkt_y /= px_count;


		//Markiere Schwerpunkt im Bild
		for(int x = 0; x< MAXXDIM; x++)
			img[x][schwerpunkt_y] = (char)bloblabel - (PIXEL_DEPTH/2);
		for(int y = 0; y< MAXXDIM; y++)
			img[schwerpunkt_x][y] = (char)bloblabel - (PIXEL_DEPTH/2);

		// Boundary Box im Bild Markieren
		for(int x = min_x; x <= max_x; x++){
			img[x][min_y] = (char)bloblabel - (PIXEL_DEPTH/2);
			img[x][max_y] = (char)bloblabel - (PIXEL_DEPTH/2);
		}
		for(int y = min_y; y <= max_y; y++){
			img[min_x][y] = (char)bloblabel - (PIXEL_DEPTH/2);
			img[max_x][y] = (char)bloblabel - (PIXEL_DEPTH/2);
		}
		printf("Schwerpunkt x: %i\n", schwerpunkt_x);
		printf("Schwerpunkt y: %i\n", schwerpunkt_y);
		printf("P1(%i,%i)P2(%i,%i)P3(%i,%i)P4(%i,%i)\n", min_x, min_y, max_x, min_y, max_y, min_x, max_x, max_y);
		printf("Fläche       : %i\n", px_count);
		printf("Press key to save result\n");
		writeImage_ppm(img, MAXXDIM, MAXYDIM);
	}
	else{
		printf("No Blob found\n");
		printf("Press key to continue\n");
		gletch_(0);
	}
}
// Annahme: Der groesste Blob ist der Hintergrund
// Der zweitgroesste Blob ist das Hauptobjekt des Bildes
// Alle anderen werden dem Hintergrund gleich gemacht
// Der Hintergrund wird mit 255 markiert
// Das Objekt mit 0

typedef struct {
     unsigned int blob_label;
     unsigned int blob_size;
} Blob;

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
	Blob *blobs = (Blob*)calloc(max_blob+1, sizeof(Blob));
	// Blosgroessen und das entsprechende Label ermitteln
	for (int x = 0; x < MAXXDIM; x++){
		for (int y = 0; y < MAXYDIM; y++){
			blobs[iIMG[x][y]].blob_size++;
			blobs[iIMG[x][y]].blob_label = iIMG[x][y];
		}
	}
	// den groessten Blob finden
	Blob biggest_blob;
	memset(&biggest_blob,sizeof(Blob),0);
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



