#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include "stddef.h"


/* Full a buffer modulo dest_len times with a watermark
 * 
 * @param buf       : The destination buffer
 * @param watermark : String to replicate into buffer
 * @param wm_len    : Length of watermark
 * @param dest_len  : Length of destination buffer
 */
void buf_fill(uint8_t *buf, char *watermark, size_t wm_len, size_t dest_len);


#endif /* _UTIL_H */
