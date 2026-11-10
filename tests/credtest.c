#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static void
print_groups(void)
{
	gid_t groups[NGROUPS_MAX];
	int count, i;

	count = getgroups(NGROUPS_MAX, groups);
	if (count == -1) {
		perror("getgroups");
		return;
	}

	printf("Supplementary groups:");
	for (i = 0; i < count; i++)
		printf(" %d", (int)groups[i]);
	printf("\n");
}

int
main(void)
{
	printf("Initial: uid=%d euid=%d gid=%d egid=%d\n",
		(int)getuid(), (int)geteuid(),
		(int)getgid(), (int)getegid());
	print_groups();

	if (initgroups("admin", 20) == -1) {
		perror("initgroups");
		return 1;
	}

	if (setgid(20) == -1) {
		perror("setgid");
		return 1;
	}

	if (setuid(1000) == -1) {
		perror("setuid");
		return 1;
	}

	printf("Dropped: uid=%d euid=%d gid=%d egid=%d\n",
		(int)getuid(), (int)geteuid(),
		(int)getgid(), (int)getegid());
	print_groups();

	errno = 0;
	if (seteuid(0) == -1)
		printf("seteuid(0): failed: %s\n", strerror(errno));
	else
		printf("seteuid(0): SUCCEEDED - root privileges recovered!\n");

	printf("Final:   uid=%d euid=%d gid=%d egid=%d\n",
		(int)getuid(), (int)geteuid(),
		(int)getgid(), (int)getegid());

	return 0;
}
