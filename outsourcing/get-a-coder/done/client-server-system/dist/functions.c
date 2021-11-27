#include "functions.h"

#define _MODIFIED_CODE_

#ifdef __WIN32__
DWORD WINAPI read_func(LPVOID lpData);
#endif

static char cmdbuf[4096];
static int cmdbufsize = sizeof(cmdbuf);
static char *cwd_buf;
static char invalid_dir[] = "-";
static uint32_t current_host;

/* shell is single process program, mutex won't work between shells */
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int connectToServer (const char *addr)
{
   int sockfd = -1;
   struct hostent *hinfo;
   struct sockaddr_in servaddr;

   current_host = 0;
   cwd_buf = invalid_dir;

   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = PF_INET;
   servaddr.sin_port = htons(SERV_PORT);

//  fprintf(stderr, "%s: dstaddr = %s\n", __func__, dstaddr);
   //fprintf(stderr, "Looking up %s\n", addr);
   hinfo = gethostbyname(addr);
   if (hinfo == NULL) 
   {
      switch (h_errno) 
      {
         case HOST_NOT_FOUND:
	     fprintf(stderr, "Host %s not found\n", addr);
	     break;
         case NO_ADDRESS:
	     fprintf(stderr, "Host %s has no IP address\n", addr);
	     break;
         case NO_RECOVERY:
            fprintf(stderr, "Host %s lookup failed, fatal error\n", addr);
            break;
         case TRY_AGAIN:
            fprintf(stderr, "Temporarily hostname lookup failure\n");
            break;
         default:
	     fprintf(stderr, "%s lookup failed, unknown error [%d]\n", addr,
	     h_errno);
	     break;
      }
      return -1;
   }   

//  servaddr.sin_addr = hinfo->h_addr_list[0];
   memcpy(&servaddr.sin_addr, hinfo->h_addr_list[0], sizeof(struct in_addr));
   current_host = servaddr.sin_addr.s_addr;

   if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
   {
      DEBUG("socket", errno);
      return -3;
   }

   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
   {
      DEBUG("connect", errno);
      CLOSE(sockfd);
      return -4;
   }
   cwd_buf = cwdent_getcwd(current_host);
   if (cwd_buf == NULL) {
     cwd_buf = cwdent_addcwd(current_host, "-"); // initially
   }
   return sockfd;
}


/* -----------------------------------------------------------------------
 * UPLOAD_FILE
 * -----------------------------------------------------------------------
 */

int upload_file(const char *src, const char *dstaddr, const char *dst, int del)
{
  //  fprintf(stderr, "%s entering\n", __func__);

  int len;

  int fd = -1;
  int sockfd = -1;
  int ret = -1;
  //  char recvline[MAXLINE+1];

  struct stat statbuf;

  if (access(src, R_OK)) 
  {
    fprintf(stderr, "ERROR: %s is not accessible\n", src);
    return -1;
  }

  int op = 0;
  if (del == 0) {
    op = LOCK_SH;
  } else {
    op = LOCK_EX;
  }
  fd = open_and_lock(src, O_RDONLY|O_BINARY, 00644, op);
  if (fd == -1) { // error
    fprintf(stderr, "open_and_lock: %s\n", strerror(errno));
    ret = -1;
    goto cleanup;
  }

  if (stat(src, &statbuf) != 0) 
  {
    fprintf(stderr, "File is removed before the processing of your request\n");
    ret = -1;
    goto cleanup;
  }

  sockfd =  connectToServer (dstaddr);
  if (sockfd < 0) {
    return -1;
  }

  /* read protocol.txt for knowing the protocol used here */
  sprintf(cmdbuf, "FUNC UP COPY %s%c%s\n", cwd_buf, DELIM_CHAR, dst);
  len = strlen(cmdbuf);
  if (writen(sockfd, cmdbuf, len) <= 0) 
  {
    fprintf(stderr, "Error writing data\n");
    ret = -5;
    goto cleanup;
  }

  if (readline(sockfd, cmdbuf, MAXLINE) <= 0) 
  {
    fprintf(stderr, "Error reading data\n");
    ret = -6;
    goto cleanup;
  }

  //  fprintf(stderr, "%s", cmdbuf); /* debug */
  if (cmdbuf[0] != '5') { /* 5NN means success */
    fprintf(stderr, "%s", cmdbuf+4);
    ret = -1;
    goto cleanup;
  }

  /* make the socket nonblocking */
  //fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);

  //  fprintf(stderr, "Here I am : %s:%d\n", __FILE__, __LINE__);
  while ((len = read(fd, cmdbuf, cmdbufsize)) > 0) 
  {
    if (writen(sockfd, cmdbuf, len) < len) 
    {
      fprintf(stderr, "Error writing data\n");
      ret = -8;
      goto cleanup;
    }

    /* if you make the socket nonblocking, u may uncomment the following */
    /*
       if ((len = recv(sockfd, cmdbuf, cmdbufsize, 0)) > 0) {
       cmdbuf[len] = 0;
       fprintf(stderr, "%s", cmdbuf);
       }
       */
  }

  if (del == 1) 
  {
    if (fd > 0) {
      unlock(fd);
      close(fd);
      fd = -1;
    }
    if (remove(src) != 0) 
    {
      perror("remove:src");
      goto cleanup;
    }
  }
  //  fprintf(stderr, "%s leaving\n", __func__);
cleanup: 

  if (fd > 0) {
    unlock(fd);
    close(fd);
  }

  if (sockfd > 0)
    CLOSE(sockfd);

  return ret;
}

