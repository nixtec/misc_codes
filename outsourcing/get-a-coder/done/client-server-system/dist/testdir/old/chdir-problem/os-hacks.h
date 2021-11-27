/*
 *	                    OS-HACKS.H					  
 *              OS-specfic (Win32) include file for building the GPC rts:
 *              Contains certain implementations for Mingw32 and Cygwin
 *
 * This file is part of the GPC package.
 *  v1.11, May 10 2002
 *  v1.12, Nov 13 2002
 *
 * Contributors:
 *   Prof. A Olowofoyeku (The African Chief) <African_Chief@bigfoot.com>
 *   Frank Heckenbach <frank@g-n-u.de>
 *
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *       1. Mingw32:
 * 	 Defines and implements:                                
 *                statfs
 *		  fsync
 *		  isinf
 *		  isnan
 *		  setpwent
 *		  endpwent
 *		  getpwent
 *		  getpwuid
 *		  getpwnam
 *		  						   
 *	 Defines/redefines:						  
 *        	  HAVE_STATFS
 *		  stat
 *        	  _sleep			 				  
 *        	  ssize_t	 					  
 *        	  realpath		 				  
 *        	  _fullpath		 				  
 *        	  mkdir
 *		  system
 * 		  WIFEXITED
 * 		  WEXITSTATUS
 *
 *									  
 *       2. Cygwin32:
 * 	 Redefines the following to use CygWin APIs:		  
 *       	  realpath						  
 *       	  getcwd
 *
 *
 *	 3. Both platforms:
 *	 Defines/Redefines:
 *		  time
 *                executable_path
 *
*/

#ifndef __OS_HACKS_H__
#define __OS_HACKS_H__

#define WIN32_LEAN_AND_MEAN  /* exclude winsock.h because of clash with select() */
#include <windows.h>         /* needed for WinAPI stuff */
#undef WIN32_LEAN_AND_MEAN 

#include <errno.h>           /* needed for vfs.h functions */
#include <lm.h>              /* needed for pwd.h functions */

#ifndef time
#include <time.h>
#endif

#ifdef __MINGW32__
/* new problem (20/5/2000): Define if you have the signal function.  */
#undef HAVE_SIGNAL

#ifndef SSIZE_MAX
  #define SSIZE_MAX INT_MAX    /* not defined elsewhere */
#endif

#ifndef ssize_t
  #define ssize_t int
  #define HAVE_SSIZE_T
#endif

#ifndef WIFEXITED
  #define WIFEXITED(S) 1
#endif

#ifndef WEXITSTATUS
  #define WEXITSTATUS(S) (S)
#endif

#define _fullpath(res,path,size) \
  (GetFullPathName ((path), (size), (res), NULL) ? (res) : NULL)

#define realpath(path,resolved_path) _fullpath(resolved_path, path, MAX_PATH)

#undef HAVE_SLEEP
#undef HAVE__SLEEP
#define HAVE__SLEEP 1
#define _sleep(seconds) Sleep(1000*seconds)

#define MKDIR_TWOARG 1
#define mkdir(path,mode) mkdir(path)

/**************** environment variables *****************/
#define HAVE_UNSETENV 1
#define SETENV_DECLARED 1
#define unsetenv(pname) SetEnvironmentVariable(pname, NULL)

#define HAVE_SETENV 1
#define UNSETENV_DECLARED 1
#define setenv(__pname,__pvalue,___overwrite) \
({ \
  int result; \
 if (___overwrite == 0 && getenv (__pname)) result = 0; \
   else \
     result = SetEnvironmentVariable (__pname,__pvalue); \
 result; \
})
 
/************** ftruncate(), truncate() ****************/
#define HAVE_FTRUNCATE
#define ftruncate chsize

static int __truncate (const char *fname, int distance)
{
 int i = -1;
 HANDLE hf = CreateFile
  (  fname,
     GENERIC_READ | GENERIC_WRITE,
     FILE_SHARE_READ | FILE_SHARE_WRITE,
     NULL,
     OPEN_EXISTING,
     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
     NULL);
 if (hf != INVALID_HANDLE_VALUE)
 {
   if (SetFilePointer (hf, distance, NULL, FILE_BEGIN) != 0xFFFFFFFF)
   {
     if (SetEndOfFile (hf))
     { i = 0; }
   }
 CloseHandle (hf);
 }
 return i;
}

#define truncate(_name,_distance) __truncate(_name,_distance)

/*************** system() ********************/
/* make sure that system() always receives back slashes */
#define system(cmdline) \
({ \
   int result; \
   char *new_cmdline = strdup (cmdline), *c; \
   for (c = new_cmdline; *c && *c != ' '; c++) \
     if (*c == '/') *c = '\\'; \
   result = system (new_cmdline); \
   free (new_cmdline); \
   result; \
})


