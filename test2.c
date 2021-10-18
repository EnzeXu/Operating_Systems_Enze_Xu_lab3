#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(void)
{
    while (1) {
        char* str = readline("shell: ");
		if (strlen(str) > 1) {
			 add_history(str);
		}
        free(str);
    }
	return 0;
}