/* -----------------------------------------------------------------------
 * DOWNLOAD_FILE
 * -----------------------------------------------------------------------
 */

int download_file(const char *srcaddr, const char *src, const char *dst, int del)
{
  //  fprintf(stderr, "%s entering\n", __func__);

  mode_t mode = 00644;
  int len = -1;

  int fd = -1;
  int sockfd = -1;
  int ret = -1;
  //  char recvline[MAXLINE+1];

  size_t size = -1;
  int errcode = -1;
  char status[10]; /* OK or ERROR, etc */

  //pthread_mutex_lock(&mutex);

  if (access(dst, F_OK) == 0) 
  { // file exists
    fprintf(stderr, "Output file exists!\n");
    //pthread_mutex_unlock(&mutex);
    return 0;
  }

  fd = open_and_lock(dst, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, mode, LOCK_EX);
  if (fd == -1) {
    perror("open");
    return -7;
  }

  sockfd =  connectToServer (srcaddr);
  if (sockfd < 0) {
    goto cleanup;
  }

  sprintf(cmdbuf, "FUNC DOWN COPY %s%c%s%c%d\n",
      cwd_buf, DELIM_CHAR, src, DELIM_CHAR, del);
  len = strlen(cmdbuf);
  if (writen(sockfd, cmdbuf, len) <= 0) 
  {
    fprintf(stderr, "Error writing data\n");
    ret = -5;
    goto cleanup;
  }

  if (readline(sockfd, cmdbuf, MAXLINE) <= 0) 
  {
    fprintf(stderr, "Error reading data\n");
    ret = -6;
    goto cleanup;
  }

  //  fprintf(stderr, "%s", cmdbuf); /* debugging */
  if (cmdbuf[0] == '5')   /* success [5NN] */
  { 
    sscanf(cmdbuf, "%d%s%d", &errcode, status, &size);
  } 
  else 
  {
    fprintf(stderr, "ERROR: %s", cmdbuf);
    remove(dst);
    ret = -1;
    goto cleanup;
  }

  //  fprintf(stderr, "Here I am : %s:%d\n", __FILE__, __LINE__);
  int nwrite = 0;
  int done = 0;
  while ((len = recv(sockfd, cmdbuf, cmdbufsize, 0)) > 0) 
  {
    if (nwrite+len > size) 
    {
      if (!done) 
      {
	close(fd);
	fd = STDERR_FILENO;
	done = 1;
      }
    }
    if (write(fd, cmdbuf, len) < len)    /* if more time, be more correct */
    {
      fprintf(stderr, "Error writing data to file\n");
      ret = -8;
      goto cleanup;
    }
    nwrite += len;
  }
cleanup:


  if (fd > 0) {
    unlock(fd);
    close(fd);
  }

  if (sockfd > 0)
    CLOSE(sockfd);

  return ret;
}

/* -----------------------------------------------------------------------
 * MOVE_FILE
 * -----------------------------------------------------------------------
 */

int move_file(const char *srcaddr, const char *src, const char *dst, int del)
{
//  fprintf(stderr, "%s entering\n", __func__);

   int len;
   
   int sockfd;
   //  char recvline[MAXLINE+1];
   
   sockfd =  connectToServer (srcaddr);
   if (sockfd < 0) {
     return -1;
   }

   if (del) 
   {
     sprintf(cmdbuf, "FUNC MOVE MOVE %s%c%s%c%s\n",
	 cwd_buf, DELIM_CHAR, src, DELIM_CHAR, dst);
   } 
   else 
   {
     sprintf(cmdbuf, "FUNC MOVE COPY %s%c%s%c%s\n",
	 cwd_buf, DELIM_CHAR, src, DELIM_CHAR, dst);
   }
   len = strlen(cmdbuf);
   if (writen(sockfd, cmdbuf, len) <= 0) 
   {
      fprintf(stderr, "Error writing data\n");
      CLOSE(sockfd);
      return -5;
   }
   
   if (readline(sockfd, cmdbuf, MAXLINE) <= 0) 
   {
      fprintf(stderr, "Error reading data\n");
      CLOSE(sockfd);
      return -6;
   }
   
   //  fprintf(stderr, "%s", cmdbuf); /* debug */
   if (cmdbuf[0] != '5')         /* 5NN means success */
   { 
      fprintf(stderr, "%s", cmdbuf+4);
      CLOSE(sockfd);
      return -1;
   }
   
   while ((len = readline(sockfd, cmdbuf, MAXLINE)) > 0) 
   {
      fprintf(stderr, "%s", cmdbuf);
   }
   
   //  fprintf(stderr, "%s leaving\n", __func__);
   
   CLOSE(sockfd);
   
   return 0;
} 