/*********************** statfs *****************************/
#undef HAVE_STATFS
#define HAVE_STATFS 1

/* fake block size */
#define FAKED_BLOCK_SIZE 512

/* linux-compatible values for fs type */
#define MSDOS_SUPER_MAGIC     0x4d44
#define NTFS_SUPER_MAGIC      0x5346544E

#ifdef HAVE_SYS_VFS_H
  #undef HAVE_SYS_VFS_H
  #define HAVE_SYS_VFS_H 1
#else  /* HAVE_SYS_VFS_H */
struct statfs {
   long    f_type;     /* type of filesystem (see below) */
   long    f_bsize;    /* optimal transfer block size */
   long    f_blocks;   /* total data blocks in file system */
   long    f_bfree;    /* free blocks in fs */
   long    f_bavail;   /* free blocks avail to non-superuser */
   long    f_files;    /* total file nodes in file system */
   long    f_ffree;    /* free file nodes in fs */
   long    f_fsid;     /* file system id */
   long    f_namelen;  /* maximum length of filenames */
   long    f_spare[6]; /* spare for later */
};
#endif /* HAVE_SYS_VFS_H */

static int __statfs (const char *path, struct statfs *buf)
  {
    HINSTANCE h;
    FARPROC f;
    int retval = 0;
    char tmp [MAX_PATH], resolved_path [MAX_PATH];
    realpath(path, resolved_path);
    if (!resolved_path)
      retval = - 1;
    else
      {
        /* check whether GetDiskFreeSpaceExA is supported */
        h = LoadLibraryA ("kernel32.dll");
        if (h)
          f = GetProcAddress (h, "GetDiskFreeSpaceExA");
        else
          f = NULL;
        if (f)
          {
            ULARGE_INTEGER bytes_free, bytes_total, bytes_free2;
            if (!f (resolved_path, &bytes_free2, &bytes_total, &bytes_free))
              {
                errno = ENOENT;
                retval = - 1;
              }
            else
              {
                buf -> f_bsize = FAKED_BLOCK_SIZE;
                buf -> f_bfree = (bytes_free.QuadPart) / FAKED_BLOCK_SIZE;
                buf -> f_files = buf -> f_blocks = (bytes_total.QuadPart) / FAKED_BLOCK_SIZE;
                buf -> f_ffree = buf -> f_bavail = (bytes_free2.QuadPart) / FAKED_BLOCK_SIZE;
              }
          }
        else
          {
            DWORD sectors_per_cluster, bytes_per_sector;
            if (h) FreeLibrary (h);
            if (!GetDiskFreeSpaceA (resolved_path, &sectors_per_cluster,
                   &bytes_per_sector, &buf -> f_bavail, &buf -> f_blocks))
              {
                errno = ENOENT;
                retval = - 1;
              }
            else
              {
                buf -> f_bsize = sectors_per_cluster * bytes_per_sector;
                buf -> f_files = buf -> f_blocks;
                buf -> f_ffree = buf -> f_bavail;
                buf -> f_bfree = buf -> f_bavail;
              }
          }
        if (h) FreeLibrary (h);
      }

    /* get the FS volume information */
    if (strspn (":", resolved_path) > 0) resolved_path [3] = '\0'; /* we want only the root */    
    if (GetVolumeInformation (resolved_path, NULL, 0, &buf -> f_fsid, &buf -> f_namelen, NULL, tmp, MAX_PATH))
     {
     	if (strcasecmp ("NTFS", tmp) == 0)
     	 {
     	   buf -> f_type = NTFS_SUPER_MAGIC;
     	 }
     	else
     	 {
     	   buf -> f_type = MSDOS_SUPER_MAGIC;
     	 }
     }
    else
     {
       errno = ENOENT;
       retval = - 1;
     }
    return retval;
}

#define statfs(_path,_buf) __statfs(_path,_buf)

/******************* statfs ****************************/

/* mingw cannot correctly stat the root directory of a drive */
#define stat(_filename,_buf) \
({ \
   char *filename = (char*) (_filename), resolved_path [MAX_PATH]; \
   struct stat *buf = (_buf); \
   int result; \
   _fullpath(resolved_path, filename, MAX_PATH); \
   if (resolved_path [0] != 0 && \
       resolved_path [1] == ':' && \
       resolved_path [2] == '\\' && \
       resolved_path [3] == 0) \
     { \
       /* There's not much information available. Just fill in the most \
          important fields. Perhaps someone knows how to get more \
          information... */ \
       memset (buf, 0, sizeof (*buf)); \
       buf->st_mode = S_IFDIR | 0777; \
       buf->st_nlink = 0xffff; /* Better be safe than sorry... */ \
       result = 0; \
     } \
   else \
     result = stat (filename, buf); \
   result; \
})
/**************** end: stat *******************/

