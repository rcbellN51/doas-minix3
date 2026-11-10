#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int
main(void)
{
	int fd[3];
	int i;
	int result;
	int failed = 0;

	for (i = 0; i < 3; i++) {
		fd[i] = open("/dev/null", O_RDONLY);
		if (fd[i] == -1) {
			perror("open");
			return EXIT_FAILURE;
		}
		printf("Opened descriptor %d\n", fd[i]);
	}

	result = closefrom(STDERR_FILENO + 1);
	printf("closefrom(3) returned %d\n", result);

	for (i = 0; i < 3; i++) {
		errno = 0;
		if (fcntl(fd[i], F_GETFD) == -1 && errno == EBADF) {
			printf("Descriptor %d: closed\n", fd[i]);
		} else {
			printf("Descriptor %d: STILL OPEN\n", fd[i]);
			failed = 1;
		}
	}

	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
