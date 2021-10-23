#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
		int c;
		while (1) {
			printf("inputs: ");
			system("stty raw");
			c = getchar();
			system("stty -raw");
			printf( "%c%c%c", c, c, c);
			c = getchar();
		}
		return 0;
}