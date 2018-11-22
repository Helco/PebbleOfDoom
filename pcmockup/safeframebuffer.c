#include "pcmockup.h"
#include <assert.h>
#include <string.h>

/* To avoid confusion which pointers to free and which not
 * there is only a single pointer to a block where all (5)
 * buffers are stored in the following order:
 *
 * 1. Canary backup front buffer
 * 2. Canary back buffer
 * 3. Screen buffer
 * 4. Canary front buffer
 * 5. Canary backup back buffer
 *
 * A canary buffer and its backup are therefore spaced apart
 * wide, to ensure a bug writing far out one side to not
 * override the backup it is checked against and thus pass
 */

typedef enum SafeFramebufferId {
    SF_CANARY_FRONT_BACKUP = 0,
    SF_CANARY_BACK,
    SF_SCREEN,
    SF_CANARY_FRONT,
    SF_CANARY_BACK_BACKUP,
    SF_BUFFERCOUNT
} SafeFramebufferId;

struct SafeFramebuffer
{
    GColor* memoryStart;
    GSize size;
    int canarySize; // in framebuffer sizes
};

static int prv_safeFramebuffer_getByteSize(GSize size)
{
    return sizeof(GColor) * size.w * size.h;
}

static GColor* prv_safeFramebuffer_getBuffer(SafeFramebuffer* me, SafeFramebufferId bufferId)
{
    int framebufferSize = prv_safeFramebuffer_getByteSize(me->size);
    GColor* buffer = me->memoryStart + (int)bufferId * framebufferSize * me->canarySize;

    // screen buffer is only one framebuffer size big
    if (bufferId > SF_SCREEN)
        buffer -= (me->canarySize - 1) * framebufferSize;

    return buffer;
}

SafeFramebuffer* safeFramebuffer_init(GSize size, int canarySize)
{
    SafeFramebuffer* me = (SafeFramebuffer*)malloc(sizeof(SafeFramebuffer));
    if (me == NULL)
        return NULL;
    memset(me, 0, sizeof(SafeFramebuffer));

    int framebufferSize = prv_safeFramebuffer_getByteSize(size);
    int totalSize = framebufferSize + 4 * canarySize * framebufferSize;
    me->memoryStart = (GColor*)malloc(totalSize);
    if (me->memoryStart == NULL)
    {
        fprintf(stderr, "Could not allocate safe framebuffers\n");
        safeFramebuffer_free(me);
        return NULL;
    }

    me->canarySize = canarySize;
    me->size = size;
    return me;
}

void safeFramebuffer_free(SafeFramebuffer* me)
{
    if (me == NULL)
        return;
    if (me->memoryStart != NULL)
        free(me->memoryStart);
    free(me);
}

GColor* safeFramebuffer_getScreenBuffer(SafeFramebuffer* me)
{
    return prv_safeFramebuffer_getBuffer(me, SF_SCREEN);
}

static void prv_fillBufferRandom(void* buffer, int byteCount)
{
    uint8_t* curByte = (uint8_t*)buffer;
    while(byteCount--)
        *(curByte++) = rand();
}

static void prv_safeFramebuffer_prepareSide(SafeFramebuffer* me, SafeFramebufferId canaryId, SafeFramebufferId backupId)
{
    GColor* canary = prv_safeFramebuffer_getBuffer(me, canaryId);
    GColor* backup = prv_safeFramebuffer_getBuffer(me, backupId);
    int framebufferSize = prv_safeFramebuffer_getByteSize(me->size);
    prv_fillBufferRandom(canary, framebufferSize);
    memcpy(backup, canary, framebufferSize);
}

void safeFramebuffer_prepare(SafeFramebuffer* me)
{
    prv_safeFramebuffer_prepareSide(me, SF_CANARY_BACK, SF_CANARY_BACK_BACKUP);
    prv_safeFramebuffer_prepareSide(me, SF_CANARY_FRONT, SF_CANARY_FRONT_BACKUP);
}

static void prv_safeFramebuffer_checkSide(SafeFramebuffer* me, SafeFramebufferId canaryId, SafeFramebufferId backupId)
{
    GColor* canary = prv_safeFramebuffer_getBuffer(me, canaryId);
    GColor* backup = prv_safeFramebuffer_getBuffer(me, backupId);
    int canaryByteSize = prv_safeFramebuffer_getByteSize(me->size) * me->canarySize;
    assert(memcmp(canary, backup, canaryByteSize) == 0);
}

void safeFramebuffer_check(SafeFramebuffer* me)
{
    prv_safeFramebuffer_checkSide(me, SF_CANARY_BACK, SF_CANARY_BACK_BACKUP);
    prv_safeFramebuffer_checkSide(me, SF_CANARY_FRONT, SF_CANARY_FRONT_BACKUP);
}
