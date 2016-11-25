#include "ILI9341_t3.h"

#define TFT_DC      15 
#define TFT_CS      20
#define TFT_RST     255
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12


int screen_ut(void){
    ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);
    tft.println("Monitor testing .. ");    

    return 0;
}
