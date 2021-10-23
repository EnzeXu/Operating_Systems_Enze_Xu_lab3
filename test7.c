#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>


#define MAXN 1000

int main(void)
{
	int arrowStatus = 0;
	int commandLength = 0;
	char historyTest[MAXN][MAXN] = {
		"previous command 0",
		"previous command 1",
		"previous command 2",
		"previous command 3",
		"previous command 4",
		"previous command 5",
		"previous command 6",
		"previous command 7",
		"previous command 8",
		"previous command 9",
		"previous command 10"
	};
	char commandLine[MAXN] = {0};
	char c;
	int z = 10;
	int arrowFlag = 0;
	
	while (1) {
		if (!arrowFlag) {
			printf("inputs: ");
		}
		system("stty raw");
		//system("stty -echo");
		c = getchar();
		system("stty -raw");
		//system("stty echo");
		//printf("c ascii = '%d' ", c);
		if (c == 27) {
			char c1, c2, c3;
			system("stty raw");
			//system("stty -echo");
			c1 = getchar();
			c2 = getchar();
			system("stty -raw");
			for (int i = 0; i < 4; ++i) {
				printf("\b \b");
			}
			//system("stty echo");
			//printf("c1 = '%c' ", c1);
			//printf("c2 = '%c' ", c2);
			if (c2 == 'A') {
				for (int i = 0; i < commandLength; ++i) {
					printf("\b \b");
				}
				arrowStatus += 1;
				if (arrowStatus > 10) arrowStatus = 10;
				memset(commandLine, 0, sizeof(commandLine));
				strcat(commandLine, historyTest[arrowStatus]);
				printf("%s", historyTest[arrowStatus]);
				commandLength = strlen(historyTest[arrowStatus]);
				arrowFlag = 1;
			}
			else if (c2 == 'B') {
				for (int i = 0; i < commandLength; ++i) {
					printf("\b \b");
				}
				if (arrowStatus == 0 || arrowStatus == 1) {
					commandLength = 0;
				}
				else {
					arrowStatus -= 1;
					memset(commandLine, 0, sizeof(commandLine));
					strcat(commandLine, historyTest[arrowStatus]);
					printf("%s", historyTest[arrowStatus]);
					commandLength = strlen(historyTest[arrowStatus]);
					arrowFlag = 1;
				}
			}
		}
		else if (c == 8) {
		}
		else if (c == 13) {
			for (int i = 0; i < 2; ++i) {
				printf("\b \b");
			}
			printf("\nDealing with command '%s'\n", commandLine);
			if (strcmp(commandLine, "exit()") == 0) {
				break;
			}
			commandLength = 0;
			memset(commandLine, 0, sizeof(commandLine));
			arrowFlag = 0;
		}
		else {
			commandLine[strlen(commandLine)] = c;
			commandLine[strlen(commandLine) + 1] = '\0';
			commandLength += 1;
			arrowFlag = 1;
		}
		
		//printf( "%c%c%c", c, c, c);
		//c = getchar();
	}
	return 0;
}