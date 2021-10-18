//  lab3.c
//  Created by ENZE XU on 2021/10/17.

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

char mainPath[MAXN];
int history_id_start;
int history_count;
int history_id[MAX_HISTORY_SAVE + 10] = {};
char history_commands[MAX_HISTORY_SAVE + 10][MAXN] = {};

char *getMainPath(void);
char *getUserName(void);
void readHistory(void);
void eraseHistory(void);
int findHistory(char* arg);
void printHistory(int num);
void saveHistory(char *line);
void printArgv(char *argv[]);
int pureExecute(char *argvOri[], int left, int right);
int commandExecutePipe(char *argv[], int left, int right);
int commandExecute(char *line);
void quitHandler(int);

// get pwd
char *getMainPath(void) {
	getcwd(mainPath, sizeof(mainPath));
	return mainPath;
}

// mine is "/home/csuser/" but if Dr. Canas tests other user...emmm...anyway, let me figure it out
char *getUserName(void) {
	struct passwd *pwd = getpwuid(getuid());
	return pwd->pw_name;
}

// only when shell starts
void readHistory(void) {
	FILE *fp = NULL;
	fp = fopen(".myhistory", "r");
	if (fp == NULL) {
		perror("Error in opening file .myhistory");
	  	return;
	}
	int n;
	fscanf(fp, "%d", &n);
	history_count = n;
	if (n == 0) {
		history_id_start = 1;
		return;
	}
	char c;
	for (int i = 0; i < n; ++i) {
		fscanf(fp, "%d", &history_id[i]);
		c = fgetc(fp); 
		fgets(history_commands[i], MAXN, fp);
		//printf("%d %s", history_id[i], history_commands[i]);
	}
	history_id_start = history_id[0];
	fclose(fp);
	return;
}

// when executing "% erase history"
void eraseHistory(void) {
	history_count = 0;
	history_id_start = 1;
	memset(history_id, 0, sizeof(history_id));
	memset(history_commands, 0, sizeof(history_commands));
	
	FILE *fp = NULL;
	fp = fopen(".myhistory", "w");
	if (fp == NULL) {
		perror("Error in opening file .myhistory");
	  	return;
	}
	
	fprintf(fp, "0\n");
	return;
}

// when executing "% !xyz"
int findHistory(char* arg) {
	int len = strlen(arg);
	for (int i = 1; i < len; ++i) {
		if (arg[i] < 48 || arg[i] > 57) {
			return -3; // not !+[integer]
		}
	}
	char tmp[MAXN];
	strcpy(tmp, &arg[1]);
	int num = atoi(tmp);
	// printf("string = '%s', tmp = '%s', num = %d\n", arg, tmp, num);
	if (history_count == 0) {
		return -2; // empty history
	}
	if (num < history_id_start || num > history_id_start + history_count - 1) {
		return -1; // id not found
	}
	return num - history_id_start;
}

// when executing "% history"
void printHistory(int num) {
	readHistory();
	if (num <= MAX_HISTORY && num <= history_count) {
		for (int i = history_count - num; i < history_count; ++i) {
			printf("%5d  %s", history_id[i], history_commands[i]);
		}
	}
	else if (MAX_HISTORY <= num && MAX_HISTORY <= history_count) {
		for (int i = history_count - MAX_HISTORY; i < history_count; ++i) {
			printf("%5d  %s", history_id[i], history_commands[i]);
		}
	}
	else if (history_count <= num && history_count <= MAX_HISTORY) {
		for (int i = 0; i < history_count; ++i) {
			printf("%5d  %s", history_id[i], history_commands[i]);
		}
	}
	return;
}

// after each execution
void saveHistory(char *line) {
	// printf("save history: %s", line);
	// to cache
	history_id[history_count] = history_id_start + history_count;
	strcpy(history_commands[history_count], line);
	history_count ++;

	// to file
	
	FILE *fp = NULL;
	fp = fopen(".myhistory", "w");
	if (fp == NULL) {
		perror("Error in opening file .myhistory");
	  	return;
	}
	if (history_count <= MAX_HISTORY) {
		fprintf(fp, "%d\n", history_count);
		for (int i = 0; i < history_count; ++i) {
			fprintf(fp, "%d %s", history_id[i], history_commands[i]);
		}
	}
	else {
		fprintf(fp, "%d\n", MAX_HISTORY);
		for (int i = history_count - MAX_HISTORY; i < history_count; ++i) {
			fprintf(fp, "%d %s", history_id[i], history_commands[i]);
		}
	}
	//fflush(fp);
	fclose(fp);
	
	return;
}

// test print function
void printArgv(char *argv[]) {
	int i = 0;
	while(argv[i] != NULL) {
		printf("argv[%d] %s (length = %ld)\n", i, argv[i], strlen(argv[i]));
		i ++;
	}
	printf("argv[%d] NULL\n", i);
}

