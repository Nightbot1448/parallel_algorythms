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


int main()
{
	srand(time(NULL));
	int len = 100;
	int *arr_for_parrallel = (int *)malloc(len*sizeof(int));
	int *arr_for_consistent = (int *)malloc(len*sizeof(int));
	for(int i=0; i<len; i++)
	{
		arr_for_parrallel[i] = rand()%301-150;
		arr_for_consistent[i] = arr_for_parrallel[i];
	}
	// consistent_calc(arr_for_consistent, len);
	parallel_calc(arr_for_parrallel, len);
	for(int i=0; i < 4; i++)
	{
		if(i != 3)
			for(int j = i*len/4; j< (i+1)*len/4; j++)
				printf("%d ", arr_for_parrallel[j]);
		else
			for(int j = i*len/4; j< len; j++)
				printf("%d ", arr_for_parrallel[j]);
		printf("\n");
	}

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
	printf("time %lf\n", omp_get_wtime()-time);
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
	// for(int i=0; i<len1+len2; i++)
	// 	printf("%d ", res_arr[i]);
	// printf("\n");
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
		res_arr = compare_exchange(parsed.arr+(parsed.id-1)*len, len, parsed.arr+parsed.id*len, parsed.len-parsed.id*len);
		memcpy(parsed.arr+(parsed.id-1)*len, res_arr, parsed.len-len*2);
	}
	free(res_arr);
	return NULL;
}


void parallel_calc(int *arr, int len)
{
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
	// for(int i=0; i<4; i++)
	// {
		// if(i % 2)
		// {
			// for(int j=1; j<4; j+=2)
			// {
			// 	infoForThread *info = (infoForThread *)malloc(sizeof(infoForThread));
			// 	info->len = len;
			// 	info->id = j;
			// 	info->arr = arr;
			// 	pthread_create(&threads[j], NULL, thread_task_ces, (void *)(info));
			// }
			// for(int j=1; j<4; j+=2)
			// 	pthread_join(threads[j], NULL);
		// }
		// else
		// {
			// for(int j=2; j<4; j+=2)//да, всего 1 итерация
			// {
			// 	infoForThread *info = (infoForThread *)malloc(sizeof(infoForThread));
			// 	info->len = len;
			// 	info->id = j;
			// 	info->arr = arr;
			// 	pthread_create(&threads[j], NULL, thread_task_ces, (void *)(info));
			// }
			// for(int j=2; j<4; j+=2)
			// 	pthread_join(threads[j], NULL);
		// }
	// }
}