/*
 * comm_man.c
 * Communication Manager
 * Communication channel simulated with FIFO
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "comm_man.h"

char *strsep(char **stringp, const char *delim);

static comm_man_t cman;

static int create_dirpath(const char *fullpath)
{
  char buf[256];
  char **bp, *bufp;
  char *ptr;
  char path[256];

  strcpy(path, "");
  strcpy(buf, fullpath);
  ptr = strrchr(buf, '/');
  *ptr = '\0'; /* only directory we need to create */
  bufp = buf;
  bp = &bufp;
  while ((ptr = strsep(bp, "/")) != NULL) {
    strcat(path, ptr);
    mkdir(path, 0777);
    strcat(path, "/");
  }

  return 0;
}

static int copy_file(const char *src, const char *dst)
{
  int fd1, fd2;
  int n;
  char buf[1024];
#if 0
  char **bp, *bufp;
  char *ptr;
  char path[256];
#endif

  fprintf(stderr, "Copying file from %s to %s\n", src, dst);
  create_dirpath(src);
  create_dirpath(dst);

#if 0
  strcpy(path, "");
  strcpy(buf, dst);
  ptr = strrchr(buf, '/');
  *ptr = '\0'; /* only directory we need to create */
  bufp = buf;
  bp = &bufp;
  while ((ptr = strsep(bp, "/")) != NULL) {
    strcat(path, ptr);
    mkdir(path, 0777);
    strcat(path, "/");
  }
#endif

  fd1 = open(src, O_RDONLY, 0644);
  if (fd1 == -1) {
    perror(src);
    return -1;
  }

  fd2 = open(dst, O_CREAT|O_WRONLY, 0644);
  if (fd2 == -1) {
    perror(dst);
    close(fd1);
    return -1;
  }

  while ((n = read(fd1, buf, sizeof(buf))) > 0) {
    write(fd2, buf, n);
  }
  close(fd2);
  close(fd1);

  return 0;
}

static int move_file(const char *src, const char *dst)
{
#if 0
  char buf[1024];
  char **bp, *bufp;
  char *ptr;
  char path[256];

  strcpy(path, "");
  strcpy(buf, dst);
  ptr = strrchr(buf, '/');
  *ptr = '\0'; /* only directory we need to create */
  bufp = buf;
  bp = &bufp;
  while ((ptr = strsep(bp, "/")) != NULL) {
    strcat(path, ptr);
    mkdir(path, 0777);
    strcat(path, "/");
  }
#endif

  create_dirpath(src);
  create_dirpath(dst);
  fprintf(stderr, "Moving file from %s to %s\n", src, dst);
  return rename(src, dst);
}

static int comm_man_sendfile(comm_man_t *c, int sockfd, const char *file)
{
  int fd;
  int i, n;
  char buf[1024];
  char tmpname[256];
  char realname[256];

  strcpy(buf, file);
  n = strlen(buf);
  for (i = 0; i < n; i++) {
    if (buf[i] == '/') {
      buf[i] = '_'; /* we don't need to create directories in temp folder */
    }
  }
  
  sprintf(tmpname,
      "%s/%s/Main/Folders/temp/%s",
      c->path_prefix, c->local_host_name, buf);
  sprintf(realname,
      "%s/%s/Main/Folders/%s",
      c->path_prefix, c->local_host_name, file);
  if (copy_file(realname, tmpname) == -1) {
    fprintf(stderr, "Error copying to temporary directory\n");
    return -1;
  }

  fd = open(tmpname, O_RDONLY, 0644);
  if (fd == -1) {
    perror(tmpname);
    return -1;
  }
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    send(sockfd, buf, n, 0);
  }
  close(fd);
  fprintf(stderr, "Removing file from temporary directory\n");
  unlink(tmpname);

  return 0;
}

int sendfile(int sockfd, const char *file)
{
  return comm_man_sendfile(&cman, sockfd, file);
}