#undef HAVE_FSYNC
#define HAVE_FSYNC 1
#define fsync(__fd) ((FlushFileBuffers ((HANDLE) _get_osfhandle (__fd))) ? 0 : -1)

#undef HAVE_ISINF
#define HAVE_ISINF 1

#undef HAVE_ISNAN
#define HAVE_ISNAN 1

#define __uint32_t unsigned int

typedef union
{
  struct 
  {
    __uint32_t lsw, msw;
  } parts;
  double value;
} ieee_double_shape_type;

#define isinf(x) \
({ \
  ieee_double_shape_type e; \
  e.value = (x); \
  e.parts.lsw != 0 ? 0 : (e.parts.msw == 0xfff00000) ? -1 \
    : (e.parts.msw == 0x7ff00000) ? 1 : 0; \
})

#define isnan(x) \
({ \
  ieee_double_shape_type e; \
  e.value = (x); \
  ((e.parts.msw & 0x7fffffff) | (e.parts.lsw != 0)) > 0x7ff00000; \
})


/******************** pwd.h functions ********************/
#ifndef _PWD_H_

#ifndef HAVE_PWD_H
  #define HAVE_PWD_H 1
  #define	_PATH_PASSWD		"/etc/passwd"
  #define	_PASSWORD_LEN		128	/* max length, not counting NULL */
struct passwd {
	char	*pw_name;		/* user name */
	char	*pw_passwd;		/* encrypted password */
	int	pw_uid;			/* user uid */
	int	pw_gid;			/* user gid */
	char	*pw_comment;		/* comment */
	char	*pw_gecos;		/* Honeywell login info */
	char	*pw_dir;		/* home directory */
	char	*pw_shell;		/* default shell */
};
#endif /* HAVE_PWD_H */

#endif /* _PWD_H_ */

/******************** pwd.c *************************/

#define pwd_max_mystr 260
typedef char pwd_Mystr [pwd_max_mystr];

/*
 * convert wide string to ansi string
 */
static int wide2ansi (LPWSTR src, pwd_Mystr dest)
{
 return WideCharToMultiByte
    ( CP_ACP, 0, src, -1, dest, pwd_max_mystr, NULL, NULL );
}

/*
 * use globals
 */
 struct _USER_INFO_3 *info2 = NULL;
 int uid2 = -1;
 DWORD Counter = 0;
 HINSTANCE DllHandle = 0;
 FARPROC pNetApiBufferFree = NULL;
 FARPROC pNetUserEnum = NULL;

/* return whether we are running under NT, and that we are able to load
 * the necessary "Net" functions. These routines don't exist under
 * Win9x so we need to load the dll functions explicitly
 */
static BOOL isNT (void)
{
  OSVERSIONINFO ver;
  ver.dwOSVersionInfoSize = sizeof (OSVERSIONINFO); 
  GetVersionEx (&ver);
  if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
      if (!DllHandle)
      {
        DllHandle = LoadLibraryA ("netapi32.dll");
        if (DllHandle)
        {
	   pNetApiBufferFree = GetProcAddress (DllHandle, "NetApiBufferFree");
	   pNetUserEnum = GetProcAddress (DllHandle, "NetUserEnum");
	   if ( pNetApiBufferFree && pNetUserEnum )
	     return TRUE;
	   else
	     {
	       FreeLibrary (DllHandle);
	       DllHandle = 0;
	       pNetApiBufferFree = NULL;
	       pNetUserEnum = NULL;
	       return FALSE;
	     }
        }
      }
      else
        return TRUE;
    }
  else
    return FALSE;  
}

/*
 * exported functions
*/ 
static void __setpwent (void)
{
  if (info2 && pNetApiBufferFree) pNetApiBufferFree (info2);  
  info2 = NULL;
  uid2 = -1;
  Counter = 0;
 }

static void __endpwent (void)
{
   if (pNetApiBufferFree) pNetApiBufferFree (info2);
   info2 = NULL;
   uid2 = -1;
   Counter = 0;
   if (DllHandle)
    {
      FreeLibrary (DllHandle);
      DllHandle = 0;
      pNetApiBufferFree = NULL;
      pNetUserEnum = NULL;
    }
}

static struct passwd *__getpwent (void)
{
 static pwd_Mystr name, comment, realname, dir, shell;
 static struct passwd psw1 = { name, "*", 0, 0, comment, realname, dir, shell };
 DWORD entries, resume;

  if (!isNT ()) return NULL;
  
