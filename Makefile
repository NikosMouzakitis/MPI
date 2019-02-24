all:
	mpicc main.c
	mpiexec -n 2 ./a.out c10-1920x1080.raw 1920 1080
