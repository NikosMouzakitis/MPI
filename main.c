#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <mpi.h>

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

void interval_extraction(imagewidth, imagelength, img, ir, intervno, myrank, total)
ulong imagewidth, imagelength;
byte img[imagelength][imagewidth];
introw_t ir[imagelength];
ulong *intervno;
int myrank;
int total;
{
	ulong x, y, i, j ;
	struct timeval tim;
	double t1, t2, t3, t4;
	int limit;
	int rest, chunk;
	int final;
	void *sendbuf;
	byte intervalfound;
	byte *rbuf;
	sendbuf = img;
	int *resultsArray;
	chunk = imagelength / total;
	rest  = imagelength % total;
	resultsArray = (int *)malloc(sizeof(int) * total);


	printf("I am %d\n", myrank);


	if(resultsArray == NULL) {
		printf("No mem\n");
		exit(-1);
	}
	rbuf = (byte*) malloc( (chunk+rest) * imagewidth);
	if(rbuf == NULL) {
		printf("Not enough memory on the system.Shutdown\n");
		exit(-1);
	}
	if(myrank == 0) {
		gettimeofday(&tim, NULL);
		t1=tim.tv_sec+(tim.tv_usec/1000000.0);
	}
	

	MPI_Scatter( img[0]+(rest*imagewidth), chunk*imagewidth, MPI_CHAR, rbuf, chunk*imagewidth, MPI_CHAR, 0, MPI_COMM_WORLD);

	printf("After scatter: %d\n", myrank);
	limit = chunk;
	if(myrank == 0) {
		


		limit = chunk+rest;
		for (y=0; y<limit; y++) {
			intervalfound=0;
			ir[y].irno=0;
			for (x=0; x < imagewidth; x++) {

				if ( img[y][x] && !intervalfound) {

					intervalfound=1;
					ir[y].x1[ir[y].irno]=x;
					(*intervno)++;
					//printf("rank: %d y=%ld, x=%ld, ir[y].irno=%ld, intervno=%ld\n",myrank, y, x, ir[y].irno, *intervno);
				}
				if (!img[y][x] && intervalfound) {
					intervalfound=0;
					ir[y].x2[ir[y].irno++]= x-1;
				}
				if (x==imagewidth-1 && img[y][x] && intervalfound) {
					ir[y].x2[ir[y].irno++] = x;
				}
			}
		}
	} else if(myrank!=0) {
		for (y=0; y<limit; y++) {
			intervalfound=0;
			ir[y].irno=0;
			for (x=0; x < imagewidth; x++) {
				char tmp = *(rbuf+(y*imagewidth)+x);
				if ( tmp && !intervalfound) {
					intervalfound=1;
					ir[y].x1[ir[y].irno]=x;
					(*intervno)++;
					//printf("rank: %d y=%ld, x=%ld, ir[y].irno=%ld, intervno=%ld\n",myrank, y, x, ir[y].irno, *intervno);
				}
				if (!tmp && intervalfound) {
					intervalfound=0;
					ir[y].x2[ir[y].irno++]= x-1;
				}
				if (x==imagewidth-1 && tmp && intervalfound) {
					ir[y].x2[ir[y].irno++] = x;
				}
			}
		}
	}
	final = (int) *intervno;

	MPI_Gather(&final, 1, MPI_INT, resultsArray, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if(myrank!= 0)
		return;
	gettimeofday(&tim, NULL);
	t2=tim.tv_sec+(tim.tv_usec/1000000.0);
	int sum = 0;

	for(int i = 0; i < total; i++)
		sum+=resultsArray[i];
	*intervno = sum;
	printf("time for interval extraction %10.5f sec, intervno %ld \n",t2-t1, *intervno);
	return;
}

void main(int argc, char * argv[]) {
	short i,j;
	char filename[50], *system_str;
	byte **img;
	ulong imagewidth, imagelength;
	ulong intervno=0;
	introw_t *ir;
	int y,rank,size,tag=1;
	int world_rank;
	int world_size;

	MPI_Init(NULL, NULL);
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
 
	rank = world_rank;
	rank = 0;
	
	
	
	//if(world_rank == 0) {
	if(rank == 0) {
		printf("-------------  Interval Extraction  -------------\n");
		printf("I AM: %d\n", world_rank);
		//printf("input image filename: ");
//		scanf("%s",filename);
		//printf("image width: ");
	//	scanf("%d", &imagewidth);
		//printf("image length: \n");
	//	scanf("%d", &imagelength);
		//strcpy(filename, "c10-1920x1080.raw");	
		strcpy(filename, argv[1]);	
		imagewidth = atoi(argv[2]);
		imagelength = atoi(argv[3]);

	
		////img
		img = (byte **) malloc (imagewidth * imagelength);

		if (img == NULL) {
			printf("%d no memory for input image\n", world_rank);
			exit(-1);
		}

		read_rawimage(filename, imagewidth, imagelength,(byte **) img);

		ir=malloc(sizeof(introw_t)*imagelength);

		if (ir == NULL) {
			printf("no memory for ir\n");
			exit(-1);
		}

		for ( y = 0; y < imagelength; y++) {

			ir[y].x1 = malloc((sizeof(ulong)) * imagewidth/2 );
			ir[y].x2 = malloc((sizeof(ulong)) * imagewidth/2);

			if (!ir[y].x1 || !ir[y].x2 )
			{
				printf("no memory for ir[]\n");
				exit(-1);
			}
		}
	}
	

//	MPI_Barrier(MPI_COMM_WORLD);
	interval_extraction(imagewidth, imagelength, img, ir, &intervno, world_rank, world_size);
	
	MPI_Finalize();

	if(rank == 0) {
		for (y=0; y<imagelength; y++) {
			if (ir[y].x1 != NULL) free(ir[y].x1);
			if (ir[y].x2 != NULL) free(ir[y].x2);
		}
		free(ir);
		free(img);
	}
}