static int comm_man_recvfile(comm_man_t *c, int sockfd, const char *file,
    int length)
{
  int fd;
  int i, n;
  int total;
  char buf[1024];
  char tmpname[256];
  char realname[256];

  strcpy(buf, file);
  n = strlen(buf);
  for (i = 0; i < n; i++) {
    if (buf[i] == '/') {
      buf[i] = '_'; /* we don't need to create directories in temp folder */
    }
  }

  sprintf(tmpname,
      "%s/%s/Main/Folders/temp/%s",
      c->path_prefix, c->local_host_name, buf);
  sprintf(realname,
      "%s/%s/Main/Folders/%s",
      c->path_prefix, c->local_host_name, file);
  fd = open(tmpname, O_CREAT|O_WRONLY, 0644);
  if (fd == -1) {
    perror(tmpname);
    return -1;
  }
  total = 0;
  while ((n = recv(sockfd, buf, sizeof(buf)-1, 0)) > 0) {
    total += n;
    write(fd, buf, n);
    if (total >= length) {
      break;
    }
  }
  close(fd);
  if (move_file(tmpname, realname) == -1) {
    fprintf(stderr, "Error moving from temporary directory\n");
    return -1;
  }

  return 0;
}

int recvfile(int sockfd, const char *filename, int length)
{
  return comm_man_recvfile(&cman, sockfd, filename, length);
}

/* specify host ip */
/* for server 'user' and 'pass' is NULL */
int comm_man_init(const char *ip, const char *user, const char *pass)
{
  char buf[256];

  memset(&cman, 0, sizeof(cman));
  strcpy(cman.local_host_name, ip);
  /* prefix/ip/Main/Folders/path */
  strcpy(cman.path_pattern, "%s/%s/Main/Folders/%s");
  strcpy(cman.path_prefix, "/MailRunners");
  sprintf(buf, cman.path_pattern, cman.path_prefix, cman.local_host_name, "/");
  strcat(buf, "temp/");
  create_dirpath(buf);

  cman.c_or_s = SERVER;
  if (user) {
    strcpy(cman.username, user);
    cman.c_or_s = CLIENT;
  }
  if (pass) {
    strcpy(cman.password, pass);
    cman.c_or_s = CLIENT;
  }

  return 0;
}

static in_port_t comm_man_get_free_port(comm_man_t *c)
{
  int i;
  in_port_t p;
  in_port_t pmax = (1<<15)-1;
  p = pmax;
  for (i = 0; p > 0 && i < MAX_SOCKETS; i++) {
    if (c->sock_table[i] && c->sock_table[i]->address.sin_port == p) {
      p--;
    }
  }

  return p;
}

static int comm_man_find_free_slot(comm_man_t *c)
{
  int i;
  for (i = 0; i < MAX_SOCKETS; i++) {
    if (!c->sock_table[i]) {
      return i;
    } else if (c->sock_table[i] && !c->sock_table[i]->dirty) {
      return i;
    }
  }
  return -1;
}

/* no extra parameter needed because we are implementing only TCP socket */
static int comm_man_socket(comm_man_t *c)
{
  int free_slot;
  socket_t *newsock;
  char sockdir[80];

  fprintf(stderr, "Creating socket ...");
  free_slot = comm_man_find_free_slot(c);
  if (free_slot < 0) {
    return -1;
  }
  newsock = calloc(1, sizeof(socket_t));
  newsock->sock_id = free_slot;
  newsock->sockfd_cs = -1; /* invalid socket, not yet connected */
  newsock->sockfd_sc = -1; /* invalid socket, not yet connected */
  c->sock_table[free_slot] = newsock;
  c->sock_table[free_slot]->dirty = true;

  sprintf(sockdir, "%s/sock", c->path_prefix);
  mkdir(c->path_prefix, 0777);
  mkdir(sockdir, 0777);

  fprintf(stderr, "done\n");

  return free_slot;
}

int socket(int domain, int type, int protocol)
{
  return comm_man_socket(&cman);
}

/* client/server */
static int comm_man_closesocket(comm_man_t *c, int sockfd)
{
  if (sockfd < 0 || sockfd >= MAX_SOCKETS || !c->sock_table[sockfd]->dirty) {
    fprintf(stderr, "Invalid socket descriptor\n");
    return -1;
  }
  if (c->sock_table[sockfd]->conn) {
    free(c->sock_table[sockfd]->conn);
  }

  fprintf(stderr, "Closing socket ...");
  if (c->c_or_s == SERVER) {
    unlink(c->sock_table[sockfd]->sockname_cs);
    unlink(c->sock_table[sockfd]->sockname_sc);
  }

  memset(c->sock_table[sockfd], 0, sizeof(socket_t));
  /* memsetting to zero implicitly set the slot to be unused */

  fprintf(stderr, "done\n");

  return 0;
}

