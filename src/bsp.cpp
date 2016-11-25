#include "esp8266_driver.h"
#include "logging.h"
#include "at24cx.h"
#include "display_plot.h"
#include "i2c_t3.h"


void bsp_init(void){
    /* i2c */
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
    Wire.setDefaultTimeout(200000);

    /* Memory */
    if(0 != at24cx_init())
        dlog("AT24CX did not properly init\r\n");

    /* WiFi Module */
    esp8266_init(); 
    
    /* Display */
    display_init();
}


//#define  LED_ON		GPIOC_PSOR=(1<<5)
//#define  LED_OFF	GPIOC_PCOR=(1<<5)
//PORTC_PCR5 = PORT_PCR_MUX(0x1); // LED is on PC5 (pin 13), config as GPIO (alt = 1)
//GPIOC_PDDR = (1<<5);			// make this an output pin
//LED_OFF;						// start with LED off
