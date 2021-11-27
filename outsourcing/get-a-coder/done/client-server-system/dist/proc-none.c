/*
 * proc-none.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 */
#include "server-funcs.h"

#if defined(USE_WORDEXP_H)
#include <wordexp.h>
#elif defined(__CYGWIN__)
#include <glob.h>
#endif

int do_delete_file(int connfd, const char *cwd, char *path);
int do_list_file(int connfd, const char *cwd, char *path);
int do_create_folder(int connfd, const char *cwd, char *path);
int do_delete_folder(int connfd, const char *cwd, char *path);
int do_change_folder(int connfd, const char *cwd, char *path);
int do_pwd(int connfd);

#ifdef __WIN32__
struct tm *localtime_r(const time_t *timep, struct tm *result)
{
  memcpy(result, localtime(timep), sizeof(struct tm));
  return result;
}
#endif

/* 500 means success */
/* please read error-codes.txt for values used in protocol */

/* please read 'protocol.txt' for understanding the protocol used */
/* handle NONE [see protocol.txt for it] */
int proc_none(int connfd, const char *buf)
{
  //  int fd = -1;
  int ret = -1;
  size_t len;
  //  mode_t mode = 00644;
  char comd[10];
  char cwd[PATH_MAX+1];
  char path[PATH_MAX+1];
  char buffer[128];

  /* LIST|DEL|MKDIR|RMDIR|CHDIR <cwd>:<path> */
  sscanf(buf, "%s", comd);
  /* check 'comd' here whether it is allowed command [not needed] */
  len = strlen(comd);
  len++; /* skip the <space> */
  buf += len; /* buf now points to <cwd>:... */

  len = strcspn(buf, DELIM);
  strncpy(cwd, buf, len);
  cwd[len] = 0;
  len++; /* skip the delim character */
  buf += len; /* buf now points to <path> */

  len = strcspn(buf, DELIM);
  strncpy(path, buf, len);
  path[len] = 0;
  len++; /* skip the delim character */
  buf += len;

  printf("<%s:%d> comd=<%s> cwd=<%s> path=<%s>\n",
      __FILE__, __LINE__, comd, cwd, path);

  sprintf(buffer, "500 OK\n"); /* command supported */
  len = strlen(buffer);
  ret = sendn(connfd, buffer, len);
  if (ret < len) {
    DEBUG("sendn", errno);
    goto cleanup;
  }

  if (!strcmp(comd, "DEL")) {
    ret = do_delete_file(connfd, cwd, path);
  } else if (!strcmp(comd, "LIST")) {
    ret = do_list_file(connfd, cwd, path);
  } else if (!strcmp(comd, "MKDIR")) {
    ret = do_create_folder(connfd, cwd, path);
  } else if (!strcmp(comd, "RMDIR")) {
    ret = do_delete_folder(connfd, cwd, path);
  } else if (!strcmp(comd, "CHDIR")) {
    ret = do_change_folder(connfd, cwd, path);
  } else if (!strcmp(comd, "GETCWD")) {
    ret = do_getcwd(connfd);
  }

  ret = 0;
cleanup:
  return ret;
}

#ifdef USE_WORDEXP_H
/* when using wildcard, don't use realpath(), it won't work */
int do_delete_file(int connfd, const char *cwd, char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);

  int n;
  char buffer[PATH_MAX+128];
  int ret = -1;

  wordexp_t p;

  char **w;
  int i;

  char *ptr;
  ptr = path;
  /* if 'path' contains starting '/', then we can safely ignore cwd */
#ifdef __WIN32__
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') // relative path name
#else
  if (ptr[0] != DIR_DELIM)
#endif
  {
    if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
      strcpy(buffer, cwd);
      n = strlen(buffer);
      if (buffer[n-1] != DIR_DELIM) {
	buffer[n] = DIR_DELIM; // add delimeter
	n++;
	buffer[n] = 0;
      }
      strcat(buffer, ptr);
      ptr = buffer;
    }
  }
  /* ptr = path */

  wordexp(ptr, &p, 0);
  w = p.we_wordv;
  for (i = 0; i < p.we_wordc; i++) {
//    printf("Deleting %s ...", w[i]);
    if ((ret = remove(w[i])) != 0) {
      perror(w[i]);
      sprintf(buffer, "server: %s: %s\n", w[i], strerror(errno));
      n = strlen(buffer);
      ret = sendn(connfd, buffer, n);
      if (ret < n) {
	DEBUG("sendn", errno);
	goto cleanup;
      }
    }

  }

