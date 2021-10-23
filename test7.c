#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
		int c;
		while (1) {
			printf("inputs: ");
			//system("stty raw");
			system("stty -echo");
			c = getchar();
			//system("stty -raw");
			system("stty echo");
			printf( "%c%c%c", c, c, c);
			c = getchar();
		}
		return 0;
}