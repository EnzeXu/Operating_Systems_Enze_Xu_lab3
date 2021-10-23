#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
		int c;
		while (1) {
			//system("stty raw");
			c = getchar();
			//system("stty -raw");
			printf( "\nyou input: %c\n", c);
		}
		return 0;
}