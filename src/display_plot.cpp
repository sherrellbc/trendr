#include "ILI9341_t3.h"
#include "logging.h"
#include "util.h"

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

    _tft.drawLine(0.1*TFT_WIDTH, 0.1*TFT_HEIGHT, 0.1*TFT_WIDTH, 0.9*TFT_HEIGHT, ILI9341_WHITE); 
    _tft.drawLine(0.1*TFT_WIDTH, 0.1*TFT_HEIGHT, 0.9*TFT_WIDTH, 0.1*TFT_HEIGHT, ILI9341_WHITE);
    _tft.drawCircle(50,100,1,ILI9341_WHITE);
    _tft.fillCircle(50,100,1,ILI9341_WHITE);
 
    tft = &_tft; 
    //tft->drawLine(0,0,239,319,ILI9341_WHITE); 
    return 0;
}



static int display_draw_plot_outline(void){
    dlog("%s: Drawing outline!\r\n", __FUNCTION__);

    tft->drawLine(0,0,239,319,ILI9341_WHITE); 
    //tft->drawLine(0.1*TFT_WIDTH, 0.1*TFT_HEIGHT, 0.1*TFT_WIDTH, 0.9*TFT_HEIGHT, ILI9341_WHITE); 
    //tft->drawLine(0.1*TFT_WIDTH, 0.1*TFT_HEIGHT, 0.9*TFT_WIDTH, 0.1*TFT_HEIGHT, ILI9341_WHITE);

    return 0;  
}



/* TODO: Add name */
int display_draw_plot(const float p_array[][2], int len){
    float max, min; 
    int idx;

    /* Draw generic plot outline */
    display_draw_plot_outline();
 
    /* Find range of data */ 
    max = find_max_float((float*) p_array, len, 2, 1);
    min = find_min_float((float*) p_array, len, 2, 1);

    return 0;    
}

