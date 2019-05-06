//
// LEVEL DATA
//

const XZ_FACTOR = 20;

function createIncremental(arr) {
    for (var i = 1; i < arr.length; i++) {
        arr[i][0] += arr[i-1][0];
        arr[i][1] += arr[i-1][1];
    }
    return arr;
}

const vertices = createIncremental([
    [0, 0],
    [0, 2],
    [0, 6],
    [-12, 0],
    [0, 12],
    [14, 0],
    [4, -4],
    [16, 0],
    [4, 0],
    [4, 4],
    [10, 0],
    [0, -12],
    [-10, 0],
    [-6, -8],
    [-16, 0],
    [0, 2],
    [-2, 0],
    [0, 2],
    [2, 0],
    [0, 6],
    [-2, 0],
    [0, 2],
    [2, 0],
    [12, -0.5],

    // at 23, first stair
    [4, -3],
    [-3.5, 3.5],
    [4, -3],
    [-3.5, 3.5],
    [4, -3],
    [-3.5, 3.5],
    [4, -3],
    [-3.5, 3.5],
    [4, -3],

    [-4, 3.5],
    [4, 0],
    [-4, 0.5],
    [4, 0],
    [-4, 0.5],
    [4, 0],
    [-4, 0.5],
    [4, 0]
]);

const textures = {
    wall: "wall"
};

const STEP = 7;
const defaultHeight = [0, 120];
const defaultFloor = [1,1,1];
const defaultCeil = [2, 2, 2];
const sectors = {
    a: {
        walls: [
            [13, 14, textures.wall]
        ]
    },
    b: {
        walls: [
            [15, 18, textures.wall],
            [19, 22, textures.wall],
            [33, 31, textures.wall],
            [31, 23, textures.wall]
        ]
    },
    c: {
        walls: [
            [6, 7, textures.wall],
            [7, 33, textures.wall],
            [22, 21, textures.wall]
        ]
    },
    d: {
        walls: [
            [14, 0, textures.wall],
            [16, 15, textures.wall]
        ]
    },
    e: {
        walls: [
            [0, 2, textures.wall],
            [21, 20, textures.wall],
            [17, 16, textures.wall],
        ]
    },
    er: {
        walls: [
            [20, 19, textures.wall],
            [18, 17, textures.wall]
        ]
    },
    f: {
        walls: [
            [24, 32, textures.wall],
            [12, 13, textures.wall]
        ]
    },
    g: {
        walls: [
            [2, 3, textures.wall],
            [3, 4, textures.wall],
            [4, 5, textures.wall],
            [5, 6, textures.wall]
        ]
    },
    h: {
        walls: [
            [32, 8, textures.wall],
            [8, 9, textures.wall],
            [9, 10, textures.wall],
            [10, 11, textures.wall],
            [11, 12, textures.wall]
        ]
    },

    // Start of stairs
    i: {
        h: [STEP * 1, 120],
        walls: [
            [23, 25, textures.wall],
            [26, 24, textures.wall]
        ]
    },
    j: {
        h: [STEP * 2, 120],
        walls: [
            [25, 27, textures.wall],
            [28, 26, textures.wall],
        ]
    },
    k: {
        h: [STEP * 3, 120],
        walls: [
            [27, 29, textures.wall],
            [30, 28, textures.wall],
        ]
    },
    l: {
        h: [STEP * 4, 120],
        walls: [
            [29, 31, textures.wall],
            [32, 30, textures.wall],
        ]
    },
    m: {
        h: [STEP * 5, 120],
        walls: [
            [31, 33, textures.wall],
            [34, 32, textures.wall],
        ]
    },
    n: {
        h: [STEP * 6, 120],
        walls: [
            [33, 35, textures.wall],
            [36, 34, textures.wall],
        ]
    },
    o: {
        h: [STEP * 7, 120],
        walls: [
            [35, 37, textures.wall],
            [38, 36, textures.wall],
        ]
    },
    p: {
        h: [STEP * 8, 120],
        walls: [
            [37, 39, textures.wall],
            [40, 38, textures.wall],
        ]
    },
    q: {
        h: [STEP * 9, 120],
        walls: [
            [39, 7, textures.wall],
            [7, 8, textures.wall],
            [8, 40, textures.wall],
        ]
    }
};

