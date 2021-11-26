#ifndef _SUPPORT_H_
#define _SUPPORT_H_
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);

/* all buffers must be large enough to handle the request */
/* returns the length of the stuffed stream */
size_t stuffing(const uint8_t *msg, size_t len, uint8_t *dest, size_t maxlen);
/* returns the length of the destuffed stream */
size_t destuffing(const uint8_t *msg, size_t len, uint8_t *dest, size_t maxlen);
/* returns the length of the binary string */
size_t str2bin(const uint8_t *str, size_t len, uint8_t *dest, size_t maxlen);
/* returns the length of the converted string */
size_t bin2str(const uint8_t *bin, size_t len, uint8_t *dest, size_t maxlen);

/* input strings must NOT be larger than this */
#define LEN 1024


#endif /* !_SUPPORT_H_ */
