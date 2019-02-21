 #include <stdio.h>
 #include <mpi.h>
 
 int main(int argc, char *argv[])
  {
          int isend[3], irecv;
          int rank, size, i;
  
  
          int mat[2][2] = { 0, 1,
                            2, 3 };
          int res[2];
  
          MPI_Init(NULL, NULL);
          MPI_Comm_rank(MPI_COMM_WORLD, &rank);
          MPI_Comm_size(MPI_COMM_WORLD, &size);
  
          if(rank == 0) {
                  for(i = 0; i < size; i++)
                          isend[i] = i+1;
          }
 
          MPI_Scatter(mat, 2, MPI_INT, res, 2, MPI_INT, 0, MPI_COMM_WORLD);
  
          printf("rank: %d got %d %d\n", rank, res[0],  res[1]);
          
          MPI_Finalize();
  
          return (0);
 }