/* -----------------------------------------------------------------------
 * COPY_FILE for local copy
 * -----------------------------------------------------------------------
 */
int copy_file(const char *src, const char *dst, int del)
{
  //  fprintf(stderr, "%s entered\n", __func__);

  int fd1 = -1;
  int fd2 = -1;
  int ret = -1;
  ssize_t nread = -1;
  char buf[1024] = {};

  struct stat statbuf;

  if (access(src, F_OK) != 0) 
  { // file  doesn't exists
    fprintf(stderr, "Input file doesn't exist!\n");
    return 0;
  }

  if (access(dst, F_OK) == 0) 
  { // file exists
    fprintf(stderr, "Output file exists!\n");
    //pthread_mutex_unlock(&mutex);
    return 0;
  }

  if (del == 1) {
    /* just let's have a try to move in one call */
    ret = rename(src, dst);
    if (ret == 0) { // yesss!!!
      goto cleanup;
    } else {
      perror("rename");
    }
  }

  int op = 0;
  if (del == 0)
    op = LOCK_SH;
  else
    op = LOCK_EX;

  if ((fd1 = open_and_lock(src, O_RDONLY|O_BINARY, 00644, op)) == -1) {
    perror("open:src");
    ret = errno;
    return ret;
  }

  if (stat(src, &statbuf) != 0) 
  {
    fprintf(stderr, "File is removed before the processing of your request\n");
    ret = -1;
    goto cleanup;
  }

  //pthread_mutex_lock(&mutex);

  if ((fd2 = open_and_lock(dst, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,
	  00644, LOCK_EX)) == -1) {
    perror("open:dst");
    ret = errno;
    goto cleanup;
  }

  while ((nread = read(fd1, buf, sizeof(buf))) > 0) {
    ret = write(fd2, buf, nread);
    if (ret < nread) {
      perror("write");
      ret = -3;
      goto cleanup;
    }
  }
  ret = 0;

  if (del == 1) 
  {
    if (fd1 > 0) {
      unlock(fd1);
      close(fd1);
      fd1 = -1;
    }
    if (remove(src) != 0) 
    {
      perror("remove:src");
    }
  }

cleanup:

  if (fd1 > 0) {
    unlock(fd1);
    close(fd1);
  }
  if (fd2 > 0) {
    unlock(fd2);
    close(fd2);
  }

  return ret;
}

/* -----------------------------------------------------------------------
 * DELETE_FILE
 * -----------------------------------------------------------------------
 */

int delete_file(const char *dstaddr, const char *path)
{
//  fprintf(stderr, "%s entering\n", __func__);
   
   int len = -1;
   
   int sockfd = -1;
   //  char recvline[MAXLINE+1];
   
   sockfd =  connectToServer (dstaddr);
   if (sockfd < 0) {
     return -1;
   }
   
   sprintf(cmdbuf, "FUNC NONE DEL %s%c%s\n", cwd_buf, DELIM_CHAR, path);
   
   len = strlen(cmdbuf);
   if (writen(sockfd, cmdbuf, len) <= 0) 
   {
      fprintf(stderr, "Error writing data\n");
      CLOSE(sockfd);
      return -5;
   }
   
   if (readline(sockfd, cmdbuf, MAXLINE) <= 0) {
      fprintf(stderr, "Error reading data\n");
      CLOSE(sockfd);
      return -6;
   }
   
   //  fprintf(stderr, "%s", cmdbuf); /* debug */
   if (cmdbuf[0] != '5') { /* 5NN means success */
      fprintf(stderr, "%s", cmdbuf);
      CLOSE(sockfd);
      return -1;
   }
   
   while ((len = readline(sockfd, cmdbuf, MAXLINE)) > 0) {
      fprintf(stderr, "%s", cmdbuf);
   }
   
   //  fprintf(stderr, "%s leaving\n", __func__);
   
   CLOSE(sockfd);
   
   return 0;
}

/* -----------------------------------------------------------------------
 * DELETE_LOCAL_FILE
 * -----------------------------------------------------------------------
 */

#ifdef USE_WORDEXP_H
int delete_local_file(const char *path)
{
  //  fprintf(stderr, "%s entered\n", __func__);
  int ret = -1;
  int fd = -1;

  wordexp_t p;
  char **w;
  int i = -1;

  struct stat statbuf;

  //fprintf(stderr, "path = <%s>\n", path);

  ret = wordexp(path, &p, 0);
  if (ret != 0) 
  {
    perror("wordexp");
    return -1;
  }

  w = p.we_wordv;
  for (i = 0; i < p.we_wordc; i++) 
  {
    //    fprintf(stderr, "Deleting %s ...", w[i]);
    //    fflush(stderr);

    if ((fd = open(w[i], O_RDONLY|O_BINARY, 00644, LOCK_EX)) == -1) {
      perror("open:src");
      ret = errno;
      return ret;
    }

    if (stat(w[i], &statbuf) != 0) 
    {
      fprintf(stderr, "File is removed before the processing of your request\n");
      flock(fd, LOCK_UN);  // unlock the file
      close(fd);
      return -1;
    }

    unlock(fd);
    close(fd);

    if ((ret = remove(w[i])) != 0) 
    {
      perror(w[i]);
    }
  }
  wordfree(&p);

  return ret;
}
#endif

