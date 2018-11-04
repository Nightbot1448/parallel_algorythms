#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

double part_of_pi(long start, long end){
	double num = 0;
	// printf("\t\t[start: %ld, end: %ld)\n", start, end);
	for(long i=start; i<2*end; i += 2){
		if ((i-1)/2%2)
			num -= 4.0/i;
		else
			num += 4.0/i;
	}
	return num;
}

int main(int argc, char *argv[]){
	int count_of_proc = 1, tmp_cop = 0, last_add_op = 0;
	long count_of_op = 1000000, tmp_coo = 0, proc_op_count = 0;
	if (argc > 2)
	{
		tmp_cop = atoi(argv[1]);
		tmp_coo = atoi(argv[2]);
		if (tmp_cop && tmp_coo){
			count_of_proc = tmp_cop;
			count_of_op = tmp_coo;
		}
		else
			printf("use defaul values\n");
	}
	printf("count of proccess: %d\n", count_of_proc);
	printf("count of operatoions: %ld\n", count_of_op);
	last_add_op = count_of_op % count_of_proc;
	// printf("last add: %d\n", last_add_op);
	proc_op_count = count_of_op / count_of_proc;
	for (int kid = 0; kid < count_of_proc; ++kid) {
	    pid_t pid = fork();
	    if(pid < 0) {
	        exit(EXIT_FAILURE);
	    }
	    else if (pid > 0) {
	    	continue;
	    }
	    else {
			// printf("kid: %d, pid: %d, ppid: %d\n", kid, getpid(), getppid());
	       	if(kid + 1 == count_of_proc) {
				// printf("\tkid: %d, op: %ld\n", kid, proc_op_count + last_add_op);
				// printf("\tkid: %d, interval: [%ld, %ld)\n", kid, kid*proc_op_count + 1, count_of_op + 1);
				printf("\tkid: %d, num: %.12lf\n", kid, part_of_pi(kid*proc_op_count + 1, count_of_op + 1));
			}
			else{
				// printf("\tkid: %d, op: %ld(1)\n", kid, proc_op_count);
				// printf("\tkid: %d, interval: [%ld, %ld)\n", kid, kid*proc_op_count + 1, (kid + 1)*proc_op_count + 1);
				printf("\tkid: %d, num: %.12lf\n", kid, part_of_pi(kid*proc_op_count + 1, (kid + 1)*proc_op_count + 1));
			}
	        exit(EXIT_SUCCESS);
	    }
	}

	for (int kid = 0; kid < count_of_proc; ++kid) {
	    int status;
	    // pid_t pid = 
	    wait(&status); // kids could be ready in any order
		// if (kid == 0)
			// printf("EXIT\n");
	    // printf("exit: %d, pid: %d, status: %d\n", kid, pid, status);
	}
	// printf("ENDFOR\n");

	// long count = 10000000;
	// double num = 4.0;
	
	// printf("%.12lf, %.12lf\n", num, fabs(num - M_PI));

	return 0;
}