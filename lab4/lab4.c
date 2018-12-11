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


void qs(int *s_arr, int first, int last);
void consistent_calc(int *arr, int len);
void parallel_calc(int *arr, int len);


int main(){
	srand(time(NULL));
	int len = 1001;
	int *arr_for_parrallel = (int *)malloc(len*sizeof(int));
	int *arr_for_consistent = (int *)malloc(len*sizeof(int));
	for(int i=0; i<len; i++)
	{
		arr_for_parrallel[i] = rand()%301-150;
		arr_for_consistent[i] = arr_for_parrallel[i];
		// printf("%d ", arr_for_parrallel[i]);
	}
	printf("\n");
	consistent_calc(arr_for_consistent, len);
	// parallel_calc(arr_for_parrallel, len);
	for(int i=0; i<len; i++)
	{
		printf("%d ", arr_for_consistent[i]);
	}
	free(arr_for_parrallel);
	free(arr_for_consistent);
	return 0;
}

void qs(int *s_arr, int first, int last)
{
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
        qs(s_arr, first, right);
        qs(s_arr, left, last);
    }
}

void consistent_calc(int *arr, int len){
	
	double time = omp_get_wtime();
	qs(arr, 0, len-1);
	printf("time %lf\n", omp_get_wtime()-time);
}

// void parallel_calc(int *arr, int len){
// 	pthread_t threads[4];
// 	for(int i=0; i<4; i++)
// 	{
// 		infoForThread message;
// 		message.low = i*len/4;
// 		if (i != 3)
// 			message.high = (i+1)*len/4;
// 		else
// 			message.high = len;
// 		message.arr = arr;
// 		printf("%d %d %d\n");
// 		pthread_create (&thing[i], NULL, start_thread, (void *)message);
// 	}



// 	// pthread_join(thing1, NULL);
// }