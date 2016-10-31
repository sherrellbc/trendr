#include "at24cx.h"
#include "i2c_t3.h"
#include "logging.h"


/* Global error code reference */
static enum at24cx_err g_err_code = AT24CX_SUCCESS; 

/* Internal pointer tracking the remote current address pointer */
static uint16_t g_current_addr = 0x00; 



int at24cx_init(){
    /* Ensure the device is on the bus */
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    Wire.endTransmission(); 
    if(0 != Wire.getError()){
        g_err_code = AT24CX_NO_DEV_AT_ADDR; 
        return -1;  
    }

    /* Set the default address pointer to 0 */
    return 0; 
}



int at24cx_write(uint16_t *dst, void *buf, int count){
    return 0; 
} 



int at24cx_read(uint16_t *addr, void *buf, int count){
    return 0; 
}



int at24cx_page_erase(){
    return 0;
}  



int at24cx_full_erase(){
    return 0; 
}



enum at24cx_err at24cx_read_error(void){
    return g_err_code;
}

