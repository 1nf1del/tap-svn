#ifndef OSDLIB_H
#define OSDLIB_H

/**
 * Draws a measuring grid across the given region with the distance 'd'
 * between each line.
 */
void osd_grid(int rgn, int d, int col);

/**
 * Draws a line of the given colour in the region.
 */
void osd_line(int rgn, int Ax, int Ay, int Bx, int By, int col);

#endif