int closesocket(int sockfd)
{
  return comm_man_closesocket(&cman, sockfd);
}

/* server */
static int comm_man_bind(comm_man_t *c, int sockfd,
    const struct sockaddr *address, socklen_t address_len)
{
  if (sockfd < 0 || sockfd >= MAX_SOCKETS || !c->sock_table[sockfd]->dirty) {
    fprintf(stderr, "Invalid socket\n");
    return -1;
  }

  fprintf(stderr, "Binding socket to %s:%hd ...",
      address->sin_addr, address->sin_port);

  if (!address) {
    fprintf(stderr, "No address specified\n");
    return -1;
  }
  sprintf(c->sock_table[sockfd]->sockname_cs, "%s/sock/%s.%hd.cs.fifo",
      c->path_prefix, address->sin_addr, address->sin_port);
  mkfifo(c->sock_table[sockfd]->sockname_cs, 0666);
  sprintf(c->sock_table[sockfd]->sockname_sc, "%s/sock/%s.%hd.sc.fifo",
      c->path_prefix, address->sin_addr, address->sin_port);
  mkfifo(c->sock_table[sockfd]->sockname_sc, 0666);
  memcpy(&c->sock_table[sockfd]->address, address, address_len);

  fprintf(stderr, "done\n");

  return 0;
}

int bind(int sockfd, const struct sockaddr *address, socklen_t address_len)
{
  return comm_man_bind(&cman, sockfd, address, address_len);
}

/* server */
/* backlog is not used in our implementation because we implement simple TCP */
static int comm_man_listen(comm_man_t *c, int sockfd, int backlog)
{
  if (sockfd < 0 || sockfd >= MAX_SOCKETS || !c->sock_table[sockfd]->dirty) {
    fprintf(stderr, "Invalid socket\n");
    return -1;
  }

  fprintf(stderr, "Listening for connections\n");
  c->sock_table[sockfd]->type = SOCK_LISTEN;

  return 0;
}

int listen(int sockfd, int backlog)
{
  return comm_man_listen(&cman, sockfd, backlog);
}

/* client */
static int comm_man_connect(comm_man_t *c, int sockfd,
    const struct sockaddr *address, socklen_t address_len)
{
  conn_t *local_conn;
  char msg[256];
  int ret;

  if (sockfd < 0 || sockfd >= MAX_SOCKETS || !c->sock_table[sockfd]->dirty) {
    fprintf(stderr, "Invalid socket\n");
    return -1;
  }

  fprintf(stderr, "Connecting to %s:%hd ...",
      address->sin_addr, address->sin_port);

  sprintf(c->sock_table[sockfd]->sockname_cs,
      "%s/sock/%s.%d.cs.fifo",
      c->path_prefix, address->sin_addr, address->sin_port);
  c->sock_table[sockfd]->sockfd_cs =
    open(c->sock_table[sockfd]->sockname_cs, O_WRONLY, 0666);
  if (c->sock_table[sockfd]->sockfd_cs < 0) {
    perror("open");
    exit(1);
  }
  sprintf(c->sock_table[sockfd]->sockname_sc,
      "%s/sock/%s.%d.sc.fifo",
      c->path_prefix, address->sin_addr, address->sin_port);
  c->sock_table[sockfd]->sockfd_sc =
    open(c->sock_table[sockfd]->sockname_sc, O_RDONLY, 0666);
  if (c->sock_table[sockfd]->sockfd_sc < 0) {
    perror("open");
    exit(1);
  }
  strcpy(c->sock_table[sockfd]->address.sin_addr, c->local_host_name);
  c->sock_table[sockfd]->address.sin_port = comm_man_get_free_port(c);

  sprintf(msg, "connect:%s:%hd:%s:%s:\n",
      c->sock_table[sockfd]->address.sin_addr,
      c->sock_table[sockfd]->address.sin_port,
      c->username,
      c->password);
  ret = write(c->sock_table[sockfd]->sockfd_cs, msg, strlen(msg));
  if (ret <= 0) {
    perror("write");
    exit(1);
  }
  ret = read(c->sock_table[sockfd]->sockfd_sc, msg, sizeof(msg)-1);
  if (ret <= 0) {
    perror("read");
    exit(1);
  }
  msg[ret] = '\0';
  close(c->sock_table[sockfd]->sockfd_cs);
  close(c->sock_table[sockfd]->sockfd_sc);
  if (strncmp(msg, "ok:", 3)) {
    fprintf(stderr, "We can't proceed anymore\n");
    return -1;
  }

  local_conn = calloc(1, sizeof(conn_t));
  local_conn->conn_id = sockfd;
  local_conn->local_sock = c->sock_table[sockfd];
  strcpy(local_conn->local_addr, c->local_host_name);
  local_conn->local_port = c->sock_table[sockfd]->address.sin_port;
  strcpy(local_conn->foreign_addr, address->sin_addr);
  local_conn->foreign_port = address->sin_port;
  c->sock_table[sockfd]->conn = local_conn;

  sprintf(c->sock_table[sockfd]->sockname_cs,
      "%s/sock/%s.%d.cs.fifo",
      c->path_prefix,
      c->sock_table[sockfd]->address.sin_addr,
      c->sock_table[sockfd]->address.sin_port);
  sprintf(c->sock_table[sockfd]->sockname_sc,
      "%s/sock/%s.%d.sc.fifo",
      c->path_prefix,
      c->sock_table[sockfd]->address.sin_addr,
      c->sock_table[sockfd]->address.sin_port);

  fprintf(stderr, "done\n");

  return 0;
}

