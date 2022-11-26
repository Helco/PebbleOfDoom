#ifdef DEBUG_WINDOWS
#include "renderer_internal.h"
#include "platform.h"
#include "sdl.include.h"
#include "cimgui.include.h"

typedef struct
{
    bool_t transform;
} RenderWorldOptions;
static const RenderWorldOptions worldSpaceOptions = {
    .transform = false
};
static const RenderWorldOptions playerSpaceOptions = {
    .transform = true
};

void renderer_setDebugColor(SDL_Renderer* sdlRenderer, GColor color)
{
    SDL_SetRenderDrawColor(sdlRenderer, color.r * (255 / 3), color.g * (255 / 3), color.b * (255 / 3), 255);
}

void renderer_renderSDLDebugLine(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, lineSeg_t line, const RenderWorldOptions* opts)
{
    lineSeg_t transformed = line;
    if (opts->transform)
        renderer_transformLine(me, &line, &transformed);
    xz_t start = xz_add(transformed.start.xz, offset);
    xz_t end = xz_add(transformed.end.xz, offset);
    SDL_RenderDrawLine(sdlRenderer,
        real_to_int(start.x), real_to_int(start.z),
        real_to_int(end.x), real_to_int(end.z));
}

void renderer_renderDebugVector(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, xz_t origin, xz_t direction, const RenderWorldOptions* opts)
{
    lineSeg_t seg;
    seg.start.xz = origin;
    seg.end.xz = xz_add(origin, direction);
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, seg, opts);

    real_t arrowLength = real_div(xz_length(direction), real_from_int(3));
    direction = xz_normalize(direction);
    xz_t arrow = xz_scale(xz_rotate(direction, real_degToRad(real_from_int(160))), arrowLength);
    seg.start.xz = seg.end.xz;
    seg.end.xz = xz_add(seg.start.xz, arrow);
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, seg, opts);

    arrow = xz_scale(xz_rotate(direction, real_degToRad(real_from_int(-160))), arrowLength);
    seg.end.xz = xz_add(seg.start.xz, arrow);
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, seg, opts);
}

xz_t angleToVector(real_t angle, real_t length)
{
    return xz_scale(xz(
        real_sin(angle),
        real_cos(angle)
    ), length);
}

void renderer_renderDebugSector(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, const Sector* sector, const RenderWorldOptions* opts)
{
    const Wall* curWall = sector->walls;
    for (int i = 0; i < sector->wallCount; i++, curWall++)
    {
        lineSeg_t wallLine;
        wallLine.start.xz = me->level->vertices[curWall->startCorner];
        wallLine.end.xz = me->level->vertices[sector->walls[(i + 1) % sector->wallCount].startCorner];
        renderer_setDebugColor(sdlRenderer, GColorFromRGB((i % 3 == 0) * 255, (i % 3 == 1) * 255, (i % 3 == 2) * 255));
        renderer_renderSDLDebugLine(me, sdlRenderer, offset, wallLine, opts);
    }

    for (int i = 0; i < sector->entityCount; i++)
    {
        renderer_setDebugColor(sdlRenderer, GColorFromRGB(0, 255, 0));
        const Entity* entity = &sector->entities[i];
        renderer_renderDebugVector(me, sdlRenderer, offset,
            entity->location.position,
            angleToVector(entity->location.angle, real_from_int(13)),
            opts);
    }
}

void renderer_debug_renderWorld(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, const void* userdata)
{
    if (me->level == NULL)
        return;
    const RenderWorldOptions* opts = (const RenderWorldOptions*)userdata;
    const Sector* curSector = me->level->sectors;
    for (int i = 0; i < me->level->sectorCount; i++, curSector++)
        renderer_renderDebugSector(me, sdlRenderer, offset, curSector, opts);

    GColor red = GColorFromRGB(255, 0, 0);
    renderer_setDebugColor(sdlRenderer, red);
    renderer_renderDebugVector(me, sdlRenderer, offset, me->location.position, angleToVector(me->location.angle, real_from_int(30)), opts);
    const real_t halfFoV = real_div(me->fov, real_from_int(2));
    lineSeg_t fovLine;
    fovLine.start.xz = me->location.position;
    fovLine.end.xz = xz_add(me->location.position, angleToVector(real_add(me->location.angle, halfFoV), real_from_int(70)));
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, fovLine, opts);
    fovLine.end.xz = xz_add(me->location.position, angleToVector(real_sub(me->location.angle, halfFoV), real_from_int(70)));
    renderer_renderSDLDebugLine(me, sdlRenderer, offset, fovLine, opts);
}

