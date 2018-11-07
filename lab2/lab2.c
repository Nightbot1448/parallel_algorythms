#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

void print_shared(double *mem, int size);
void get_proc_and_op(int argc, char *argv[], int *count_of_proc, long *count_of_op);
double part_of_pi(long start, long end);
double parrallel_calculate(double *SHARED, int count_of_proc, long proc_op_count, long count_of_op);

int main(int argc, char *argv[])
{
	int count_of_proc = 1;
	long count_of_op = 1000000000, proc_op_count = 0;
	get_proc_and_op(argc, argv, &count_of_proc, &count_of_op);
	printf("count of proccess: %d\n", count_of_proc);
	printf("count of operations: %ld\n", count_of_op);
	proc_op_count = count_of_op / count_of_proc;

	double *SHARED;
	int shm_id;
	shm_id = shmget( IPC_PRIVATE, count_of_proc*sizeof(double), 0666 | IPC_CREAT | IPC_EXCL );
	SHARED = (double *)shmat(shm_id, NULL, 0);
	for(int i=0; i<count_of_proc; i++)
	{
		SHARED[i] = 0;
	}

	double pi = parrallel_calculate(SHARED, count_of_proc, proc_op_count, count_of_op);
	printf("pi number: %.12lf\nmath.pi:   %.12lf\ndiff: %.3e\n", pi, M_PI, fabs(pi-M_PI));
	shmctl(shm_id, IPC_RMID, NULL);
	return 0;
}

double parrallel_calculate(double *SHARED, int count_of_proc, long proc_op_count, long count_of_op)
{
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
			double intermediate_val = 0;
			if(i + 1 == count_of_proc)
			{
				intermediate_val = part_of_pi(i*proc_op_count + 1, count_of_op + 1);
			}
			else{
				intermediate_val = part_of_pi(i*proc_op_count + 1, (i + 1)*proc_op_count + 1);
			}
			SHARED[i] = intermediate_val;
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < count_of_proc; i++)
	{
		int status;
		// pid_t pid = 
		wait(&status); // kids could be ready in any order
	}
	double pi = 0;
	for(int i=0; i < count_of_proc; i++)
	{
		pi += SHARED[i];
	}
	return pi;
}


void print_shared(double *mem, int size)
{
	for(int i=0; i<size; i++)
		printf("SH[i] = %.2lf, ", mem[i]);
	printf("\n");
}

void get_proc_and_op(int argc, char *argv[], int *count_of_proc, long *count_of_op)
{
	int tmp_cop = 0;
	long tmp_coo = 0;
	if (argc > 2)
	{
		tmp_cop = atoi(argv[1]);
		tmp_coo = atol(argv[2]);
		if (tmp_cop && tmp_coo)
		{
			*count_of_proc = tmp_cop;
			*count_of_op = tmp_coo;
		}
		else
			printf("use defaul values\n");
	}
}

double part_of_pi(long start, long end)
{
	double num = 0;
	for(long i=start; i<2*end; i += 2)
	{
		if ((i-1)/2%2)
			num -= 4.0/i;
		else
			num += 4.0/i;
	}
	return num;
}
