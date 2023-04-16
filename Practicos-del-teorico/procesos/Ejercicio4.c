#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{

	pid_t pidC;
	pidC = fork();
	char *args[] = {};

	if (pidC == 0)
	{
		char *args[] = {"./ejecutable", NULL};
		execvp(args[0], args);
	}
	else
	{
		char *args[] = {"./ejecutable2", NULL};
		execvp(args[0], args);
	}
	printf("execvp() failed. Error: %s\n", strerror(errno));

	return 0;
}
