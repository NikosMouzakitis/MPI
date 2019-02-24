#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>

#define ulong unsigned long
#define byte unsigned char
typedef struct {
	ulong irno, *x1, *x2;
} introw_t; //interval row

void read_rawimage(char *fname, uint32_t width, uint32_t length, byte image[width][length]) {
	short i;
	FILE *file;

	file=fopen(fname,"r");
	for (i=0; i<width; i++)
		fread(image[i], 1, length, file);
	fclose(file);
}

void write_rawimage(char *fname, uint32_t width, uint32_t length, byte image[width][length]) {
	short i;
	FILE *file;

	file=fopen(fname,"w");
	for (i=0; i<width; i++)
		fwrite(image[i], 1, length, file);
	fclose (file);
}

void interval_extraction(imagewidth, imagelength, img, ir, intervno)
ulong imagewidth, imagelength;
byte img[imagelength][imagewidth];
introw_t ir[imagelength];
ulong *intervno;
{
	ulong x, y, i, j ;
	struct timeval tim;
	double t1, t2, t3, t4;
	byte intervalfound;
	gettimeofday(&tim, NULL);
	t1=tim.tv_sec+(tim.tv_usec/1000000.0);

	for (y=0; y<imagelength; y++) {
		intervalfound=0;
		ir[y].irno=0;
		for (x=0; x<imagewidth; x++) {
		//	printf(" %3d ", img[y][x]);	
			if (img[y][x] && !intervalfound) {
				intervalfound=1;
				ir[y].x1[ir[y].irno]=x;
				(*intervno)++;
		//		printf("y=%d, x=%d, ir[y].irno=%d, intervno=%d\n", y, x, ir[y].irno, *intervno)				;
			}
			if (!img[y][x] && intervalfound) {
				intervalfound=0;
				ir[y].x2[ir[y].irno++]=x-1;
			}
			if (x==imagewidth-1 && img[y][x] && intervalfound) {
				ir[y].x2[ir[y].irno++]=x;
			}
		}
		//printf("\n");
	}

	gettimeofday(&tim, NULL);
	t2=tim.tv_sec+(tim.tv_usec/1000000.0);
	printf("time for interval extraction %10.5f sec, intervno %d \n",t2-t1, *intervno);

	return;
} // interval extraction

void main() {
	short i,j;
	char filename[50], *system_str;
	byte **img;
	ulong imagewidth, imagelength;
	ulong intervno=0;
	introw_t *ir;
	int y,rank,size,tag=1;


	printf("-------------  Interval Extraction  -------------\n");
	printf("input image filename: ");
	scanf("%s",filename);
	printf("image width: ");
	scanf("%d", &imagewidth);
	printf("image length: ");
	scanf("%d", &imagelength);
	img = (byte **) malloc (imagewidth * imagelength);
	if (img == NULL) {
		printf("no memory for input image\n");
		exit(-1);
	}

	read_rawimage(filename, imagewidth, imagelength, img);

	ir=malloc(sizeof(introw_t)*imagelength);
	if (ir == NULL) {
		printf("no memory for ir\n");
		exit(-1);
	}
	for (y=0; y<imagelength; y++) {
		ir[y].x1 = malloc((sizeof(ulong)) * imagewidth/2 );
		ir[y].x2 = malloc((sizeof(ulong)) * imagewidth/2);
		//	ir[y].b = malloc((sizeof(ulong)) + imagewidth/2);
		if (!ir[y].x1 || !ir[y].x2 )
		{
			printf("no memory for ir[]\n");
			exit(-1);
		}
	}

	interval_extraction(imagewidth, imagelength, img, ir, &intervno);
	//interval_extraction(32, 2, img, ir, &intervno);

	for (y=0; y<imagelength; y++) {
		if (ir[y].x1 != NULL) free(ir[y].x1);
		if (ir[y].x2 != NULL) free(ir[y].x2);
		//if (ir[y].b != NULL) free(ir[y].b);
	}
	free(ir);
	free(img);
}