int connect(int sockfd, const struct sockaddr *address, socklen_t address_len)
{
  return comm_man_connect(&cman, sockfd, address, address_len);
}

/* call after connect() */
static int comm_man_client_handle_file_service(comm_man_t *c, int sockfd)
{
  int ret;
  int choice; /* 1. Read Mail, 2. Upload, 3. Download, 4. Logout */
  char buf[256];
  char src[256], dst[256];
  int flength;
  struct stat st;

  c->sock_table[sockfd]->sockfd_cs =
    open(c->sock_table[sockfd]->sockname_cs, O_WRONLY, 0666);
  c->sock_table[sockfd]->sockfd_sc =
    open(c->sock_table[sockfd]->sockname_sc, O_RDONLY, 0666);

  fprintf(stderr, "Loading list of services from server\n");

  ret = recv(sockfd, buf, sizeof(buf)-1, 0);
  buf[ret] = '\0';
  fprintf(stderr, "%s\n>>> ", buf);
  scanf("%d", &choice);
  switch (choice) {
    case 1:
      fprintf(stderr, "Not implemented\n");
      break;
    case 2:
      fprintf(stderr, "Enter <local> <remote> paths\n");
      scanf("%s%s", src, dst);
      sprintf(buf,
	  c->path_pattern, /* initialized in comm_man_init() */
	  c->path_prefix, /* same as above */
	  c->local_host_name,
	  src);
      /* sprintf(buf, "%s/%s", c->local_host_name, src); */
      ret = stat(buf, &st);
      if (ret < 0) {
	perror(buf);
      } else {
	sprintf(buf, "upload:%s:%ld\n", dst, (long) st.st_size);
	send(sockfd, buf, strlen(buf), 0);
	ret = recv(sockfd, buf, sizeof(buf)-1, 0);
	if (ret <= 0) {
	  fprintf(stderr, "Error receiving data from server\n");
	} else {
	  buf[ret] = '\0';
	  if (!strncmp(buf, "ok:", 3)) {
	    ret = sendfile(sockfd, src);
	    if (ret < 0) {
	      fprintf(stderr, "Error sending file\n");
	    } else {
	      fprintf(stderr, "File upload successful\n");
	    }
	  } else {
	    fprintf(stderr, "Message from server: %s\n", buf);
	  }
	}
      }
      break;
    case 3:
      fprintf(stderr, "Enter <remote> <local> paths\n");
      scanf("%s%s", src, dst);
      sprintf(buf, "download:%s\n", src);
      send(sockfd, buf, strlen(buf), 0);
      ret = recv(sockfd, buf, sizeof(buf)-1, 0);
      if (ret <= 0) {
	fprintf(stderr, "Error receiving data from server\n");
      } else {
	buf[ret] = '\0';
	if (!strncmp(buf, "ok:", 3)) {
	  sscanf(buf, "ok:%d", &flength);
	  fprintf(stderr, "length = %d\n", flength);
	  ret = recvfile(sockfd, dst, flength);
	  if (ret < 0) {
	    fprintf(stderr, "Error receiving file\n");
	  } else {
	    fprintf(stderr, "File download successful\n");
	  }
	} else {
	  fprintf(stderr, "Message from server: %s\n", buf);
	}
      }
      break;
    case 4:
      strcpy(buf, "logout\n");
      send(sockfd, buf, strlen(buf), 0);
      break;
    default:
      fprintf(stderr, "Invalid choice\n");
  }

  return 0;
}