const portals = [
    [6, 21, sectors.g, sectors.c],
    [21, 2, sectors.g, sectors.e],
    [20, 17, sectors.e, sectors.er],
    [16, 0, sectors.e, sectors.d],
    [33, 22, sectors.c, sectors.b],
    [18, 19, sectors.b, sectors.er],
    [14, 15, sectors.b, sectors.d],
    [23, 14, sectors.b, sectors.a],
    [24, 13, sectors.a, sectors.f],
    [32, 12, sectors.f, sectors.h],

    // Start of stairs
    [23, 24, sectors.a, sectors.i],
    [25, 26, sectors.i, sectors.j],
    [27, 28, sectors.j, sectors.k],
    [29, 30, sectors.k, sectors.l],
    [31, 32, sectors.l, sectors.m],
    [33, 34, sectors.m, sectors.n],
    [35, 36, sectors.n, sectors.o],
    [37, 38, sectors.o, sectors.p],
    [39, 40, sectors.p, sectors.q],
];

const player = {
    sector: sectors.a,
    startPos: [ 20, 4],
    angle: 0,
    height: 5
};

//
// TOOL
//

// Setup texture indices
const textureIndices = {};
{
    let i = 0;
    for (let tex of Object.keys(textures)) {
        textureIndices[
            (typeof textures[tex] !== "string") ? tex : textures[tex]
        ] = i++;
    }
}

function getTextureIndex(name)
{
    if (!(name in textureIndices))
        throw new Error("Invalid texture set");
    return textureIndices[name];
}

// Check vertices
function checkVector(vector)
{
    if (!Array.isArray(vector) ||
        vector.length != 2 ||
        typeof vector[0] !== "number" ||
        typeof vector[1] !== "number")
        throw new Error("Invalid vector");
    return vector;
}

{
    for (let vertex of vertices)
        checkVector(vertex);
}

function checkVertexIndex(i)
{
    if (typeof i !== "number" || i < 0 || i >= vertices.length)
        throw new Error("Invalid vertex index");
    return i;
}

// Setup sector indices
{
    let i = 0;
    for (let sector of Object.keys(sectors)) {
        sectors[sector].index = i++;
    }
}

function checkColor(color) {
    function checkComponent(c) {
        return typeof c === "number" && c >= 0 && c <= 3;
    }
    if (!Array.isArray(color) ||
        color.length !== 3 ||
        !checkComponent(color[0]) ||
        !checkComponent(color[1]) ||
        !checkComponent(color[2]))
        throw new Error("Invalid color")
    return color;
}

// Convert wall structures
let sectorObjects = [];
{
    for (let sectorName of Object.keys(sectors)) {
        const sector = sectors[sectorName];
        let sectorObject = {
            index: sector.index,
            name: sectorName,
            floor: checkColor(sector.floor || defaultFloor.slice(0)),
            ceil: checkColor(sector.ceil || defaultCeil.slice(0)),
            height: checkVector(sector.h || defaultHeight.slice(0)),
            walls: []
        };
        for (let i = 0; i < sector.walls.length; i++)
        {
            let arr = sector.walls[i];
            if (arr.length < 2 || arr.length > 5)
                throw new Error("Invalid wall");
            if (arr.length < 3 || arr.length === 4)
                arr.unshift(sectorObject.walls[i-1].to);
            if (arr.length < 5)
                arr.push([0, 0], [1, 1]);
            sectorObject.walls[i] = {
                from: checkVertexIndex(arr[1]),
                to: checkVertexIndex(arr[0]),
                textureI: getTextureIndex(arr[2]),
                texStart: checkVector(arr[3]),
                texEnd: checkVector(arr[4]),
                portalTo: -1
            };
        }
        sectorObjects[sector.index] = sectorObject;
    }
}

// Construct portal walls
{
    for (let portal of portals) {
        if (portal.length < 4)
            throw new Error("Invalid portal");
        if (portal.length < 5)
            portal.push(Object.keys(textureIndices)[0]);
        if (portal.length < 7)
            portal.push([0, 0], [1, 1]);

        const fromI = portal[2].index, toI = portal[3].index;
        sectorObjects[fromI].walls.push({
            from: checkVertexIndex(portal[1]),
            to: checkVertexIndex(portal[0]),
            textureI: getTextureIndex(portal[4]),
            texStart: checkVector(portal[5]),
            texEnd: checkVector(portal[6]),
            portalTo: toI
        });
        sectorObjects[toI].walls.push({
            from: checkVertexIndex(portal[0]),
            to: checkVertexIndex(portal[1]),
            textureI: getTextureIndex(portal[4]),
            texStart: checkVector(portal[5]),
            texEnd: checkVector(portal[6]),
            portalTo: fromI
        });
    }
}

