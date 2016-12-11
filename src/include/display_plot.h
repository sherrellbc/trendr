#ifndef _DISPLAY_PLOT_H
#define _DISPLAY_PLOT_H

//TODO: "Plot" structure containing axis titles, tick marks, data pointers, etc


/*
 * Initialize the screen in preparation of displaying plot data
 */
int display_init(void);



/*
 * Clear the display (to BLACK)
 */
void display_clear(void);



/*
 * Draw a plot using the provided points. If len is at least two,
 * lines will be drawn between points connecting them 
 *
 * @param title     : The title of the new plot being drawn
 * @param xtitle    : X-axis title
 * @param ytitle    : Y-axis title
 * @param p_array   : An array of points (X,Y)
 * @param len       : The length of p_array
 *
 * @return          : -1 on failure
 */
int display_draw_plot(const char *title, const char *xtitle, const char *ytitle, const int p_array[][2], int len);



#endif /* _DISPLAY_PLOT_H */
