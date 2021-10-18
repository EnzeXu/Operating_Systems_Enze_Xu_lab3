#include <stdlib.h>
#include <readline/readline.h>

int main(void)
{
	while (1)
	{
		char * str = readline("Myshell $ ");
		add_history(str);
		free(str);
	}
	return 0;
}