// pure execution
int pureExecute(char *argvOri[], int left, int right) {
	char *argv[MAXN] = {};
	int argc = right - left;
	for (int i = left; i < right; ++i) {
		argv[i - left] = argvOri[i];
	}
	argv[right] = NULL;
	
	// deal with history command
	if (strcmp(argv[0], "history") == 0) {
		if (argc == 1) {
			printHistory(MAX_HISTORY);
		} else if (argc == 2) {
			int len = strlen(argv[1]);
			for (int i = 0; i < len; ++i) {
				if (argv[1][i] < 48 || argv[1][i] > 57) {
					printf("\033[32m[Enze Shell] argv[1] of a history command should be a positive integer\033[0m\n");
					return -1;
				}
			}
			int num = atoi(argv[1]);
			printHistory(num);
		} else {
			printf("\033[32m[Enze Shell] argc of a history command should be 1 or 2, but current argc = %d\033[0m\n", argc);
		}
		return 0;
	}
	
	// fork
	pid_t pid, wait_pid;
	int status;
	pid = fork();
	if (pid == 0) {
	// printf("command = %s\n", commandFull);
		int execvp_return = execvp(argv[0], argv);
		if (execvp_return < 0) {
			exit(EXIT_FAILURE);
		}
	}
	wait_pid = waitpid(pid, &status, 0);
	if (wait_pid == -1) {
		return -1;
	}
	return 0;
}

// deal with pipe
int commandExecutePipe(char *argv[], int left, int right) {
	//printf("commandExecutePipe: [%d, %d)\n", left, right);
	
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
		printf("\n\033[32m[Enze Shell] no available command after \"|\"\033[0m\n");
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

	if (wait_pid == -1) { // error in child
		close(f_des[1]);
		dup2(f_des[0], fileno(stdin));
		close(f_des[0]);
		printf("\n\033[32m[Enze Shell] errors occur in pipe, please check your input and try again!\033[0m\n");
		
		char info[4096] = {0};
		char line[1000];
		while(fgets(line, 1000, stdin) != NULL) { // 读取子进程的错误信息
			strcat(info, line);
		}
		printf("%s", info); // 打印错误信息
		result = -1;
	} else if (pipeSeat < right - 1){
		close(f_des[1]);
		dup2(f_des[0], fileno(stdin));
		close(f_des[0]);
		result = commandExecutePipe(argv, pipeSeat + 1, right);
	}
	return result;
}

// deal command
int commandExecute(char *line) {
	char line_origin[MAXN];
	strcpy(line_origin, line);
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
	if (argc == 0) return 0; // empty command, do nothing, just print the prompt again
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

	if (argc == 0) return 0; // empty command, do nothing, just print the prompt again
	
	// deal with !xyz history command
	if (argc == 1 && argv[0][0] == '!') {
		int findHistoryReturn = findHistory(argv[0]);
		if (findHistoryReturn == -3) {
			printf("\033[32m[Enze Shell] in command !xyz, xyz must be an integer\033[0m\n");
			return -1;
		} else if (findHistoryReturn == -2) {
			printf("\033[32m[Enze Shell] sorry history is empty now, so !xyz command is rejected\033[0m\n");
			return -1;
		} else if (findHistoryReturn == -1) {
			printf("\033[32m[Enze Shell] xyz is out of range(%d to %d) in !xyz command\033[0m\n", history_id_start, history_id_start + history_count - 1);
			return -1;
		}
		printf("\033[32m[Enze Shell] executing command %d: %s\033[0m", history_id[findHistoryReturn], history_commands[findHistoryReturn]);
		char tmp[MAXN];
		strcpy(tmp, history_commands[findHistoryReturn]);
		//printf("\033[0m");
		commandExecute(tmp);
		return 0;
	}

	// save the command into history
	saveHistory(line_origin);
	
	// deal with cd command
	char pathUser[MAXN] = "";
	if (strcmp(argv[0], "cd") == 0) {
		if (argv[1] == NULL) return 0;
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
		return 0;
	}
	
	// deal with erase history command
	if (argc == 2 && strcmp(argv[0], "erase") == 0 && strcmp(argv[1], "history") == 0) {
		printf("\033[32m[Enze Shell] history is erasesd (%d removed)\033[0m\n", (history_count > MAX_HISTORY)? MAX_HISTORY: history_count);
		eraseHistory();
		return 0;
	}	
	
	pid_t pid, wait_pid;
	int status;
	pid = fork();
	if (pid == 0) {
		int in_f = dup(STDIN_FILENO);
		int out_f = dup(STDOUT_FILENO);
		int result = commandExecutePipe(argv, 0, argc);
		dup2(in_f, STDIN_FILENO);
		dup2(out_f, STDOUT_FILENO);
		exit(result);
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

// signal handler used to ignore Ctrl-C
void quitHandler(int theInt) {
	//fflush(stdin);
	//printf("\n[Enze Shell] Not QUITTING (SIGINT = %d)\n", theInt);
	//printf("\n%s %% ", getMainPath());
	//fflush(stdout);
	return;
}

int main(){
	signal(SIGINT, quitHandler); // signal handler used to ignore Ctrl-C
	time_t t;
	time(&t);
	readHistory();
	printf("\033[32m[Enze Shell] version: v1.0\033[0m\n");
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
			saveHistory(line);
			printf("\033[32m[Enze Shell] please use exit() or Ctrl-D to exit\033[0m\n");
			continue;
		}
		if (strcmp(line, "exit()\n") == 0) {
			saveHistory(line);
			printf("\033[32m[Enze Shell] OK close shop and go home (type: \"exit()\", pid: %d)\033[0m\n", getpid());
			break;
		}
		int result = commandExecute(line);
	}
	return 0;
}
