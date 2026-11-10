#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <security/pam_appl.h>
#include <security/openpam.h>

int
main(void)
{
	pam_handle_t *pamh = NULL;
	struct pam_conv conv = { openpam_ttyconv, NULL };
	int ret;
	int saved_errno;

	errno = 0;
	ret = pam_start("doas", "admin", &conv, &pamh);
	saved_errno = errno;

	fprintf(stderr, "pam_start returned %d, pamh=%p\n",
		ret, (void *)pamh);

	if (ret != PAM_SUCCESS) {
		fprintf(stderr, "pam_start: %s\n",
		    pam_strerror(pamh, ret));
		fprintf(stderr, "errno: %d (%s)\n",
		    saved_errno, strerror(saved_errno));
		if (pamh != NULL)
			pam_end(pamh, ret);
		return 1;
	}

	ret = pam_authenticate(pamh, 0);
	printf("pam_authenticate: %s\n", pam_strerror(pamh, ret));

	if (ret == PAM_SUCCESS) {
		ret = pam_acct_mgmt(pamh, 0);
		printf("pam_acct_mgmt: %s\n", pam_strerror(pamh, ret));
	}

	pam_end(pamh, ret);
	return ret == PAM_SUCCESS ? 0 : 1;
}
