#include "pcmockup.h"

/* WindowGrid is a grid-layout system based on
 * alternating incremented grid dimensions, eg:
 *
 *  2*2     3*2      3*3
 *  xx      xxx      xxx
 *  xx  ->  xxx  ->  xxx
 *                   xxx
 *
 * Thus we have a grid which can be adjusted
 * dynamically to answer growing window demand.
 * There are a few important values:
 *
 * Grid size index
 * i  0 1 2 3 4  5  6  7  8
 * Base size s = i / 2 + 1
 * s  1 1 2 2 3  3  4  4  5
 * Grid width w = s + (i % 2)
 * w  1 2 2 3 3  4  4  5  5
 * Grid height h = s
 * w  1 1 2 2 3  3  4  4  5
 * Maximum number of windows m = w * h
 * m  1 2 4 6 9 12 16 20 25
 *
 * As further demonstration all possible window
 * counts per size index
 *    1 2 3 5 7 10 13 17 21
 *        4 6 8 11 14 18 22
 *            9 12 15 19 23
 *                 16 20 24
 *                       25
 */

#define WINDOWGRID_PADDING (GSize(32, 64))

GSize windowGrid_getGridSize(const WindowGrid* grid)
{
    int i = 0, w, h;
    do {
        h = i / 2;
        w = h + (i % 2);
        i++;
    } while(w * h <= grid->windowCount);
    return GSize(w, h);
}

GRect windowGrid_getSingleBounds(const WindowGrid* grid, int windowI)
{
    GSize gridSize = windowGrid_getGridSize(grid);
    GRect totalBounds = { .origin = { 0, 0 }, .size = { grid->totalSize.w, grid->totalSize.h } };
    totalBounds = findBestFit(totalBounds, (float)gridSize.w / gridSize.h);
    if (windowI < 0)
        windowI = gridSize.w * gridSize.h + windowI;

    GRect bounds;
    bounds.size.w = totalBounds.size.w / gridSize.w;
    bounds.size.h = totalBounds.size.h / gridSize.h;
    bounds.origin.x = totalBounds.origin.x + bounds.size.w * (windowI % gridSize.w);
    bounds.origin.y = totalBounds.origin.y + bounds.size.h * (windowI / gridSize.w);

    return padRect(bounds, WINDOWGRID_PADDING);
}