// Construct wall chains
for (let sectorObject of sectorObjects)
{
    let chain = [ sectorObject.walls[0] ];
    do
    {
        const last = chain[chain.length - 1];
        let nextI = sectorObject.walls.findIndex(w => w.from === last.to);
        if (nextI < 0)
            throw new Error("Cannot complete chain");
        chain.push(sectorObject.walls[nextI]);
    } while (chain[chain.length - 1].to !== chain[0].from);
    if (chain.length !== sectorObject.walls.length)
        throw new Error("Chain did not contain all walls");
    sectorObject.wallChain = chain;
}

// Output functions
function printReal(n) {
    if (n === 0)
        return "real_zero";
    else if (n === 1)
        return "real_one";
    let str = "" + n;
    return str.indexOf(".") >= 0
        ? `real_from_float(${str}f)`
        : `real_from_int(${str})`;
}
function printRawVec(vector) {
    if (vector[0] === 0 && vector[1] === 0)
        return "_zero";
    if (vector[0] === 1 && vector[1] === 1)
        return "_one";
    return "(" + printReal(vector[0]) + ", " + printReal(vector[1]) + ")";
}
function printXZ(vector) {
    return "xz" + printRawVec([vector[0] * XZ_FACTOR, vector[1] * XZ_FACTOR]);
}
function printXY(vector) {
    return "xy" + printRawVec(vector);
}
function printColor(color) {
    const F = 255 / 3;
    return "GColorFromRGB(" +
        (color[0] * F) + "," +
        (color[1] * F) + "," +
        (color[2] * F) + ")";
}

let OUTPUT =
`#include <pebble.h>
#include "platform.h"
#include "level.h"

Level *level_load(int levelId)
{
    UNUSED(levelId);
`;

// Output walls_template
OUTPUT += "\tWall walls_template[] = {\n";
for (let sectorObject of sectorObjects)
{
    OUTPUT += `\t\t// ${sectorObject.index}: ${sectorObject.name}\n`;
    for (let wall of sectorObject.wallChain)
    {
        OUTPUT +=
            `\t\t{.startCorner = ${printXZ(vertices[wall.from])},\n` +
            `\t\t .texture = ${wall.textureI},\n` +
            `\t\t .texCoord = { ${printXY(wall.texStart)}, ${printXY(wall.texEnd)} },\n` +
            `\t\t .portalTo = ${wall.portalTo}},\n`;
    }
    OUTPUT += "\n";
}
OUTPUT += "\t};\n\n";

// Output sectors
OUTPUT += "\tSector sectors_template[] = {\n";
for (let sectorObject of sectorObjects)
{
    OUTPUT +=
        `\t\t{.wallCount = ${sectorObject.wallChain.length},\n` +
        `\t\t .height = ${sectorObject.height[1] - sectorObject.height[0]},\n` +
        `\t\t .heightOffset = ${sectorObject.height[0]},\n` +
        `\t\t .floorColor = ${printColor(sectorObject.floor)},\n` +
        `\t\t .ceilColor = ${printColor(sectorObject.ceil)},\n` +
        `\t\t .walls = NULL},\n`;
}
OUTPUT += "\t};\n\n";

// Output level
OUTPUT +=
    `\tLevel level_template = {\n` +
    `\t\t.sectorCount = ${sectorObjects.length},\n` +
    `\t\t.playerStart = {\n` +
    `\t\t\t.sector = ${player.sector.index},\n` +
    `\t\t\t.position = ${printXZ(checkVector(player.startPos))},\n` +
    `\t\t\t.angle = real_degToRad(${printReal(player.angle)}),\n` +
    `\t\t\t.height = ${printReal(player.height)}\n` +
    `\t\t},\n` +
    `\t\t.sectors = NULL\n` +
    `\t};\n\n`;

// Output intermission
OUTPUT +=
`\tchar* memory = (char*)malloc(sizeof(Level) + sizeof(sectors_template) + sizeof(walls_template));
\tif (memory == NULL)
\t\treturn NULL;
\tLevel* level = (Level*)memory;
\tSector* sectors = (Sector*)(memory + sizeof(Level));
\tWall* walls = (Wall*)(memory + sizeof(Level) + sizeof(sectors_template));
\tmemcpy(level, &level_template, sizeof(Level));
\tmemcpy(sectors, sectors_template, sizeof(sectors_template));
\tmemcpy(walls, walls_template, sizeof(walls_template));
\tlevel->sectors = sectors;
`

// Output sector wall offsets
let wallOffset = 0;
for (let sectorObject of sectorObjects)
{
    OUTPUT += `\tsectors[${sectorObject.index}].walls = walls + ${wallOffset};\n`;
    wallOffset += sectorObject.wallChain.length;
}

OUTPUT +=
`return level;
}

void level_free(Level *me)
{
    if (me == NULL)
        return;
    free(me);
}
`;

require("fs").writeFileSync("levelTempl.c", OUTPUT);
