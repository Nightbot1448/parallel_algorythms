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
void calculate(int *SHARED, int matrix_size, int process_count);
void get_proc_and_size(int argc, char *argv[], int *count_of_proc, int *size);
void multiply(int *SHARED, int start, int end, int matrix_size, int process_number);
void clear_result_ans_vector(int *SHARED, int size, int count_of_proc);

int main(int argc, char *argv[])
{
	int count_of_proc = 1;
	int size = 5;
	srand(time(NULL));
	get_proc_and_size(argc, argv, &count_of_proc, &size);
	int *SHARED;
	int shm_id;
	shm_id = shmget( IPC_PRIVATE, size*(size+1+count_of_proc)*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL );
	SHARED = (int *)shmat(shm_id, NULL, 0);
	printf("Generating matrix and vector\n");
	generate(SHARED, size);
	// printf("Calculations\n");
	for(int i=1; i <= count_of_proc; i++){
		printf("Count of process: %d\n", i);
		clear_result_ans_vector(SHARED, size, i);
		calculate(SHARED, size, i);
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
	for(int i=0; i<count_of_proc; i++){
		for(int j=0; j<size; j++){
			SHARED[res_vec_shift+i*size+j] = 0;
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
			printf("use default values\n");
	}
}

void calculate(int *SHARED, int matrix_size, int process_count)
{
	int cols_for_proc = matrix_size/process_count;
	double start = omp_get_wtime();
	for (int i = 0; i < process_count; i++)
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
			if (i != process_count-1 ){
				multiply(SHARED, i*cols_for_proc, (i+1)*cols_for_proc, matrix_size, i);
			}
			else
			{
				multiply(SHARED, i*cols_for_proc, matrix_size, matrix_size, i);
			}
			exit(EXIT_SUCCESS);
		}
	}
	for (int i = 0; i < matrix_size; i++)
	{
		int status;
		wait(&status);
	}
	printf("multiply time: %lf\n", omp_get_wtime() - start);
	int res_vec_shift = matrix_size*(matrix_size+1);
	for(int i=0; i<matrix_size; i++)
	{
		for(int j=0; j<process_count-1; j++)
		{
			SHARED[res_vec_shift] += SHARED[res_vec_shift + matrix_size + j*matrix_size + i];
		}
	}
}

void multiply(int *SHARED, int start, int end, int matrix_size, int process_number)
{
	int after_matrix = matrix_size*matrix_size;
	for(int i=start; i<end; i++)
	{
		for(int j=0; j<matrix_size; j++)
		{
			SHARED[after_matrix + (process_number+1)*matrix_size + j] += SHARED[matrix_size*j+i] * SHARED[after_matrix+i];
		}
	}
}