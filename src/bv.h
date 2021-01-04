#include "image_io.h"

// Strukturen zur vereinfachung
typedef struct {
     unsigned int blob_label;
     unsigned int blob_size;
} Blob;

typedef struct {
	unsigned int x1;
	unsigned int y1;
	unsigned int x2;
	unsigned int y2;
} Box;

typedef struct {
	unsigned int x;	// Schwerpunkt x
	unsigned int y;	// Schwerpunkt y
	unsigned int A;	// Fläche
	Box boundary_box;	// Boundary Box des Objektes
} Schwerpunkt;

typedef struct{
	long int Ix;	// Widerstandsmoment in x Richtung
	long int Iy;	// Widerstandsmoment in y Richtung
	long int Ixy;	// Deviationsmoment in xy Richtung
} Momente;


		// bin�re Bildverarbeitung
void shrink(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]);
void blow(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]);
void funk_open(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]);
void funk_close(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]);
void grassfire(unsigned char img[MAXXDIM][MAXYDIM]);
void count_white(unsigned char img[MAXXDIM][MAXYDIM]);

// Preprocessing
void histogramm(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int ART);
void grauwert_dehnung(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]);
void linearer_histo_ausgleich(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int anzGrauWerte);
void calc_absolut_histo(unsigned char img[MAXXDIM][MAXYDIM], int grey[PIXEL_DEPTH]);
void calc_kumulativ_histo(unsigned char img[MAXXDIM][MAXYDIM], int grey[PIXEL_DEPTH]);
void median_filter(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iDIMxy);
void mittelwert_filter(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iDIMxy, int Gewichtung);
void gauss_filter(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int scale);

// Kantendetektion
void sobelx(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int sobelx[MAXXDIM][MAXYDIM]);
void sobely(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int sobely[MAXXDIM][MAXYDIM]);
void sobelxy(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int sobelx[MAXXDIM][MAXYDIM], int sobely[MAXXDIM][MAXYDIM]);
void laplace(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int Umgebung);
void difference_of_gaussian(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int scale, int grundton);

// Texturen
void laws_textur(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]);
void cooccurence_matrix(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], float fIMG[MAXXDIM][MAXYDIM], int direction, int save);
void cooc_matrix_kombi_asm(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], float fIMG[MAXXDIM][MAXYDIM], int graustufen);
void calc_asm_energie(float fIMG[MAXXDIM][MAXYDIM]);

//Segmentierung
void segmentierung_von_otsu(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM]);
void segmentierung_binaer(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int threshold);
void blob_coloring_imagesensitiv(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM],
		int iteration, int keine_fransen, int writeImage, int iterationen);
void fast_blob_coloring(unsigned char img[MAXYDIM][MAXXDIM], int bereich);
int bwLabel(unsigned char img[MAXXDIM][MAXYDIM]);
void blob_coloring_markersensitiv(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM], int bereich, int writeImage);
void blister_blob(unsigned char img[MAXXDIM][MAXYDIM], unsigned char img2[MAXXDIM][MAXYDIM], int iIMG[MAXXDIM][MAXYDIM]);
void biggestBlob(unsigned char img[MAXXDIM][MAXYDIM],unsigned int iIMG[MAXXDIM][MAXYDIM], int background_threshold, int min_blobsize);
void invert(unsigned char img[MAXXDIM][MAXYDIM]);
//void biggestBlob(unsigned char img[MAXXDIM][MAXYDIM], int background_threshold);

// Merkmalsextraktion

void zeige_schwerpunkt(unsigned char img[MAXXDIM][MAXYDIM],unsigned int bloblabel);
Schwerpunkt schwerpunkt(unsigned char img[MAXXDIM][MAXYDIM], unsigned int bloblabel);
Momente widerstandsmomente(unsigned char img[MAXXDIM][MAXYDIM],Schwerpunkt s, unsigned int object_label);
void zeige_rotation(unsigned char img[MAXXDIM][MAXYDIM], unsigned int object_label);
double orientierung(Momente m);
double winkel_rechteck(unsigned char img[MAXXDIM][MAXYDIM],Schwerpunkt s, unsigned int bloblabel);

// Anderes
void init_cMatrix(unsigned char cMatrix[MAXXDIM][MAXYDIM], unsigned char val);
void init_iMatrix(int iMatrix[MAXXDIM][MAXYDIM]);
void init_fMatrix(float fMatrix[MAXXDIM][MAXYDIM]);
int  find_abs_extremum_iMatrix(int min_max, int iMatrix[MAXXDIM][MAXYDIM]);
float  find_abs_extremum_fMatrix(int min_max, float fMatrix[MAXXDIM][MAXYDIM]);
void get_bin_koeff(float bin_ver[50], int n, float normierung);
double fakultaet(int n);
void bubblesort(int *array, int length);
void reset_blob_label(int iIMG[MAXXDIM][MAXYDIM], int oldLabel, int newLabel);




#define LAPLACE_4 4
#define LAPLACE_8 8
#define BINAER 0
#define BIT8 1
#define MIN 0
#define MAX 1

#define SKALIERT 0
#define NICHT_SKALIERT 1
