#ifndef CONFIG_H
#define CONFIG_H

#define LISTENQ 5		/* maximum queue length for server */
#define MAXLINE 256		/* maximum length of a line (message) */
/* server will listen on this port */
#define SERV_PORT 1234		/* convert it to network byte order */

//#define LOGFILE "/tmp/cs.server.log"	/* server log file [unused] */
#define LOGFILE "cs.server.log"		/* server log file [unused] */

/* cygwin may not have PATH_MAX defined */
#ifndef PATH_MAX		/* maximum length of a pathname */
#define PATH_MAX 512
#endif

/* cygwin may not have INET_ADDRSTRLEN defined */
#ifndef INET_ADDRSTRLEN		/* size of ipv4 [dotted quad] address buffer */
#define INET_ADDRSTRLEN 16
#endif

#define DELIM_CHAR '\x2'	// delimiter for parsing request
#define DELIM "\x2\n"		// use same char at the start as DELIM_CHAR

#define DEFAULT_CWD "-"
#define DEFAULT_CWD_CHAR '-'

#ifdef __WIN32__
#define DIR_DELIM '\\'
#define INV_DIR_DELIM '/'
#else
#define DIR_DELIM '/'
#define INV_DIR_DELIM '\\'
#endif

#if defined(linux)
#define USE_WORDEXP_H
#define HAVE_INET_NTOP
#elif defined(__CYGWIN__)
#define USE_GLOB_H
#endif

/* syntax of file:
 * CMD:UNIXCMD:WINCMD
 * ListDir:ls:dir
 */
#define CMDMAPFILE "cmdmap.conf"

#include "oshacks.h"

#endif /* CONFIG_H */
