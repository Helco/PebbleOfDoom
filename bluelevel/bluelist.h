#ifndef BLUELIST_H
#define BLUELIST_H
#include <stdbool.h>

/* A simple expandable list for any type, augmented with a few macros
 * for ease of use and a bit of type safety.
 * An ID is a weak identification, e.g. operations receiving one should
 * accept invalid IDs.
 *
 * Implicit API (backed by macros):
 * typedef void BlueListDestructor(type* element, void* userdata);
 * typedef void BlueListComparer(const type* a, const type* b, void* userdata);
 * BlueList* bluelist_new(typename type);
 * void bluelist_setDestructor(typename type, BlueList* list, BlueListDestructor destructor, void* userdata);
 * void bluelist_sort(typename type, BlueList* list, BlueListComparer comparer, void* userdata);
 * type* bluelist_get(typename type, BlueList* list, int index)
 * const type* bluelist_get(typename type, const BlueList* list, int index)
 */

#define BlueList(type) BlueList
typedef struct BlueList BlueList;
typedef int BlueEntryID;

void bluelist_free(BlueList* list);
int bluelist_add(BlueList* list, const void* element);
int bluelist_addWithId(BlueList* list, const void* element, BlueEntryID id);
void bluelist_swap(BlueList* list, int fromIndex, int toIndex);
void bluelist_removeByIndex(BlueList* list, int index);
bool bluelist_removeById(BlueList* list, BlueEntryID id);
void bluelist_removeByPtr(BlueList* list, const void* element);
int bluelist_getCount(const BlueList* list);
int bluelist_getChangeCount(const BlueList* list);
BlueEntryID bluelist_getIdByIndex(const BlueList* list, int index);
BlueEntryID bluelist_getIdByPtr(const BlueList* list, const void* element);
int bluelist_findById(const BlueList* list, BlueEntryID id);
int bluelist_findByPtr(const BlueList* list, const void* element);

// Internal, don't look :(

#define bluelist_new(type) raw_bluelist_new(#type, sizeof(type))

#define raw_bluelist_destructor(name,type) void (*name)(type* element, void* userdata)
typedef raw_bluelist_destructor(RawBlueListDestructor, void);
#define bluelist_setDestructor(type,list,destructor,userdata) do { \
        raw_bluelist_requireType(list, #type); \
        typedef raw_bluelist_destructor(__dtorType, type); \
        const __dtorType __dtor = destructor; \
        raw_bluelist_setDestructor(list, (RawBlueListDestructor)__dtor, userdata); \
    while(false)

#define raw_bluelist_comparer(name,type) int (*name)(const type* a, const type* b, void* userdata)
typedef raw_bluelist_comparer(RawBlueListComparer, void);
#define bluelist_sort(type,list,comparer,userdata) do { \
        raw_bluelist_requireType(list, #type); \
        typedef raw_bluelist_comparer(__comparerType, type); \
        const __comparerType __comparer = comparer; \
        raw_bluelist_sort(list, (RawBlueListComparer)__comparer, userdata); \
    while(false)

#define bluelist_get(type,list,index) (raw_bluelist_requireType(list, #type), _Generic(list, \
    BlueList*: (type*)raw_bluelist_get(list, (index)) \
    const BlueList*: (const type*)raw_bluelist_constget(list, (index))))

#define bluelist_foreach(type,name,list) \
    raw_bluelist_requireType(list, #type); \
    for ( \
        int i##name = 0, const type* name = NULL; \
        i##name < in->count && (name = bluelist_get(type, in, i##name)); \
        i##name++)

BlueList* raw_bluelist_new(const char* typeName, int elementSize);
void raw_bluelist_setDestructor(BlueList* list, RawBlueListDestructor destructor, void* userdata);
void raw_bluelist_requireType(BlueList* list, const char* typeName);
void* raw_bluelist_get(BlueList* list, int index);
const void* raw_bluelist_constget(const BlueList* list, int index);
void raw_bluelist_sort(BlueList* list, RawBlueListComparer comparer, void* userdata);

#endif
