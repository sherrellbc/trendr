#ifndef _AT24CX_H
#define _AT24CX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define AT24CX_PAGE_SIZE    64
#define AT24CX_NUM_PAGES    512
    

/* i2c bus address */
#define DEV_A0              0
#define DEV_A1              1
#define AT24CX_I2C_ADDR     0x50 | DEV_A1<<1 | DEV_A0


/*
 * TODO: All writes/reads are limited by the i2c_t3 I2C_TX_BUFFER_LENGTH buffer limit
 *  - Ensure this implementation properly segments larger requests
 *
 * TODO: All write implementations do not take into account the current address and 
 *  the requested number of bytes. Segmenting the writes across pages will require
 *  some attention.  
 */
 



enum at24cx_err {
    AT24CX_SUCCESS = 0,
    AT24CX_NO_RESPONSE,
    AT24CX_ERROR,   /* Generic error, for now */
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
 * @param len   : Number of bytes to read
 *
 * @return  : -1 on error
 */
int at24cx_read(uint16_t addr, uint8_t *buf, int len); 



/*
 * Write primitive 
 * 
 * @param addr  : Address to write
 * @param buf   : Buffer containing data to write
 * @param len   : Number of bytes to write
 *
 * @return  : -1 on error
 */ 
int at24cx_write(uint16_t addr, uint8_t *buf, int len); 



/*
 * Erase (set to 0x00) memory
 *
 * @param addr  : Address to begin erasing
 * @param len   : Number of bytes to erase
 *
 * @error   : -1 on error
 */
int at24cx_erase(uint16_t addr, int len);



/*
 * Erase a specified page 
 *
 * @param page_num  :   The page to erase
 *
 * @return  : -1 on failure
 */
int at24cx_page_erase(uint16_t page_num);



/*
 * Perform a full erase
 *
 * @return  : -1 on error
 */
int at24cx_full_erase(void);



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
