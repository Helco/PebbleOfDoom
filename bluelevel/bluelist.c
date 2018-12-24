#include "bluelist.h"
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

static int bluelist_getNextCapacity(int oldCapacity) {
    const int CHUNK_SIZE = 16;
    return oldCapacity <= 0
        ? CHUNK_SIZE
        : oldCapacity >= 128
            ? oldCapacity + CHUNK_SIZE
            : oldCapacity * 2;
}

BlueList* raw_bluelist_new(const char* typeName, int elementSize)
{
    BlueList* list = (BlueList*)malloc(sizeof(BlueList));
    assert(list != NULL);
    list->typeName = strdup(typeName);
    assert(list->typeName != NULL);

    list->count = list->capacity = 0;
    list->nextId = 1;
    list->changeCount = 0;
    list->elementSize = elementSize;
    list->entries = NULL;
    return list;
}

void bluelist_free(BlueList* list)
{
    assert(list != NULL);

    if (list->count > 0 && list->destructor != NULL) {
        for (int i = 0; i < list->count; i++)
            list->destructor(raw_bluelist_get(list, i), list->destructorUserdata);
    }
    if (list->typeName != NULL)
        free(list->typeName);
    if (list->entries != NULL)
        free(list->entries);
    free(list);
}

void raw_bluelist_setDestructor(BlueList* list, RawBlueListDestructor destructor, void* userdata)
{
    assert(list != NULL);
    list->destructor = destructor;
    list->destructorUserdata = userdata;
}

void raw_bluelist_requireType(BlueList* list, const char* typeName)
{
    assert(list != NULL && typeName != NULL);
    assert(strcmp(list->typeName, typeName) == 0);
}

void* raw_bluelist_get(BlueList* list, int index)
{
    assert(list != NULL && index >= 0 && index < list->count);
    return list->entries[index].data;
}

const void* raw_bluelist_constget(const BlueList* list, int index)
{
    assert(list != NULL && index >= 0 && index < list->count);
    return list->entries[index].data;
}

int bluelist_add(BlueList* list, const void* element)
{
    return bluelist_addWithId(list, element, list->nextId++);
}

int bluelist_addWithId(BlueList* list, const void* element, BlueEntryID id)
{
    assert(list != NULL && list->count <= list->capacity);
    assert(element != NULL);
    if (list->count == list->capacity) {
        list->capacity = bluelist_getNextCapacity(list->capacity);
        const int memorySize = list->capacity * (sizeof(BlueListEntry) + list->elementSize);
        list->entries = (BlueListEntry*)realloc(list->entries, memorySize);
        assert(list->entries != NULL);
    }

    int index = list->count++;
    list->entries[index].id = id;
    memcpy(&list->entries[index], element, list->elementSize);
    list->changeCount++;
    return index;
}

void bluelist_swap(BlueList* list, int fromIndex, int toIndex)
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

void bluelist_removeByIndex(BlueList* list, int index)
{
    void* element = raw_bluelist_get(list, index);
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

bool bluelist_removeById(BlueList* list, BlueEntryID id)
{
    int index = bluelist_findById(list, id);
    if (index >= 0) {
        bluelist_removeByIndex(list, index);
        return true;
    }
    return false;
}

void bluelist_removeByPtr(BlueList* list, const void* element)
{
    int index = bluelist_findByPtr(list, element);
    assert(index >= 0);
    bluelist_removeByIndex(list, index);
}

int bluelist_getCount(const BlueList* list)
{
    assert(list != NULL);
    return list->count;
}

int bluelist_getChangeCount(const BlueList* list)
{
    return list->changeCount;
}

BlueEntryID bluelist_getIdByIndex(const BlueList* list, int index)
{
    assert(list != NULL);
    assert(index >= 0 && index < list->count);
    return list->entries[index].id;
}

BlueEntryID bluelist_getIdByPtr(const BlueList* list, const void* element)
{
    assert(list != NULL && element != NULL);
    int index = bluelist_findByPtr(list, element);
    return bluelist_getIdByIndex(list, index);
}

int bluelist_findById(const BlueList* list, BlueEntryID id)
{
    assert(list != NULL);
    for (int i = 0; i < list->count; i++) {
        if (list->entries[i].id == id)
            return i;
    }
    return -1;
}

int bluelist_findByPtr(const BlueList* list, const void* element)
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

static void raw_bluelist_innersort(BlueList* list, RawBlueListComparer comparer, void* userdata, int left, int right)
{
    if (left >= right)
        return;
    const void* pivot = raw_bluelist_constget(list, (left + right) / 2);
    int i = left;
    int j = right;
    while(true) {
        while (comparer(raw_bluelist_constget(list, i), pivot, userdata) < 0)
            i++;
        while (comparer(raw_bluelist_constget(list, j), pivot, userdata) > 0)
            j--;
        if (i >= j)
            break;
        bluelist_swap(list, i, j);
    }

    raw_bluelist_innersort(list, comparer, userdata, left, j);
    raw_bluelist_innersort(list, comparer, userdata, j + 1, right);
}

void raw_bluelist_sort(BlueList* list, RawBlueListComparer comparer, void* userdata)
{
    assert(list != NULL && comparer != NULL);
    raw_bluelist_innersort(list, comparer, userdata, 0, list->count - 1);
}
