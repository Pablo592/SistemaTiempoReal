#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {


	printf("SOY UN TROYANO \n");
	sleep(2);
	char *args[] = {"clear",0};
	execvp("clear",args );
	printf("execvp() failed. Error: %s\n", strerror(errno));

	return 0;
}
