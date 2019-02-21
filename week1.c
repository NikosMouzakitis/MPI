#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {

        MPI_Init(NULL, NULL);

        int world_size;
        int data = 10;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        int world_rank;
        MPI_Status status;

        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        char processor_name[MPI_MAX_PROCESSOR_NAME];
        int name_len;

        MPI_Get_processor_name(processor_name, &name_len);
        printf("Hello world from processor %s, rank %d out of %d processors.data is %d\n", processor_name, world_rank, world_size, data);
        // simple data exchange from rank 0 to rank1

        if(world_size >=2) {

                if(world_rank == 0) {
                        data = 3;
                        MPI_Send(&data, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
                        printf("Data : %d rank %d\n", data, world_rank);
                }

                if(world_rank == 1) {
                        MPI_Recv(&data, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
                        printf("Data : %d rank %d\n", data, world_rank);
                }
        } else {
                printf("At least 2 proceeses needed\n");
        }
        MPI_Finalize();

        return (0);
}
~       