int client_handle_file_service(int sockfd)
{
  return comm_man_client_handle_file_service(&cman, sockfd);
}

static int comm_man_accept(comm_man_t *c, int sockfd,
    struct sockaddr *address, socklen_t *address_len)
{
  int connfd;
  int nread;
  int nwrite;
  conn_t *local_conn;
  char rbuf[256];
  char a[16]; /* address string */
  char p[10]; /* port string */
  char user[32];
  char pass[16];

  if (sockfd < 0 || sockfd >= MAX_SOCKETS || !c->sock_table[sockfd]->dirty) {
    fprintf(stderr, "Invalid socket\n");
    return -1;
  }

  fprintf(stderr, "Waiting for accepting connections\n");

  c->sock_table[sockfd]->sockfd_cs = open(c->sock_table[sockfd]->sockname_cs,
      O_RDONLY, 0666);
  if (c->sock_table[sockfd]->sockfd_cs == -1) {
    perror(c->sock_table[sockfd]->sockname_cs);
    return -1;
  }
  c->sock_table[sockfd]->sockfd_sc = open(c->sock_table[sockfd]->sockname_sc,
      O_WRONLY, 0666);
  if (c->sock_table[sockfd]->sockfd_sc == -1) {
    perror(c->sock_table[sockfd]->sockname_sc);
    return -1;
  }

  nread = read(c->sock_table[sockfd]->sockfd_cs, rbuf, sizeof(rbuf)-1);
  if (nread <= 0) {
    fprintf(stderr, "Error accepting connection\n");
    return -1;
  }
  rbuf[nread-1] = '\0'; /* NUL terminate string */
  sscanf(rbuf, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:\n]",
      c->cmdbuf, a, p, user, pass);
  strcpy(address->sin_addr, a);
  address->sin_port = (in_port_t) atoi(p);
  *address_len = sizeof(struct sockaddr);
  /* validate user here */
  if (validateCredential(user, pass) != 0) {
    strcpy(rbuf, "failed: Unauthorized user\n");
    nwrite = write(c->sock_table[sockfd]->sockfd_sc, rbuf, strlen(rbuf));
    fprintf(stderr, "Authorization failure. Connection not initiated\n");
    close(c->sock_table[sockfd]->sockfd_cs);
    close(c->sock_table[sockfd]->sockfd_sc);
    return -1;
  } else {
    fprintf(stderr, "User %s logged in successfully!\n", user);
  }

  /* user validated, now we can establish communication channel with user */
  connfd = socket(PF_INET, SOCK_STREAM, 0); /* socket for new communication */
  if (connfd < 0) {
    fprintf(stderr, "Error creating socket\n");
    close(c->sock_table[sockfd]->sockfd_cs);
    close(c->sock_table[sockfd]->sockfd_sc);
    return -1;
  }
  c->sock_table[connfd]->type = c->sock_table[sockfd]->type; /* SOCK_LISTEN */

  sprintf(c->sock_table[connfd]->sockname_cs, "%s/sock/%s.%s.cs.fifo",
      c->path_prefix, a, p);
  mkfifo(c->sock_table[connfd]->sockname_cs, 0666);
  sprintf(c->sock_table[connfd]->sockname_sc, "%s/sock/%s.%s.sc.fifo",
      c->path_prefix, a, p);
  mkfifo(c->sock_table[connfd]->sockname_sc, 0666);

  strcpy(rbuf, "ok: Authenticated\n");
  write(c->sock_table[sockfd]->sockfd_sc, rbuf, strlen(rbuf));

  close(c->sock_table[sockfd]->sockfd_cs);
  close(c->sock_table[sockfd]->sockfd_sc);

  local_conn = calloc(1, sizeof(conn_t));
  local_conn->conn_id = connfd;
  local_conn->local_sock = c->sock_table[connfd];
  strcpy(local_conn->local_addr, c->local_host_name);
  local_conn->local_port = c->sock_table[sockfd]->address.sin_port;
  strcpy(local_conn->foreign_addr, a);
  local_conn->foreign_port = (in_port_t) atoi(p);
  c->sock_table[connfd]->conn = local_conn;

  fprintf(stderr, "Connection accepted from %s:%s\n", a, p);

  return connfd;
}

