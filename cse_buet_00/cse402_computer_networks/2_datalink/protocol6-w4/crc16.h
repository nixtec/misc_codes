#ifndef _CRC16_H_
#define _CRC16_H_
/*
 * crc16.h
 */
#include <stdint.h>         /* integer types */
#include <sys/types.h>

/*
 * CRC16 standard : 1-1000-0000-0000-0101 (0x18005)
 * CRC16 reversed : 1-0100-0000-0000-0011
 */

#define CRCPOLY 0x18005

uint16_t compute_crc16(uint8_t *msg, size_t len);

#endif /* !_CRC16_H_ */
