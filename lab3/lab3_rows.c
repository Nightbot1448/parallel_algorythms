// compile: gcc lab3_rows.c -Wall -fopenmp -o mul_by_row
// run:		./mul_by_row 4 25000
// check processors: htop

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <omp.h>

void generate(int *SHARED, int size);
void parrallel_calculate(int *SHARED, int count_of_proc, int size);
void get_proc_and_size(int argc, char *argv[], int *count_of_proc, int *size);
void multiply(int *SHARED, int start_row, int last_row, int size);
void clear_result_vector(int *SHARED, int size);

int main(int argc, char *argv[])
{
	int count_of_proc = 1;
	int size = 5;
	srand(time(NULL));
	get_proc_and_size(argc, argv, &count_of_proc, &size);
	int *SHARED;
	int shm_id;
	shm_id = shmget( IPC_PRIVATE, size*(size+2)*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL );
	SHARED = (int *)shmat(shm_id, NULL, 0);
	printf("Generating matrix and vector\n");
	generate(SHARED, size);
	printf("Calculations\n");
	for(int i=1; i <= count_of_proc; i++){
		printf("Count of process: %d\n", i);
		clear_result_vector(SHARED, size);
		parrallel_calculate(SHARED, i, size);
	}

	shmctl(shm_id, IPC_RMID, NULL);
	return 0;
}

void generate(int *SHARED, int size)
{
	for(int i=0; i<size; i++)
	{ 
		for(int j=0; j<size; j++)
		{
			SHARED[i*size+j] = rand()%11-5;
		}
	}
	for(int j=0; j<size; j++)
	{
		SHARED[size*size+j] = rand()%11-5;
	}
}

void clear_result_vector(int *SHARED, int size)
{
	int res_vec_shift = size*(size+1);
	for(int j=0; j<size; j++)
	{
		SHARED[res_vec_shift+j] = 0;
	}
}


void parrallel_calculate(int *SHARED, int count_of_proc, int size)
{
	int rows_for_proc = size / count_of_proc;
	// printf("rows_for_proc: %d\n", rows_for_proc);
	double start = omp_get_wtime();
	for (int i = 0; i < count_of_proc; i++)
	{
		pid_t pid = fork();
		if(pid < 0)
		{
			exit(EXIT_FAILURE);
		}
		else if (pid > 0)
		{
			continue;
		}
		else {
			if (i != count_of_proc-1 ){
				multiply(SHARED, i*rows_for_proc, (i+1)*rows_for_proc, size);
			}
			else
			{
				multiply(SHARED, i*rows_for_proc, size, size);
			}
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < size; i++)
	{
		int status;
		wait(&status);
	}
	printf("end of calculatoins\ntime of parrallel calculate: %lf\n", omp_get_wtime()-start);//result vector in file \'output.txt\'\n");
	// FILE *fout = fopen("output.txt", "w");
	// int res_shift = size*(size+1);
	// for(int i=0; i<size; i++)
	// {
	// 	fprintf(fout, "%2d ", SHARED[res_shift+i]);
	// }
	// fclose(fout);
}

void multiply(int *SHARED, int start_row, int last_row, int size)
{
	// printf("start:%d, last: %d\n", start_row, last_row);	
	int vec_shift = size*size;
	int res_shift = vec_shift + size;
	int matrix_shift = size*start_row;
	for(int i = start_row; i < last_row; i++){
		for(int j=0; j < size; j++)
		{
			SHARED[res_shift + i] += SHARED[matrix_shift+j]*SHARED[vec_shift+j];
		}
		matrix_shift += size;
	}
}


void get_proc_and_size(int argc, char *argv[], int *count_of_proc, int *size)
{
	int tmp_cop = 0;
	int tmp_sz = 0;
	if (argc > 2)
	{
		tmp_cop = atoi(argv[1]);
		tmp_sz = atol(argv[2]);
		if (tmp_cop && tmp_sz)
		{
			*count_of_proc = tmp_cop;
			*size = tmp_sz;
		}
		else
			printf("use defaul values\n");
	}
}