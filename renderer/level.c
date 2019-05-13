#include <pebble.h>
#include "platform.h"
#include "level.h"

int level_findSectorAt(const Level* level, xz_t point)
{
    for (int i = 0; i < level->sectorCount; i++)
    {
        if (sector_isInside(&level->sectors[i], point))
            return i;
    }
    return -1;
}

bool sector_isInside(const Sector* sector, xz_t point)
{
    for (int i = 0; i < sector->wallCount; i++)
    {
        lineSeg_t line = {
            .start = { .xz = sector->walls[i].startCorner },
            .end = { .xz = sector->walls[(i + 1) % sector->wallCount].startCorner }
        };
        if (xz_isOnRight(point, line))
            return false;
    }
    return true;
}

bool location_updateSector(Location* me, const Level* level)
{
    if (me->sector >= 0) {
        const Sector* oldSector = &level->sectors[me->sector];
        if (sector_isInside(oldSector, me->position))
            return false;

        // Near search
        for (int i = 0; i < oldSector->wallCount; i++)
        {
            int targetI = oldSector->walls[i].portalTo;
            if (targetI >= 0 && sector_isInside(&level->sectors[targetI], me->position))
            {
                me->sector = targetI;
                return true;
            }
        }
    }

    // Thorough search
    int newSectorI = level_findSectorAt(level, me->position);
    bool result = newSectorI == me->sector;
    me->sector = newSectorI;
    return result;
}
