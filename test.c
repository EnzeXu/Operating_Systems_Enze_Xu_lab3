#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#define MAX_HISTORY 10
#define MAX_HISTORY_SAVE 1000
#define MAXN 10000

int history_id[MAX_HISTORY_SAVE + 10] = {};
char history_commands[MAX_HISTORY_SAVE + 10][MAXN] = {};
int history_id_start;
int history_count;

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
		history_id_start = -1;
		return;
	}
	char c;
	for (int i = 0; i < n; ++i) {
		fscanf(fp, "%d", &history_id[i]);
		c = fgetc(fp); 
		fgets(history_commands[i], MAXN, fp);
		printf("%d %s", history_id[i], history_commands[i]);
		//printf("n = %d\n", n);
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
	printf("string = '%s', tmp = '%s', num = %d\n", arg, tmp, num);
	if (history_count == 0) {
		return -2; // empty history
	}
	if (num < history_id_start || num > history_id_start + history_count - 1) {
		return -1; // id not found
	}
	return num - history_id_start;
}

// when executing "% history"
void printHistory(void) {
	if (history_count <= MAX_HISTORY) {
		for (int i = 0; i < history_count; ++i) {
			printf("%5d  %s", history_id[i], history_commands[i]);
		}
	}
	else {
		for (int i = history_count - MAX_HISTORY; i < history_count; ++i) {
			printf("%5d  %s", history_id[i], history_commands[i]);
		}
	}
	return;
}


// after each execution
void saveHistory(char *line) {
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
	fclose(fp);
	return;
}


int main() {
	printf("Read:\n");
	readHistory();
	printf("\nPrint:\n");
	printHistory();
	// printf("\nErase:\n");
	// eraseHistory();
	//char a[100] = "hello world!\n";
	//char b[100] = "xuenze!\n";
	//char c[100] = "okkkk!\n";
	//char d[100] = "world!\n";
	//char e[100] = "test!\n";
	//printf("\nSave:\n");
	//saveHistory(a);
	//saveHistory(b);
	//saveHistory(c);
	//saveHistory(d);
	//saveHistory(e);
	char line[100];
	while (1) {
		scanf("%s", line);
		//int len = strlen(line);
		//line[len - 1] = 0;
		int ret = findHistory(line);
		printf("findHistory return: %d\n", ret);
	}
	//printf("\nRead:\n");
	//readHistory();
	//printf("\nPrint:\n");
	//printHistory();

	return 0;
}
