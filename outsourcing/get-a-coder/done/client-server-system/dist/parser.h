#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAXLINE
#define MAXLINE 256
#endif


int parse_command_line(const char *cmdline,
    char *cmd,
    char *srcaddr, char *src,
    char *dstaddr, char *dst,
    char *redirect_in, char *redirect_out);

#endif
