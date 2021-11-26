/*
 * crc16.c
 * calculates crc of a message
 */
#include "crc16.h"

uint16_t compute_crc16(uint8_t *msg, size_t len)
{
  uint16_t reg     = 0x0000;
  uint8_t popped   = 0;
  uint8_t next_bit = 0;
  uint8_t nextchar = 0;

  size_t i, j;

  for (i = 0; i < len; i++) { /* loop for whole message */
    nextchar = msg[i];
    for (j = 0; j < 8; j++) { /* loop for each character in message */
      popped = (uint8_t) (reg >> 15); /* check msb of reg */
      reg = (uint16_t) (reg << 1);
      next_bit = (uint8_t) (nextchar >> 7); /* next bit of char */
      reg = (uint16_t) (reg & 0xfffe); /* zero last bit of reg */
      reg = (uint16_t) (reg | next_bit);
      nextchar = (uint8_t) (nextchar << 1);
      if (popped) {
	reg = (uint16_t) (reg ^ CRCPOLY);
      }
    }
  }

  /* padded 16 0 bits */
  for (i = 0; i < 16; i++) {
    popped = (uint8_t) (reg >> 15); /* check msb of reg */
    reg = (uint16_t) (reg << 1);
    if (popped) {
      reg = (uint16_t) (reg ^ CRCPOLY);
    }
  }

  return reg;
}
