#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printArgv(char *argv[]) { // test print function
	int i = 0;
	while(argv[i] != NULL) {
		printf("argv[%d] %s (length = %ld)\n", i, argv[i], strlen(argv[i]));
		i ++;
	}
	printf("argv[%d] NULL\n", i);
}


void pureExecute(char *argvOri[], int left, int right, int flagBackgroundExecution) {
	printf("\nCommand:\n%d %d %d\n", left, right, flagBackgroundExecution);
	printArgv(argv);
	return;
}


int commandExecutePipe(char *argv[], int left, int right) {
	printf("commandExecutePipe: [%d, %d)", left, right);
	
	if (left >= right) return 1;
	
	int pipeSeat = -1;
	for (int i = left; i < right; ++i) {
		if (strcmp(argv[i], "|") == 0) {
			pipeSeat = i;
			break;
		}
	}
	
	if (pipeSeat == -1) { // if there is no pipe
		return pureExecute(argv, left, right);
	}
	
	if (pipeSeat == right - 1) {
		printf("no available command after \"|\"\n");
		return -1;
	}

	int f_des[2];
	if (pipe(f_des) == -1) {
		perror("Pipe");
		return -1;
	}
	
	int result = 0;
	pid_t pid = fork();
	if (pid == 0) { // child
		close(f_des[0]);
		dup2(f_des[1], fileno(stdout));
		close(f_des[1]);

		result = pureExecute(argv, left, pipeSeat);
		exit(result);
	}
	
	// parent
	int status;
	pid_t wait_pid;
	wait_pid = waitpid(pid, &status, 0);

	if (wait_pid == -1) { // 子进程的指令没有正常退出，打印错误信息
		close(f_des[1]);
		dup2(f_des[0], STDIN_FILENO);
		close(f_des[0]);
		printf("Errors occur in pipe, please try again!\n");

		result = -1
	} else if (pipeSeat < right - 1){
		close(fds[1]);
		dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
		result = commandExecutePipe(pipeSeat + 1, right); // 递归执行后续指令
	}

	return result;
}


int main() {
	
	char lines[1000][1000] = {"env", "|", "grep", "lab3"};
	char *argv = lines;
	commandExecutePipe(argv, 0, 4);
	return 0;
}