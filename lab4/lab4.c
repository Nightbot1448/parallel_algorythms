// compile: gcc lab4.c -Wall -fopenmp -pthread -o task
// run:		./task 30000000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <omp.h>
	
typedef struct infoForThread
{
	int id;
	int len;
	int *arr;
} infoForThread;


void qs(int *arr, int first, int last);
void consistent_calc(int *arr, int len);
void parallel_calc(int *arr, int len);
void *thread_task_qs(void *info);
void *thread_task_ces(void *info);
int *compare_exchange(int *arr1, int len1, int *arr2, int len2);
int check_sorted(int *arr, int len);
void parse_len(int argc, char *argv[], int *len);

int main(int argc, char *argv[])
{
	srand(time(NULL));
	int len = 10000000;
	parse_len(argc, argv, &len);
	int *arr_for_parrallel = (int *)malloc(len*sizeof(int));
	int *arr_for_consistent = (int *)malloc(len*sizeof(int));
	// printf("generating and cloning array\n");
	for(int i=0; i<len; i++)
	{
		arr_for_parrallel[i] = rand()%301-150;
	}
	memcpy(arr_for_consistent, arr_for_parrallel, len*sizeof(int));
	// printf("consistent calculating\n");
	consistent_calc(arr_for_consistent, len);
	// printf("parallel calculating\n");
	parallel_calc(arr_for_parrallel, len);
	free(arr_for_parrallel);
	free(arr_for_consistent);
	return 0;
}

void qs(int *s_arr, int first, int last)
{
	if (first < last)
	{
		int left = first, 
			right = last, 
			middle = s_arr[(left + right) / 2];
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
		qs(s_arr, first, right);
		qs(s_arr, left, last);
	}	
}

void consistent_calc(int *arr, int len)
{	
	double time = omp_get_wtime();
	qs(arr, 0, len-1);
	printf("consistent time: %lf\n", omp_get_wtime()-time);
	if (!check_sorted(arr, len))
		printf("\nALARM!!!\nconsistent not sorted\n");
}


void *thread_task_qs(void *info)
{
	infoForThread parsed = *((infoForThread *)info);
	if(parsed.id != 3)
		qs(parsed.arr, parsed.len*parsed.id/4, parsed.len*(parsed.id+1)/4-1);
	else
		qs(parsed.arr, parsed.len*parsed.id/4, parsed.len-1);
	free(info);
	return NULL;
}

int *compare_exchange(int *arr1, int len1, int *arr2, int len2)
{
	int first_c = 0;
	int second_c = 0;
	int count = 0;
	int *res_arr = (int *)malloc(sizeof(int)*(len1+len2));
	while(count < len1+len2)
	{
		if(((arr1[first_c] < arr2[second_c]) || (second_c >= len2))
			&& (first_c < len1))
		{
			res_arr[count] = arr1[first_c];
			first_c++;
		}
		else
		{
			res_arr[count] = arr2[second_c];
			second_c++;
		}
		count++;
	}
	return res_arr;
}

void *thread_task_ces(void *info)
{
	infoForThread parsed = *((infoForThread *)info);
	int len = parsed.len/4;
	int *res_arr = NULL;
	if(parsed.id != 3)
	{
		res_arr = compare_exchange(parsed.arr+(parsed.id-1)*len, len, parsed.arr+parsed.id*len, len);
		memcpy(parsed.arr+(parsed.id-1)*len, res_arr, len*2*sizeof(int));
	}
	else
	{
		res_arr = compare_exchange(parsed.arr+(parsed.id-1)*len, len, parsed.arr+parsed.id*len, parsed.len-3*len);
		memcpy(parsed.arr+(parsed.id-1)*len, res_arr, (parsed.len - len*2)*sizeof(int));
	}
	free(res_arr);
	return NULL;
}


void parallel_calc(int *arr, int len)
{
	double time = omp_get_wtime();
	pthread_t threads[4];
	for(int i=0; i<4; i++)
	{
		infoForThread *info = (infoForThread *)malloc(sizeof(infoForThread));
		info->len = len;
		info->id = i;
		info->arr = arr;
		pthread_create(&threads[i], NULL, thread_task_qs, (void *)(info));
	}
	for(int i=0; i<4;i++)
	{
		pthread_join(threads[i], NULL);
	}
	for(int i=0; i<4; i++)
	{
		if(i % 2)
		{

			for(int j=1; j<4; j+=2)
			{
				infoForThread *info = (infoForThread *)malloc(sizeof(infoForThread));
				info->len = len;
				info->id = j;
				info->arr = arr;
				pthread_create(&threads[j], NULL, thread_task_ces, (void *)(info));
			}
			for(int j=1; j<4; j+=2)
				pthread_join(threads[j], NULL);
		}
		else
		{
			for(int j=2; j<4; j+=2)//да, всего 1 итерация
			{
				infoForThread *info = (infoForThread *)malloc(sizeof(infoForThread));
				info->len = len;
				info->id = j;
				info->arr = arr;
				pthread_create(&threads[j], NULL, thread_task_ces, (void *)(info));
			}
			for(int j=2; j<4; j+=2)
				pthread_join(threads[j], NULL);
		}
	}
	printf("parallel time:   %lf\n", omp_get_wtime()-time);
	if (!check_sorted(arr, len))
		printf("\nALARM!!!\nparallel not sorted\n");
}

int check_sorted(int *arr, int len)
{
	for(int i=0; i<len-1; i++)
	{
		if(arr[i]>arr[i+1])
			return 0;
	}
	return 1;
}

void parse_len(int argc, char *argv[], int *len)
{
	if (argc > 1)
		*len = atoi(argv[1]);
	printf("len of arrs: %d\n", *len);
}