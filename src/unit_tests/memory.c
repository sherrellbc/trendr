#include "at24cx.h"
#include "logging.h"
#include "util.h"
#include <string.h>


/* Test string to write and read from memory */
static const char * test_str = "DEADBEEFDEADBEEFDEADBEEFDEADBEEF"; 

/* Test memory address */
static const uint16_t test_addr = 512*10; 

/* Note: This function assumes no valuable data is at the test location! */ 
int mem_ut(void){
    uint8_t buf[32] = {0}; 
    
    dlog("Starting memory (at24cx) test\r\n");
    
    /* Read original */
    at24cx_read(test_addr, buf, sizeof(buf));
    dlog("Read: %s\r\n", itohs(buf, 32));

    /* Write pattern */
    at24cx_write(test_addr, (uint8_t *)test_str, strlen(test_str));   
 
    /* Read pattern -- check if same as what was written */
    at24cx_read(test_addr, buf, sizeof(buf));   
    dlog("Read: %s\r\n", itohs(buf, 32));

    if(0 != memcmp(buf, test_str, strlen(test_str))){
        dlog("%s unit-test failed\r\n", __FUNCTION__);
        return -1; 
    }

    /* Erase our changes */
    at24cx_erase(test_addr, sizeof(buf));
    
    /* Read data back to ensure it has been erased */
    /* TODO: Loop and ensure memory is cleared */
    at24cx_read(test_addr, buf, sizeof(buf));   
    dlog("Read: %s\r\n", itohs(buf, 32));

    return 0; 
}


