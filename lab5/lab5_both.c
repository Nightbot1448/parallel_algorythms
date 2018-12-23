#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void with_r(long long steps);
void without_r(long long steps);

int main(int argc, char const *argv[])
{
	long long steps = 500000000;
	int threads_num = 4;
	if(argc>1)
	{
		int tmp_threads_num = atoi(argv[1]);
		if(tmp_threads_num)
			threads_num = tmp_threads_num;
	}
	if(argc>2)
	{
		long long tmp_steps = atol(argv[2]);
		if(tmp_steps)
			steps = tmp_steps;
	}
	printf("r diff\t  time | w_r diff\ttime\n");
	for(int i=1; i<=threads_num; i++)
	{
		omp_set_num_threads(i);
		printf("%d ", i);
		with_r(steps);
		without_r(steps);
	}
	return 0;
}

void without_r(long long steps)
{
	int i=0;	
	double pi_num = 0.0, step, x, tmp;
	step = 1/(double)steps;
	double time = omp_get_wtime();
	#pragma omp parallel shared(pi_num) private(tmp)
	{
		tmp = 0;
		#pragma omp for private(x)
		for (i=0; i < steps; i++) 
		{
			x = (i+0.5)*step;
			tmp += 4.0 / (1.0+x*x); 
		}
		#pragma omp atomic
		pi_num += tmp;
	}
	pi_num = step * pi_num;
	time = omp_get_wtime() - time;
	printf("%.3g %lf\n", fabs(M_PI - pi_num), time);
}

void with_r(long long steps)
{
	int i=0;
	double pi_num, sum = 0.0, step, x;
	step = 1/(double)steps;
	double time = omp_get_wtime();
	#pragma omp parallel reduction(+:sum)
	{
		#pragma omp for private(x)
		for (i=0; i < steps; i++) {
			x = (i+0.5)*step;
			sum += 4.0 / (1.0+x*x); 
		}
	}
	time = omp_get_wtime() - time;
	pi_num = step * sum;
	printf("%.3g %lf ", fabs(M_PI - pi_num), time);
}