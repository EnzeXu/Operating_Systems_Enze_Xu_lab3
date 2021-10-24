#include <stdio.h>

int main() {
	printf("hello\r\033[kworld!\n");
	printf("hello\bworld!\n");
	printf("hello\r\bworld!\n");
	printf("hello\t\bworld!\n");
	printf("hello%cworld!\n", 8);
	return 0;
}