cleanup:
  wordfree(&p);

  return ret;
}
#endif

#ifdef USE_WORDEXP_H
/* you will find an example code segment in 'man 3 wordexp' manpage */
int do_list_file(int connfd, const char *cwd, char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);
  int ret = -1;

  wordexp_t p;
  char **w;
  int i, j, n, len;

  char buffer[PATH_MAX+128];

  struct stat sstat;
  struct tm stm;
  char timebuf[32];

  int have_slash = 0;

  char redir_file[PATH_MAX+1];

  char *ptr;

  redir_file[0] = buffer[0] = 0; /* initialize [no redirection file] */

  n = 0;
  ptr = path;
  /* if 'path' contains starting '/', then we can safely ignore cwd */
#ifdef __WIN32__
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') // relative path name
#else
  if (ptr[0] != DIR_DELIM)
#endif
  { // relative path name
    if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
      strcpy(buffer, cwd);
      n = strlen(buffer);
      if (buffer[n-1] != DIR_DELIM) {
	buffer[n] = DIR_DELIM; // add delimeter
	n++;
	buffer[n] = 0;
      }
    }
    /* handle redirection here */
    len = strlen(ptr);
    i = strcspn(ptr, ">");
    if (i < len) { /* '>' found */
      /* save and strip the redirection here */
      /* must have a space before and after '>' */
      if (cwd[0] != '-') {
	strcpy(redir_file, cwd);
	j = strlen(redir_file);
	if (redir_file[j-1] != DIR_DELIM) {
	  redir_file[j-1] = DIR_DELIM; // add delimeter
	  j++;
	  redir_file[j] = 0;
	}
      }
#ifdef __WIN32__
      if (*(ptr+i+2) != DIR_DELIM && *(ptr+i+3) != ':') // relative path name
#else
      if (*(ptr+i+2) != DIR_DELIM)
#endif
      {
	strcat(redir_file, ptr+i+2); /* +2 is to skip "> " */
      } else {
	strcpy(redir_file, ptr+i+2); /* +2 is to skip "> " */
      }
      //fprintf(stderr, "redir_file=<%s>\n", redir_file);
      /* now we can ignore characters from '>' in path */
      ptr[i-1] = 0; /* mark end of string one character before '>' */
    }
    if (n > 0) {
      strcat(buffer, ptr);
      ptr = buffer;
    }
  } else { // absolute path name
    /* handle redirection here */
    len = strlen(ptr);
    i = strcspn(ptr, ">");
    if (i < len) { /* '>' found */
      /* save and strip the redirection here */
      /* must have a space after '>' */
      if (cwd[0] != '-') {
	strcpy(redir_file, cwd);
	j = strlen(redir_file);
	if (redir_file[j-1] != DIR_DELIM) {
	  redir_file[j-1] = DIR_DELIM; // add delimeter
	  j++;
	  redir_file[j] = 0;
	}
      }
#ifdef __WIN32__
      if (*(ptr+i+2) != DIR_DELIM && *(ptr+i+3) != ':') // relative path name
#else
      if (*(ptr+i+2) != DIR_DELIM)
#endif
      {
	strcat(redir_file, ptr+i+2); /* +2 is to skip "> " */
      } else {
	strcpy(redir_file, ptr+i+2); /* +2 is to skip "> " */
      }
      //fprintf(stderr, "redir_file=<%s>\n", redir_file);
      /* now we can ignore characters from '>' in path */
      ptr[i-1] = 0; /* mark end of string one character before '>' */
    }
  }

  if (strrchr(ptr, DIR_DELIM) == NULL) {
    have_slash = 0;
  } else {
    have_slash = 1;
  }

  ret = wordexp(ptr, &p, 0);
  if (ret != 0) {
    switch (ret) {
      case WRDE_BADCHAR:
	fprintf(stderr, "Illegal character\n");
	break;
      case WRDE_NOSPACE:
	fprintf(stderr, "Out of memory\n");
	break;
      default:
	fprintf(stderr, "ret=%d\n", ret);
	break;
    }

    return -1;
  }

  memset(timebuf, 0, sizeof(timebuf));

  int fd = -1; // invalid file handle

  if (redir_file[0]) { // redirection needed
    //fprintf(stderr, "redir_file=<%s>\n", redir_file);
    fd = open(redir_file, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, 00644);
    if (fd == -1) {
      perror("open");
      sprintf(buffer, "server: %s: %s\n", redir_file, strerror(errno));
      n = strlen(buffer);
      ret = sendn(connfd, buffer, n);
      goto cleanup;
    }
    if ((ret = pthread_rwlock_wrlock(&rwlock)) != 0) {
      close(fd);
      wordfree(&p);
      fprintf(stderr, "pthread_rwlock_wrlock: %s\n", strerror(ret));
      return -1;
    }
  }

  char usr[40];
  char grp[40];

  struct passwd pd;
  struct passwd *pwdptr;
  struct passwd *tempPwdPtr;
  char pwdbuf[200]; // adjust size according to your need
  int pwdlinelen;
  struct group gr;
  struct group *grpptr;
  struct group *tempGrpPtr;
  char grpbuf[200]; // adjust size according to your need
  int grplinelen;

  memset(usr, 0, sizeof(usr));
  memset(grp, 0, sizeof(grp));

  /* matched strings are saved in p.we_wordv, p.we_wordc contains count */
  w = p.we_wordv;
  for (i = 0; i < p.we_wordc; i++) {
    if (stat(w[i], &sstat) != 0) {
      sprintf(buffer, "server: %s: %s\n", w[i], strerror(errno));
      n = strlen(buffer);
      ret = sendn(connfd, buffer, n);
      if (ret < n) {
	DEBUG("sendn", errno);
	goto cleanup;
      }
      perror(w[i]);
    } else {
      pwdptr = &pd;
      tempPwdPtr = NULL;
      memset(pwdbuf, 0, sizeof(pwdbuf));
      pwdlinelen = sizeof(pwdbuf);
      getpwuid_r(sstat.st_uid, pwdptr, pwdbuf, pwdlinelen, &tempPwdPtr);
      if (!tempPwdPtr) { // error/not found [not a fatal error]
	sprintf(usr, "%d", sstat.st_uid);
      } else {
	strncpy(usr, pd.pw_name, sizeof(usr)-1);
      }
      grpptr = &gr;
      tempGrpPtr = NULL;
      memset(grpbuf, 0, sizeof(grpbuf));
      grplinelen = sizeof(grpbuf);
      getgrgid_r(sstat.st_gid, grpptr, grpbuf, grplinelen, &tempGrpPtr);
      if (!tempGrpPtr) { /* no such group */
	sprintf(grp, "%d", sstat.st_gid);
      } else {
	strncpy(grp, gr.gr_name, sizeof(grp)-1);
      }

      localtime_r(&sstat.st_atime, &stm); // use access time
      //localtime_r(&sstat.st_mtime, &stm); // use modification time
      //strftime(timebuf, sizeof(timebuf)-1, "%a %b %d %T %Y", &stm);
      //
      // Feb 14 02:14
      strftime(timebuf, sizeof(timebuf)-1, "%b %d %R", &stm);
      /* %c is not working in my Windows, so using the above format */
//      strftime(timebuf, sizeof(timebuf)-1, "%c ", &stm);
      sprintf(buffer, "%c" // file type d or -
	  "%c%c%c" // owner permission
	  "%c%c%c" // group permission
	  "%c%c%c" // other permission
	  " %d" // number of hard links
	  //" %d %d" // uid gid
	  " %8s %8s" // user group
	  " %10d" // size
	  " %s" // modification time
	  " %s" // filename
	  "\n",
	  S_ISDIR(sstat.st_mode) ? 'd' : '-',
	  /* owner permission */
	  ((sstat.st_mode & S_IRWXU) & S_IRUSR) ? 'r' : '-',
	  ((sstat.st_mode & S_IRWXU) & S_IWUSR) ? 'w' : '-',
	  ((sstat.st_mode & S_IRWXU) & S_IXUSR) ? 'x' : '-',
	  /* group permission */
	  ((sstat.st_mode & S_IRWXG) & S_IRGRP) ? 'r' : '-',
	  ((sstat.st_mode & S_IRWXG) & S_IWGRP) ? 'w' : '-',
	  ((sstat.st_mode & S_IRWXG) & S_IXGRP) ? 'x' : '-',
	  /* others permission */
	  ((sstat.st_mode & S_IRWXO) & S_IROTH) ? 'r' : '-',
	  ((sstat.st_mode & S_IRWXO) & S_IWOTH) ? 'w' : '-',
	  ((sstat.st_mode & S_IRWXO) & S_IXOTH) ? 'x' : '-',
	  /* number of hard links */
	  sstat.st_nlink,
	  //sstat.st_uid, sstat.st_gid,
	  usr, grp,
	  (int) sstat.st_size,
	  timebuf,
	  have_slash ? strrchr(w[i], '/')+1 : w[i]
	  );
	  //"%10d\t%s\t%s\n", (int) sstat.st_size, timebuf,
	  //have_slash ? strrchr(w[i], '/')+1 : w[i]);
      /*
      sprintf(buffer, "%10d\t%s\t%s\n", (int) sstat.st_size, timebuf,
	  have_slash ? strrchr(w[i], '/')+1 : w[i]);
	  */
      n = strlen(buffer);
      if (fd != -1) {
	ret = write(fd, buffer, n);
	if (ret < n) {
	  perror("write");
	  sprintf(buffer, "server: write: %s\n", strerror(errno));
	  n = strlen(buffer);
	  ret = sendn(connfd, buffer, n);
	  goto cleanup;
	}
      } else {
	ret = sendn(connfd, buffer, n);
	if (ret < n) {
	  DEBUG("sendn", errno);
	  goto cleanup;
	}
      }
//      printf("%10d\t%s\t%s\n", (int) sstat.st_size, timebuf, w[i]);
    }
  }