int accept(int sockfd, struct sockaddr *address, socklen_t *address_len)
{
  return comm_man_accept(&cman, sockfd, address, address_len);
}

/* after accept() server will call this */
static int comm_man_server_handle_file_service(comm_man_t *c, int sockfd)
{
  int len;
  int n;
  char buf[256];
  char filename[256];
  char filesize[10];
  long flength;
  struct stat st;

  c->sock_table[sockfd]->sockfd_cs =
    open(c->sock_table[sockfd]->sockname_cs, O_RDONLY, 0666);
  c->sock_table[sockfd]->sockfd_sc =
    open(c->sock_table[sockfd]->sockname_sc, O_WRONLY, 0666);

  strcpy(buf,
      "\nPlease choose one of the following options:\n"
      "1. Check Mail\n"
      "2. Upload File\n"
      "3. Download File\n"
      "4. Logout from server");
  len = strlen(buf);
  n = send(sockfd, buf, len, 0);
  if (n < len) {
    fprintf(stderr, "Error sending message to server\n");
    return -1;
  }

  n = recv(sockfd, buf, sizeof(buf)-1, 0);
  if (n <= 0) {
    fprintf(stderr, "Terminating socket connection with client\n");
    return -1;
  }
  buf[n] = '\0';
  if (!strncmp(buf, "logout\n", 7)) {
    goto end;
  } else if (!strncmp(buf, "upload:", 7)) {
    sscanf(buf, "upload:%[^:]:%[^:\n]", filename, filesize);
    flength = strtol(filesize, NULL, 10);
    strcpy(buf, "ok: start transfer\n");
    send(sockfd, buf, strlen(buf), 0);
    n = recvfile(sockfd, filename, flength);
    if (n < 0) {
      fprintf(stderr, "Error receiving file\n");
    }
  } else if (!strncmp(buf, "download:", 9)) {
    sscanf(buf, "download:%[^:\n]", filename);
    sprintf(buf, "%s/%s", c->local_host_name, filename);
    n = stat(buf, &st);
    if (n < 0) {
      perror(buf);
      strcpy(buf, strerror(errno));
      send(sockfd, buf, strlen(buf), 0);
    } else {
      flength = st.st_size;
      sprintf(buf, "ok:%ld\n", flength);
      send(sockfd, buf, strlen(buf), 0);
      n = sendfile(sockfd, filename);
      if (n < 0) {
	fprintf(stderr, "Error sending file\n");
      }
    }
  } else {
    fprintf(stderr, "Unrecognized command\n");
  }

end:
  close(c->sock_table[sockfd]->sockfd_cs);
  close(c->sock_table[sockfd]->sockfd_sc);

  return 0;
}

int server_handle_file_service(int sockfd)
{
  return comm_man_server_handle_file_service(&cman, sockfd);
}

static int comm_man_send(comm_man_t *c, int sockfd, const void *buffer,
    size_t length, int flags)
{
  int fd;
  if (c->c_or_s == CLIENT) {
    fd = c->sock_table[sockfd]->sockfd_cs;
  } else {
    fd = c->sock_table[sockfd]->sockfd_sc;
  }
  return write(fd, buffer, length);
}

int send(int sockfd, const void *buffer, size_t length, int flags)
{
  return comm_man_send(&cman, sockfd, buffer, length, flags);
}

static int comm_man_recv(comm_man_t *c, int sockfd, void *buffer,
    size_t length, int flags)
{
  int fd;
  if (c->c_or_s == CLIENT) {
    fd = c->sock_table[sockfd]->sockfd_sc;
  } else {
    fd = c->sock_table[sockfd]->sockfd_cs;
  }
  return read(fd, buffer, length);
}

int recv(int sockfd, void *buffer, size_t length, int flags)
{
  return comm_man_recv(&cman, sockfd, buffer, length, flags);
}

static int comm_man_shutdown(comm_man_t *c, int sockfd, int how)
{
  return 0;
}

int shutdown(int sockfd, int how)
{
  return comm_man_shutdown(&cman, sockfd, how);
}

