/*
 * myregex.h
 */
#ifndef MYREGEX_H
#define MYREGEX_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

#define match_regex(preg, regex, str) regexec(preg, str, 0, NULL, REG_NOTBOL)
//int match_regex(const char *regex, const char *str);

#endif /* !MYREGEX_H */
