// compile: gcc -fopenmp -Wall lab5_without_r.c -o task_without_r
// run: ./task_without_r 2000000000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


int main(int argc, char const *argv[]) {
	int i=0;
    long long steps = 500000000;
    double pi_num = 0.0, step, x, tmp;
    if(argc>1)
    {
        long long tmp_steps = atol(argv[1]);
        if(tmp_steps)
            steps = tmp_steps;
    }
    step = 1/(double)steps;
	double time = omp_get_wtime();
	#pragma omp parallel shared(pi_num) private(tmp)
    {
        #pragma omp for private(x)
        for (i=0; i < steps; i++) {
            x = (i+0.5)*step;
            tmp += 4.0 / (1.0+x*x); 
        }
        #pragma omp atomic
        pi_num += tmp;
    }
    pi_num = step * pi_num;
	time = omp_get_wtime() - time;
	printf("PI = %.16g\ndiff: %.16g\ntime of parallel calc: %lf\n", pi_num, fabs(M_PI - pi_num), time);
}