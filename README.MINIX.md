# doas for MINIX 3

This document describes building, installing, and configuring the portable `doas` utility on MINIX 3.

The MINIX-specific changes are maintained in [doas-minix3](https://github.com/rcbellN51/doas-minix3), based on the [portable doas project](https://codeberg.org/thejessesmith/doas).

For general usage and configuration syntax, see [README.md](README.md).

## Tested environment

The port has been developed and tested with:

- **Operating system:** MINIX 3.4.0rc6 development snapshot (May 9, 2017)
- **Architecture:** i386
- **Compiler:** Clang 3.6
- **Build utility:** GNU Make
- **Authentication:** OpenPAM 20140912
- **Installation prefix:** `/usr/local`

Other MINIX releases and architectures have not been tested.

## Prerequisites

A working MINIX development environment is required, including:

- C compiler and development headers
- GNU Make (`gmake`)
- Yacc
- OpenPAM headers and libraries

The OpenPAM installation used for testing places headers and libraries under `/usr/pkg`.

## Building

From the repository directory, run:

```sh
gmake clean

gmake \
    SYSCONFDIR=/etc \
    CPPFLAGS='-include compat/compat.h -I/usr/pkg/include' \
    LDFLAGS='-L/usr/pkg/lib -Wl,-rpath,/usr/pkg/lib -lpam'
```

The resulting executable is named `doas`.

The `compat/compat.h` header supplies compatibility declarations while accommodating MINIX's native `closefrom()` implementation.

The build uses `/etc/doas.conf` as the authorization configuration file.

## Installing

Become root using `su -`, then install the executable:

```sh
install -o root -g wheel -m 4755 \
    /path/to/doas \
    /usr/local/bin/doas
```

Replace `/path/to/doas` with the path to the compiled executable.

The executable must be owned by root and have the setuid bit enabled so that authorized users can execute commands with elevated privileges.

Verify the installation:

```sh
ls -l /usr/local/bin/doas
```

The expected permissions are:

```text
-rwsr-xr-x  1 root  wheel  ...
```

## Configuring OpenPAM

Create the PAM service configuration:

`/usr/pkg/etc/pam.d/doas`

Use the following configuration, which was tested on MINIX 3:

```text
auth            required        /usr/pkg/lib/security/pam_unix.so
account         required        /usr/pkg/lib/security/pam_unix.so
session         required        /usr/pkg/lib/security/pam_deny.so
password        required        /usr/pkg/lib/security/pam_deny.so
```

Authentication and account validation are performed by `pam_unix.so`.

The session and password management stacks use `pam_deny.so`. The tested `doas` implementation does not open a PAM session or invoke PAM password-changing operations.

This configuration is specific to the tested OpenPAM installation. Other installations may require different module paths or policies.

## Configuring authorization

Create `/etc/doas.conf` with the following rule:

```text
permit :wheel as root
```

This permits members of the `wheel` group to execute commands as root after successful authentication.

Set restrictive permissions:

```sh
chown root:wheel /etc/doas.conf
chmod 600 /etc/doas.conf
```

The rule grants broad administrative privileges to members of `wheel`. Review group membership before enabling it.

For more restrictive authorization rules, consult the upstream README and configuration documentation.

## Verification

As a regular user belonging to the `wheel` group, run:

```sh
doas whoami
```

After successful password authentication, the expected output is:

```text
root
```

You can also verify the effective credentials:

```sh
doas id
```

An incorrect password should result in authentication failure.

## MINIX-specific implementation notes

The port includes adaptations for MINIX's available system interfaces and authentication environment:

- Uses `setuid()` and `setgid()` in place of unavailable `setresuid()` and `setresgid()` interfaces.
- Uses the MINIX-compatible OpenPAM authentication path.
- Accommodates the native MINIX `closefrom()` function.
- Defines appropriate executable search paths for `/usr/pkg` and `/usr/local`.
- Applies a controlled executable search path when executing commands.

These changes are intended to preserve the functionality of portable `doas` while supporting the MINIX 3 environment.

## Security considerations

`doas` is a privileged utility installed with the setuid-root bit. Incorrect configuration or implementation defects can have significant security consequences.

This port has been built and functionally tested on the MINIX 3 environment described above, including successful and unsuccessful authentication and command authorization.

**It has not undergone a comprehensive independent security audit.**

Review the source code, authorization rules, PAM configuration, and installation permissions before deploying it on systems requiring stronger security assurances.

## Upstream project

This repository retains the original portable `doas` source, history, and licensing information.

Upstream: https://codeberg.org/thejessesmith/doas

MINIX 3 port: https://github.com/rcbellN51/doas-minix3
