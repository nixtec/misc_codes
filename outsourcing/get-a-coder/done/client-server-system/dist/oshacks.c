/*
 * oshacks.c
 */
#include "oshacks.h"

#ifdef __WIN32__
/* this function will unlock from 0 to 'region' bytes */
int flock(int fd, int operation)
{
  int cur = 0;
  int ret = 0;
  static const int region = 0xffffffff;

  cur = _lseek(fd, 0, SEEK_CUR);
  _lseek(fd, 0, SEEK_SET); /* set file pointer to 0 */
  switch (operation) {
    case LOCK_SH:
    case LOCK_EX:
      ret = _locking(fd, _LK_LOCK, region);
      break;
    case LOCK_UN:
      ret = _locking(fd, _LK_UNLCK, region);
      break;
    default:
      fprintf(stderr, "Invalid operation requested!\n");
      break;
  }
  _lseek(fd, cur, SEEK_SET); /* set to old position */
  if (ret != 0) {
    return -1;
  }

  return 0;
}

#endif /* __WIN32__ */

/* mingw in my system doesn't have _sopen_s(), so using _sopen() */
/* when using this function, you _must_ use the 'unlock' function below */
int open_and_lock(const char *pathname, int flags, mode_t mode, int operation)
{
#ifdef __WIN32__
  int shflags = 0;
  if (operation == LOCK_SH) {
    shflags = _SH_DENYWR; /* shared lock, can't write */
  } else if (operation == LOCK_EX) {
    shflags = _SH_DENYRD | _SH_DENYWR; /* exclusive lock, can't read/write */
  } else {
    shflags = _SH_DENYNO;
  }

  return _sopen(pathname, flags, shflags, mode);

#else
  int fd;
  fd = open(pathname, flags, mode);
  if (fd == -1) {
    return -1;
  }
  if (flock(fd, operation) == -1) {
    close(fd);
    return -1;
  }

  return fd;

#endif
}

int unlock(int fd)
{
#ifdef __WIN32__
  return 0;
#else
  return (flock(fd, LOCK_UN));
#endif
}
