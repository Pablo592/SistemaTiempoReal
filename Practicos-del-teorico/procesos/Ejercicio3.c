#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {
	char *args[] = {"ls","-l",0};
	execvp("ls", args);

	printf("execvp() failed. Error: %s\n", strerror(errno));

	return 0;
}
