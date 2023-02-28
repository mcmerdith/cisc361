#include <stdio.h>

void test(char*);

int main() {
	test("Hello World!");
	return 0;
}

void test(char *message) {
	printf("This is a test message: %s\n", message);
}
