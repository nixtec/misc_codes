/*
 * authenticate - Check the user name and password against the system
 * password database, and login the user if OK.
 *
 * This function has been modified to support the Linux Shadow Password
 * Suite if USE_SHADOW is defined.
 *
 * returns:
 *      UPAP_AUTHNAK: Login failed.
 *      UPAP_AUTHACK: Login succeeded.
 * In either case, msg points to an appropriate message.
 */
#include "authenticate.h"

/* only for Linux version */
#ifndef __WIN32__

int authenticate(const char *user, const char *passwd)
{
  char *pw = NULL;
  char *epasswd = NULL;

#ifdef USE_SHADOW
  struct spwd pd;
  struct spwd *pwdptr = &pd;
  struct spwd *tempPwdPtr = NULL;
  char pwdbuf[512]; // adjust size according to your need
  int pwdlinelen = sizeof(pwdbuf);

  getspnam_r(user, pwdptr, pwdbuf, pwdlinelen, &tempPwdPtr);
  if (tempPwdPtr == NULL) {
    return UPAP_AUTHNAK;
  }
  pw = pd.sp_pwdp;
#endif

  /*
   * XXX If no passwd, let NOT them login without one.
   */
  if (pw == NULL) {
    return (UPAP_AUTHNAK);
  }

  struct crypt_data data;
  data.initialized = 0;
  epasswd = crypt_r(passwd, pw, &data);
  if (strcmp(epasswd, pw)) {
    return (UPAP_AUTHNAK);
  }

  return (UPAP_AUTHACK);
}

#endif /* !__WIN32__ */
