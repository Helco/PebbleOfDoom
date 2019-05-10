#include <pebble.h>
#include "platform.h"
#include "level.h"

/*Level *level_load(int levelId)
{
    UNUSED(levelId);
    Level* level = (Level*)malloc(sizeof(Level));
    Sector* sectors = (Sector*)malloc(sizeof(Sector) * 18);
    Wall* walls = (Wall*)malloc(sizeof(Wall ) * 96);
    if (level == NULL || sectors == NULL || walls == NULL) {
        return NULL;
    }

	// 0: a
	walls[0].startCorner = xz(real_from_int(160), real_zero);
	walls[0].texture = 7;
	walls[0].texCoord = (TexCoord){ xy_zero, xy(real_from_int(2), real_one) };
	walls[0].portalTo = -1;
	walls[1].startCorner = xz(real_from_int(480), real_zero);
	walls[1].texture = 0;
	walls[1].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[1].portalTo = 6;
	walls[2].startCorner = xz(real_from_int(480), real_from_int(170));
	walls[2].texture = 1;
	walls[2].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[2].portalTo = 9;
	walls[3].startCorner = xz(real_from_int(400), real_from_int(230));
	walls[3].texture = 0;
	walls[3].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[3].portalTo = 1;

	// 1: b
	walls[4].startCorner = xz(real_from_int(160), real_from_int(80));
	walls[4].texture = 2;
	walls[4].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[4].portalTo = -1;
	walls[5].startCorner = xz(real_from_int(160), real_from_int(40));
	walls[5].texture = 0;
	walls[5].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[5].portalTo = 3;
	walls[6].startCorner = xz(real_from_int(160), real_zero);
	walls[6].texture = 0;
	walls[6].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[6].portalTo = 0;
	walls[7].startCorner = xz(real_from_int(400), real_from_int(230));
	walls[7].texture = 1;
	walls[7].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[7].portalTo = 9;
	walls[8].startCorner = xz(real_from_int(410), real_from_int(240));
	walls[8].texture = 1;
	walls[8].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[8].portalTo = 10;
	walls[9].startCorner = xz(real_from_int(420), real_from_int(250));
	walls[9].texture = 1;
	walls[9].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[9].portalTo = 11;
	walls[10].startCorner = xz(real_from_int(430), real_from_int(260));
	walls[10].texture = 1;
	walls[10].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[10].portalTo = 12;
	walls[11].startCorner = xz(real_from_int(440), real_from_int(270));
	walls[11].texture = 1;
	walls[11].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[11].portalTo = 13;
	walls[12].startCorner = xz(real_from_int(440), real_from_int(280));
	walls[12].texture = 0;
	walls[12].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[12].portalTo = 2;
	walls[13].startCorner = xz(real_from_int(160), real_from_int(240));
	walls[13].texture = 2;
	walls[13].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[13].portalTo = -1;
	walls[14].startCorner = xz(real_from_int(160), real_from_int(200));
	walls[14].texture = 0;
	walls[14].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[14].portalTo = 5;

	// 2: c
	walls[15].startCorner = xz(real_from_int(440), real_from_int(320));
	walls[15].texture = 4;
	walls[15].texCoord = (TexCoord){ xy_zero, xy(real_from_float(1.333f), real_one) };
	walls[15].portalTo = -1;
	walls[16].startCorner = xz(real_from_int(120), real_from_int(320));
	walls[16].texture = 0;
	walls[16].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[16].portalTo = 7;
	walls[17].startCorner = xz(real_from_int(120), real_from_int(240));
	walls[17].texture = 2;
	walls[17].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[17].portalTo = -1;
	walls[18].startCorner = xz(real_from_int(160), real_from_int(240));
	walls[18].texture = 0;
	walls[18].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[18].portalTo = 1;
	walls[19].startCorner = xz(real_from_int(440), real_from_int(280));
	walls[19].texture = 1;
	walls[19].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[19].portalTo = 14;
	walls[20].startCorner = xz(real_from_int(440), real_from_int(290));
	walls[20].texture = 1;
	walls[20].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[20].portalTo = 15;
	walls[21].startCorner = xz(real_from_int(440), real_from_int(300));
	walls[21].texture = 1;
	walls[21].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[21].portalTo = 16;
	walls[22].startCorner = xz(real_from_int(440), real_from_int(310));
	walls[22].texture = 1;
	walls[22].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[22].portalTo = 17;

	// 3: d
	walls[23].startCorner = xz_zero;
	walls[23].texture = 7;
	walls[23].texCoord = (TexCoord){ xy_zero, xy(real_from_float(0.5f), real_one) };
	walls[23].portalTo = -1;
	walls[24].startCorner = xz(real_from_int(160), real_zero);
	walls[24].texture = 0;
	walls[24].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[24].portalTo = 1;
	walls[25].startCorner = xz(real_from_int(160), real_from_int(40));
	walls[25].texture = 2;
	walls[25].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[25].portalTo = -1;
	walls[26].startCorner = xz(real_from_int(120), real_from_int(40));
	walls[26].texture = 0;
	walls[26].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[26].portalTo = 4;

	// 4: e
	walls[27].startCorner = xz(real_zero, real_from_int(160));
	walls[27].texture = 6;
	walls[27].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[27].portalTo = -1;
	walls[28].startCorner = xz_zero;
	walls[28].texture = 0;
	walls[28].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[28].portalTo = 3;
	walls[29].startCorner = xz(real_from_int(120), real_from_int(40));
	walls[29].texture = 2;
	walls[29].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[29].portalTo = -1;
	walls[30].startCorner = xz(real_from_int(120), real_from_int(80));
	walls[30].texture = 0;
	walls[30].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[30].portalTo = 5;
	walls[31].startCorner = xz(real_from_int(120), real_from_int(200));
	walls[31].texture = 2;
	walls[31].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[31].portalTo = -1;
	walls[32].startCorner = xz(real_from_int(120), real_from_int(240));
	walls[32].texture = 0;
	walls[32].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[32].portalTo = 7;

	// 5: er
	walls[33].startCorner = xz(real_from_int(160), real_from_int(200));
	walls[33].texture = 2;
	walls[33].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[33].portalTo = -1;
	walls[34].startCorner = xz(real_from_int(120), real_from_int(200));
	walls[34].texture = 0;
	walls[34].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[34].portalTo = 4;
	walls[35].startCorner = xz(real_from_int(120), real_from_int(80));
	walls[35].texture = 2;
	walls[35].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[35].portalTo = -1;
	walls[36].startCorner = xz(real_from_int(160), real_from_int(80));
	walls[36].texture = 0;
	walls[36].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[36].portalTo = 1;

	// 6: f
	walls[37].startCorner = xz(real_from_int(480), real_zero);
	walls[37].texture = 3;
	walls[37].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[37].portalTo = -1;
	walls[38].startCorner = xz(real_from_int(600), real_from_int(160));
	walls[38].texture = 0;
	walls[38].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[38].portalTo = 8;
	walls[39].startCorner = xz(real_from_int(520), real_from_int(210));
	walls[39].texture = 1;
	walls[39].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[39].portalTo = 12;
	walls[40].startCorner = xz(real_from_int(510), real_from_int(200));
	walls[40].texture = 1;
	walls[40].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[40].portalTo = 11;
	walls[41].startCorner = xz(real_from_int(500), real_from_int(190));
	walls[41].texture = 1;
	walls[41].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[41].portalTo = 10;
	walls[42].startCorner = xz(real_from_int(490), real_from_int(180));
	walls[42].texture = 1;
	walls[42].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[42].portalTo = 9;
	walls[43].startCorner = xz(real_from_int(480), real_from_int(170));
	walls[43].texture = 0;
	walls[43].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[43].portalTo = 0;

	// 7: g
	walls[44].startCorner = xz(real_from_int(-240), real_from_int(160));
	walls[44].texture = 0;
	walls[44].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[44].portalTo = -1;
	walls[45].startCorner = xz(real_zero, real_from_int(160));
	walls[45].texture = 0;
	walls[45].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[45].portalTo = 4;
	walls[46].startCorner = xz(real_from_int(120), real_from_int(240));
	walls[46].texture = 0;
	walls[46].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[46].portalTo = 2;
	walls[47].startCorner = xz(real_from_int(120), real_from_int(320));
	walls[47].texture = 4;
	walls[47].texCoord = (TexCoord){ xy_zero, xy(real_from_float(0.3333f), real_one) };
	walls[47].portalTo = -1;
	walls[48].startCorner = xz(real_from_int(40), real_from_int(400));
	walls[48].texture = 0;
	walls[48].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[48].portalTo = -1;
	walls[49].startCorner = xz(real_from_int(-240), real_from_int(400));
	walls[49].texture = 5;
	walls[49].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[49].portalTo = -1;

	// 8: h
	walls[50].startCorner = xz(real_from_int(600), real_from_int(400));
	walls[50].texture = 4;
	walls[50].texCoord = (TexCoord){ xy_zero, xy(real_from_float(0.3333f), real_one) };
	walls[50].portalTo = -1;
	walls[51].startCorner = xz(real_from_int(520), real_from_int(320));
	walls[51].texture = 1;
	walls[51].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[51].portalTo = 17;
	walls[52].startCorner = xz(real_from_int(520), real_from_int(310));
	walls[52].texture = 1;
	walls[52].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[52].portalTo = 16;
	walls[53].startCorner = xz(real_from_int(520), real_from_int(300));
	walls[53].texture = 1;
	walls[53].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[53].portalTo = 15;
	walls[54].startCorner = xz(real_from_int(520), real_from_int(290));
	walls[54].texture = 1;
	walls[54].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[54].portalTo = 14;
	walls[55].startCorner = xz(real_from_int(520), real_from_int(280));
	walls[55].texture = 1;
	walls[55].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[55].portalTo = 13;
	walls[56].startCorner = xz(real_from_int(520), real_from_int(210));
	walls[56].texture = 0;
	walls[56].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[56].portalTo = 6;
	walls[57].startCorner = xz(real_from_int(600), real_from_int(160));
	walls[57].texture = 0;
	walls[57].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[57].portalTo = -1;
	walls[58].startCorner = xz(real_from_int(800), real_from_int(160));
	walls[58].texture = 5;
	walls[58].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[58].portalTo = -1;
	walls[59].startCorner = xz(real_from_int(800), real_from_int(400));
	walls[59].texture = 0;
	walls[59].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[59].portalTo = -1;

	// 9: i
	walls[60].startCorner = xz(real_from_int(400), real_from_int(230));
	walls[60].texture = 1;
	walls[60].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[60].portalTo = 0;
	walls[61].startCorner = xz(real_from_int(480), real_from_int(170));
	walls[61].texture = 1;
	walls[61].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[61].portalTo = 6;
	walls[62].startCorner = xz(real_from_int(490), real_from_int(180));
	walls[62].texture = 1;
	walls[62].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[62].portalTo = 10;
	walls[63].startCorner = xz(real_from_int(410), real_from_int(240));
	walls[63].texture = 1;
	walls[63].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[63].portalTo = 1;

	// 10: j
	walls[64].startCorner = xz(real_from_int(410), real_from_int(240));
	walls[64].texture = 1;
	walls[64].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[64].portalTo = 9;
	walls[65].startCorner = xz(real_from_int(490), real_from_int(180));
	walls[65].texture = 1;
	walls[65].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[65].portalTo = 6;
	walls[66].startCorner = xz(real_from_int(500), real_from_int(190));
	walls[66].texture = 1;
	walls[66].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[66].portalTo = 11;
	walls[67].startCorner = xz(real_from_int(420), real_from_int(250));
	walls[67].texture = 1;
	walls[67].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[67].portalTo = 1;

	// 11: k
	walls[68].startCorner = xz(real_from_int(420), real_from_int(250));
	walls[68].texture = 1;
	walls[68].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[68].portalTo = 10;
	walls[69].startCorner = xz(real_from_int(500), real_from_int(190));
	walls[69].texture = 1;
	walls[69].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[69].portalTo = 6;
	walls[70].startCorner = xz(real_from_int(510), real_from_int(200));
	walls[70].texture = 1;
	walls[70].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[70].portalTo = 12;
	walls[71].startCorner = xz(real_from_int(430), real_from_int(260));
	walls[71].texture = 1;
	walls[71].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[71].portalTo = 1;

	// 12: l
	walls[72].startCorner = xz(real_from_int(430), real_from_int(260));
	walls[72].texture = 1;
	walls[72].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[72].portalTo = 11;
	walls[73].startCorner = xz(real_from_int(510), real_from_int(200));
	walls[73].texture = 1;
	walls[73].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[73].portalTo = 6;
	walls[74].startCorner = xz(real_from_int(520), real_from_int(210));
	walls[74].texture = 1;
	walls[74].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[74].portalTo = 13;
	walls[75].startCorner = xz(real_from_int(440), real_from_int(270));
	walls[75].texture = 1;
	walls[75].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[75].portalTo = 1;

	// 13: m
	walls[76].startCorner = xz(real_from_int(440), real_from_int(270));
	walls[76].texture = 1;
	walls[76].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[76].portalTo = 12;
	walls[77].startCorner = xz(real_from_int(520), real_from_int(210));
	walls[77].texture = 1;
	walls[77].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[77].portalTo = 8;
	walls[78].startCorner = xz(real_from_int(520), real_from_int(280));
	walls[78].texture = 1;
	walls[78].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[78].portalTo = 14;
	walls[79].startCorner = xz(real_from_int(440), real_from_int(280));
	walls[79].texture = 1;
	walls[79].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[79].portalTo = 1;

	// 14: n
	walls[80].startCorner = xz(real_from_int(440), real_from_int(280));
	walls[80].texture = 1;
	walls[80].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[80].portalTo = 13;
	walls[81].startCorner = xz(real_from_int(520), real_from_int(280));
	walls[81].texture = 1;
	walls[81].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[81].portalTo = 8;
	walls[82].startCorner = xz(real_from_int(520), real_from_int(290));
	walls[82].texture = 1;
	walls[82].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[82].portalTo = 15;
	walls[83].startCorner = xz(real_from_int(440), real_from_int(290));
	walls[83].texture = 1;
	walls[83].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[83].portalTo = 2;

	// 15: o
	walls[84].startCorner = xz(real_from_int(440), real_from_int(290));
	walls[84].texture = 1;
	walls[84].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[84].portalTo = 14;
	walls[85].startCorner = xz(real_from_int(520), real_from_int(290));
	walls[85].texture = 1;
	walls[85].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[85].portalTo = 8;
	walls[86].startCorner = xz(real_from_int(520), real_from_int(300));
	walls[86].texture = 1;
	walls[86].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[86].portalTo = 16;
	walls[87].startCorner = xz(real_from_int(440), real_from_int(300));
	walls[87].texture = 1;
	walls[87].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[87].portalTo = 2;

	// 16: p
	walls[88].startCorner = xz(real_from_int(440), real_from_int(300));
	walls[88].texture = 1;
	walls[88].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[88].portalTo = 15;
	walls[89].startCorner = xz(real_from_int(520), real_from_int(300));
	walls[89].texture = 1;
	walls[89].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[89].portalTo = 8;
	walls[90].startCorner = xz(real_from_int(520), real_from_int(310));
	walls[90].texture = 1;
	walls[90].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[90].portalTo = 17;
	walls[91].startCorner = xz(real_from_int(440), real_from_int(310));
	walls[91].texture = 1;
	walls[91].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[91].portalTo = 2;

	// 17: q
	walls[92].startCorner = xz(real_from_int(520), real_from_int(320));
	walls[92].texture = 8;
	walls[92].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[92].portalTo = -1;
	walls[93].startCorner = xz(real_from_int(440), real_from_int(320));
	walls[93].texture = 1;
	walls[93].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[93].portalTo = 2;
	walls[94].startCorner = xz(real_from_int(440), real_from_int(310));
	walls[94].texture = 1;
	walls[94].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[94].portalTo = 16;
	walls[95].startCorner = xz(real_from_int(520), real_from_int(310));
	walls[95].texture = 1;
	walls[95].texCoord = (TexCoord){ xy_zero, xy_one };
	walls[95].portalTo = 8;

	sectors[0].wallCount = 4;
	sectors[0].height = 120;
	sectors[0].heightOffset = 0;
	sectors[0].floorColor = GColorFromRGB(170,170,170);
	sectors[0].ceilColor = GColorFromRGB(255,255,255);
	sectors[0].walls = walls + 0;
	sectors[1].wallCount = 11;
	sectors[1].height = 120;
	sectors[1].heightOffset = 0;
	sectors[1].floorColor = GColorFromRGB(170,170,170);
	sectors[1].ceilColor = GColorFromRGB(255,255,255);
	sectors[1].walls = walls + 4;
	sectors[2].wallCount = 8;
	sectors[2].height = 120;
	sectors[2].heightOffset = 0;
	sectors[2].floorColor = GColorFromRGB(170,170,170);
	sectors[2].ceilColor = GColorFromRGB(255,255,255);
	sectors[2].walls = walls + 15;
	sectors[3].wallCount = 4;
	sectors[3].height = 120;
	sectors[3].heightOffset = 0;
	sectors[3].floorColor = GColorFromRGB(170,170,170);
	sectors[3].ceilColor = GColorFromRGB(255,255,255);
	sectors[3].walls = walls + 23;
	sectors[4].wallCount = 6;
	sectors[4].height = 120;
	sectors[4].heightOffset = 0;
	sectors[4].floorColor = GColorFromRGB(170,170,170);
	sectors[4].ceilColor = GColorFromRGB(255,255,255);
	sectors[4].walls = walls + 27;
	sectors[5].wallCount = 4;
	sectors[5].height = 120;
	sectors[5].heightOffset = 0;
	sectors[5].floorColor = GColorFromRGB(170,170,170);
	sectors[5].ceilColor = GColorFromRGB(255,255,255);
	sectors[5].walls = walls + 33;
	sectors[6].wallCount = 7;
	sectors[6].height = 120;
	sectors[6].heightOffset = 0;
	sectors[6].floorColor = GColorFromRGB(170,170,170);
	sectors[6].ceilColor = GColorFromRGB(255,255,255);
	sectors[6].walls = walls + 37;
	sectors[7].wallCount = 6;
	sectors[7].height = 120;
	sectors[7].heightOffset = 0;
	sectors[7].floorColor = GColorFromRGB(170,170,170);
	sectors[7].ceilColor = GColorFromRGB(255,255,255);
	sectors[7].walls = walls + 44;
	sectors[8].wallCount = 10;
	sectors[8].height = 120;
	sectors[8].heightOffset = 0;
	sectors[8].floorColor = GColorFromRGB(170,170,170);
	sectors[8].ceilColor = GColorFromRGB(255,255,255);
	sectors[8].walls = walls + 50;
	sectors[9].wallCount = 4;
	sectors[9].height = 113;
	sectors[9].heightOffset = 7;
	sectors[9].floorColor = GColorFromRGB(85,85,85);
	sectors[9].ceilColor = GColorFromRGB(255,255,255);
	sectors[9].walls = walls + 60;
	sectors[10].wallCount = 4;
	sectors[10].height = 106;
	sectors[10].heightOffset = 14;
	sectors[10].floorColor = GColorFromRGB(85,85,85);
	sectors[10].ceilColor = GColorFromRGB(255,255,255);
	sectors[10].walls = walls + 64;
	sectors[11].wallCount = 4;
	sectors[11].height = 99;
	sectors[11].heightOffset = 21;
	sectors[11].floorColor = GColorFromRGB(85,85,85);
	sectors[11].ceilColor = GColorFromRGB(255,255,255);
	sectors[11].walls = walls + 68;
	sectors[12].wallCount = 4;
	sectors[12].height = 92;
	sectors[12].heightOffset = 28;
	sectors[12].floorColor = GColorFromRGB(85,85,85);
	sectors[12].ceilColor = GColorFromRGB(255,255,255);
	sectors[12].walls = walls + 72;
	sectors[13].wallCount = 4;
	sectors[13].height = 85;
	sectors[13].heightOffset = 35;
	sectors[13].floorColor = GColorFromRGB(85,85,85);
	sectors[13].ceilColor = GColorFromRGB(255,255,255);
	sectors[13].walls = walls + 76;
	sectors[14].wallCount = 4;
	sectors[14].height = 78;
	sectors[14].heightOffset = 42;
	sectors[14].floorColor = GColorFromRGB(85,85,85);
	sectors[14].ceilColor = GColorFromRGB(255,255,255);
	sectors[14].walls = walls + 80;
	sectors[15].wallCount = 4;
	sectors[15].height = 71;
	sectors[15].heightOffset = 49;
	sectors[15].floorColor = GColorFromRGB(85,85,85);
	sectors[15].ceilColor = GColorFromRGB(255,255,255);
	sectors[15].walls = walls + 84;
	sectors[16].wallCount = 4;
	sectors[16].height = 64;
	sectors[16].heightOffset = 56;
	sectors[16].floorColor = GColorFromRGB(85,85,85);
	sectors[16].ceilColor = GColorFromRGB(255,255,255);
	sectors[16].walls = walls + 88;
	sectors[17].wallCount = 4;
	sectors[17].height = 57;
	sectors[17].heightOffset = 63;
	sectors[17].floorColor = GColorFromRGB(85,85,85);
	sectors[17].ceilColor = GColorFromRGB(255,255,255);
	sectors[17].walls = walls + 92;
	*level = (Level){
		.sectorCount = 18,
		.playerStart = {
			.sector = 13,
			.position = xz(real_from_int(490), real_from_int(260)),
			.angle = real_degToRad(real_from_int(260)),
			.height = real_from_int(42)
		},
		.sectors = sectors
	};

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
