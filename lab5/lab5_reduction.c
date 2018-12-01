// compile: gcc -fopenmp -Wall lab5_reduction.c -o task_reduction
// run: ./task_reduction 2000000000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


int main(int argc, char const *argv[]) {
    int i=0;
    long long steps = 500000000;
    double pi_num, sum = 0.0, step, x;
    if(argc>1)
    {
        long long tmp_steps = atol(argv[1]);
        if(tmp_steps)
            steps = tmp_steps;
    }
    step = 1/(double)steps;
    double time = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum) private(x)
        for (i=0; i < steps; i++) {
            x = (i+0.5)*step;
            sum += 4.0 / (1.0+x*x); 
        }
    time = omp_get_wtime() - time;
    pi_num = step * sum;
    printf("PI = %.16g\ndiff: %.16g\ntime of parallel calc: %lf\n", pi_num, fabs(M_PI - pi_num), time);
}