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
 * BlueList* blueList_new(typename type);
 * void blueList_setDestructor(typename type, BlueList* list, BlueListDestructor destructor, void* userdata);
 * void blueList_sort(typename type, BlueList* list, BlueListComparer comparer, void* userdata);
 * type* blueList_get(typename type, BlueList* list, int index)
 * const type* blueList_get(typename type, const BlueList* list, int index)
 */

#define BlueList(type) BlueList
#define INVALID_BLUEENTRYID ((BlueEntryID)0)
typedef struct BlueList BlueList;
typedef int BlueEntryID;

void blueList_free(BlueList* list);
int blueList_addEmpty(BlueList* list);
int blueList_add(BlueList* list, const void* element);
int blueList_addWithId(BlueList* list, const void* element, BlueEntryID id);
void blueList_swap(BlueList* list, int fromIndex, int toIndex);
void blueList_removeByIndex(BlueList* list, int index);
bool blueList_removeById(BlueList* list, BlueEntryID id);
void blueList_removeByPtr(BlueList* list, const void* element);
int blueList_getCount(const BlueList* list);
int blueList_getChangeCount(const BlueList* list);
BlueEntryID blueList_getIdByIndex(const BlueList* list, int index);
BlueEntryID blueList_getIdByPtr(const BlueList* list, const void* element);
int blueList_findById(const BlueList* list, BlueEntryID id);
int blueList_findByPtr(const BlueList* list, const void* element);

// Internal, don't look :(

#define blueList_new(type) raw_blueList_new(#type, sizeof(type))

#define raw_blueList_destructor(name,type) void (*name)(type* element, void* userdata)
typedef raw_blueList_destructor(RawBlueListDestructor, void);
#define blueList_setDestructor(type,list,destructor,userdata) do { \
        raw_blueList_requireType(list, #type); \
        typedef raw_blueList_destructor(__dtorType, type); \
        const __dtorType __dtor = destructor; \
        raw_blueList_setDestructor(list, (RawBlueListDestructor)__dtor, userdata); \
    } while(false)

#define raw_blueList_comparer(name,type) int (*name)(const type* a, const type* b, void* userdata)
typedef raw_blueList_comparer(RawBlueListComparer, void);
#define blueList_sort(type,list,comparer,userdata) do { \
        raw_blueList_requireType(list, #type); \
        typedef raw_blueList_comparer(__comparerType, type); \
        const __comparerType __comparer = comparer; \
        raw_blueList_sort(list, (RawBlueListComparer)__comparer, userdata); \
    while(false)

#define blueList_get(type,list,index) (raw_blueList_requireType(list, #type), \
    _Generic(list, \
        BlueList*: (type*)raw_blueList_get(list, (index)), \
        const BlueList*: (const type*)raw_blueList_constget(list, (index)) \
    ))

#define blueList_foreach(type,name,list) \
    raw_blueList_requireType(list, #type); \
    for ( \
        int i##name = 0, const type* name = NULL; \
        i##name < in->count && (name = blueList_get(type, in, i##name)); \
        i##name++)

BlueList* raw_blueList_new(const char* typeName, int elementSize);
void raw_blueList_setDestructor(BlueList* list, RawBlueListDestructor destructor, void* userdata);
void raw_blueList_requireType(BlueList* list, const char* typeName);
void* raw_blueList_get(BlueList* list, int index);
const void* raw_blueList_constget(const BlueList* list, int index);
void raw_blueList_sort(BlueList* list, RawBlueListComparer comparer, void* userdata);

#endif
