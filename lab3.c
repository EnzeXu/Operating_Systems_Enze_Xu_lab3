//
//  prog2.c
//  prog2
//
//  Created by ENZE XU on 2021/9/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define MAXN 10000

void quitHandler(int);
char * getMainPath(void);
char * getUserName(void);


void printArgv(char *argv[]) { // test print function
	int i = 0;
	while(argv[i] != NULL) {
		printf("argv[%d] %s (length = %ld)\n", i, argv[i], strlen(argv[i]));
		i ++;
	}
	printf("argv[%d] NULL\n", i);
}

void commandExecute(char *line) {
	char commandPathBin[MAXN] = "/bin/";
	char commandPathUsrBin[MAXN] = "/usr/bin/";
	char commandFullBin[MAXN];
	char commandFullUsrBin[MAXN];
	char commandFull[MAXN];
	//char pathUser[MAXN] = "/home/csuser/";
	int argc = 0;
	char *argv[MAXN];
	char *token;

	token = strtok(line, " ");
	while(token != NULL){
		argv[argc] = token;
		token = strtok(NULL, " ");
		argc++;
	}

	//printArgv(argv);
	//printf("argc = %d\n, %d", argc, argv[0]);

	if (strlen(argv[argc - 1]) == 1 && argv[argc - 1][0] == 10) argc -= 1;
	else if (argv[argc - 1][strlen(argv[argc - 1]) - 1] == 10) argv[argc - 1][strlen(argv[argc - 1]) - 1] = '\0';

	// printf("argc = %d\n", argc);
	if (argc == 0) return; // empty command, do nothing, just print the prompt again
	int flagBackgroundExecution = 0;
	if (argv[argc - 1][strlen(argv[argc - 1]) - 1] == '&') {
		flagBackgroundExecution = 1;
		printf("\033[32m[Enze Shell] background execution\033[0m\n");
		if (strlen(argv[argc - 1]) == 1) {
			argv[argc - 1] = NULL;
			argc --;
		}
		else {
			argv[argc - 1][strlen(argv[argc - 1]) - 1] = '\0';
			argv[argc] = NULL;
		}
	}
	else argv[argc] = NULL;

	if (argc == 0) return; // empty command, do nothing, just print the prompt again

	// deal with cd command
	char pathUser[MAXN] = "";
	if(strcmp(argv[0], "cd") == 0) {
		if (argv[1] == NULL) return;
		if (argv[1][0] == '~') {
			argv[1][0] = '/';
			char tmpPath[MAXN];
			char pathHead[MAXN] = "/home/";
			char pathTail[MAXN] = "/";
			strcat(pathUser, pathHead);
			strcat(pathUser, getUserName());
			strcat(pathUser, pathTail);
			// printf("pathUser: %s\n", pathUser);
			strcpy(tmpPath, pathUser);
			strcat(tmpPath, argv[1]);
			strcpy(argv[1], tmpPath);
			// printf("newpath: %s\n", argv[1]);
		}
		int chdir_return = chdir(argv[1]);
		if (chdir_return ==  -1) perror("cd");
		return;
	}

	// check if the command is available
	strcpy(commandFullBin, commandPathBin);
	strcat(commandFullBin, argv[0]);
	strcpy(commandFullUsrBin, commandPathUsrBin);
	strcat(commandFullUsrBin, argv[0]);
	
	if (access(commandFullBin, F_OK) == 0) {
		// strcpy(commandFull, commandFullBin);
	}
	else if (access(commandFullUsrBin, F_OK) == 0) {
		// strcpy(commandFull, commandFullUsrBin);
	}
	else if (access(argv[0], F_OK) == 0) {
		// strcpy(commandFull, commandFullUsrBin);
	}
	else {
		printf("\033[32m[Enze Shell] %s: command not found\033[0m\n", argv[0]);
		return;
	}
	

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
		return ;
	}
	wait_pid = waitpid(pid, &status, 0);
	// printf("waitpid return %d\n", wait_pid);
	if (wait_pid == -1) {
		printf("\033[32m[Enze Shell] parent process cannot wait any more, return\033[0m\n");
	}
}

char mainPath[MAXN];
char * getMainPath(void) {
	getcwd(mainPath, sizeof(mainPath));
	return mainPath;
}

char * getUserName(void) { // mine is "/home/csuser/" but if Professor try other user...emmm...anyway, let me figure it out
	struct passwd *pwd = getpwuid(getuid());
	return pwd->pw_name;
}

int main(){
	signal(SIGINT, quitHandler); // signal handler used to ignore Ctrl-C
	time_t t;
	time(&t);
	printf("\033[32m[Enze Shell] version: v1.0\033[0m\n", ctime(&t));
	printf("\033[32m[Enze Shell] pid = %d\033[0m\n", getpid()); // if execute lab2 in lab2, can help to identify
	printf("\033[32m[Enze Shell] start at (GMT) %s\033[0m", ctime(&t)); // GMT time
	while(1) {
		char line[MAXN];
		printf("\033[34m%s\033[37m %% ", getMainPath());
		if (!fgets(line, MAXN, stdin)) {
			printf("\n\033[32m[Enze Shell] OK close shop and go home (type: \"Ctrl-D\", pid: %d)\033[0m\n", getpid());
			break;
		}
		if (strcmp(line, "exit\n") == 0) {
			printf("\033[32m[Enze Shell] please use exit() or Ctrl-D to exit\033[0m\n");
			continue;
		}
		if (strcmp(line, "exit()\n") == 0) {
			printf("\033[32m[Enze Shell] OK close shop and go home (type: \"exit()\", pid: %d)\033[0m\n", getpid());
			break;
		}
		commandExecute(line);
	}
	return 0;
}

void quitHandler(int theInt) { // signal handler used to ignore Ctrl-C
	//fflush(stdin);
	//printf("\n[Enze Shell] Not QUITTING (SIGINT = %d)\n", theInt);
	//printf("\n%s %% ", getMainPath());
	//fflush(stdout);
	return;
}
