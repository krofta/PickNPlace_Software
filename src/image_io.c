
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "image_io.h"
#include <unistd.h>

typedef struct {
     unsigned char red,green,blue;
} PPMPixel;

typedef struct {
     int x, y;
     PPMPixel *data;
} PPMImage;


int get_file(char* imagedir){
	//char imagedir[512];
	char	dirbvdir[550];
	char	fname[30];
	if (getcwd(imagedir, sizeof(imagedir)) != NULL) {
		printf("Current working dir: %s\n", imagedir);
	} else {
		perror("getcwd() error\n");
		return -1;
	}
	sprintf(dirbvdir, "ls %s | grep .ppm", imagedir);
	system(dirbvdir);
	printf("--------------------\nBild lesen::Dateiname (ohne '.ext'!) : ");
	scanf("%s", &fname);
	strcat(fname, ".ppm");
	strcat(imagedir, "/");
	strcat(imagedir, fname);
	printf("%s\n",imagedir);
	return 0;
}


/*
int readImage_ppm(unsigned char img[MAXXDIM][MAXYDIM])
{

	char	fname[30];
	FILE	*fpimage;
	char	z1[3];
	char    z2[24];
	int		xdim, ydim, type, i, j, x, y;
	int		r, g, b;

	char *imagedir = get_file();

	if ((fpimage = fopen(imagedir, "r")) == NULL) {
		printf("Kann Datei <%s> nicht oeffnen!\n", imagedir);
		return 1;
	}
	else {
		// Header lesen ...
		fgets(z1, 3, fpimage);
		fgets(z2, 24, fpimage);
		fseek(fpimage, 26, SEEK_SET);
		fscanf(fpimage, "%d", &xdim);
		fscanf(fpimage, "%d", &ydim);
		fscanf(fpimage, "%d", &type);

		printf("xdim = %d\tydim = %d\ttype = %d\n", xdim, ydim, type);

		printf("Lese Daten aus Datei: <%s>\n", imagedir);
		// Bilddaten lesen ...
		for (i = 0; i < MAXXDIM; i++) {
			for (j = 0; j < MAXYDIM; j++) {
				fscanf(fpimage, "%d", &r);
				fscanf(fpimage, "%d", &g);
				fscanf(fpimage, "%d", &b);
				img[i][j] = g;
			}
		}
	}
	fclose(fpimage);
	printf("Bild wird eingelesen ...\n");
	sleep(2);
	getch_(0);
	return 0;
}
*/
int readImage_ppm(unsigned char imgage[MAXXDIM][MAXYDIM])
{
	char imagedir[512];
	char *ptr = imagedir[0];
	int stage=0;
	printf("stage %i\n",stage++);
	char *filename;
	char buff[16];
	if(get_file(ptr)){
		printf("failed to get filename\n");
		return -1;
	}

	PPMImage *img;
	FILE *fp;
	int c, rgb_comp_color;
	//open PPM file for reading
	printf("stage %i\n",stage++);
	fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//read image format
	printf("stage %i\n",stage++);
	if (!fgets(buff, sizeof(buff), fp)) {
		perror(filename);
		exit(1);
	}

    //check the image format
	 /*
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }
    */
    // binary ppm
     //check for comments
	printf("stage %i\n",stage++);
     c = getc(fp);
     while (c == '#') {
     while (getc(fp) != '\n') ;
          c = getc(fp);
     }
     printf("stage %i\n",stage++);
     ungetc(c, fp);
     //read image size information
     if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
          fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
          return -1;
     }
     printf("stage %i\n",stage++);
     //read rgb component
     if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
          fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
          return -1;
     }
     printf("stage %i\n",stage++);
     //check rgb component depth
     if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
          fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
          return -1;
     }
     printf("stage %i\n",stage++);
     while (fgetc(fp) != '\n') ;

     printf("stage %i\n",stage++);
     if(buff[0] == 'P' && buff[1] == '6'){
        //alloc memory form image
    	 printf("allocate first shit\n");
		img = (PPMImage *)malloc(sizeof(PPMImage));
		if (!img) {
			fprintf(stderr, "Unable to allocate memory\n");
			return -1;
		}


		//memory allocation for pixel data
		printf("allocate second shit\n");
		img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

		if (!img) {
			fprintf(stderr, "Unable to allocate memory\n");
			return -1;
		}

		//read pixel data from file
		printf("read binary file\n");
		if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
			fprintf(stderr, "Error loading image '%s'\n", filename);
			exit(1);
		}
		fclose(fp);
		unsigned int i = 0;
		printf("calculate grey scale\n");
        for(int x = 0; x < MAXXDIM; x++){
        	for(int y = 0; y < MAXYDIM; y++){
        		imgage[x][y] = (char)
        				(
        				((double)img->data[i].red * 0.2126) +
        				((double)img->data[i].green * 0.7152) +
						((double)img->data[i].blue * 0.0722)
						);
        		++i;
        	}
        }
        free(img->data);
        free(img);
        printf("Bild wird eingelesen ...\nPress Key\n");
        getch_(0);
        return 0;

    }
    // ascii ppm
    else if(buff[0] == 'P' && buff[1] == '3'){
    	int r,g,b;
		// Bilddaten lesen ...
		for (int i = 0; i < MAXXDIM; i++) {
			for (int j = 0; j < MAXYDIM; j++) {
				fscanf(fp, "%d", &r);
				fscanf(fp, "%d", &g);
				fscanf(fp, "%d", &b);
        		imgage[i][j] = (char)(((double)r * 0.2126) +
        				((double)g * 0.7152) +
						((double)b * 0.0722));
			}
		}

    	fclose(fp);
    	printf("Bild wird eingelesen ...\nPress Key\n");
    	getch_(0);
    	return 0;
    }
    return -1;

}