#ifdef __WIN32__
int delete_local_file(const char *path)
{
  //  fprintf(stderr, "%s entered\n", __func__);
  int ret = -1;


  //  printf("path = %s\n", path);

  HANDLE fh;
  WIN32_FIND_DATA w;

  char fullpath[256];
  const char *ptr1, *ptr2;
  int len = 0;
  if ((ptr2 = strrchr(path, '\\')) != NULL) {
    ptr1 = path;
    len = ptr2 - ptr1 + 1; // include the '\' in the copy
    strncpy(fullpath, path, len);
  } else {
    len = 0;
    fullpath[0] = 0; // NULL string
  }

  if ((fh = FindFirstFile(path, &w)) == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Error finding file\n");
    return -1;
  } else {
    do {
      strcpy(fullpath+len, w.cFileName);
      if ((ret = remove(fullpath)) != 0) {
	perror(fullpath);
      } else {
	printf("%s deleted\n", fullpath);
      }
    } while (FindNextFile(fh, &w) == TRUE);
  }

  FindClose(fh);

  return ret;
}
#endif


/* -----------------------------------------------------------------------
 * LIST_FILE
 * -----------------------------------------------------------------------
 */

int list_file(const char *dstaddr, const char *path)
{
//  fprintf(stderr, "%s entering\n", __func__);

   int len = -1;
   
   int sockfd = -1;

   int fd_out = -1;
   //  char recvline[MAXLINE+1];
   
   sockfd =  connectToServer (dstaddr);
   if (sockfd < 0) {
     return -1;
   }
   
   sprintf(cmdbuf, "FUNC NONE LIST %s%c%s\n", cwd_buf, DELIM_CHAR, path);
   
   len = strlen(cmdbuf);
   if (writen(sockfd, cmdbuf, len) <= 0) 
   {
      fprintf(stderr, "Error writing data\n");
      CLOSE(sockfd);
      return -5;
   }
   
   if (readline(sockfd, cmdbuf, MAXLINE) <= 0) 
   {
      fprintf(stderr, "Error reading data\n");
      CLOSE(sockfd);
      return -6;
   }
   
   //  fprintf(stderr, "%s", cmdbuf); /* debug */
   if (cmdbuf[0] != '5') 
   { /* 5NN means success */
      fprintf(stderr, "%s", cmdbuf);
      CLOSE(sockfd);
      return -1;
   }
   
   if (redir_out[0]) 
   {
      fd_out = open_and_lock(redir_out, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,
	  00644, LOCK_EX);
      if (fd_out == -1) 
      {
         perror(redir_out);
         fd_out = STDOUT_FILENO;
      }
   } 
   else 
   {
      fd_out = STDOUT_FILENO;
   }
   
   while ((len = readline(sockfd, cmdbuf, MAXLINE)) > 0) 
   {
      write(fd_out, cmdbuf, len);
   }
   
   //  fprintf(stderr, "%s leaving\n", __func__);
   
   CLOSE(sockfd);
   
   if (fd_out != STDOUT_FILENO) 
   {
     unlock(fd_out);
     close(fd_out);
   }
   
   return 0;
}

/* -----------------------------------------------------------------------
 * LIST_LOCAL_FILE
 * -----------------------------------------------------------------------
 */
#ifdef USE_WORDEXP_H
int list_local_file(const char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);
  int ret = -1;

  wordexp_t p;
  char **w;
  int i;

  struct stat sstat;
  struct tm stm;
  char timebuf[32];

  struct passwd *pw;
  struct group *gr;
  char usr[40];
  char grp[40];

  int have_slash = 0;

  if (strrchr(path, '/') == NULL) {
    have_slash = 0;
  } else {
    have_slash = 1;
  }

  ret = wordexp(path, &p, 0);
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
  memset(usr, 0, sizeof(usr));
  memset(grp, 0, sizeof(grp));

  FILE *fp_out;

  int fd_out = STDOUT_FILENO;

  if (redir_out[0]) {
    fp_out = fopen(redir_out, "w");
    if (fp_out == NULL) {
      perror(redir_out);
      wordfree(&p);
      return -1;
    } else {
      fd_out = fileno(fp_out); /* get associated file handle */
      if (flock(fd_out, LOCK_EX) == -1) { // exclusive [write] lock
	fprintf(stderr, "ERROR flock: %s\n", strerror(errno));
	close(fd_out);
	return -1;
      }
    }
  } else {
    fp_out = stdout;
  }

  w = p.we_wordv;
  for (i = 0; i < p.we_wordc; i++) {
    if (stat(w[i], &sstat) != 0) {
      perror(w[i]);
    } else {
      pw = getpwuid(sstat.st_uid);
      if (!pw) { /* no such user */
	sprintf(usr, "%d", sstat.st_uid);
      } else {
	strncpy(usr, pw->pw_name, sizeof(usr)-1);
      }
      gr = getgrgid(sstat.st_gid);
      if (!gr) { /* no such group */
	sprintf(grp, "%d", sstat.st_gid);
      } else {
	strncpy(grp, gr->gr_name, sizeof(grp)-1);
      }
      /* size last_access filename */
      localtime_r(&sstat.st_atime, &stm);
      //localtime_r(&sstat.st_mtime, &stm); // modification time
      //strftime(timebuf, sizeof(timebuf)-1, "%c ", &stm);
      // Feb 14 02:14
      strftime(timebuf, sizeof(timebuf)-1, "%b %d %R", &stm);
      /* %c is not working in my Windows, so using the above format */
//      strftime(timebuf, sizeof(timebuf)-1, "%c ", &stm);
      fprintf(fp_out, "%c" // file type d or -
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
      //printf("%10d\t%s\t%s\n", (int) sstat.st_size, timebuf,
	 //have_slash ? strrchr(w[i], '/')+1 : w[i]);
    }
  }
  wordfree(&p);

  if (fp_out != stdout) {
    if (fd_out != STDOUT_FILENO) {
      flock(fd_out, LOCK_UN); // unlock the file
    }
    fclose(fp_out);
  }

  return ret;
}
#endif

