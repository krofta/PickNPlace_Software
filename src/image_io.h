#define MAXXDIM 256
#define MAXYDIM 256

#define PIXEL_DEPTH 256
#define HISTO_KUMULATIV 1
#define HISTO_NORMAL 0

#define CREATOR "GURKENNASE"
#define RGB_COMPONENT_COLOR 255



//extern PPMImage *image;
//#define MAXXDIM image->x
//#define MAXYDIM image->y

extern int readImage_ppm(unsigned char img[MAXXDIM][MAXYDIM]);
extern int writeImage_ppm(unsigned char img[MAXXDIM][MAXYDIM], int xdim, int ydim);
extern void viewImage_ppm();
