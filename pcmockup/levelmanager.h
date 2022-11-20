#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H
#include "level.h"

typedef struct LevelManager LevelManager;
LevelManager* levelManager_init(const char* baseDirectory);
void levelManager_free(LevelManager* me);
LevelId levelManager_registerFile(LevelManager* me, const char* filename, LevelId level); // relative to level folder
int levelManager_getLevelCount(LevelManager* me);
const Level* levelManager_getLevelByIndex(LevelManager* me, int index);
const Level* levelManager_getLevelById(LevelManager* me, LevelId id);

#endif