cleanup:
  wordfree(&p);

  if (fd != -1) {
    pthread_rwlock_unlock(&rwlock);
    close(fd);
  }

  return ret;
}
#endif

/* reimplement using Win32 APIs */
#ifdef __WIN32__
int do_delete_file(int connfd, const char *cwd, char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);

  int n;
  char buffer[PATH_MAX+128];
  int ret = -1;

  HANDLE fh;
  WIN32_FIND_DATA w;

  char *ptr;
  ptr = path;
  /* if 'path' contains starting '\', then we can safely ignore cwd */
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') { // relative path [NO \ or :]
    if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
      strcpy(buffer, cwd);
      n = strlen(buffer);
      if (buffer[n-1] != DIR_DELIM) {
	buffer[n] = DIR_DELIM; // add delimeter
	n++;
	buffer[n] = 0;
      }
      strcat(buffer, ptr);
      ptr = buffer;
    }
  }
  /* ptr = path */

  fprintf(stderr, "<%s:%d> ptr=%s\n", __FILE__, __LINE__, ptr);

  char fullpath[PATH_MAX+1];
  const char *ptr1, *ptr2;
  int len = 0;
  if ((ptr2 = strrchr(ptr, DIR_DELIM)) != NULL) {
    ptr1 = ptr;
    len = ptr2 - ptr1 + 1; // include the '\' in the copy
    strncpy(fullpath, ptr, len);
  } else {
    len = 0;
    fullpath[0] = 0; // NULL string
  }

  if ((fh = FindFirstFile(ptr, &w)) == INVALID_HANDLE_VALUE) {
    sprintf(buffer, "server: %s: %s\n", path, "not found");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    if (ret < n) {
      DEBUG("sendn", errno);
    }
    return -1;
  } else {
    do {
      //    printf("Deleting %s ...", w[i]);
      strcpy(fullpath+len, w.cFileName);
      if ((ret = remove(fullpath)) != 0) {
	perror(fullpath); // debugging only
	sprintf(buffer, "server: %s: %s\n",
	    fullpath, strerror(errno));
	n = strlen(buffer);
	ret = sendn(connfd, buffer, n);
	if (ret < n) {
	  DEBUG("sendn", errno);
	  break;
	}
      }
    } while (FindNextFile(fh, &w) == TRUE);
  }

  FindClose(fh);

  return ret;
}
#endif

