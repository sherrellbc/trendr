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
static int at24cx_set_read_addr(uint16_t addr){
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



/*
 * The at24cx does not respond to bus queries while busy. As 
 * such, we can discern device readiness by checking for a 
 * bus address confirmation
 *
 * @return  :   1 if the at24cx is ready, otherwise 0
 */
static int at24cx_ready(void){
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    Wire.endTransmission();
    
    /* Device is not ready */
    if(0 != Wire.getError())
        return 0;
    
    return 1; 
}



/*
 * Convert the given page number to its respective physical address
 *
 * @param page_num  :   The page number to convert
 *
 * @return  : The physical address corresponding to page_num
 */
static uint16_t at24cx_page_to_addr(uint16_t page_num){
    return (page_num*AT24CX_PAGE_SIZE); 
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
    if(0 != at24cx_set_read_addr(0x0000))
        return -1; 
    
    return 0; 
}



int at24cx_read(uint16_t addr, uint8_t *buf, int len){
    int i;

    /* Wait for the device to be ready for the requsted read operation */
    while(!at24cx_ready())
        ;

    if(0 != at24cx_set_read_addr(addr))
        return -1; 

    /* Loop and read "len" bytes of data */
    if(len != (int) Wire.requestFrom(AT24CX_I2C_ADDR, len))
        return -1; 

    for(i=0; i<len; i++)
        buf[i] = Wire.read();

    /* Check for error */
    if(0 != Wire.getError()){
        g_err_code = AT24CX_NO_RESPONSE; 
        return -1;  
    }

    g_current_addr = addr + len; 
    return 0; 
}



static int at24cx_page_write(uint16_t addr, uint8_t *buf){
    /* Wait for the device to be ready for the requsted write operation */
    while(!at24cx_ready())
        ;
                                                                           
    /* Set the destination address and write "len" bytes */
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    Wire.send( (uint8_t) ((addr>>8) & 0x00FF) );
    Wire.send( (uint8_t) (addr      & 0x00FF) );
    Wire.write(buf, AT24CX_PAGE_SIZE); 
    Wire.endTransmission();
                                                                           
    /* Check for error */
    if(0 != Wire.getError()){
        g_err_code = AT24CX_NO_RESPONSE; 
        return -1;  
    }
                                                                           
    g_current_addr = addr + AT24CX_PAGE_SIZE;
    return 0; 
}



/* 
 * TODO: Implementation is broken for writing multiple pages 
 *   -- Internal addr counter of the at24cx rolls over to the same page 
*/
int at24cx_write(uint16_t addr, uint8_t *buf, int len){
    /* Wait for the device to be ready for the requsted write operation */
    while(!at24cx_ready())
        ;

    /* Set the destination address and write "len" bytes */
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    Wire.send( (uint8_t) ((addr>>8) & 0x00FF) );
    Wire.send( (uint8_t) (addr      & 0x00FF) );
    
    if(NULL == buf){
        while(len--)
            Wire.write((uint8_t)0);
    }else
        Wire.write(buf, len); 
    Wire.endTransmission();

    /* Check for error */
    if(0 != Wire.getError()){
        g_err_code = AT24CX_NO_RESPONSE; 
        return -1;  
    }

    g_current_addr = addr + len; 
    return 0; 
} 



int at24cx_erase(uint16_t addr, int len){
    return at24cx_write(addr, NULL, len); 
}  



int at24cx_page_erase(uint16_t page_num){
    return at24cx_erase(at24cx_page_to_addr(page_num), AT24CX_PAGE_SIZE);
}  



int at24cx_full_erase(){
    int i;

    for(i=0; i<AT24CX_NUM_PAGES; i++){
        if(0 != at24cx_erase(at24cx_page_to_addr(i), AT24CX_PAGE_SIZE))
            return -1;
    }    
    
    return 0; 
}



enum at24cx_err at24cx_read_error(void){
    return g_err_code;
}

