#ifndef BLUEUTIL_H
#define BLUEUTIL_H
// bluelevel internal file, do not use externally

// these never fail and are always 0-initialized
void* blueSafeAlloc(unsigned int size);
void* blueSafeRealloc(void* ptr, unsigned int newSize);
void blueSafeFree(void* ptr);
char* blueSafeStrdup(const char* str);

#endif
