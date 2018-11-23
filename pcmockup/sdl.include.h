/* WARNING DEVELOPER RANT INCOMING
 * if you find any, *ANY* way to do this via CMake, feel free to send a PR
 * if you work on this increase this work hour counter: 5
 * things which were tried:
 *  - use target_include_directories SYSTEM
 *  - change cmake system include flag
 *  - use target_compile_definitions
 *  - use compile_definitions
 *  - use CMAKE_C_FLAGS
 * most with three or four different ways trying to properly escape doublequote
 * without them being escaped again in a later automatic step
 * also another great thing to track - cmake issues related to this problem:
 *  - #18272
 *  - #17808
 *  - #17904
 *  - #17878
 * DEVELOPER RANT DONE
 */

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wpragma-pack"
#pragma GCC diagnostic ignored "-Wmacro-redefined"
#endif
#include<SDL.h>
#pragma GCC diagnostic pop
