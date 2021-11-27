/*
 * authenticate.h
 */
#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

/* this file is _only_ for Linux version */
#ifndef __WIN32__

#include <stdio.h>
#include <string.h>
#include <pwd.h>
#ifndef __USE_GNU
#define __USE_GNU		/* crypt_r() is GNU extension */
#endif
#include <crypt.h>
#include <unistd.h>

#ifndef _BSD_SOURCE
#define _BSD_SOURCE		/* getspnam_r() */
#endif
#include <shadow.h>

/* if your system doesn't use shadow password, then comment the following */
#define USE_SHADOW

#define UPAP_AUTHNAK -1
#define UPAP_AUTHACK 0

int authenticate(const char *u, const char *p);

#endif /* !__WIN32__ */
#endif /* AUTHENTICATE_H */
