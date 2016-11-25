#ifndef _UNIT_TESTS_H
#define _UNIT_TESTS_H


#include <stdint.h>


/* Flags representing various modes of unit test */
#define AP_UTS      0x01
#define EEPROM_UTS  0x02


/*
 * Entry point to execute the prescribed unit tests
 *
 * @param which : A bitwise OR'd flag representing which unit tests to execute
 */
void do_unit_tests(uint32_t which);



/*
 * Write content to the ili9341 display
 * 
 * @return  : -1 on failure
 */
int screen_ut(void);


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Connect to an AP for 5 seconds and disconnect
 *
 * @return  : -1 on failure
 */
int ap_connect_ut(void);


/*
 * Perform a read/write/read memory test
 *
 * @return  : -1 on failure
 */
int mem_ut(void);


#ifdef __cplusplus
}
#endif

#endif /* _UNIT_TESTS_H */
