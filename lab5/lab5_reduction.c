// compile: gcc -fopenmp -Wall lab5_reduction.c -o task_reduction
// run: ./task_reduction 3000000000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


int main(int argc, char const *argv[]) {
	long count_of_op = 3000000000;
	if (argc > 1){
		long tmp_count_of_op = atol(argv[1]);
		if (tmp_count_of_op)
			count_of_op = tmp_count_of_op;
	}
	printf("Count of operations: %ld\n", count_of_op);
	double pi_num=0.0;
	double time = omp_get_wtime();
	#pragma omp parallel for reduction(+:pi_num)
	for(long i=0; i<=count_of_op; i += 1)
	{
		if (!((i-1)%2))
		{
			pi_num += (-4.0)/(2*i+1);
		}
		else{
			pi_num += 4.0/(2*i+1);
		}
	}
	time = omp_get_wtime() - time;
	printf("PI = %.16g\ndiff: %.16g\ntime of parallel calc: %lf\n", pi_num, fabs(M_PI - pi_num), time);
}