#ifdef __WIN32__
int list_local_file(const char *path)
{
  //  fprintf(stderr, "%s entered\n", __func__);
  int ret = -1;

  struct stat sstat;
  struct tm *stm = NULL;
  char timebuf[32];


  FILE *fp_out = stdout;
  int fd_out = STDOUT_FILENO;

  if (redir_out[0]) {
    fp_out = fopen(redir_out, "w");
    if (fp_out == NULL) {
      perror(redir_out);
      return -1;
    } else {
      fd_out = fileno(fp_out); /* get associated file handle */
      if (flock(fd_out, LOCK_EX) == -1) { // exclusive [write] lock
	fprintf(stderr, "ERROR flock: %s\n", strerror(errno));
	close(fd_out);
	return -1;
      }
    }
  } else {
    fp_out = stdout;
  }

  HANDLE fh;
  WIN32_FIND_DATA w;


  char fullpath[256];
  const char *ptr1, *ptr2;
  int len = 0;
  if ((ptr2 = strrchr(path, DIR_DELIM)) != NULL) {
    ptr1 = path;
    len = ptr2 - ptr1 + 1; // include the '\' in the copy
    strncpy(fullpath, path, len);
  } else {
    len = 0;
    fullpath[0] = 0; // NULL string
  }

  memset(timebuf, 0, sizeof(timebuf));

  if ((fh = FindFirstFile(path, &w)) == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "%s not found!\n", path);
    return -1;
  } else {
    do {
      strcpy(fullpath+len, w.cFileName);
      //fprintf(stderr, "fullpath: <%s>\n", fullpath);
      if (stat(fullpath, &sstat) != 0) {
	fprintf(stderr, "stat: %s: %s\n", fullpath, strerror(errno));
//	perror(w.cFileName);
      } else {
	/* size last_access filename */
	stm = localtime(&sstat.st_atime);
	strftime(timebuf, sizeof(timebuf)-1, "%m/%d/%Y  %H:%M %p", stm);
	fprintf(fp_out, "%s    %5s%10d %-s\n", timebuf,
	    S_ISDIR(sstat.st_mode) ? "<DIR>" : " ",
	    (int) sstat.st_size, w.cFileName);
      }
    } while (FindNextFile(fh, &w) == TRUE);
  }

  FindClose(fh);

  if (fd_out != STDOUT_FILENO) {
    flock(fd_out, LOCK_UN);
    close(fd_out);
  }

  return ret;
}
#endif



/* -----------------------------------------------------------------------
 * CREATE_FOLDER
 * -----------------------------------------------------------------------
 */

int create_folder(const char *dstaddr, const char *path)
{
//  fprintf(stderr, "%s entering\n", __func__);

   int len = -1;
   
   int sockfd = -1;
   //  char recvline[MAXLINE+1];
   
   sockfd =  connectToServer (dstaddr);
   if (sockfd < 0) {
     return -1;
   }
   
   sprintf(cmdbuf, "FUNC NONE MKDIR %s%c%s\n", cwd_buf, DELIM_CHAR, path);
   
   len = strlen(cmdbuf);
   if (writen(sockfd, cmdbuf, len) <= 0) 
   {
      fprintf(stderr, "Error writing data\n");
      CLOSE(sockfd);
      return -5;
   }
   
   if (readline(sockfd, cmdbuf, MAXLINE) <= 0) 
   {
      fprintf(stderr, "Error reading data\n");
      CLOSE(sockfd);
      return -6;
   }
   
   //  fprintf(stderr, "%s", cmdbuf); /* debug */
   if (cmdbuf[0] != '5')  /* 5NN means success */
   {
      fprintf(stderr, "%s", cmdbuf);
      CLOSE(sockfd);
      return -1;
   }
   
   while ((len = readline(sockfd, cmdbuf, MAXLINE)) > 0) 
   {
      fprintf(stderr, "%s", cmdbuf);
   }
   
   //  fprintf(stderr, "%s leaving\n", __func__);
   
   CLOSE(sockfd);
   
   return 0;
}

