/*
 * exec_cmd.c
 */
#ifdef __WIN32__
#include "exec_cmd.winx.c"
#else
#include "exec_cmd.unix.c"
#endif
