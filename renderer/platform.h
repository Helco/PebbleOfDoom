#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdint.h>

// Platform/Compiler-specific utilities

/* Declare unused parameter, to be used like
 * ```
 * void bla(int foo, int bar) {
 *     UNUSED(foo, bar);
 * }
 * ```
 */
#define UNUSED(...) do { \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunused-value\"") \
    (void)(__VA_ARGS__); \
    _Pragma("GCC diagnostic pop") \
    } while(0)

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define FOURCC(a,b,c,d)  \
    ((uint32_t)(((a & 0xff) << 24) | ((b & 0xff) << 16) | ((c & 0xff) << 8) | ((d & 0xff) << 0)))
#else
#define FOURCC(a,b,c,d)  \
    ((uint32_t)(((a & 0xff) << 0) | ((b & 0xff) << 8) | ((c & 0xff) << 16) | ((d & 0xff) << 24)))
#endif

#ifdef POD_PEBBLE
#include <pebble.h>
#ifdef assert
#undef assert
#endif
// unfortunately no other way than to immediately exit the pebble than to crash it
#define assert(cond) (!!(cond) || ( \
    APP_LOG(APP_LOG_LEVEL_ERROR, "Assertion failed: %s\n", #cond), \
    APP_LOG(APP_LOG_LEVEL_ERROR, "Bring it dooooown %d", *((int*)-1)), \
    false))
#else
#include <assert.h>
#endif

#endif
