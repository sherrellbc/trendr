#include "display_plot.h"
#include "logging.h"
#include "delay.h"


/* Fixed values for testing the display plotting function */
static const int g_point_array[][2] =     {
                                                {0,  0},
                                                {20, 1},
                                                {21, 10},
                                                {23, 21},
                                                {44,  17},
                                                {57, 20},
                                                {61, 20},
                                                {62, 220},
                                                {69,  211},
                                                {79, 100},
                                                {86, 10},
                                                {90, 55} 
                                            };


int plot_ut(void){
    dlog("\t[%s] Simple Plot\r\n", __FUNCTION__); 
    display_draw_plot("Test Title", "Time", "Points", g_point_array, sizeof(g_point_array)/sizeof(int)/2); 
    dlog("\t\tDoes the screen contain a plot?\r\n");
    delay_us(2*1000*1000);
    
    return 0; 
}










