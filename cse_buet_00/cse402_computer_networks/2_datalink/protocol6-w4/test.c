/*
 * structfunc.c
 * demonstration code for using functions from structures
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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

int main(int argc, char **argv)
{
  uint8_t bitstr[LEN*8];
  size_t len;
  size_t maxlen;
  uint8_t str[LEN];
  uint8_t buf[LEN+LEN/5+1];
  int ret;


  while ((len = read(0, str, sizeof(str))) > 0) {
//    fprintf(stderr, "read returned %d\n", len);
//    maxlen = sizeof(bitstr);
//    len = str2bin(str, len, bitstr, maxlen);
//    printf("str2bin returned %d\n", len);
//    write(2, bitstr, len);
//    write(2, "\n", 1);
//    maxlen = sizeof(str);
//    len = bin2str(bitstr, len, str, sizeof(str));
//    printf("bin2str returned %d\n", len);
//    write(1, str, len);
    ret = stuffing(str, len, buf, sizeof(buf));
//    fprintf(stderr, "stuffing returned %d\n", len);
//    write(2, buf, len);
//    len = ret;
    ret = destuffing(buf, ret, str, sizeof(str));
//    fprintf(stderr, "destuffing returned %d\n", len);
    write(1, str, ret);
  }

  return 0;
}

/*
 * it doesn't modify the source stream
 * you can memcpy/strcpy the output
 */
/* input will be byte stream */
/*
 * this routine will first convert the input message to bit string
 * then it will stuff as necessary and then at last padding if necessary
 * and then convert it again to a byte stream
 */
/* static data is ok since only one function will call this routine
 * at a single moment
 */
size_t stuffing(const uint8_t *msg, size_t len, uint8_t *buf, size_t maxlen)
{
  static uint8_t src[LEN*8];
  static uint8_t dest[LEN*8+ LEN/5 + 1]; /* destination may be bit-stuffed */

  /*
  memset(src, 0, sizeof(src));
  memset(dest, 0, sizeof(dest));
  */

//  size_t srclen = 0;
  size_t dlen = 0;
  size_t ret = 0;
  int count = 0;

  register int i, j, k;

  j = k = 0; /* number of stuffed bits */

#ifdef __DEBUG__
  fprintf(stderr, "%s: message length : %d\n", __func__, len);
#endif

  ret = str2bin(msg, len, src, sizeof(src));

  for (i = 0; i < ret; i++) {
    if (src[i] == '1') {
      count++;
    }
    else {
      count = 0;
    }

    dest[i+j] = src[i];
    if (count >= 5) {
      j++;
      count = 0;
      dest[i+j] = '0';
#ifdef __DEBUG__
      fprintf(stderr, "%s: stuffed at %d\n", __func__, i+j);
#endif
    }
  }

//  fprintf(stderr, "%d: i = %d\n", __LINE__, i);
//  fprintf(stderr, "j = %d\n", j);
  if (j > 0) {
    for (k = 0; k < (8 - (j % 8)); k++) { /* padding */
      dest[i+j+k] = '0';
    }
#ifdef __DEBUG__
    fprintf(stderr, "%s: padded %d bits\n", __func__, k);
#endif
  }

//  dest[i+j+k] = 0; /* null terminate */
  dlen = i+j+k;
#ifdef __DEBUG__
  fprintf(stderr, "%s: dlen = %d bits\n", __func__, dlen);
#endif

//  fprintf(stderr, "%s: stuffed %d bits\n", __func__, j);

  ret = bin2str(dest, dlen, src, sizeof(src));
//  fprintf(stderr, "%s: ret = %d bits\n", __func__, ret);
  ret = (ret < maxlen) ? ret : maxlen;
  memcpy(buf, src, ret);

#ifdef __DEBUG__
  fprintf(stderr, "%s: stuffed length : %d\n", __func__, ret);
#endif

  return (ret);
}

size_t destuffing(const uint8_t *msg, size_t len, uint8_t *buf, size_t maxlen)
{
  static uint8_t src[(LEN+LEN/5)*8]; /* input stream may be bit stuffed */
  static uint8_t dest[LEN*8]; /* dest will hold buffer with bit destuffed */
  register int i, j, k;
  int count = 0;
  int ret = 0;

  j = k = 0;

  ret = str2bin(msg, len, src, sizeof(src));

#ifdef __DEBUG__
  fprintf(stderr, "%s: bin %d bits\n", __func__, ret);
#endif

  for (i = 0; i < ret; i++) {
    if (count >= 5) {
      j++; /* how many bits were stuffed */
      count = 0;
      continue; /* skip the stuffed zero */
    }
    if (src[i] == '1') {
      count++;
    }
    else {
      count = 0;
    }

    dest[i-j] = src[i];
  }

#ifdef __DEBUG__
  fprintf(stderr, "%s: destuffed %d bits\n", __func__, j);
#endif
  if (j > 0) {
    k = 8 - (j % 8); /* how many bits were padded */
  }

  len = (size_t) (i - j - k);

  ret = bin2str(dest, len, buf, maxlen);
#ifdef __DEBUG__
  fprintf(stderr, "%s: ret = %d\n", __func__, ret);
#endif
//  len = (len < maxlen) ? len : maxlen; /* if maxlen is little */
//  msg[i-j-k] = 0;
//  memcpy(buf, dest, len);

  /* return length of the original data. removed
   * stuffed bits, padded bits
   */
  return (ret);
}

/* dest must be at least (len * 8) + 1 bytes long */
size_t str2bin(const uint8_t *str, size_t len, uint8_t *dest, size_t maxlen)
{
  int i, j;
  uint8_t nextchar;
  uint8_t firstbit;
  size_t dlen;

  if (maxlen < len * 8) {
    fprintf(stderr, "%s: dest must be at least 8 times of len\n", __func__);
    return 0;
  }

  for (i = 0; i < len; i++) {
    nextchar = str[i];
    for (j = 0; j < 8; j++) {
      firstbit = nextchar >> 7;
      nextchar = nextchar << 1;
      dest[i*8+j] = firstbit + 48;
    }
  }

  dlen = (i * 8);
//  dest[dlen] = 0; /* null terminate */

  return (dlen);
}

/* dest must be at least (len / 8) + 1 bytes long */
size_t bin2str(const uint8_t *bin, size_t len, uint8_t *dest, size_t maxlen)
{
  int i, j;

  if (maxlen < len / 8) {
    fprintf(stderr, "%s: dest must be at least 1/8 times of len\n", __func__);
    return 0;
  }

  for (i = 0; i < (int) (len/8); i++) {
    dest[i] = 0;
    for (j = 0; j < 8; j++) {
      dest[i] = dest[i] | (bin[i*8+j] - (uint8_t) 48);
      if (j < 7) /* don't shift after the last bit entered */
	dest[i] = dest[i] << 1;
    }
  }
//  dest[i] = 0; /* null terminate */

  return i;
}