int writeImage_ppm(unsigned char img[MAXXDIM][MAXYDIM], int xdim, int ydim)
{
	char imagedir[512];
	//char	imagedir[128] = "C:\\bv\\";
	char	fname[30];
	FILE	*fpimage;
	int		i, j;
	int		type = 255;



	if (getcwd(imagedir, sizeof(imagedir)) != NULL) {
		printf("Current working dir: %s\n", imagedir);
	} else {
		perror("getcwd() error\n");
		return 1;
	}

	char	dirbvdir[550];// = "dir C:\\bv\\*.ppm /B";
	sprintf(dirbvdir, "ls %s | grep .ppm", imagedir);


	system(dirbvdir);

	printf("--------------------\nBild speichern::Dateiname (ohne '.ext'!) : ");
	scanf("%s", &fname);
	strcat(fname, ".ppm");
	strcat(imagedir,"/");
	strcat(imagedir, fname);


	if ((fpimage = fopen(imagedir, "w+")) == NULL) {
		printf("Kann Datei <%s> nicht oeffnen!\n", imagedir);
		return 1;
	}
	else {
		fprintf(fpimage, "P3\n");
		fprintf(fpimage, "# Created by IrfanView\n");
		fprintf(fpimage, "%d %d\n", xdim, ydim);
		fprintf(fpimage, "%d\n", type);

		for (i = 0; i<xdim; i++) {
			for (j = 0; j<ydim; j++) {
				fprintf(fpimage, "%d %d %d ", img[i][j], img[i][j], img[i][j]);
			}
		}
	}
	fclose(fpimage);
	printf("Bild wird gespeichert ...");
	sleep(2);
	getch_(0);
	return 0;
}

//
// es wird das externe Programm irfanview als Viewer verwendet
// die Bilder muessen im Verzeichnis C:\bv\ gespeichert sein!
//
void viewImage_ppm(void)
{
	char	imagedir[512]; // = "C:\\bv\\";
	char	viewer[256] = "gwenview ";
	char	fname[30];


	if (getcwd(imagedir, sizeof(imagedir)) != NULL) {
		printf("Current working dir: %s\n", imagedir);
	} else {
		perror("getcwd() error\n");
		return ;
	}
	char	dirbvdir[550];// = "dir C:\\bv\\*.ppm /B";
	sprintf(dirbvdir, "ls %s | grep .ppm", imagedir);


	system(dirbvdir);

	printf("--------------------\nBildanzeige::Dateiname (ohne '.ext'!) : ");
	scanf("%s", &fname);
	strcat(fname, ".ppm");
	strcat(imagedir,"/");
	strcat(imagedir, fname);
	strcat(viewer, imagedir);
	printf("%s\n",viewer);
	system(viewer);
	getch_(0);
}

/*
#include<stdio.h>
#include<stdlib.h>

typedef struct {
     unsigned char red,green,blue;
} PPMPixel;

typedef struct {
     int x, y;
     PPMPixel *data;
} PPMImage;

#define CREATOR "RPFELGUEIRAS"
#define RGB_COMPONENT_COLOR 255

static PPMImage *readPPM(const char *filename)
{
         char buff[16];
         PPMImage *img;
         FILE *fp;
         int c, rgb_comp_color;
         //open PPM file for reading
         fp = fopen(filename, "rb");
         if (!fp) {
              fprintf(stderr, "Unable to open file '%s'\n", filename);
              exit(1);
         }

         //read image format
         if (!fgets(buff, sizeof(buff), fp)) {
              perror(filename);
              exit(1);
         }

    //check the image format
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }

    //alloc memory form image
    img = (PPMImage *)malloc(sizeof(PPMImage));
    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
    while (getc(fp) != '\n') ;
         c = getc(fp);
    }

    ungetc(c, fp);
    //read image size information
    if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

    while (fgetc(fp) != '\n') ;
    //memory allocation for pixel data
    img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //read pixel data from file
    if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
         fprintf(stderr, "Error loading image '%s'\n", filename);
         exit(1);
    }

    fclose(fp);
    return img;
}
void writePPM(const char *filename, PPMImage *img)
{
    FILE *fp;
    //open file for output
    fp = fopen(filename, "wb");
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", filename);
         exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P6\n");

    //comments
    fprintf(fp, "# Created by %s\n",CREATOR);

    //image size
    fprintf(fp, "%d %d\n",img->x,img->y);

    // rgb component depth
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // pixel data
    fwrite(img->data, 3 * img->x, img->y, fp);
    fclose(fp);
}

void changeColorPPM(PPMImage *img)
{
    int i;
    if(img){

         for(i=0;i<img->x*img->y;i++){
              img->data[i].red=RGB_COMPONENT_COLOR-img->data[i].red;
              img->data[i].green=RGB_COMPONENT_COLOR-img->data[i].green;
              img->data[i].blue=RGB_COMPONENT_COLOR-img->data[i].blue;
         }
    }
}

int main(){
    PPMImage *image;
    image = readPPM("can_bottom.ppm");
    changeColorPPM(image);
    writePPM("can_bottom2.ppm",image);
    printf("Press any key...");
    getchar();
}
*/

