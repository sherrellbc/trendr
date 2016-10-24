#include <stdio.h>
#include <string.h>
#include "common.h"
#include "uart.h"
#include "stdarg.h"



/* Needs some serious optimization */
void buf_fill(uint8_t *buf, char *watermark, size_t wm_len, size_t dest_len){
    int num_times = dest_len/wm_len;
    int index = 0; 
    
    for(; num_times>=0; num_times--){
        memcpy((void *) (&buf[index]), (void *)watermark, wm_len); 
        index += wm_len; 
    }
}
