#ifndef PLATFORM_H
#define PLATFORM_H

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

#endif
