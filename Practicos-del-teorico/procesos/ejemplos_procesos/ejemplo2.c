#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {

	pid_t ret = fork();

	if (ret) {								//proceso padre
		for (size_t i = 0; i < 5; i++) {
			printf("AAA\n");
			usleep(1);
		}
	} else {								//proceso hijo
		for (size_t i = 0; i < 5; i++) {
			printf("BBBBBB\n");
			usleep(1);
		}
	}

	return 0;
}