#ifdef __WIN32__
/* you will find an example code segment in 'man 3 wordexp' manpage */
int do_list_file(int connfd, const char *cwd, char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);
  int ret = -1;

  HANDLE fh;
  WIN32_FIND_DATA w;

  int n;

  char buffer[PATH_MAX+128];

  struct stat sstat;
  struct tm stm;
  char timebuf[32];
  //size_t total = 0;

  /* if you need additional info, you may use GetFileInformationByHandle() */
  //BY_HANDLE_FILE_INFORMATION FileInfo;

  memset(timebuf, 0, sizeof(timebuf));

  char redir_file[PATH_MAX+1];

  char *ptr = NULL;
  int len = 0;
  int i = 0;
  int j = 0;

  redir_file[0] = buffer[0] = 0; /* initialize [no redirection file] */

  n = 0;
  ptr = path;
  /* if 'path' contains starting '\', then we can safely ignore cwd */
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') { // relative path name
    if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
      strcpy(buffer, cwd);
      n = strlen(buffer);
      if (buffer[n-1] != DIR_DELIM) {
	buffer[n] = DIR_DELIM; // add delimeter
	n++;
	buffer[n] = 0;
      }
    }
    /* handle redirection here */
    len = strlen(ptr);
    i = strcspn(ptr, ">");
    if (i < len) { /* '>' found */
      /* save and strip the redirection here */
      /* must have a space before and after '>' */
      if (cwd[0] != '-') {
	strcpy(redir_file, cwd);
	j = strlen(redir_file);
	if (redir_file[j-1] != DIR_DELIM) {
	  redir_file[j-1] = DIR_DELIM; // add delimeter
	  j++;
	  redir_file[j] = 0;
	}
      }
#ifdef __WIN32__
      if (*(ptr+i+2) != DIR_DELIM && *(ptr+i+3) != ':') // relative path name
#else
      if (*(ptr+i+2) != DIR_DELIM)
#endif
      {
	strcat(redir_file, ptr+i+2); /* +2 is to skip "> " */
      } else {
	strcpy(redir_file, ptr+i+2); /* +2 is to skip "> " */
      }
      //fprintf(stderr, "redir_file=<%s>\n", redir_file);
      /* now we can ignore characters from '>' in path */
      ptr[i-1] = 0; /* mark end of string one character before '>' */
    }
    if (n > 0) {
      strcat(buffer, ptr);
      ptr = buffer;
    }
  } else { // absolute path name
    /* handle redirection here */
    len = strlen(ptr);
    i = strcspn(ptr, ">");
    if (i < len) { /* '>' found */
      /* save and strip the redirection here */
      /* must have a space after '>' */
      if (cwd[0] != '-') {
	strcpy(redir_file, cwd);
	j = strlen(redir_file);
	if (redir_file[j-1] != DIR_DELIM) {
	  redir_file[j-1] = DIR_DELIM; // add delimeter
	  j++;
	  redir_file[j] = 0;
	}
      }
#ifdef __WIN32__
      if (*(ptr+i+2) != DIR_DELIM && *(ptr+i+3) != ':') // relative path name
#else
      if (*(ptr+i+2) != DIR_DELIM)
#endif
      {
	strcat(redir_file, ptr+i+2); /* +2 is to skip "> " */
      } else {
	strcpy(redir_file, ptr+i+2); /* +2 is to skip "> " */
      }
      //fprintf(stderr, "redir_file=<%s>\n", redir_file);
      /* now we can ignore characters from '>' in path */
      ptr[i-1] = 0; /* mark end of string one character before '>' */
    }
  }

  char fullpath[PATH_MAX+1];
  const char *ptr1, *ptr2;
  if ((ptr2 = strrchr(ptr, DIR_DELIM)) != NULL) {
    ptr1 = ptr;
    len = ptr2 - ptr1 + 1; // include the '\' in the copy
    strncpy(fullpath, ptr, len);
  } else {
    len = 0;
    fullpath[0] = 0; // NULL string
  }

  int fd = -1; // invalid file handle

  if ((fh = FindFirstFile(ptr, &w)) == INVALID_HANDLE_VALUE) {
    sprintf(buffer, "server: %s: %s\n", path, "not found");
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    if (ret < n) {
      DEBUG("sendn", errno);
    }
    return -1;
  } else {
    if (redir_file[0]) { // redirection needed
      //fprintf(stderr, "redir_file=<%s>\n", redir_file);
      fd = open(redir_file, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, 00644);
      if (fd == -1) {
	perror("open");
	sprintf(buffer, "server: %s: %s\n", redir_file, strerror(errno));
	n = strlen(buffer);
	sendn(connfd, buffer, n);
	FindClose(fh);
	return -1;
      }
      if ((ret = pthread_rwlock_wrlock(&rwlock)) != 0) {
	close(fd);
	FindClose(fh);
	fprintf(stderr, "pthread_rwlock_wrlock: %s\n", strerror(ret));
	return -1;
      }
    }
    do {
      strcpy(fullpath+len, w.cFileName);
      if (stat(fullpath, &sstat) != 0) {
	sprintf(buffer, "server: %s: %s\n", fullpath, strerror(errno));
	n = strlen(buffer);
	ret = sendn(connfd, buffer, n);
	if (ret < n) {
	  DEBUG("sendn", errno);
	  break;
	}
	sprintf(buffer, "server: stat: %s: %s", fullpath, strerror(errno));
	//perror(w.cFileName);
      } else {
	//total += (int) sstat.st_size;
	/* size last_access filename */
	(void) localtime_r(&sstat.st_atime, &stm);
	strftime(timebuf, sizeof(timebuf)-1, "%m/%d/%Y  %H:%M %p", &stm);
	sprintf(buffer, "%s    %5s%10d %-s\n", timebuf,
	    S_ISDIR(sstat.st_mode) ? "<DIR>" : " ",
	    (int) sstat.st_size, w.cFileName);
	n = strlen(buffer);
	if (fd != -1) {
	  ret = write(fd, buffer, n);
	  if (ret < n) {
	    perror("write");
	    sprintf(buffer, "server: write: %s\n", strerror(errno));
	    n = strlen(buffer);
	    ret = sendn(connfd, buffer, n);
	    goto cleanup;
	  }
	} else {
	  ret = sendn(connfd, buffer, n);
	  if (ret < n) {
	    DEBUG("sendn", errno);
	    break;
	  }
	}
      }
    } while (FindNextFile(fh, &w) == TRUE);
  }