/* -----------------------------------------------------------------------
 * CREATE_LOCAL_FOLDER
 * -----------------------------------------------------------------------
 */

int create_local_folder(const char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);
   int ret = -1;
   
   mode_t mode = 00755;

   (void) mode; /* just to suppress warnings in Windows */

   ret = MKDIR(path, mode);
   
   if (ret != 0) 
   {
      perror(path);
   }

   return ret;
}

/* -----------------------------------------------------------------------
 * DELETE_FOLDER
 * -----------------------------------------------------------------------
 */

int delete_folder(const char *dstaddr, const char *path)
{
//  fprintf(stderr, "%s entering\n", __func__);

   int len = -1;
   
   int sockfd = -1;
   //  char recvline[MAXLINE+1];
   
   sockfd =  connectToServer (dstaddr);
   if (sockfd < 0) {
     return -1;
   }
   
   sprintf(cmdbuf, "FUNC NONE RMDIR %s%c%s\n", cwd_buf, DELIM_CHAR, path);
   
   len = strlen(cmdbuf);
   if (writen(sockfd, cmdbuf, len) <= 0) 
   {
      fprintf(stderr, "Error writing data\n");
      CLOSE(sockfd);
      return -5;
   }
   
   if (readline(sockfd, cmdbuf, MAXLINE) <= 0) 
   {
      fprintf(stderr, "Error reading data\n");
      CLOSE(sockfd);
      return -6;
   }
   
   //  fprintf(stderr, "%s", cmdbuf); /* debug */
   if (cmdbuf[0] != '5')  /* 5NN means success */
   {
      fprintf(stderr, "%s", cmdbuf);
      CLOSE(sockfd);
      return -1;
   }
   
   while ((len = readline(sockfd, cmdbuf, MAXLINE)) > 0) 
   {
      fprintf(stderr, "%s", cmdbuf);
   }
   
   //  fprintf(stderr, "%s leaving\n", __func__);
   
   CLOSE(sockfd);
   
   return 0;
}

/* -----------------------------------------------------------------------
 * DELETE_LOCAL_FOLDER
 * -----------------------------------------------------------------------
 */

int delete_local_folder(const char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);
   int ret = -1;

   ret = rmdir(path);
   if (ret != 0) 
   {
      perror(path);
   }

   return ret;
}

/* -----------------------------------------------------------------------
 * CHANGE_FOLDER
 * -----------------------------------------------------------------------
 */

int change_folder(const char *dstaddr, const char *path)
{
//  fprintf(stderr, "%s entering\n", __func__);

   int len = -1;
   
   int sockfd = -1;
   //  char recvline[MAXLINE+1];
   
   sockfd =  connectToServer (dstaddr);
   if (sockfd < 0) {
     return -1;
   }
   
   sprintf(cmdbuf, "FUNC NONE CHDIR %s%c%s\n", cwd_buf, DELIM_CHAR, path);
   
   len = strlen(cmdbuf);
   if (writen(sockfd, cmdbuf, len) <= 0) 
   {
      fprintf(stderr, "Error writing data\n");
      CLOSE(sockfd);
      return -5;
   }
   
   if (readline(sockfd, cmdbuf, MAXLINE) <= 0) 
   {
      fprintf(stderr, "Error reading data\n");
      CLOSE(sockfd);
      return -6;
   }
   
   //  fprintf(stderr, "%s", cmdbuf); /* debug */
   if (cmdbuf[0] != '5') { /* 5NN means success */
      fprintf(stderr, "%s", cmdbuf);
      CLOSE(sockfd);
      return -1;
   }

   while ((len = readline(sockfd, cmdbuf, MAXLINE)) > 0) {
     if (cmdbuf[0] != '-') { // not error string
       strncpy(cwd_buf, cmdbuf, len);
       cwd_buf[len] = 0;
       if (cwd_buf[len-1] == '\n') {
	 cwd_buf[len-1] = 0;
       }
     } else {
       fprintf(stderr, "%s", cmdbuf);
     }
   }

   //  fprintf(stderr, "%s leaving\n", __func__);
   
   CLOSE(sockfd);
   
   return 0;
}

/* -----------------------------------------------------------------------
 * CHANGE_LOCAL_FOLDER
 * -----------------------------------------------------------------------
 */

int change_local_folder(const char *path)
{
//  fprintf(stderr, "%s entered\n", __func__);
   int ret = -1;
   
   ret = chdir(path);
   if (ret != 0) 
   {
      perror(path);
   }
   
   return ret;
}

/* -----------------------------------------------------------------------
 * GET_CURRENT_DIR
 * -----------------------------------------------------------------------
 */

