#ifndef PEBBLE_H
#define PEBBLE_H

// Compatibility header to trick renderer into believing
// that this is mockup is an actual pebble

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef union GColor {
    uint8_t argb;
    struct
    {
        uint8_t a : 2;
        uint8_t r : 2;
        uint8_t g : 2;
        uint8_t b : 2;
    };
} GColor;

GColor GColorFromRGBA(int r, int g, int b, int a);

#define GColorFromRGB(r,g,b) GColorFromRGBA(r,g,b,255)

typedef struct GPoint
{
    int16_t x;
    int16_t y;
} GPoint;
#define GPoint(x, y) (__makeGPoint((x), (y)))
GPoint __makeGPoint(int16_t x, int16_t y);

typedef struct GSize
{
    int16_t w;
    int16_t h;
} GSize;
#define GSize(w, h) (__makeGSize((w), (h)))
GSize __makeGSize(int16_t w, int16_t h);

typedef struct GRect
{
    GPoint origin;
    GSize size;
} GRect;
#define GRect(origin, size) (__makeGRect((origin), (size)))
GRect __makeGRect(GPoint origin, GSize size);

enum
{
    APP_LOG_LEVEL_ERROR = 0,
    APP_LOG_LEVEL_WARNING,
    APP_LOG_LEVEL_INFO,
    APP_LOG_LEVEL_DEBUG,
    APP_LOG_LEVEL_DEBUG_VERBOSE
};
#define APP_LOG(level,fmt,...) app_log(level, __FILE__, __LINE__, fmt __VA_ARGS__)
void app_log(uint8_t log_level, const char *src_filename, int src_line_number, const char *fmt, ...);

#define TRIG_MAX_RATIO 0xffff
#define TRIG_MAX_ANGLE 0x10000

int32_t sin_lookup(int32_t angle);
int32_t cos_lookup(int32_t angle);
int32_t atan2_lookup(int16_t y, int16_t x);

#endif
