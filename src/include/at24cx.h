#ifndef _AT24CX_H
#define _AT24CX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define PAGE_SIZE       64

/* i2c bus address */
#define DEV_A0              0
#define DEV_A1              1
#define AT24CX_I2C_ADDR     82//0x50 | DEV_A1<<1 | DEV_A0


enum at24cx_err {
    AT24CX_SUCCESS = 0,
    AT24CX_NO_DEV_AT_ADDR,
};



/*
 * Initialize the AT24Cx
 *
 * @return  : -1 on error
 */
int at24cx_init();



/*
 * Read primitive 
 *
 * @param addr  : Address to read from
 * @param buf   : Preallocated destination buffer to place data read from the device
 * @param count : Number of bytes to read
 *
 * @return  : -1 on error
 */
int at24cx_read(uint16_t *addr, void *buf, int count); 



/*
 * Write primitive 
 * 
 * @param dst   : Address to write
 * @param buf   : Buffer containing data to write
 * @param count : Number of bytes to write
 *
 * @return  : -1 on error
 */ 
int at24cx_write(uint16_t *dst, void *buf, int count); 



/*
 * Query the error mode
 *
 * @return : The last error mode
 */
enum at24cx_err at24cx_read_error(void);

#ifdef __cplusplus
}
#endif

#endif /* _AT24CX_H */ 
