#include <stdio.h>
#include <string.h>
#include "stdarg.h"
#include "util.h"
#include "logging.h"
#include "i2c_t3.h"



/* Needs some serious optimization */
void buf_fill(uint8_t *buf, char *watermark, size_t wm_len, size_t dest_len){
    int num_times = dest_len/wm_len;
    int index = 0; 
    
    for(; num_times>=0; num_times--){
        memcpy((void *) (&buf[index]), (void *)watermark, wm_len); 
        index += wm_len; 
    }
}



int i2c_addr_stat(uint8_t addr){
    Wire.beginTransmission(addr);
    Wire.endTransmission();
    return Wire.getError()?(-1):(0);
}



void i2c_scan(void){
     int i; 
 
     for(i=1; i<127; i++){
         if(0 == i2c_addr_stat(i))
             dlog("Found i2c addr: 0x%.2x[%d]\r\n", i, i);
     }
}



char *itohs(const uint8_t * const buf, const unsigned int len){
    static char res[128 + 1]; /* 64 hex characters + \0 */
    unsigned int i = 0;

    /* Sanity check */
    if(2*len > sizeof(res)-1)
        return NULL;  

    /* Use printf so the identifier for the line is not present for each character */
    for(; i<len; i++)
        sprintf(&res[i*2], "%.2x", buf[i]); /* sprintf adds trailing null-terminator */
    
    return res; 
}
