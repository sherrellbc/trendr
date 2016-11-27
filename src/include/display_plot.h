#ifndef _DISPLAY_PLOT_H
#define _DISPLAY_PLOT_H



/*
 * Initialize the screen in preparation of displaying plot data
 */
int display_init(void);



/*
 * Draw a plot using the provided points. If len is at least two,
 * lines will be drawn between points connecting them 
 *
 * @param p_array   : An array of points (X,Y)
 * @param len       : The length of p_array
 *
 * @return          : -1 on failure
 */
int display_draw_plot(const float p_array[][2], int len);



#endif /* _DISPLAY_PLOT_H */
