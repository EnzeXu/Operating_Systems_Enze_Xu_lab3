#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAXN 100

int main(void)
{
	int arrowStatus = 0;
	int tmpLength = 0;
	char c;
	while (1) {
		printf("inputs: ");
		system("stty raw");
		system("stty -echo");
		c = getchar();
		printf("c = '%c' ", c);
		system("stty -raw");
		system("stty echo");
		if (c == '^') {
			char c1, c2, c3;
			c1 = getchar();
			c2 = getchar();
			c3 = getchar();
			printf("c1 = '%c' ", c1);
			printf("c2 = '%c' ", c2);
			printf("c3 = '%c' ", c3);
			if (c3 == 'A') {
				for (int i = 0; i < tmpLength; ++i) {
					printf("\b");
				}
				arrowStatus += 1;
				char pre[MAXN] = "previous";
				printf("%s %3d", pre, arrowStatus);
				tmpLength = strlen(pre) + 4;
			}
			else if (c3 == 'B') {
				for (int i = 0; i < tmpLength; ++i) {
					printf("\b");
				}
				if (arrowStatus == 0 || arrowStatus == 1) {
					tmpLength = 0;
				}
				arrowStatus -= 1;
				char pre[MAXN] = "previous";
				printf("%s %3d", pre, arrowStatus);
				tmpLength = strlen(pre) + 4;
			}
		}
		//printf( "%c%c%c", c, c, c);
		//c = getchar();
	}
	return 0;
}