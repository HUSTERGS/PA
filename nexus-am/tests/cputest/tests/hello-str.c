#include "trap.h"

char buf[128];

int main() {
	sprintf(buf, "%s", "Hello world!");
	nemu_assert(strcmp(buf, "Hello world!") == 0);

	sprintf(buf, "%d + %d = %d", 1, 1, 2);
	nemu_assert(strcmp(buf, "1 + 1 = 2") == 0);

	sprintf(buf, "%d + %d = %d", 2, 10, 12);
	nemu_assert(strcmp(buf, "2 + 10 = 12") == 0);

	return 0;
}
