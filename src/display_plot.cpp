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

/* Rotation Modes */
#define ROTATION_TOP_LEFT       0
#define ROTATION_TOP_RIGHT      1
#define ROTATION_BOTTOM_RIGHT   2
#define ROTATION_BOTTOM_LEFT    3

/* Plot Building Params */
#define PLOT_X_ORIGIN           0.1*TFT_WIDTH
#define PLOT_Y_ORIGIN           0.1*TFT_HEIGHT

#define PLOT_XAXIS_X_END        0.1*TFT_WIDTH
#define PLOT_XAXIS_Y_END        0.9*TFT_HEIGHT

#define PLOT_YAXIS_X_END        0.9*TFT_WIDTH
#define PLOT_YAXIS_Y_END        0.1*TFT_HEIGHT

#define PIX_HEIGHT_Y_AXIS       (PLOT_YAXIS_X_END-PLOT_X_ORIGIN)
#define PIX_WIDTH_X_AXIS        (PLOT_XAXIS_Y_END-PLOT_Y_ORIGIN)
#define DEFAULT_TICK_COUNT      10
#define TICK_WIDTH              6

/* Global screen handle */
static ILI9341_t3 *tft;


void display_init(void){
    static ILI9341_t3 _tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
    _tft.begin();
    _tft.fillScreen(ILI9341_BLACK);
    //_tft.setTextColor(ILI9341_YELLOW);

    tft = &_tft; 
}



void display_write_text(int x, int y, int color, int size, const char *text, int rotation){
    int r = tft->getRotation();

    tft->setRotation(rotation);
    tft->setCursor(x,y);
    tft->setTextColor(color,ILI9341_BLACK);
    tft->println(text);
    tft->setRotation(r);
}



/* FIXME: Simple, hardcoded, should consider length of text and center appropriately */
static void display_draw_plot_outline(const char *title, const char *xtitle, const char *ytitle, int xmax, int ymax){
    char buf[16]; 
    int i, rotation;
    int x,y;

    tft->drawLine(PLOT_X_ORIGIN, PLOT_Y_ORIGIN, PLOT_XAXIS_X_END, PLOT_XAXIS_Y_END, ILI9341_WHITE);  /* X axis */
    tft->drawLine(PLOT_X_ORIGIN, PLOT_Y_ORIGIN, PLOT_YAXIS_X_END, PLOT_YAXIS_Y_END, ILI9341_WHITE);  /* Y axis */
    
    display_write_text(0.4*TFT_HEIGHT, 0.05*TFT_WIDTH, ILI9341_WHITE, 10, title, ROTATION_TOP_RIGHT);
    display_write_text(0.45*TFT_HEIGHT, (1-0.03)*TFT_WIDTH, ILI9341_WHITE, 4, xtitle, ROTATION_TOP_RIGHT);
    display_write_text(0.4*TFT_WIDTH, 0.02*TFT_HEIGHT, ILI9341_WHITE, 4, ytitle, ROTATION_TOP_LEFT);

    /* Draw Y-axis tick marks */
    rotation = tft->getRotation();
    tft->setRotation(ROTATION_TOP_LEFT);
    for(i=1; i<=DEFAULT_TICK_COUNT; i++){
        x = PLOT_X_ORIGIN + (PIX_HEIGHT_Y_AXIS/DEFAULT_TICK_COUNT)*i; 
        y = PLOT_Y_ORIGIN - (TICK_WIDTH/2); 
        tft->drawLine(x,y,x,y+TICK_WIDTH,ILI9341_WHITE);

        /* Write the value in text alongside the tick */
        sprintf(buf, "%d", (int) (ymax/DEFAULT_TICK_COUNT)*i);
        display_write_text(x-5, y-10, ILI9341_WHITE, 1, buf, ROTATION_TOP_LEFT);
    }
    tft->setRotation(rotation);

    /* Draw X-axis tick marks */
    tft->setRotation(ROTATION_TOP_RIGHT);
    for(i=1; i<=DEFAULT_TICK_COUNT; i++){
        x = PLOT_Y_ORIGIN + (PIX_WIDTH_X_AXIS/DEFAULT_TICK_COUNT)*i; 
        y = PLOT_X_ORIGIN + PIX_HEIGHT_Y_AXIS - (TICK_WIDTH/2); 
        tft->drawLine(x,y,x,y+TICK_WIDTH+2,ILI9341_WHITE); //Without +2 the tick was uneven .. for some reason 

        /* Write the value in text alongside the tick */
        sprintf(buf, "%d", (int) (xmax/DEFAULT_TICK_COUNT)*i);
        display_write_text(x-5, y+TICK_WIDTH+2+3, ILI9341_WHITE, 1, buf, ROTATION_TOP_RIGHT);
    }
    tft->setRotation(rotation);

    /* Draw min and max values next to axis endpoints */
}



void display_clear(void){
    tft->fillScreen(ILI9341_BLACK);
}



/* TODO: Add name */
/* FIXME: Simple, hardcoded */
int display_draw_plot(const char *title, const char *xtitle, const char *ytitle, const int p_array[][2], int len){
    int max_x, max_y; 
    int idx;
    int scaled_x, scaled_y; 
    int prev_scaled_x, prev_scaled_y;

    /* Clear everything. This implementation is simple; We are not using damaged areas */
    display_clear();

    /* Find range of data */ 
    max_x = find_max_int((int *) p_array, len, 2, 0);
    max_y = find_max_int((int *) p_array, len, 2, 1);
    dlog("Maxes=%d,%d\r\n", max_x,max_y);
    

    /* Draw generic plot outline, tick marks, title, etc */
    display_draw_plot_outline(title, xtitle, ytitle, max_x, max_y);

    /* Iterate over the data points and put them on the screen */
    for(idx=0; idx<len; idx++){
        scaled_y = 0.1*TFT_WIDTH + 0.8*TFT_WIDTH*((float)p_array[idx][1]/max_y); 
        scaled_x = 0.1*TFT_HEIGHT + 0.8*TFT_HEIGHT*((float)p_array[idx][0]/max_x); 
    
        /* Place the marker for the current x,y pair */
        tft->drawCircle(scaled_y, scaled_x, 3, ILI9341_RED);   
        tft->fillCircle(scaled_y, scaled_x, 3, ILI9341_RED);        
    
        /* Connect the current mairker with the last using a line */
        if(0 != idx)
            tft->drawLine(scaled_y, scaled_x, prev_scaled_y, prev_scaled_x, ILI9341_YELLOW);
    
        /* Save the previous points so we can draw a line on the next iteration */
        prev_scaled_x = scaled_x; 
        prev_scaled_y = scaled_y; 
    }

    return 0;    
}


//        dlog("Plotting point (%d,%d) -> (%d,%d)\r\n", (int)p_array[idx][0], (int)p_array[idx][1], scaled_x, scaled_y);


