#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <omp.h>

typedef struct infoForThread
{
	int low;
	int high;
	int *arr;
} infoForThread;


void *qs(void *mes);
void consistent_calc(int *arr, int len);
void parallel_calc(int *arr, int len);


int main(){
	srand(time(NULL));
	int len = 100;
	int *arr_for_parrallel = (int *)malloc(len*sizeof(int));
	int *arr_for_consistent = (int *)malloc(len*sizeof(int));
	for(int i=0; i<len; i++)
	{
		arr_for_parrallel[i] = rand()%301-150;
		arr_for_consistent[i] = arr_for_parrallel[i];
	}
	// printf("\n");
	// consistent_calc(arr_for_consistent, len);
	parallel_calc(arr_for_parrallel, len);
	// for(int i=0; i < 4; i++)
	// {
	// 	if(i != 3)
	// 		for(int j = i*len/4; j< (i+1)*len/4; j++)
	// 			printf("%d ", arr_for_consistent[j]);
	// 	else
	// 		for(int j = i*len/4; j< len; j++)
	// 			printf("%d ", arr_for_consistent[j]);
	// 	printf("\n");
	// }
	printf("\n\n");
	for(int i=0; i < 4; i++)
	{
		if(i != 3)
			for(int j = i*len/4; j< (i+1)*len/4; j++)
				printf("%d ", arr_for_parrallel[j]);
		else
			for(int j = i*len/4; j< len; j++)
				printf("%d ", arr_for_parrallel[j]);
		printf("\n\n");
	}

	free(arr_for_parrallel);
	free(arr_for_consistent);
	return 0;
}

void *qs(void *mes)
{
	infoForThread n_mes = *((infoForThread *)mes);
	int *s_arr = n_mes.arr;
	int first = n_mes.low,
		last = n_mes.high;
	if(last - first > 25)
		printf("%d %d\n", first, last);
	if (first < last)
	{
		int left = first, right = last, middle = s_arr[(left + right) / 2];
		do
		{
			while (s_arr[left] < middle) left++;
			while (s_arr[right] > middle) right--;
			if (left <= right)
			{
				int tmp = s_arr[left];
				s_arr[left] = s_arr[right];
				s_arr[right] = tmp;
				left++;
				right--;
			}
		} while (left <= right);
		n_mes.low = first;
		n_mes.high = right;
		qs(&n_mes);
		n_mes.low = left;
		n_mes.high = last;
		qs(&n_mes);
	}
	return NULL;
}

void consistent_calc(int *arr, int len)
{	
	double time = omp_get_wtime();
	infoForThread mes;
	mes.arr = arr;
	mes.low = 0;
	mes.high = len-1;
	qs(&mes);
	printf("time %lf\n", omp_get_wtime()-time);
}

void parallel_calc(int *arr, int len){
	pthread_t threads[4];
	for(int i=0; i<4; i++)
	{
		infoForThread message;
		message.low = i*len/4;
		if (i != 3)
			message.high = (i+1)*(len)/4-1;
		else
			message.high = len-1;
		printf("%d %d\n", message.low, message.high);
		message.arr = arr;
		pthread_create (&threads[i], NULL, qs, (void *)(&message));
	}
	for(int i=0; i<4; i++)
	{
		pthread_join(threads[i], NULL);
	}
}