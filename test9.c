#include <stdio.h>

int main() {
	printf("hello\r\033[kworld!\n");
	printf("hello\bworld!\n");
	printf("hello\t\bworld!\n");
	return 0;
}