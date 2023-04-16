#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {
	char *args[] = {"./ejecutable",NULL};
	execvp(args[0], args);

	printf("execvp() failed. Error: %s\n", strerror(errno));

	return 0;
}
