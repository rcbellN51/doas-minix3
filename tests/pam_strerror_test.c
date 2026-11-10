#include <stdio.h>
#include <security/pam_appl.h>

int
main(void)
{
	const char *message;

	message = pam_strerror(NULL, PAM_SYSTEM_ERR);

	if (message == NULL) {
		fprintf(stderr, "pam_strerror returned NULL\n");
		return 1;
	}

	printf("pam_strerror: %s\n", message);
	return 0;
}