void renderer_debug_renderTexture(Renderer* me, SDL_Renderer* sdlRenderer, xz_t offset, const void* userdata)
{
    UNUSED(offset, userdata);
    const int pxsize = 4;
    const Texture* texture = texture_load(me->textureManager, 0);
    for (int y = 0; y < texture->size.h; y++)
    {
        for (int x = 0; x < texture->size.w; x++)
        {
            renderer_setDebugColor(sdlRenderer, texture->pixels[y * texture->size.w + x]);
            SDL_Rect rct = {
                x * pxsize + real_to_int(offset.x),
                y * pxsize + real_to_int(offset.z),
                pxsize, pxsize
            };
            SDL_RenderFillRect(sdlRenderer, &rct);
        }
    }
    texture_free(me->textureManager, texture);
}

bool igcSliderReal(const char* label, real_t* value, float v_min, float v_max)
{
    float floatValue = real_to_float(*value);
    bool wasChanged = igSliderFloat(label, &floatValue, v_min, v_max, "%.2f", 1.0f);
    if (wasChanged)
        *value = real_from_float(floatValue);
    return wasChanged;
}

bool igcDragReal(const char* label, real_t* value, float v_speed, float v_min, float v_max)
{
    float floatValue = real_to_float(*value);
    bool wasChanged = igDragFloat(label, &floatValue, v_speed, v_min, v_max, "%.2f", 1.0f);
    if (wasChanged)
        *value = real_from_float(floatValue);
    return wasChanged;
}

bool igcDragXZ(const char* label, xz_t* value, float v_speed, float v_min, float v_max)
{
    float floatValues[2] = { real_to_float(value->x), real_to_float(value->z) };
    bool wasChanged = igDragFloat2(label, floatValues, v_speed, v_min, v_max, "%.2f", 1.0f);
    if (wasChanged) {
        value->x = real_from_float(floatValues[0]);
        value->z = real_from_float(floatValues[1]);
    }
    return wasChanged;
}

bool igcLocation(Location* location)
{
    bool wasChanged = false;
    int sector = location->sector;
    igDragInt("Sector", &sector, 0.0f, 0, 1000, "%d");

    wasChanged = wasChanged || igcDragXZ("Position", &location->position, 0.5f, -1000.0f, 1000.0f);
    wasChanged = wasChanged || igcDragReal("Height", &location->height, 0.5f, -100.0f, 100.0f);

    real_t angleDeg = real_radToDeg(location->angle);
    if (igcDragReal("Angle", &angleDeg, 0.7f, 0.0f, 360.0f)) {
        location->angle = real_degToRad(angleDeg);
        wasChanged = true;
    }

    return wasChanged;
}

void renderer_debug_cameraOptions(Renderer* me, ImGuiWindowFlags* flags, const void* userdata)
{

    UNUSED(flags, userdata);
    igcSliderReal("Eye Height", &me->eyeHeight, -20.0f, 20.0f);

    real_t fovDeg = real_radToDeg(me->fov);
    if (igcSliderReal("Field of View", &fovDeg, 1.0f, 179.0f))
        renderer_setFieldOfView(me, real_degToRad(fovDeg));

    igSeparator();
    igText("Camera location");
    if (igcLocation(&me->location) && me->level != NULL)
        location_updateSector(&me->location, me->level);
}

static const DebugView debugViews[] = {
    {
        .name = "world-space",
        .type = DebugViewType_SDL,
        .callback = { .sdl = renderer_debug_renderWorld },
        .userdata = &worldSpaceOptions,
        .startsOpened = true
    },
    {
        .name = "player-space",
        .type = DebugViewType_SDL,
        .callback = { .sdl = renderer_debug_renderWorld },
        .userdata = &playerSpaceOptions
    },
    {
        .name = "texture",
        .type = DebugViewType_SDL,
        .callback = { .sdl = renderer_debug_renderTexture },
        .userdata = NULL
    },
    {
        .name = "camera",
        .type = DebugViewType_ImGui,
        .callback = { .imgui = renderer_debug_cameraOptions },
        .userdata = NULL,
        .startsOpened = true
    }
};

int renderer_getDebugCount(const Renderer* renderer)
{
    UNUSED(renderer);
    return sizeof(debugViews) / sizeof(DebugView);
}

const DebugView* renderer_getDebugViews(const Renderer* renderer)
{
    UNUSED(renderer);
    return debugViews;
}

#endif
