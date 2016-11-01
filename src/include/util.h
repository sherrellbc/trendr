#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Full a buffer modulo dest_len times with a watermark
 * 
 * @param buf       : The destination buffer
 * @param watermark : String to replicate into buffer
 * @param wm_len    : Length of watermark
 * @param dest_len  : Length of destination buffer
 */
void buf_fill(uint8_t *buf, char *watermark, size_t wm_len, size_t dest_len);



/*
 * Query the i2c bus for a device with the passed address
 *
 * @param addr  : The address to query
 *
 * @return      : -1 if address did not respond 
 */
int i2c_addr_stat(uint8_t addr);



/* 
 * Supports printing up to a 256 bit/32 byte unsigned int
 *
 * @param buf : The buffer containing the unsigned inti
 * @param len : Length of buf in bytes
 * 
 * @return    : Pointer to static buffer with corresponding hex string
 */

char *itohs(const uint8_t * const buf, const unsigned int len);



#ifdef __cplusplus
}
#endif

#endif /* _UTIL_H */
