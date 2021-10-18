//
//  prog3.c
//  prog3
//
//  Created by ENZE XU on 2021/10/17.
//

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

void quitHandler(int);
char * getMainPath(void);
char * getUserName(void);

int history_id_start;
int history_count;
int history_id[MAX_HISTORY_SAVE + 10] = {};
char history_commands[MAX_HISTORY_SAVE + 10][MAXN] = {};

int pureExecute(char *argvOri[], int left, int right, int flagBackgroundExecution);

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


void printArgv(char *argv[]) { // test print function
	int i = 0;
	while(argv[i] != NULL) {
		printf("argv[%d] %s (length = %ld)\n", i, argv[i], strlen(argv[i]));
		i ++;
	}
	printf("argv[%d] NULL\n", i);
}

int commandExecutePipe(char *argv[], int left, int right, int flagBackgroundExecution) {
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

		result = pureExecute(argv, left, pipeSeat, flagBackgroundExecution);
		exit(0);
	}
	
	// parent
	int status;
	pid_t wait_pid;
	wait_pid = waitpid(pid, &status, 0);

	if (wait_pid == -1) { // error in child
		close(f_des[1]);
		dup2(f_des[0], fileno(stdin));
		close(f_des[0]);
		char info[MAXN] = {0};
		char line[MAXN];
		
		while(fgets(line, MAXN, stdin) != NULL) {
			strcat(info, line);
		}
		printf("ce: %s", info);
		
		printf("Errors occur in pipe, please check your input and try again!\n");
		result = -1;
	} else if (pipeSeat < right - 1){
		close(f_des[1]);
		dup2(f_des[0], fileno(stdin));
		close(f_des[0]);
		result = commandExecutePipe(argv, pipeSeat + 1, right, flagBackgroundExecution);
	}
	char info[MAXN] = {0};
	char line[MAXN];
	
	while(fgets(line, MAXN, stdin) != NULL) {
		strcat(info, line);
	}
	printf("p: %s", info);
	return result;
}



void commandExecute(char *line) {
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
	
	// deal with !xyz history command
	if (argc == 1 && argv[0][0] == '!') {
		int findHistoryReturn = findHistory(argv[0]);
		if (findHistoryReturn == -3) {
			printf("\033[32m[Enze Shell] in command !xyz, xyz must be an integer\033[0m\n");
			return;
		} else if (findHistoryReturn == -2) {
			printf("\033[32m[Enze Shell] sorry history is empty now, so !xyz command is rejected\033[0m\n");
			return;
		} else if (findHistoryReturn == -1) {
			printf("\033[32m[Enze Shell] xyz is out of range(%d to %d) in !xyz command\033[0m\n", history_id_start, history_id_start + history_count - 1);
			return;
		}
		printf("\033[32m[Enze Shell] executing command %d: %s\033[0m", history_id[findHistoryReturn], history_commands[findHistoryReturn]);
		char tmp[MAXN];
		strcpy(tmp, history_commands[findHistoryReturn]);
		//printf("\033[0m");
		commandExecute(tmp);
		return;
	}

	// save the command into history
	saveHistory(line_origin);
	
	// deal with cd command
	char pathUser[MAXN] = "";
	if (strcmp(argv[0], "cd") == 0) {
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
	
	// deal with history command
	if (strcmp(argv[0], "history") == 0) {
		if (argc == 1) {
			printHistory(MAX_HISTORY);
		} else if (argc == 2) {
			int len = strlen(argv[1]);
			for (int i = 0; i < len; ++i) {
				if (argv[1][i] < 48 || argv[1][i] > 57) {
					printf("\033[32m[Enze Shell] argv[1] of a history command should be a positive integer\033[0m\n");
					return;
				}
			}
			int num = atoi(argv[1]);
			printHistory(num);
		} else {
			printf("\033[32m[Enze Shell] argc of a history command should be 1 or 2, but current argc = %d\033[0m\n", argc);
		}
		return;
	}
	
	// deal with erase history command
	if (argc == 2 && strcmp(argv[0], "erase") == 0 && strcmp(argv[1], "history") == 0) {
		printf("\033[32m[Enze Shell] history is erasesd (%d removed)\033[0m\n", (history_count > MAX_HISTORY)? MAX_HISTORY: history_count);
		eraseHistory();
		return;
	}	
	
	
	//printArgv(argv);
	int ret = commandExecutePipe(argv, 0, argc, flagBackgroundExecution);
	return;
}

int pureExecute(char *argvOri[], int left, int right, int flagBackgroundExecution) {
	// printf("hello\n");
	char *argv[MAXN] = {};
	for (int i = left; i < right; ++i) {
		// printf("copy argvOri[%d] to argv[%d]\n", i, i - left);
		argv[i - left] = argvOri[i];
	}

	argv[right] = NULL;
	
	//printf("In pureExecute: %d %d %d\n", left, right, flagBackgroundExecution);
	//printArgv(argv);
	
	//fflush(stdout);
	
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
	readHistory();
	printf("\033[32m[Enze Shell] version: v1.0\033[0m\n");
	printf("\033[32m[Enze Shell] pid = %d\033[0m\n", getpid()); // if execute lab2 in lab2, can help to identify
	printf("\033[32m[Enze Shell] start at (GMT) %s\033[0m", ctime(&t)); // GMT time
	while(1) {
		char line[MAXN];
		printf("\033[34m%s\033[37m %% ", getMainPath());
		char *tmp = fgets(line, MAXN, stdin);
		//if (!tmp) {
		//	printf("bad: %s", tmp);
		//	printf("\n\033[32m[Enze Shell] OK close shop and go home (type: \"Ctrl-D\", pid: %d)\033[0m\n", getpid());
		//	break;
		//}
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
