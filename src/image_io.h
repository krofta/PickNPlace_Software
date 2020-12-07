#define MAXXDIM 256
#define MAXYDIM 256
#define PIXEL_DEPTH 256
#define HISTO_KUMULATIV 1
#define HISTO_NORMAL 0

extern int readImage_ppm(unsigned char img[MAXXDIM][MAXYDIM]);
extern int writeImage_ppm(unsigned char img[MAXXDIM][MAXYDIM], int xdim, int ydim);
extern void viewImage_ppm();