cleanup:
  FindClose(fh);

  if (fd != -1) {
    pthread_rwlock_unlock(&rwlock);
    close(fd);
  }

  return ret;
}
#endif /* __WIN32__ */

/* create folder locally */
int do_create_folder(int connfd, const char *cwd, char *path)
{
  //  fprintf(stderr, "%s entered\n", __func__);

  int n;
  char buffer[128];
  int ret = -1;
//  mode_t mode = 00755;

  char *ptr1, *ptr;
  ptr = path;
  /* if 'path' contains starting '/', then we can safely ignore cwd */
#ifdef __WIN32__
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') // relative path name
#else
  if (ptr[0] != DIR_DELIM)
#endif
  { // relative path name
    if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
      strcpy(buffer, cwd);
      n = strlen(buffer);
      if (buffer[n-1] != DIR_DELIM) {
	buffer[n] = DIR_DELIM; // add delimeter
	n++;
	buffer[n] = 0;
      }
      strcat(buffer, ptr);
      ptr = buffer;
      ptr1 = realpath(ptr, path);
      if (!ptr1) {
	perror("realpath");
	sprintf(buffer, "400 ERROR server: %s\n", strerror(errno));
	sendn(connfd, buffer, strlen(buffer));
	return 1;
      } else {
	ptr = ptr1;
      }
    }
  }
  /* ptr = path */

  ret = MKDIR(ptr, 00755);
  if (ret != 0) {
    sprintf(buffer, "server: %s: %s\n", path, strerror(errno));
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    if (ret < n)
      DEBUG("sendn", errno);
  }

  return ret;
}

