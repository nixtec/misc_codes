/*
 * parser.c
 * Copyright (C) Ayub <mrayub@gmail.com>
 * command line parser
 */
#include "parser.h"

int parse_command_line(const char *cmdline,
    char *cmd,
    char *srcaddr, char *src,
    char *dstaddr, char *dst,
    char *redirect_in, char *redirect_out)
{
  int i;
  int n;
  char ch;
  static char tmpbuf[1024]; /* adjust according to your needs */

  /* example command:
   * copy srcaddr:src dstaddr:dst <file.in >file.out
   * 'src' and/or 'dst' may contain double quote '"'
   */
  int in_quote = 0; /* are we inside '"' ? */
  int in_cmd = 0; /* are we in the command [argv[0]] */
  int in_src = 0; /* whether we are in src fragment */
  int in_dst = 0; /* whether we are in dst fragment */
  int safe; /* safe to ignore whitespace and exit from block */
  int in_input_redir = 0; /* whether we are in input redirection file */
  int in_output_redir = 0; /* whether we are in output redirection file */

  safe = 0; /* is it safe to copy the temporary buffer and break from loop? */
  in_cmd = 1; /* we're at first in command [argv[0]] */
  n = 0; /* index for characters in tmpbuf */

  for (i = 0; cmdline[i]; i++) {
    ch = cmdline[i];

    if (in_cmd) {
      if (ch == ' ' || ch == '\t' || ch == '\n') {
	if (safe) { /* command ended */
	  in_cmd = 0;
	  in_src = 1; /* now src is expected */
	  strncpy(cmd, tmpbuf, n);
	  cmd[n] = 0; /* NUL terminated */
	  n = 0;
	  safe = 0; /* set safe value for next block */
	}
      } else {
	tmpbuf[n++] = ch;
	safe = 1;
      }
    }


    else if (in_src || in_dst) {

      /* by chance if the user doesn't put space before '<' or '>' */
      if ((ch == '<' || ch == '>') && !in_quote) { /* input redirection */
	if (ch == '<') {
	  in_input_redir = 1;
	  in_output_redir = 0;
	} else {
	  in_output_redir = 1;
	  in_input_redir = 0;
	}
	if (in_src) {
	  in_src = 0;
	  strncpy(src, tmpbuf, n);
	  src[n] = 0;
	} else {
	  in_dst = 0;
	  strncpy(dst, tmpbuf, n);
	  dst[n] = 0;
	}
	n = 0;
	safe = 0;
	continue;
      }

      else if (ch == ':') { /* address copied */
	if (!in_quote) { // in quote, ':' is allowed
	  if (in_src) {
	    strncpy(srcaddr, tmpbuf, n);
	    srcaddr[n] = 0; /* NUL terminated */
	  } else {
	    strncpy(dstaddr, tmpbuf, n);
	    dstaddr[n] = 0; /* NUL terminated */
	  }
	  n = 0;
	} else {
	  tmpbuf[n++] = ch;
	}
      } else if (ch == '"') { /* quote started or ended */
	if (!in_quote) { /* quote started */
	  in_quote = 1;
	  safe = 0;
	} else { /* quote ended */
	  in_quote = 0;
	  if (in_src) {
	    strncpy(src, tmpbuf, n);
	    src[n] = 0;
	    in_src = 0; /* we are no longer in this section */
	    in_dst = 1; /* next section may be destination */
	  } else {
	    strncpy(dst, tmpbuf, n);
	    dst[n] = 0;
	    in_dst = 0; /* we are no longer in this section */
	  }
	  n = 0;
	  safe = 0;
	}
      } else if (ch == ' ' || ch == '\t' || ch == '\n') {
	if (in_quote) { /* check whether we are in '"' */
	  tmpbuf[n++] = ch; /* we allow whitespace in quote */
	} else { /* argument finished */
	  if (safe) { /* safe to exit from block */
	    if (in_src) {
	      strncpy(src, tmpbuf, n);
	      src[n] = 0;
	      in_src = 0;
	      in_dst = 1; /* next section may be destination */
	    } else {
	      strncpy(dst, tmpbuf, n);
	      dst[n] = 0;
	      in_dst = 0;
	    }
	    n = 0;
	    safe = 0;
	  }
	}
      } else {
	tmpbuf[n++] = ch; /* nonblank character */
	safe = 1;
      }
    }


    else { /* redirection maybe */
      if (ch == '<') {
	if (in_output_redir) { /* we were already in output redir */
	  if (in_quote) { /* we were in quote, error */
	    return -1;
	  } else { /* copy to redirect_out */
	    if (safe) {
	      strncpy(redirect_out, tmpbuf, n);
	    }
	  }
	}
	in_input_redir = 1;
	in_output_redir = 0;
	n = 0;
	safe = 0;
	continue;
      } else if (ch == '>') {
	if (in_input_redir) { /* we were already in output redir */
	  if (in_quote) { /* we were in quote, error */
	    return -1;
	  } else { /* copy to redirect_out */
	    if (safe) {
	      strncpy(redirect_in, tmpbuf, n);
	    }
	  }
	}
	in_input_redir = 0;
	in_output_redir = 1;
	n = 0;
	safe = 0;
	continue;
      }

      if (in_input_redir || in_output_redir) {
	if (ch == '"') { /* quote started or ended */
	  if (!in_quote) { /* quote started */
	    in_quote = 1;
	    safe = 0;
	  } else { /* quote ended */
	    in_quote = 0;
	    if (in_input_redir) {
	      strncpy(redirect_in, tmpbuf, n);
	      redirect_in[n] = 0;
	      in_input_redir = 0; /* we are no longer in this section */
	    } else if (in_output_redir) {
	      strncpy(redirect_out, tmpbuf, n);
	      redirect_out[n] = 0;
	      in_output_redir = 0; /* we are no longer in this section */
	    }
	    n = 0;
	    safe = 0;
	  }
	} else if (ch == ' ' || ch == '\t' || ch == '\n') {
	  if (in_quote) { /* check whether we are in '"' */
	    tmpbuf[n++] = ch; /* we allow whitespace in quote */
	  } else { /* argument finished */
	    if (safe) { /* safe to exit from block */
	      if (in_input_redir) {
		strncpy(redirect_in, tmpbuf, n);
		redirect_in[n] = 0;
		in_input_redir = 0;
	      } else if (in_output_redir) {
		strncpy(redirect_out, tmpbuf, n);
		redirect_out[n] = 0;
		in_output_redir = 0;
	      }
	      n = 0;
	      safe = 0;
	    }
	  }
	} else {
	  tmpbuf[n++] = ch; /* nonblank character */
	  safe = 1;
	}
      }
    }
  }

  /* server name/address must be ended with ':' otherwise error */

  /* command line ended but buffer not yet copied */
  if (in_cmd) {
    strncpy(cmd, tmpbuf, n);
    cmd[n] = 0;
    n = 0;
  } else if (in_src) {
    if (!in_quote) {
      strncpy(src, tmpbuf, n);
      src[n] = 0;
      n = 0;
    } else {
      n = -1;
    }
  } else if (in_dst) {
    if (!in_quote) {
      strncpy(dst, tmpbuf, n);
      dst[n] = 0;
      n = 0;
    } else {
      n = -1;
    }
  } else if (in_input_redir) {
    if (!in_quote) {
      strncpy(redirect_in, tmpbuf, n);
      redirect_in[n] = 0;
      n = 0;
    } else {
      n = -1;
    }
  } else if (in_output_redir) {
    if (!in_quote) {
      strncpy(redirect_out, tmpbuf, n);
      redirect_out[n] = 0;
      n = 0;
    } else {
      n = -1;
    }
  } else {
    n = 0;
  }

  return n;
}
