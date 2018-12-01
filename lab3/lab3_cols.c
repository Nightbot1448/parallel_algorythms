// compile: gcc lab3_cols.c -Wall -fopenmp -o mul_by_col
// run:		./mul_by_col 4 15000
// check processors: htop

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <omp.h>

void generate(int *SHARED, int size);
void parrallel_calculate(int *SHARED, int count_of_proc, int size);
void get_proc_and_size(int argc, char *argv[], int *count_of_proc, int *size);
void multiply(int *SHARED, int start_col, int last_col, int size, int num_of_proc);
void clear_result_ans_vector(int *SHARED, int size, int count_of_proc);

int main(int argc, char *argv[])
{
	int count_of_proc = 1;
	int size = 5;
	srand(time(NULL));
	get_proc_and_size(argc, argv, &count_of_proc, &size);
	int *SHARED;
	int shm_id;
	shm_id = shmget( IPC_PRIVATE, size*(size+2+count_of_proc)*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL );
	SHARED = (int *)shmat(shm_id, NULL, 0);
	printf("Generating matrix and vector\n");
	generate(SHARED, size);
	printf("Calculations\n");
	for(int i=1; i <= count_of_proc; i++){
		printf("Count of process: %d\n", i);
		clear_result_ans_vector(SHARED, size, i);
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

void clear_result_ans_vector(int *SHARED, int size, int count_of_proc)
{
	int res_vec_shift = size*(size+1);
	int tmp_vecs_shift = res_vec_shift + size;
	for(int j=0; j<size; j++)
	{
		SHARED[res_vec_shift+j] = 0;
	}
	for(int i=0; i<count_of_proc; i++){
		for(int j=0; j<size; j++){
			SHARED[tmp_vecs_shift+i*size+j] = 0;
		}
	}
}


void parrallel_calculate(int *SHARED, int count_of_proc, int size)
{
	int cols_for_proc = size / count_of_proc;
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
				multiply(SHARED, i*cols_for_proc, (i+1)*cols_for_proc, size, i);
				printf("range: %d %d\n", i*cols_for_proc, (i+1)*cols_for_proc);
			}
			else
			{
				multiply(SHARED, i*cols_for_proc, size, size, i);
				printf("range: %d %d\n", i*cols_for_proc, size);
			}
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < size; i++)
	{
		int status;
		wait(&status);
	}
	double end = omp_get_wtime();
	int res_vec_shift = size*(size+1);
	int tmp_vecs_shift = res_vec_shift + size;
	printf("time of parrallel multiply: %lf\n", end-start);
	start = omp_get_wtime();
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<count_of_proc; j++)
		{
			SHARED[res_vec_shift + i] += SHARED[tmp_vecs_shift + j*size + i];
		}
	}
	end = omp_get_wtime();
	printf("time of sum calculatoins: %lf\nend of calculatoins\n", end-start);
}

void multiply(int *SHARED, int start_col, int last_col, int size, int num_of_proc)
{
	int vec_shift = size*size;
	int tmp_vec_shift = vec_shift + size*(num_of_proc+1);
	for(int i = start_col; i < last_col; i++){
		for(int j=0; j < size; j++)
		{
			SHARED[tmp_vec_shift + j] += SHARED[size*j + i]*SHARED[vec_shift+i];
		}
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