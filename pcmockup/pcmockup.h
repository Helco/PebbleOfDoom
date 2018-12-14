#ifndef PCMOCKUP_H
#define PCMOCKUP_H
#include "sdl.include.h"
#include "pebble.h"
#include "renderer.h"
#include "texgen/texgen.h"
#include "window.h"
#include <glad/glad.h>

#define TEXTURE_PATH "resources/textures/"

SDL_Rect findBestFit(SDL_Rect target, float aspect);
SDL_Rect padRect(SDL_Rect rect, GSize amount);

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

typedef struct SafeFramebuffer SafeFramebuffer;
SafeFramebuffer* safeFramebuffer_init(GSize size, int canarySize); // canarySize in framebuffer sizes
void safeFramebuffer_free(SafeFramebuffer* me);
GColor* safeFramebuffer_getScreenBuffer(SafeFramebuffer* me);
void safeFramebuffer_prepare(SafeFramebuffer* me);
void safeFramebuffer_check(SafeFramebuffer* me);

typedef struct PebbleWindow PebbleWindow;
PebbleWindow* pebbleWindow_init(WindowContainer* parent, SDL_Rect initialBounds, GSize pebbleSize, Renderer* renderer);
GColor* pebbleWindow_getPebbleFramebuffer(PebbleWindow* window);
ImageWindow* pebbleWindow_asImageWindow(PebbleWindow* window);

typedef bool (*DebugWindowConstructor)(WindowContainer* parent, SDL_Rect bounds, const DebugView* view, Renderer* renderer);
typedef struct DebugWindowSDL DebugWindowSDL;
DebugWindowSDL* debugWindowSDL_init(WindowContainer* parent, SDL_Rect bounds, const DebugView* debugView, Renderer* renderer);
typedef struct DebugWindowImGui DebugWindowImGui;
DebugWindowImGui* debugWindowImGui_init(WindowContainer* parent, SDL_Rect bounds, const DebugView* debugView, Renderer* renderer);

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

typedef struct TextureWindow TextureWindow;
TextureWindow* textureWindow_init(WindowContainer* parent, TextureManager* manager);
Window* textureWindow_asWindow(TextureWindow* me);

#endif
