#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	pid_t pid;
	printf("p1: id = %d, p_id = %d\n", getpid(), getppid());
	pid = fork();
	if (pid == -1) {
		printf("fork error\n");
		exit(1);
	}
	else if (pid == 0) {
		printf("p2: id = %d, p_id = %d\n", getpid(), getppid());
		pid = fork();
		if (pid == -1) {
			printf("fork error\n");
			exit(1);
		}
		else if (pid == 0) {
			printf("p3: id = %d, p_id = %d\n", getpid(), getppid());
		}
		else {
			pid = fork();
			if (pid == -1) {
				printf("fork error\n");
				exit(1);
			}
			else if(pid ==  0) {
				printf("p4: id = %d, p_id = %d\n", getpid(), getppid());
			}
			else {
				wait(NULL);
			}
		}
	}
	else {
		wait(NULL);
	}
	return 0;
}