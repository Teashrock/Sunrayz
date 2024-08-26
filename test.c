#include <stdio.h>

void main() {
#ifdef _WIN32
	puts("_WIN32");
#endif
#ifdef AF_UNIX
	puts("AF_UNIX");
#endif
}