#include <pebble.h>
#include "platform.h"
#include "level.h"

/*Level *level_load(int levelId)
{
    UNUSED(levelId);
    Wall walls_template[] = {
        // 0: a
        {.startCorner = xz(real_from_int(160), real_zero),
         .texture = 7,
         .texCoord = { xy_zero, xy(real_from_int(2), real_one) },
         .portalTo = -1},
        {.startCorner = xz(real_from_int(480), real_zero),
         .texture = 0,
         .texCoord = { xy_zero, xy_one },
         .portalTo = 6},
        {.startCorner = xz(real_from_int(480), real_from_int(170)),
         .texture = 1,
         .texCoord = { xy_zero, xy_one },
         .portalTo = 9},
        {.startCorner = xz(real_from_int(400), real_from_int(230)),
         .texture = 0,
         .texCoord = { xy_zero, xy_one },
         .portalTo = 1},

         // 1: b
         {.startCorner = xz(real_from_int(160), real_from_int(80)),
          .texture = 2,
          .texCoord = { xy_zero, xy_one },
          .portalTo = -1},
         {.startCorner = xz(real_from_int(160), real_from_int(40)),
          .texture = 0,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 3},
         {.startCorner = xz(real_from_int(160), real_zero),
          .texture = 0,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 0},
         {.startCorner = xz(real_from_int(400), real_from_int(230)),
          .texture = 1,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 9},
         {.startCorner = xz(real_from_int(410), real_from_int(240)),
          .texture = 1,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 10},
         {.startCorner = xz(real_from_int(420), real_from_int(250)),
          .texture = 1,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 11},
         {.startCorner = xz(real_from_int(430), real_from_int(260)),
          .texture = 1,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 12},
         {.startCorner = xz(real_from_int(440), real_from_int(270)),
          .texture = 1,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 13},
         {.startCorner = xz(real_from_int(440), real_from_int(280)),
          .texture = 0,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 2},
         {.startCorner = xz(real_from_int(160), real_from_int(240)),
          .texture = 2,
          .texCoord = { xy_zero, xy_one },
          .portalTo = -1},
         {.startCorner = xz(real_from_int(160), real_from_int(200)),
          .texture = 0,
          .texCoord = { xy_zero, xy_one },
          .portalTo = 5},

          // 2: c
          {.startCorner = xz(real_from_int(440), real_from_int(320)),
           .texture = 4,
           .texCoord = { xy_zero, xy(real_from_float(1.333f), real_one) },
           .portalTo = -1},
          {.startCorner = xz(real_from_int(120), real_from_int(320)),
           .texture = 0,
           .texCoord = { xy_zero, xy_one },
           .portalTo = 7},
          {.startCorner = xz(real_from_int(120), real_from_int(240)),
           .texture = 2,
           .texCoord = { xy_zero, xy_one },
           .portalTo = -1},
          {.startCorner = xz(real_from_int(160), real_from_int(240)),
           .texture = 0,
           .texCoord = { xy_zero, xy_one },
           .portalTo = 1},
          {.startCorner = xz(real_from_int(440), real_from_int(280)),
           .texture = 1,
           .texCoord = { xy_zero, xy_one },
           .portalTo = 14},
          {.startCorner = xz(real_from_int(440), real_from_int(290)),
           .texture = 1,
           .texCoord = { xy_zero, xy_one },
           .portalTo = 15},
          {.startCorner = xz(real_from_int(440), real_from_int(300)),
           .texture = 1,
           .texCoord = { xy_zero, xy_one },
           .portalTo = 16},
          {.startCorner = xz(real_from_int(440), real_from_int(310)),
           .texture = 1,
           .texCoord = { xy_zero, xy_one },
           .portalTo = 17},

           // 3: d
           {.startCorner = xz_zero,
            .texture = 7,
            .texCoord = { xy_zero, xy(real_from_float(0.5f), real_one) },
            .portalTo = -1},
           {.startCorner = xz(real_from_int(160), real_zero),
            .texture = 0,
            .texCoord = { xy_zero, xy_one },
            .portalTo = 1},
           {.startCorner = xz(real_from_int(160), real_from_int(40)),
            .texture = 2,
            .texCoord = { xy_zero, xy_one },
            .portalTo = -1},
           {.startCorner = xz(real_from_int(120), real_from_int(40)),
            .texture = 0,
            .texCoord = { xy_zero, xy_one },
            .portalTo = 4},

            // 4: e
            {.startCorner = xz(real_zero, real_from_int(160)),
             .texture = 6,
             .texCoord = { xy_zero, xy_one },
             .portalTo = -1},
            {.startCorner = xz_zero,
             .texture = 0,
             .texCoord = { xy_zero, xy_one },
             .portalTo = 3},
            {.startCorner = xz(real_from_int(120), real_from_int(40)),
             .texture = 2,
             .texCoord = { xy_zero, xy_one },
             .portalTo = -1},
            {.startCorner = xz(real_from_int(120), real_from_int(80)),
             .texture = 0,
             .texCoord = { xy_zero, xy_one },
             .portalTo = 5},
            {.startCorner = xz(real_from_int(120), real_from_int(200)),
             .texture = 2,
             .texCoord = { xy_zero, xy_one },
             .portalTo = -1},
            {.startCorner = xz(real_from_int(120), real_from_int(240)),
             .texture = 0,
             .texCoord = { xy_zero, xy_one },
             .portalTo = 7},

             // 5: er
             {.startCorner = xz(real_from_int(160), real_from_int(200)),
              .texture = 2,
              .texCoord = { xy_zero, xy_one },
              .portalTo = -1},
             {.startCorner = xz(real_from_int(120), real_from_int(200)),
              .texture = 0,
              .texCoord = { xy_zero, xy_one },
              .portalTo = 4},
             {.startCorner = xz(real_from_int(120), real_from_int(80)),
              .texture = 2,
              .texCoord = { xy_zero, xy_one },
              .portalTo = -1},
             {.startCorner = xz(real_from_int(160), real_from_int(80)),
              .texture = 0,
              .texCoord = { xy_zero, xy_one },
              .portalTo = 1},

              // 6: f
              {.startCorner = xz(real_from_int(480), real_zero),
               .texture = 3,
               .texCoord = { xy_zero, xy_one },
               .portalTo = -1},
              {.startCorner = xz(real_from_int(600), real_from_int(160)),
               .texture = 0,
               .texCoord = { xy_zero, xy_one },
               .portalTo = 8},
              {.startCorner = xz(real_from_int(520), real_from_int(210)),
               .texture = 1,
               .texCoord = { xy_zero, xy_one },
               .portalTo = 12},
              {.startCorner = xz(real_from_int(510), real_from_int(200)),
               .texture = 1,
               .texCoord = { xy_zero, xy_one },
               .portalTo = 11},
              {.startCorner = xz(real_from_int(500), real_from_int(190)),
               .texture = 1,
               .texCoord = { xy_zero, xy_one },
               .portalTo = 10},
              {.startCorner = xz(real_from_int(490), real_from_int(180)),
               .texture = 1,
               .texCoord = { xy_zero, xy_one },
               .portalTo = 9},
              {.startCorner = xz(real_from_int(480), real_from_int(170)),
               .texture = 0,
               .texCoord = { xy_zero, xy_one },
               .portalTo = 0},

               // 7: g
               {.startCorner = xz(real_from_int(-240), real_from_int(160)),
                .texture = 0,
                .texCoord = { xy_zero, xy_one },
                .portalTo = -1},
               {.startCorner = xz(real_zero, real_from_int(160)),
                .texture = 0,
                .texCoord = { xy_zero, xy_one },
                .portalTo = 4},
               {.startCorner = xz(real_from_int(120), real_from_int(240)),
                .texture = 0,
                .texCoord = { xy_zero, xy_one },
                .portalTo = 2},
               {.startCorner = xz(real_from_int(120), real_from_int(320)),
                .texture = 4,
                .texCoord = { xy_zero, xy(real_from_float(0.3333f), real_one) },
                .portalTo = -1},
               {.startCorner = xz(real_from_int(40), real_from_int(400)),
                .texture = 0,
                .texCoord = { xy_zero, xy_one },
                .portalTo = -1},
               {.startCorner = xz(real_from_int(-240), real_from_int(400)),
                .texture = 5,
                .texCoord = { xy_zero, xy_one },
                .portalTo = -1},

                // 8: h
                {.startCorner = xz(real_from_int(600), real_from_int(400)),
                 .texture = 4,
                 .texCoord = { xy_zero, xy(real_from_float(0.3333f), real_one) },
                 .portalTo = -1},
                {.startCorner = xz(real_from_int(520), real_from_int(320)),
                 .texture = 1,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = 17},
                {.startCorner = xz(real_from_int(520), real_from_int(310)),
                 .texture = 1,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = 16},
                {.startCorner = xz(real_from_int(520), real_from_int(300)),
                 .texture = 1,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = 15},
                {.startCorner = xz(real_from_int(520), real_from_int(290)),
                 .texture = 1,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = 14},
                {.startCorner = xz(real_from_int(520), real_from_int(280)),
                 .texture = 1,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = 13},
                {.startCorner = xz(real_from_int(520), real_from_int(210)),
                 .texture = 0,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = 6},
                {.startCorner = xz(real_from_int(600), real_from_int(160)),
                 .texture = 0,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = -1},
                {.startCorner = xz(real_from_int(800), real_from_int(160)),
                 .texture = 5,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = -1},
                {.startCorner = xz(real_from_int(800), real_from_int(400)),
                 .texture = 0,
                 .texCoord = { xy_zero, xy_one },
                 .portalTo = -1},

                 // 9: i
                 {.startCorner = xz(real_from_int(400), real_from_int(230)),
                  .texture = 1,
                  .texCoord = { xy_zero, xy_one },
                  .portalTo = 0},
                 {.startCorner = xz(real_from_int(480), real_from_int(170)),
                  .texture = 1,
                  .texCoord = { xy_zero, xy_one },
                  .portalTo = 6},
                 {.startCorner = xz(real_from_int(490), real_from_int(180)),
                  .texture = 1,
                  .texCoord = { xy_zero, xy_one },
                  .portalTo = 10},
                 {.startCorner = xz(real_from_int(410), real_from_int(240)),
                  .texture = 1,
                  .texCoord = { xy_zero, xy_one },
                  .portalTo = 1},

                  // 10: j
                  {.startCorner = xz(real_from_int(410), real_from_int(240)),
                   .texture = 1,
                   .texCoord = { xy_zero, xy_one },
                   .portalTo = 9},
                  {.startCorner = xz(real_from_int(490), real_from_int(180)),
                   .texture = 1,
                   .texCoord = { xy_zero, xy_one },
                   .portalTo = 6},
                  {.startCorner = xz(real_from_int(500), real_from_int(190)),
                   .texture = 1,
                   .texCoord = { xy_zero, xy_one },
                   .portalTo = 11},
                  {.startCorner = xz(real_from_int(420), real_from_int(250)),
                   .texture = 1,
                   .texCoord = { xy_zero, xy_one },
                   .portalTo = 1},

                   // 11: k
                   {.startCorner = xz(real_from_int(420), real_from_int(250)),
                    .texture = 1,
                    .texCoord = { xy_zero, xy_one },
                    .portalTo = 10},
                   {.startCorner = xz(real_from_int(500), real_from_int(190)),
                    .texture = 1,
                    .texCoord = { xy_zero, xy_one },
                    .portalTo = 6},
                   {.startCorner = xz(real_from_int(510), real_from_int(200)),
                    .texture = 1,
                    .texCoord = { xy_zero, xy_one },
                    .portalTo = 12},
                   {.startCorner = xz(real_from_int(430), real_from_int(260)),
                    .texture = 1,
                    .texCoord = { xy_zero, xy_one },
                    .portalTo = 1},

                    // 12: l
                    {.startCorner = xz(real_from_int(430), real_from_int(260)),
                     .texture = 1,
                     .texCoord = { xy_zero, xy_one },
                     .portalTo = 11},
                    {.startCorner = xz(real_from_int(510), real_from_int(200)),
                     .texture = 1,
                     .texCoord = { xy_zero, xy_one },
                     .portalTo = 6},
                    {.startCorner = xz(real_from_int(520), real_from_int(210)),
                     .texture = 1,
                     .texCoord = { xy_zero, xy_one },
                     .portalTo = 13},
                    {.startCorner = xz(real_from_int(440), real_from_int(270)),
                     .texture = 1,
                     .texCoord = { xy_zero, xy_one },
                     .portalTo = 1},

                     // 13: m
                     {.startCorner = xz(real_from_int(440), real_from_int(270)),
                      .texture = 1,
                      .texCoord = { xy_zero, xy_one },
                      .portalTo = 12},
                     {.startCorner = xz(real_from_int(520), real_from_int(210)),
                      .texture = 1,
                      .texCoord = { xy_zero, xy_one },
                      .portalTo = 8},
                     {.startCorner = xz(real_from_int(520), real_from_int(280)),
                      .texture = 1,
                      .texCoord = { xy_zero, xy_one },
                      .portalTo = 14},
                     {.startCorner = xz(real_from_int(440), real_from_int(280)),
                      .texture = 1,
                      .texCoord = { xy_zero, xy_one },
                      .portalTo = 1},

                      // 14: n
                      {.startCorner = xz(real_from_int(440), real_from_int(280)),
                       .texture = 1,
                       .texCoord = { xy_zero, xy_one },
                       .portalTo = 13},
                      {.startCorner = xz(real_from_int(520), real_from_int(280)),
                       .texture = 1,
                       .texCoord = { xy_zero, xy_one },
                       .portalTo = 8},
                      {.startCorner = xz(real_from_int(520), real_from_int(290)),
                       .texture = 1,
                       .texCoord = { xy_zero, xy_one },
                       .portalTo = 15},
                      {.startCorner = xz(real_from_int(440), real_from_int(290)),
                       .texture = 1,
                       .texCoord = { xy_zero, xy_one },
                       .portalTo = 2},

                       // 15: o
                       {.startCorner = xz(real_from_int(440), real_from_int(290)),
                        .texture = 1,
                        .texCoord = { xy_zero, xy_one },
                        .portalTo = 14},
                       {.startCorner = xz(real_from_int(520), real_from_int(290)),
                        .texture = 1,
                        .texCoord = { xy_zero, xy_one },
                        .portalTo = 8},
                       {.startCorner = xz(real_from_int(520), real_from_int(300)),
                        .texture = 1,
                        .texCoord = { xy_zero, xy_one },
                        .portalTo = 16},
                       {.startCorner = xz(real_from_int(440), real_from_int(300)),
                        .texture = 1,
                        .texCoord = { xy_zero, xy_one },
                        .portalTo = 2},

                        // 16: p
                        {.startCorner = xz(real_from_int(440), real_from_int(300)),
                         .texture = 1,
                         .texCoord = { xy_zero, xy_one },
                         .portalTo = 15},
                        {.startCorner = xz(real_from_int(520), real_from_int(300)),
                         .texture = 1,
                         .texCoord = { xy_zero, xy_one },
                         .portalTo = 8},
                        {.startCorner = xz(real_from_int(520), real_from_int(310)),
                         .texture = 1,
                         .texCoord = { xy_zero, xy_one },
                         .portalTo = 17},
                        {.startCorner = xz(real_from_int(440), real_from_int(310)),
                         .texture = 1,
                         .texCoord = { xy_zero, xy_one },
                         .portalTo = 2},

                         // 17: q
                         {.startCorner = xz(real_from_int(520), real_from_int(320)),
                          .texture = 8,
                          .texCoord = { xy_zero, xy_one },
                          .portalTo = -1},
                         {.startCorner = xz(real_from_int(440), real_from_int(320)),
                          .texture = 1,
                          .texCoord = { xy_zero, xy_one },
                          .portalTo = 2},
                         {.startCorner = xz(real_from_int(440), real_from_int(310)),
                          .texture = 1,
                          .texCoord = { xy_zero, xy_one },
                          .portalTo = 16},
                         {.startCorner = xz(real_from_int(520), real_from_int(310)),
                          .texture = 1,
                          .texCoord = { xy_zero, xy_one },
                          .portalTo = 8},

    };

    Sector sectors_template[] = {
        {.wallCount = 4,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 11,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 8,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 6,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 7,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 6,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 10,
         .height = 120,
         .heightOffset = 0,
         .floorColor = GColorFromRGB(170,170,170),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 113,
         .heightOffset = 7,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 106,
         .heightOffset = 14,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 99,
         .heightOffset = 21,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 92,
         .heightOffset = 28,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 85,
         .heightOffset = 35,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 78,
         .heightOffset = 42,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 71,
         .heightOffset = 49,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 64,
         .heightOffset = 56,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
        {.wallCount = 4,
         .height = 57,
         .heightOffset = 63,
         .floorColor = GColorFromRGB(85,85,85),
         .ceilColor = GColorFromRGB(255,255,255),
         .walls = NULL},
    };

    Level level_template = {
        .sectorCount = 18,
        .playerStart = {
            .sector = 13,
            .position = xz(real_from_int(490), real_from_int(255)),
            .angle = real_degToRad(real_zero),
            .height = real_from_int(42)
        },
        .sectors = NULL
    };

    char* memory = (char*)malloc(sizeof(Level) + sizeof(sectors_template) + sizeof(walls_template));
    if (memory == NULL)
        return NULL;
    Level* level = (Level*)memory;
    Sector* sectors = (Sector*)(memory + sizeof(Level));
    Wall* walls = (Wall*)(memory + sizeof(Level) + sizeof(sectors_template));
    memcpy(level, &level_template, sizeof(Level));
    memcpy(sectors, sectors_template, sizeof(sectors_template));
    memcpy(walls, walls_template, sizeof(walls_template));
    level->sectors = sectors;
    sectors[0].walls = walls + 0;
    sectors[1].walls = walls + 4;
    sectors[2].walls = walls + 15;
    sectors[3].walls = walls + 23;
    sectors[4].walls = walls + 27;
    sectors[5].walls = walls + 33;
    sectors[6].walls = walls + 37;
    sectors[7].walls = walls + 44;
    sectors[8].walls = walls + 50;
    sectors[9].walls = walls + 60;
    sectors[10].walls = walls + 64;
    sectors[11].walls = walls + 68;
    sectors[12].walls = walls + 72;
    sectors[13].walls = walls + 76;
    sectors[14].walls = walls + 80;
    sectors[15].walls = walls + 84;
    sectors[16].walls = walls + 88;
    sectors[17].walls = walls + 92;
    return level;
}

void level_free(Level *me)
{
    if (me == NULL)
        return;
    free(me);
}*/

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
