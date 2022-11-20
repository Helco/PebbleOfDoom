#ifndef RESOURCES_H
#define RESOURCES_H

#ifndef POD_PEBBLE
enum {
    RESOURCE_ID_SPR_BOOK = 1,
    RESOURCE_ID_ICON_DIGITS,
    RESOURCE_ID_ICON_BOOTS,
    RESOURCE_ID_ICON_GOLD,
    RESOURCE_ID_SPR_HEART, // actually double-duty as icon
    RESOURCE_ID_ICON_KEY
};
#else
#include <pebble.h>
#endif

#endif