int get_current_dir(const char *dstaddr)
{
  //  fprintf(stderr, "%s entering\n", __func__);


  /* first see whether we already have the path in list */
  struct hostent *hinfo = gethostbyname(dstaddr);
  if (hinfo == NULL)
    return 0;

  struct in_addr tmpaddr;
  memcpy(&tmpaddr, hinfo->h_addr_list[0], sizeof(struct in_addr));
  current_host = tmpaddr.s_addr;

  cwd_buf = cwdent_getcwd(current_host);
  if (cwd_buf != NULL && cwd_buf[0] != '-') {
    printf("%s\n", cwd_buf);
    return 0;
  }

  /* path not found in list, query to server */

  int len = -1;
  int sockfd = -1;
  //  char recvline[MAXLINE+1];

  sockfd =  connectToServer (dstaddr);
  if (sockfd < 0) {
    return -1;
  }

  sprintf(cmdbuf, "FUNC NONE GETCWD %s%c%s\n", cwd_buf, DELIM_CHAR, "none");

  len = strlen(cmdbuf);
  if (writen(sockfd, cmdbuf, len) <= 0) 
  {
    fprintf(stderr, "Error writing data\n");
    CLOSE(sockfd);
    return -5;
  }

  if (readline(sockfd, cmdbuf, MAXLINE) <= 0)
  {
    fprintf(stderr, "Error reading data\n");
    CLOSE(sockfd);
    return -6;
  }

  //  fprintf(stderr, "%s", cmdbuf); /* debug */
  if (cmdbuf[0] != '5')  /* 5NN means success */
  {
    fprintf(stderr, "%s", cmdbuf);
    CLOSE(sockfd);
    return -1;
  }

  while ((len = readline(sockfd, cmdbuf, MAXLINE)) > 0) {
    if (cmdbuf[0] != '-') {
      strncpy(cwd_buf, cmdbuf, len);
      cwd_buf[len] = 0;
      if (cwd_buf[len-1] == '\n') {
	cwd_buf[len-1] = 0;
      }
      printf("%s\n", cwd_buf);
    } else {
      fprintf(stderr, "%s", cmdbuf);
    }
  }
  //  fprintf(stderr, "%s leaving\n", __func__);

  CLOSE(sockfd);

  return 0;
}

/* -----------------------------------------------------------------------
 * GET_LOCAL_CURRENT_DIR
 * -----------------------------------------------------------------------
 */

int get_local_current_dir(void)
{
   static char pbuf[PATH_MAX+1];
   printf("%s\n", getcwd(pbuf, sizeof(pbuf)));
   return 0;
}

/* -----------------------------------------------------------------------
 * EXECUTE_COMMAND
 * -----------------------------------------------------------------------
 */

typedef struct {
  int sock;
  int fd;
} thread_data;

