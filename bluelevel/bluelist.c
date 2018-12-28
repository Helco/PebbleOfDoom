#include "bluelist.h"
#include "blueutil.h"
#include "stb.include.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

typedef struct BlueListEntry {
    BlueEntryID id;
    char data[];
} BlueListEntry;

struct BlueList {
    int count;
    int capacity;
    BlueEntryID nextId;
    int changeCount;
    int elementSize;
    char* typeName;
    RawBlueListDestructor destructor;
    void* destructorUserdata;
    BlueListEntry* entries;
};

static int blueList_getNextCapacity(int oldCapacity) {
    const int CHUNK_SIZE = 16;
    return oldCapacity <= 0
        ? CHUNK_SIZE
        : oldCapacity >= 128
            ? oldCapacity + CHUNK_SIZE
            : oldCapacity * 2;
}

BlueList* raw_blueList_new(const char* typeName, int elementSize)
{
    BlueList* list = (BlueList*)blueSafeAlloc(sizeof(BlueList));
    list->typeName = blueSafeStrdup(typeName);

    list->count = list->capacity = 0;
    list->nextId = 1;
    list->changeCount = 0;
    list->elementSize = elementSize;
    list->entries = NULL;
    return list;
}

void blueList_free(BlueList* list)
{
    assert(list != NULL);

    if (list->count > 0 && list->destructor != NULL) {
        for (int i = 0; i < list->count; i++)
            list->destructor(raw_blueList_get(list, i), list->destructorUserdata);
    }
    blueSafeFree(list->typeName);
    if (list->entries != NULL)
        blueSafeFree(list->entries);
    blueSafeFree(list);
}

void raw_blueList_setDestructor(BlueList* list, RawBlueListDestructor destructor, void* userdata)
{
    assert(list != NULL);
    list->destructor = destructor;
    list->destructorUserdata = userdata;
}

void raw_blueList_requireType(BlueList* list, const char* typeName)
{
    assert(list != NULL && typeName != NULL);
    assert(strcmp(list->typeName, typeName) == 0);
}

void* raw_blueList_get(BlueList* list, int index)
{
    assert(list != NULL && index >= 0 && index < list->count);
    return list->entries[index].data;
}

const void* raw_blueList_constget(const BlueList* list, int index)
{
    assert(list != NULL && index >= 0 && index < list->count);
    return list->entries[index].data;
}

int blueList_add(BlueList* list, const void* element)
{
    return blueList_addWithId(list, element, list->nextId++);
}

int blueList_addWithId(BlueList* list, const void* element, BlueEntryID id)
{
    assert(list != NULL && list->count <= list->capacity);
    assert(element != NULL);
    if (list->count == list->capacity) {
        list->capacity = blueList_getNextCapacity(list->capacity);
        const int memorySize = list->capacity * (sizeof(BlueListEntry) + list->elementSize);
        list->entries = (BlueListEntry*)blueSafeRealloc(list->entries, memorySize);
    }

    int index = list->count++;
    list->entries[index].id = id;
    memcpy(&list->entries[index], element, list->elementSize);
    list->changeCount++;
    return index;
}

void blueList_swap(BlueList* list, int fromIndex, int toIndex)
{
    assert(list != NULL);
    assert(fromIndex >= 0 && fromIndex < list->count);
    assert(toIndex >= 0 && toIndex < list->count);
    if (toIndex == fromIndex)
        return;

    stb_swap(
        &list->entries[fromIndex],
        &list->entries[toIndex],
        sizeof(BlueListEntry) + list->elementSize);
    list->changeCount++;
}

void blueList_removeByIndex(BlueList* list, int index)
{
    void* element = raw_blueList_get(list, index);
    if (list->destructor != NULL)
        list->destructor(element, list->destructorUserdata);

    int bytesToMove = (list->count - index - 1) *
        sizeof(BlueListEntry) + list->elementSize;
    memmove(
        ((char*)element) - sizeof(BlueListEntry),
        ((char*)element) + list->elementSize,
        bytesToMove
    );
    list->changeCount++;
}

bool blueList_removeById(BlueList* list, BlueEntryID id)
{
    int index = blueList_findById(list, id);
    if (index >= 0) {
        blueList_removeByIndex(list, index);
        return true;
    }
    return false;
}

void blueList_removeByPtr(BlueList* list, const void* element)
{
    int index = blueList_findByPtr(list, element);
    assert(index >= 0);
    blueList_removeByIndex(list, index);
}

int blueList_getCount(const BlueList* list)
{
    assert(list != NULL);
    return list->count;
}

int blueList_getChangeCount(const BlueList* list)
{
    return list->changeCount;
}

BlueEntryID blueList_getIdByIndex(const BlueList* list, int index)
{
    assert(list != NULL);
    assert(index >= 0 && index < list->count);
    return list->entries[index].id;
}

BlueEntryID blueList_getIdByPtr(const BlueList* list, const void* element)
{
    assert(list != NULL && element != NULL);
    int index = blueList_findByPtr(list, element);
    return blueList_getIdByIndex(list, index);
}

int blueList_findById(const BlueList* list, BlueEntryID id)
{
    assert(list != NULL);
    for (int i = 0; i < list->count; i++) {
        if (list->entries[i].id == id)
            return i;
    }
    return -1;
}

int blueList_findByPtr(const BlueList* list, const void* element)
{
    assert(list != NULL && element != NULL);
    const int fullEntrySize = list->elementSize + sizeof(BlueListEntry);
    const int index = ((char*)element - (char*)list->entries) / fullEntrySize;
#ifdef _DEBUG
    const void* recalculated = ((char*)list->entries) + index * fullEntrySize;
    asset(recalculated == element);
#endif
    return index;
}

static void raw_blueList_innersort(BlueList* list, RawBlueListComparer comparer, void* userdata, int left, int right)
{
    if (left >= right)
        return;
    const void* pivot = raw_blueList_constget(list, (left + right) / 2);
    int i = left;
    int j = right;
    while(true) {
        while (comparer(raw_blueList_constget(list, i), pivot, userdata) < 0)
            i++;
        while (comparer(raw_blueList_constget(list, j), pivot, userdata) > 0)
            j--;
        if (i >= j)
            break;
        blueList_swap(list, i, j);
    }

    raw_blueList_innersort(list, comparer, userdata, left, j);
    raw_blueList_innersort(list, comparer, userdata, j + 1, right);
}

void raw_blueList_sort(BlueList* list, RawBlueListComparer comparer, void* userdata)
{
    assert(list != NULL && comparer != NULL);
    raw_blueList_innersort(list, comparer, userdata, 0, list->count - 1);
}
