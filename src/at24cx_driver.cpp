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
inline uint16_t at24cx_page_to_addr(uint16_t page_num){
    return (page_num*AT24CX_PAGE_SIZE); 
}



/*
 * Convert the given physical address to its corresponding page number
 *
 * @param addr  :   The physical address to convert
 *
 * @return  : The page number corresponding to the passed address
 */
inline int at24cx_addr_to_page(uint16_t addr){
    return (addr/AT24CX_PAGE_SIZE);
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
#ifdef MEM_DEBUG
    dlog("Requested to read %d bytes\r\n", len);
#endif
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
 * FIXME: Currently implements recursive write (no practical for large writes
 * - We would have one stack frame per page if doing a write to the entire address space
 * - Internal addr counter of the at24cx rolls over to the same page 
 */
int at24cx_write(uint16_t addr, uint8_t *buf, int len){
    int page_num;
    int remaining_bytes;
    
    /* Compute to which page the address corresponds */
    page_num = at24cx_addr_to_page(addr);

    /* Compute the number of bytes remaining before the end of the current page */
    remaining_bytes = AT24CX_PAGE_SIZE-(addr-AT24CX_PAGE_SIZE*page_num); 
#if MEM_DEBUG
    dlog("Requested write; page=%d (addr=%p), rem_bytes=%d, len=%d\r\n", page_num, addr, remaining_bytes, len); 
#endif

    /* Wait for the device to be ready for the requsted write operation */
    while(!at24cx_ready())
        ;

    /* If the write will not fit on the current page, recursve */
    if(remaining_bytes < len){
#if MEM_DEBUG
         int total_pages_to_write = (len-remaining_bytes)/AT24CX_PAGE_SIZE;
         dlog("Writes across page boundaries; Requires the current partial page and %d additional pages\r\n", total_pages_to_write);
#endif
         
         /* Recursive call */ 
         at24cx_write(at24cx_page_to_addr(page_num+1), buf+remaining_bytes, len-remaining_bytes);
    
        /* Because of the recursion we must wait after a return to ensure the device is ready before writing */
        while(!at24cx_ready())
            ;
    }

#if MEM_DEBUG
    dlog("Request fits; Writing %d bytes to address=%p\r\n", remaining_bytes, addr);
#endif


    /* Set the destination address and write "len" bytes */
    Wire.beginTransmission(AT24CX_I2C_ADDR);
    Wire.send( (uint8_t) ((addr>>8) & 0x00FF) );
    Wire.send( (uint8_t) (addr      & 0x00FF) );
    
    if(NULL == buf){
        while(len--)
            Wire.write((uint8_t)0);
    }else
        Wire.write(buf, remaining_bytes); 
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

