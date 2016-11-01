#include "at24cx.h"
#include "i2c_t3.h"
#include "logging.h"


/* Global error code reference */
static enum at24cx_err g_err_code = AT24CX_SUCCESS; 

/* Internal pointer tracking the remote current address pointer */
static uint16_t g_current_addr = 0x00; 



/*
 * Set the current internal state of the address register
 *
 * @param addr  : The address to set 
 */
static int at24cx_set_addr_register(uint16_t addr){
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    Wire.write( (uint8_t) ((addr>>8) & 0x00FF) );
    Wire.write( (uint8_t) (addr & 0x00FF) );
    Wire.endTransmission();

    if(0 != Wire.getError()){
        g_err_code = AT24CX_ERROR; 
        return -1; 
    }
    
    return 0; 
}



int at24cx_init(){
    /* Ensure the device is on the bus */
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    Wire.endTransmission(); 
    if(0 != Wire.getError()){
        g_err_code = AT24CX_NO_RESPONSE; 
        return -1;  
    }
    
    /* Set the internal register to 0x00 by default */
    if(0 != at24cx_set_addr_register(0x0000))
        return -1; 
    
    return 0; 
}



int at24cx_write(uint16_t addr, uint8_t *buf, int count){
//    int i;
//
//    if(0 != at24cx_set_addr_register(addr))
//        return -1; 
//
//    /* Loop and read "count" bytes of data */
//    Wire.beginTransmission(AT24CX_I2C_ADDR);
//    for(i=0; i<count; i++)
//        Wire.write(buf[i]);
//    Wire.sendTransmission();
//
//    /* Check for error */
//    if(0 != Wire.getError()){
//        g_err_code = AT24CX_NO_RESPONSE; 
//        return -1;  
//    }

    return 0; 
} 



int at24cx_read(uint16_t addr, uint8_t *buf, int count){
    int i;

    if(0 != at24cx_set_addr_register(addr))
        return -1; 

    /* Loop and read "count" bytes of data */
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    for(i=0; i<count; i++)
        buf[i] = Wire.read();
    Wire.sendTransmission();

    /* Check for error */
    if(0 != Wire.getError()){
        g_err_code = AT24CX_NO_RESPONSE; 
        return -1;  
    }

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

