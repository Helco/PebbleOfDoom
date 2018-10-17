#include "pebble.h"

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

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

static const double PI = 3.141592653;

int32_t atan2_lookup(int16_t y, int16_t x)
{
    return (int32_t)(atan2((double)y, (double)x) / (2 * PI) * TRIG_MAX_ANGLE);
}

int32_t sin_lookup(int32_t angle)
{
    return (int32_t)(sin((double)angle / TRIG_MAX_RATIO * 2 * PI) * TRIG_MAX_RATIO);
}

int32_t cos_lookup(int32_t angle)
{
    return (int32_t)(cos((double)angle / TRIG_MAX_RATIO * 2 * PI) * TRIG_MAX_RATIO);
}
