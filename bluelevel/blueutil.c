#include "bluelevel_internal.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BLUE_SAFE_ALLOC_MAGIC ((unsigned int )0xC0FFEE)

typedef struct BlueSafeHeader {
    unsigned int magic;
    unsigned int size;
    char data[];
} BlueSafeHeader;

#ifdef _DEBUG
#define BLUE_SAFE_ASSERT(cond) assert(cond)
#else
#define BLUE_SAFE_ASSERT(cond) \
    do { \
        if (!!(cond)) break; \
        fprintf(stderr, "%s:%d - Assertion failed: %s\n", __FILE__, __LINE__, #cond); \
        exit(-1); \
    } while(false)
#endif

void* blueSafeAlloc(unsigned int size)
{
    BlueSafeHeader* header = (BlueSafeHeader*)malloc(sizeof(BlueSafeHeader) + size);
    BLUE_SAFE_ASSERT(header != NULL);
    header->magic = BLUE_SAFE_ALLOC_MAGIC;
    header->size = size;
    memset(header->data, 0, size);
    return header->data;
}

BlueSafeHeader* blueSafeGetHeader(void* ptr)
{
    BLUE_SAFE_ASSERT(ptr != NULL);
    BlueSafeHeader* header = (BlueSafeHeader*)((char*)ptr - sizeof(BlueSafeHeader));
    BLUE_SAFE_ASSERT(header->magic == BLUE_SAFE_ALLOC_MAGIC);
    return header;
}

void* blueSafeRealloc(void* ptr, unsigned int newSize)
{
    if (ptr == NULL)
        return blueSafeAlloc(newSize);
    BlueSafeHeader* header = blueSafeGetHeader(ptr);
    BLUE_SAFE_ASSERT(header->magic == BLUE_SAFE_ALLOC_MAGIC);
    ptr = realloc(ptr, newSize);
    BLUE_SAFE_ASSERT(ptr != NULL);
    if (newSize > header->size)
        memset(header->data + header->size, 0, newSize - header->size);
    return ptr;
}

void blueSafeFree(void* ptr)
{
    BLUE_SAFE_ASSERT(ptr != NULL);
    BlueSafeHeader* header = blueSafeGetHeader(ptr);
    BLUE_SAFE_ASSERT(header->magic == BLUE_SAFE_ALLOC_MAGIC);
    memset(header->data, 0xcd, header->size);
    free(ptr);
}

char* blueSafeStrdup(const char* str)
{
    size_t len = strlen(str);
    char* clone = (char*)blueSafeAlloc(len + 1);
    BLUE_SAFE_ASSERT(clone != NULL);
    memcpy(clone, str, len);
    clone[len] = 0;
    return clone;
}
