#ifndef PCMOCKUP_H
#define PCMOCKUP_H
#include "sdl.include.h"
#include "pebble.h"
#include "renderer.h"
#include "texgen/texgen.h"
#include "window.h"
#include "platform.h"
#include "levelmanager.h"
#include <glad/glad.h>

#define RESOURCES_PATH "resources/"
#define TEXTURE_PATH RESOURCES_PATH "textures/"
#define LEVEL_PATH RESOURCES_PATH "levels/"

GRect findBestFit(GRect target, float aspect);
GRect padRect(GRect rect, GSize amount);

extern const Uint32 imageWindow_SDLPixelFormat;
typedef struct UploadedTexture UploadedTexture;
UploadedTexture* uploadedTexture_init();
void uploadedTexture_free(UploadedTexture* me);
GLuint uploadedTexture_getGLTextureId(UploadedTexture* me);
GSize uploadedTexture_getSize(UploadedTexture* me);
void uploadedTexture_setFrom32Bit(UploadedTexture* me, GSize size, const SDL_Color* pixels);
void uploadedTexture_setFrom8Bit(UploadedTexture* me, GSize size, const GColor* pixels);
void uploadedTexture_setFromTexture(UploadedTexture* me, const Texture* texture);

typedef struct ImageWindow ImageWindow;
ImageWindow* imageWindow_init(WindowContainer* parent, const char* title, GRect initialBounds, bool_t isEssential);
void imageWindow_setImageData(ImageWindow* me, GSize size, const SDL_Color* data);
void imageWindow_toggle(ImageWindow* me, bool_t isOpen);
bool_t imageWindow_isOpen(ImageWindow* me);
Window* imageWindow_asWindow(ImageWindow* me);
#define ImageWindow_Tag FOURCC('I', 'M', 'G', 'W')

typedef struct SafeFramebuffer SafeFramebuffer;
SafeFramebuffer* safeFramebuffer_init(GSize size, RendererColorFormat format, int canarySize); // canarySize in framebuffer sizes
void safeFramebuffer_free(SafeFramebuffer* me);
void* safeFramebuffer_getScreenBuffer(SafeFramebuffer* me);
void safeFramebuffer_prepare(SafeFramebuffer* me);
void safeFramebuffer_check(SafeFramebuffer* me);

typedef struct PebbleWindow PebbleWindow;
PebbleWindow* pebbleWindow_init(WindowContainer* parent, GRect initialBounds, GSize pebbleSize, RendererColorFormat format, Renderer* renderer);
ImageWindow* pebbleWindow_asImageWindow(PebbleWindow* window);
#define PebbleWindow_Tag FOURCC('P', 'E', 'B', 'B')

typedef bool (*DebugWindowConstructor)(WindowContainer* parent, GRect bounds, const DebugView* view, Renderer* renderer);
typedef struct DebugWindowSDL DebugWindowSDL;
DebugWindowSDL* debugWindowSDL_init(WindowContainer* parent, GRect bounds, const DebugView* debugView, Renderer* renderer);
typedef struct DebugWindowImGui DebugWindowImGui;
DebugWindowImGui* debugWindowImGui_init(WindowContainer* parent, GRect bounds, const DebugView* debugView, Renderer* renderer);
#define DebugWindow_Tag FOURCC('D', 'E', 'B', 'V')

typedef struct PCMockup PCMockup;
PCMockup* pcmockup_init();
void pcmockup_free(PCMockup* me);
void pcmockup_update(PCMockup* me);
void pcmockup_mainLoop(PCMockup* me);

typedef struct TextureManager TextureManager;
TextureManager* textureManager_init();
void textureManager_free(TextureManager* me); // asserts that all textures are freed
TextureId textureManager_registerFile(TextureManager* me, const char* filename); // relative to texture folder
const Texture* textureManager_loadTexture(TextureManager* me, TextureId id);
const Texture* textureManager_createEmptyTexture(TextureManager* me, GSize size, GColor** outputPtr);
void textureManager_resizeEmptyTexture(TextureManager* me, TextureId id, GSize newSize, GColor** outputPtr);
void textureManager_freeTexture(TextureManager* me, const Texture* texture);
TexGenerationContext* textureManager_createGeneratedTexture(TextureManager* me, TexGeneratorID id, int size);
TexGenerationContext* textureManager_getGenerationContext(TextureManager* me, TextureId id);
int textureManager_getTextureCount(TextureManager* me);
const Texture* textureManager_getTextureByIndex(TextureManager* me, int index);
const char* textureManager_getTextureSource(TextureManager* me, const Texture* texture);
SpriteId textureManager_registerSpriteFile(TextureManager* me, const char* filename); // relative to texture folder
const Sprite* textureManager_loadSprite(TextureManager* me, SpriteId id);
void textureManager_freeSprite(TextureManager* me, const Sprite* sprite);

typedef struct TextureWindow TextureWindow;
TextureWindow* textureWindow_init(WindowContainer* parent, TextureManager* manager);
Window* textureWindow_asWindow(TextureWindow* me);

#endif
