#include "pebble.h"

#include <stdio.h>
#include <stdarg.h>

GPoint __makeGPoint(int16_t x, int16_t y)
{
    GPoint point = { x, y };
    return point;
}

GSize __makeGSize(int16_t w, int16_t h)
{
    GSize size = { w, h };
    return size;
}

GRect __makeGRect(GPoint origin, GSize size)
{
    GRect rect = { origin, size };
    return rect;
}

void app_log(uint8_t log_level, const char *src_filename, int src_line_number, const char *fmt, ...)
{
    static const char* LOG_LEVEL_NAMES[] =
    {
        "ERROR",
        "WARNING",
        "INFO",
        "DEBUG",
        "VERBOSE"
    };
    fprintf(stdout, "[%s][%s:%d]:", LOG_LEVEL_NAMES[log_level], src_filename, src_line_number);

    va_list arg_list;
    va_start(arg_list, fmt);
    vfprintf(stderr, fmt, arg_list);
    va_end(arg_list);

    fputc('\n', stdout);
}
