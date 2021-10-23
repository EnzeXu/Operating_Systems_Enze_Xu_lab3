#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
        int c;
 
        system("stty raw");
        c = getchar();
        system("stty -raw");
 
        printf( "\nyou input: %c", c);
 
        printf( "\n" );
 
        return 0;
}