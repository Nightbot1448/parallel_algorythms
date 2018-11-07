#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

void generate(int *SHARED, int size);
void parrallel_calculate(int *SHARED, int size);

int main(int argc, char *argv[])
{
	int size = 5;
	srand(time(NULL));
	if (argc>1)
		size = atoi(argv[1]);
	int *SHARED;
	int shm_id;
	shm_id = shmget( IPC_PRIVATE, size*(size+2)*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL );
	SHARED = (int *)shmat(shm_id, NULL, 0);
	generate(SHARED, size);
	parrallel_calculate(SHARED, size);	

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
		SHARED[size*(size+1)+j] = 0;
	}
}


void parrallel_calculate(int *SHARED, int size)
{
	int vec_shift = size*size;
	int res_shift = vec_shift + size;
	for (int i = 0; i < size; i++)
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
			int matrix_shift = size*i;
			for(int j=0; j < size; j++)
			{
				SHARED[res_shift + i] += SHARED[matrix_shift+j]*SHARED[vec_shift+j];
			}
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 0; i < size; i++)
	{
		int status;
		wait(&status);
	}
	printf("end of calculatoins\nresult vector in file \'output.txt\'\n");
	FILE *fout = fopen("output.txt", "w");
	for(int i=0; i<size; i++)
	{
		fprintf(fout, "%2d ", SHARED[res_shift+i]);
	}
	fclose(fout);
}