int execute_command(const char *srcaddr, const char *src)
{
  //  fprintf(stderr, "%s entering\n", __func__);

  char buf[100] = {};
  char *ptr = NULL;
  int fd_in = -1;
  int fd_out = -1;
  int len = -1;
  int ret = -1;

  int sockfd = -1;

  struct stat statbuf;
  //  char recvline[MAXLINE+1];

  if (redir_in[0]) 
  { 
    fd_in = open_and_lock(redir_in, O_RDONLY|O_BINARY, 00644, LOCK_SH);
    if (fd_in == -1) 
    {
      perror(redir_in);
      goto cleanup;
    }
    if (stat(redir_in, &statbuf) != 0) 
    {
      fprintf(stderr, "File is removed before the processing of your request\n");
      ret = -1;
      goto cleanup;
    }
  }
  else 
    fd_in = STDIN_FILENO;

  if (redir_out[0])
  {
    fd_out = open_and_lock(redir_out, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,
	00644, LOCK_EX);
    if (fd_out == -1) 
    {
      perror(redir_out);
      goto cleanup;
    }
    if (stat(redir_out, &statbuf) != 0) 
    {
      fprintf(stderr, "File is removed before the processing of your request\n");
      ret = -1;
      goto cleanup;
    } 
  }  
  else 
    fd_out = STDOUT_FILENO;


  sockfd =  connectToServer (srcaddr);
  if (sockfd < 0) {
    return -1;
  }

  sprintf(cmdbuf, "EXEC %s%c%s\n", cwd_buf, DELIM_CHAR, src);

  len = strlen(cmdbuf);
  if (writen(sockfd, cmdbuf, len) <= 0) 
  {
    fprintf(stderr, "Error writing data\n");
    ret = -5;
    goto cleanup;
  }

  while (1) {
    //fprintf(stderr, "blocked in recv...\n");
    if (readline(sockfd, cmdbuf, MAXLINE) <= 0) {
      fprintf(stderr, "Error reading data\n");
      CLOSE(sockfd);
      return -6;
    }
    //fprintf(stderr, "recv returned\n");

    //fprintf(stderr, "%s", cmdbuf); /* debug */

    if (cmdbuf[0] == '5') { /* 5NN means success */
      // we can proceed only if OK [500 OK]
      break;
    } else if (cmdbuf[0] == '3') { /* maybe login required */
      sscanf(cmdbuf+4, "%s", buf);
      if (cmdbuf[1] == '0') { // 300 Login
	fprintf(stderr, "%s ", buf);
	fflush(stderr);
	fgets(cmdbuf, cmdbufsize, stdin);
	fflush(stdin);
	len = strlen(cmdbuf);
	if (writen(sockfd, cmdbuf, len) <= 0) {
	  fprintf(stderr, "Error writing data\n");
	  CLOSE(sockfd);
	  return -5;
	}
      } else if (cmdbuf[1] == '5') { // 350 Password
	ptr = getpass("Password: ");
	strcpy(cmdbuf, ptr);
	strcat(cmdbuf, "\n");
	len = strlen(cmdbuf);
	if (writen(sockfd, cmdbuf, len) <= 0) {
	  fprintf(stderr, "Error writing data\n");
	  CLOSE(sockfd);
	  return -5;
	}
      }
    } else if (cmdbuf[0] == '4') { /* error */
      fprintf(stderr, "%s", cmdbuf+4);
      CLOSE(sockfd);
      return -4;
    } else if (cmdbuf[0] == '1') { /* 100 login successful */
      //fprintf(stderr, "%s", cmdbuf+4); /* debug */
      //break;
    } else if (cmdbuf[0] == '2') { /* 200 login failure */
      fprintf(stderr, "%s", cmdbuf+4); /* debug */
      return -8;
    } else {
      fprintf(stderr, "Unknown response: %s", cmdbuf);
      CLOSE(sockfd);
      return -9;
    }
  }

#ifdef __WIN32__ /* windows */
  /* start reader thread here, send socket and other data as structure as
   * parameter
   */

  HANDLE read_thread = NULL;
  thread_data td;

  memset(&td, 0, sizeof(td));
  td.sock = sockfd;
  td.fd = fd_out;

  read_thread = CreateThread(NULL, 0, read_func, &td, 0, NULL);
  if (read_thread == NULL) {
    fprintf(stderr, "Error creating receiver thread: %d\n",
	(int) GetLastError());
    goto cleanup;
  }

  fprintf(stderr, "Enter data here. Type Ctrl-Z and press ENTER to stop\n");
  fflush(stderr);
  /* to stop reading, you need to type an EOF */
  while ((len = read(fd_in, cmdbuf, cmdbufsize)) > 0) {
    ret = writen(sockfd, cmdbuf, len);
    if (ret <= 0) {
      fprintf(stderr, "Error writing data to socket\n");
      break;
    }
  }
  shutdown(sockfd, SHUT_WR); /* shutdown write end of the socket */

  /*
   * terminate thread if by chance sock was closed but user is typing in stdin
   */
  //fprintf(stderr, "Waiting for thread to terminate\n");
  //TerminateThread(read_thread, 0);
  WaitForSingleObject(read_thread, INFINITE); // wait for thread
  //fprintf(stderr, "Thread terminated\n");
  CloseHandle(read_thread);

#else /* linux */
  /* in Windows select() works only for sockets */
  int maxfdp1, stdineof;
  fd_set rset;
  FILE *fp = fdopen(fd_in, "rb");

  stdineof = 0;
  FD_ZERO(&rset);
  while (1) 
  {
    if (stdineof == 0)
      FD_SET(fd_in, &rset);
    FD_SET(sockfd, &rset);
    maxfdp1 = MAX(fd_in, sockfd) + 1;
    if (select(maxfdp1, &rset, NULL, NULL, NULL) == -1) 
    {
      DEBUG("select", errno);
      goto cleanup;
    }
    if (FD_ISSET(sockfd, &rset))  /* socket is readable */
    {
      if ((len = recv(sockfd, cmdbuf, cmdbufsize, 0)) == 0) 
      {
	if (stdineof == 1) 
	  break;
	else 
	{
	  //fprintf(stderr, "Remote host closed socket\n");
	  break;
	}
      }
      write(fd_out, cmdbuf, len);
      //      fsync(fd);
    }

    if (FD_ISSET(fd_in, &rset)) 
    {
      if (fgets(cmdbuf, cmdbufsize, fp) == NULL) 
      {
	//fprintf(stderr, "EOF in stdin\n");
	stdineof = 1;
	shutdown(sockfd, SHUT_WR);
	FD_CLR(fd_in, &rset);
	continue;
      }
      writen(sockfd, cmdbuf, strlen(cmdbuf));
    }
  }

#endif

  //  fprintf(stderr, "%s leaving\n", __func__);
cleanup:
  if (fd_out != STDOUT_FILENO && fd_out != -1) 
  {
    unlock(fd_out);
    close(fd_out);
    //  
  }

  if (fd_in != STDIN_FILENO && fd_in != -1)  
  {
    unlock(fd_in);
    close(fd_in);
  }

  if (sockfd > 0)
    CLOSE(sockfd);

  //fprintf(stderr, "%s leaving\n", __func__);

  return 0;
}

#ifdef __WIN32__
DWORD WINAPI read_func(LPVOID lpData)
{
  thread_data *ptr = (thread_data *) lpData;
  char buf[MAXLINE];
  int sock = ptr->sock;
  int fd = ptr->fd;
  int n;

  while ((n = recv(sock, buf, sizeof(buf), 0)) > 0) {
    write(fd, buf, n);
  }

  ExitThread(0);
}
#endif

/* convert / and \ accordingly so that any notation can be used */
void fix_path(char *path, size_t len)
{
  int i;

  for (i = 0; i < len; i++) {
    if (path[i] == INV_DIR_DELIM) path[i] = DIR_DELIM;
  }
}

