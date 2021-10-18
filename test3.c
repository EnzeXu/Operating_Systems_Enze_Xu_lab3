#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define MAX_HISTORY 100
#define MAX_HISTORY_SAVE 1000
#define MAXN 1000

void printArgv(char *argv[]) { // test print function
	int i = 0;
	while(argv[i] != NULL) {
		printf("argv[%d] %s (length = %ld)\n", i, argv[i], strlen(argv[i]));
		i ++;
	}
	printf("argv[%d] NULL\n", i);
}


//int pureExecute(char *argvOri[], int left, int right, int flagBackgroundExecution) {
//	printf("\nCommand:\n%d %d %d\n", left, right, flagBackgroundExecution);
//	printArgv(argvOri);
//	return 0;
//}

int pureExecute(char *argvOri[], int left, int right, int flagBackgroundExecution) {
	// printf("hello\n");
	char *argv[MAXN] = {};
	for (int i = left; i < right; ++i) {
		// printf("copy argvOri[%d] to argv[%d]\n", i, i - left);
		argv[i - left] = argvOri[i];
	}

	argv[right] = NULL;
	
	//printf("%d %d %d\n", left, right, flagBackgroundExecution);
	//printArgv(argv);
	// fork
	pid_t pid, wait_pid;
	int status;
	pid = fork();
	if (pid == 0) {
	// printf("command = %s\n", commandFull);
		int execvp_return = execvp(argv[0], argv);
		if (execvp_return < 0) {
			perror("\033[32m[Enze Shell] child failed\033[0m");
			printf("\033[0m");
			exit(EXIT_FAILURE);
		}
	}

	if (flagBackgroundExecution == 1) { // iff receive an '&', skip waitpid
		printf("\033[32m[Enze Shell] child pid = %d\033[0m\n", pid);
		return 0;
	}
	wait_pid = waitpid(pid, &status, 0);
	// printf("waitpid return %d\n", wait_pid);
	if (wait_pid == -1) {
		printf("\033[32m[Enze Shell] parent process cannot wait any more, return\033[0m\n");
		return -1;
	}
	return 0;
}


int commandExecutePipe(char *argv[], int left, int right, int flagBackgroundExecution) {
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
		return pureExecute(argv, left, right, flagBackgroundExecution);
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

		result = (argv, left, pipeSeat, flagBackgroundExecution);
		exit(result);
	}
	
	// parent
	int status;
	pid_t wait_pid;
	wait_pid = waitpid(pid, &status, 0);

	if (wait_pid == -1) { // error in child
		close(f_des[1]);
		dup2(f_des[0], STDIN_FILENO);
		close(f_des[0]);
		printf("Errors occur in pipe, please check your input and try again!\n");
		result = -1;
	} else if (pipeSeat < right - 1){
		close(f_des[1]);
		dup2(f_des[0], STDIN_FILENO);
		close(f_des[0]);
		result = commandExecutePipe(argv, pipeSeat + 1, right, flagBackgroundExecution);
	}

	return result;
}


int main() {
	char a[100] = "env";
	char b[100] = "|";
	char c[100] = "grep";
	char d[100] = "lab3";
	char *argv[10] = {a, b, c, d};
	//char lines[1000][1000] = {"env", "|", "grep", "lab3"};
	//char **argv = lines;
	commandExecutePipe(argv, 0, 4, 0);
	return 0;
}