/* delete folder locally */
int do_delete_folder(int connfd, const char *cwd, char *path)
{
  //  fprintf(stderr, "%s entered\n", __func__);

  int n;
  char buffer[128];
  int ret = -1;

  char *ptr1, *ptr;
  ptr = path;
  /* if 'path' contains starting '\', then we can safely ignore cwd */
#ifdef __WIN32__
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') // relative path name
#else
  if (ptr[0] != DIR_DELIM)
#endif
  {
    if (cwd[0] != '-') { // if invalid cwd, use current directory [path]
      strcpy(buffer, cwd);
      n = strlen(buffer);
      if (buffer[n-1] != DIR_DELIM) {
	buffer[n] = DIR_DELIM; // add delimeter
	n++;
	buffer[n] = 0;
      }
      strcat(buffer, ptr);
      ptr = buffer;
      ptr1 = realpath(ptr, path);
      if (!ptr1) {
	perror("realpath");
	sprintf(buffer, "400 ERROR server: %s\n", strerror(errno));
	sendn(connfd, buffer, strlen(buffer));
	return 1;
      } else {
	ptr = ptr1;
      }
    }
  }
  /* ptr = path */

  ret = rmdir(path);
  if (ret != 0) {
    sprintf(buffer, "server: %s: %s\n", path, strerror(errno));
    n = strlen(buffer);
    ret = sendn(connfd, buffer, n);
    if (ret < n)
      DEBUG("sendn", errno);
  }

  return ret;
}

