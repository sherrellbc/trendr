#include "ILI9341_t3.h"
#include "logging.h"

#define TFT_DC      15 
#define TFT_CS      20
#define TFT_RST     255
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12

/* Characteristics of the TFT screen */
#define TFT_WIDTH   239
#define TFT_HEIGHT  319 

/* Global screen handle */
static ILI9341_t3 *tft;


int display_init(void){
    ILI9341_t3 _tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
    _tft.begin();
    _tft.fillScreen(ILI9341_BLACK);
    _tft.setTextColor(ILI9341_YELLOW);

    tft = &_tft; 
    return 0;
}