  if (uid2 < 0)
  {
    uid2++;
    resume = 0;
    Counter = 0;
    if ( pNetUserEnum ( NULL, 3, FILTER_NORMAL_ACCOUNT, (LPBYTE *) &info2,
                sizeof (*info2), &Counter, &entries, &resume ) )
        return NULL;
  }
  else
  { /* move to next record */
    uid2++;
    if (uid2 < Counter)
      info2++;
    else
      return NULL;  
  }

   wide2ansi (info2->usri3_name, psw1.pw_name);
   wide2ansi(info2->usri3_full_name, psw1.pw_gecos);
   wide2ansi(info2->usri3_home_dir, psw1.pw_dir);
   wide2ansi(info2->usri3_usr_comment, psw1.pw_comment);
   psw1.pw_uid = info2->usri3_user_id;
   psw1.pw_gid = info2->usri3_primary_group_id;
   psw1.pw_shell = (char *) getenv ("SHELL");
   if (!psw1.pw_shell) psw1.pw_shell = (char *) getenv ("COMSPEC");

   return &psw1;
}

static struct passwd *__getpwuid (int uid)
{
 static struct passwd *psw0;
  
  if (!isNT ()) return NULL; 
  __setpwent ();
  psw0 = __getpwent ();
  if (!psw0) return NULL;
  
  do
   {
     if (psw0->pw_uid == uid)
      {
        __endpwent ();
        return psw0;
      }
     else
      {
        psw0 = __getpwent();
        if (!psw0)
        {
  	  __endpwent ();
  	  return NULL;
        }
      }
   } while (psw0 != NULL);
}

static struct passwd *__getpwnam (const char * name)
{
 static struct passwd *psw0;
 
  if (!isNT ()) return NULL; 
  __setpwent ();
  psw0 = __getpwent ();
  if (!psw0) return NULL;
  
  do
   {
     if (strcmp (psw0->pw_name, name) == 0)
      {
        __endpwent ();
        return psw0;
      }
     else
      {
        psw0 = __getpwent();
        if (!psw0)
         {
  	   __endpwent ();
  	   return NULL;
         }
      }
   } while (psw0 != NULL);
}

/* macros */
#define setpwent __setpwent
#define endpwent __endpwent
#define getpwent __getpwent
#define getpwuid(_uid) __getpwuid(_uid)
#define getpwnam(_name) __getpwnam(_name)

/******************** pwd.c *************************/

#endif /* __MINGW32__ */



/******************* cygwin stuff *****************/

#ifdef __CYGWIN32__

#define realpath(path,resolved_path) \
  (getenv ("CYGWIN_USE_WIN32_PATHS") \
   ? (cygwin_conv_to_full_win32_path ((path), (resolved_path)), resolved_path) \
   : realpath ((path), (resolved_path)))

#define getcwd(_buf,_size) \
( \
  { \
    char *buf = (_buf); \
    size_t size = (_size); \
    char tmp [260]; \
    getcwd (tmp, size); \
    realpath (tmp, buf); \
    buf; \
  } \
)

#endif /* __CYGWIN32__ */


/******************* common stuff *****************/

/* we undefine HAVE_GETTIMEOFDAY because of Cygwin's date/time bug */
#undef HAVE_GETTIMEOFDAY

/* new time() that uses the WinAPI to call GetLocalTime */
#undef time
#define time(_timer) \
( \
  { \
  time_t *timer = (_timer); \
  SYSTEMTIME tmp; \
  time_t tmp2 = 0; \
  struct tm gnu; \
    memset (&gnu, 0, sizeof (gnu)); \
    GetLocalTime (&tmp); \
    gnu.tm_isdst = -1; \
    gnu.tm_year  = tmp.wYear - 1900; \
    gnu.tm_mon   = tmp.wMonth - 1; \
    gnu.tm_mday  = tmp.wDay; \
    gnu.tm_hour  = tmp.wHour; \
    gnu.tm_min   = tmp.wMinute; \
    gnu.tm_sec   = tmp.wSecond; \
    tmp2 = mktime (&gnu); \
    if (timer) *timer = tmp2; \
    tmp2; \
  } \
)

#undef HAVE_EXECUTABLE_PATH
#define HAVE_EXECUTABLE_PATH 1

#define executable_path(buffer) \
( \
 { \
   char *buf = (buffer); \
   GetModuleFileName(NULL,buf,MAX_PATH); \
   buf; \
 } \
)

/*
#define HAVE_GETTMPDIR
#define gettmpdir(Buffer, Size) (GetTempPath((Size), (Buffer)) > 0 ? (Buffer) : NULL)
*/

/**********************************************************/
/* new problem (7/8/2000) */
#undef HAVE_SYS_SIGLIST
#undef HAVE__SYS_SIGLIST
/**********************************************************/

#endif /* __OS_HACKS_H__ */