/* chdir() call is not thread-safe. To get around this problem, clients will
 * tell their directory they know the server will use as current directory
 * for that client.
 */
/* NOTE:
 * For a multi-threaded server it is never safe to call chdir() system call.
 * kernel maintains current directory per process basis. so if a thread in
 * a process calls chdir(), the current directory will be changed for all of
 * the threads of this process.
 */

/* client will check whether returned string contains a starting '-', if found
 * then error, otherwise client will assume server current directory is the
 * returned string
 */
/*
 * server is actually not changing its working directory, it is just getting
 * the path where it would go if it would change the directory. then sending
 * the path to the client. client memorizes it for future 'pwd' calls
 */
int do_change_folder(int connfd, const char *cwd, char *path)
{
  //  fprintf(stderr, "%s entered\n", __func__);
  //pid_t pid;
  int n;
  char buffer[PATH_MAX+128];
  //int status;

  const char *p, *ptr;

  p = ptr = NULL;

  ptr = path;
  /* if 'path' contains starting '/', then we can safely ignore cwd */
#ifdef __WIN32__
  if (ptr[0] != DIR_DELIM && ptr[1] != ':') // relative path name
#else
  if (ptr[0] != DIR_DELIM)
#endif
  { // relative path name
    if (cwd[0] == '-') { // invalid cwd, use current directory
      if (!getcwd(buffer, sizeof(buffer))) {
	sprintf(buffer, "- server: getcwd: %s\n", strerror(errno));
	n = strlen(buffer);
	sendn(connfd, buffer, n);
	return -1;
      }
    } else {
      strcpy(buffer, cwd);
    }
    n = strlen(buffer);
    if (buffer[n-1] != DIR_DELIM) {
      buffer[n] = DIR_DELIM; // add delimeter
      n++;
      buffer[n] = 0;
    }
    strcat(buffer, ptr);
  } else {
    strcpy(buffer, path);
  }
  ptr = buffer;
  p = realpath(ptr, path);
  if (!p) {
    /* error */
    sprintf(buffer, "- server: %s: %s\n", path, strerror(errno));
  } else {
    struct stat sbuf;
    if (stat(p, &sbuf) == 0 && S_ISDIR(sbuf.st_mode)) {
      sprintf(buffer, "%s\n", p);
    } else {
      sprintf(buffer, "- server: %s: No such directory\n", path);
    }
  }
  n = strlen(buffer);
  sendn(connfd, buffer, n);

  return 0;
}

/* gets current working directory */
/* on error, buffer will contain starting '-', clients should check for this
 */
int do_getcwd(int connfd)
{
  //  fprintf(stderr, "%s entered\n", __func__);

  int n;
  char buffer[PATH_MAX+128];
  char pwd[PATH_MAX+1];
  int ret = -1;

  pwd[PATH_MAX] = 0;
  if (getcwd(pwd, sizeof(pwd)) != NULL) {
    sprintf(buffer, "%s\n", pwd);
  } else {
    sprintf(buffer, "- server: getcwd: %s\n", strerror(errno));
  }
  n = strlen(buffer);
  ret = sendn(connfd, buffer, n);
  if (ret < n)
    DEBUG("sendn", errno);

  return ret;
}
