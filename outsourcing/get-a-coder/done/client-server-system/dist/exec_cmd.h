/*
 * exec_cmd.h
 */
#ifndef EXEC_CMD_H
#define EXEC_CMD_H

#include "server-funcs.h"

void exec_cmd(int connfd,
    const char *cwd,
    const char *shell,
    const char *arg, // additional argument to shell if needed [-c or -k]
    const char *cmd,
    uid_t uid);

#endif /* EXEC_CMD_H */
