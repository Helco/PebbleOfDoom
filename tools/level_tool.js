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
    wall: "solid_white.png",
    stairs: "solid_grey.png",
    column: "solid_yellow.png",
    infodesk: "info_desk.png",
    cantina: "cantina.png",
    elevator: "elevators.png",
    blackboard: "blackboard.png",
    entry: "entry.png",
    notfound: "404.png"
};

const STEP = 7;
const defaultHeight = [0, 120];
const defaultFloor = [2, 2, 2];
const defaultCeil = [3, 3, 3];
const sectors = {
    a: {
        walls: [
            [13, 14, textures.entry, [0, 0], [2, 1]]
        ]
    },
    b: {
        walls: [
            [15, 18, textures.column],
            [19, 22, textures.column],
        ]
    },
    c: {
        walls: [
            [6, 7, textures.cantina, [0, 0], [1.333, 1]],
            [22, 21, textures.column]
        ]
    },
    d: {
        walls: [
            [14, 0, textures.entry, [0, 0], [0.5, 1]],
            [16, 15, textures.column]
        ]
    },
    e: {
        walls: [
            [0, 2, textures.blackboard],
            [21, 20, textures.column],
            [17, 16, textures.column],
        ]
    },
    er: {
        walls: [
            [20, 19, textures.column],
            [18, 17, textures.column]
        ]
    },
    f: {
        walls: [
            [12, 13, textures.infodesk]
        ]
    },
    g: {
        walls: [
            [2, 3, textures.wall],
            [3, 4, textures.elevator],
            [4, 5, textures.wall],
            [5, 6, textures.cantina, [0, 0], [0.3333, 1]]
        ]
    },
    h: {
        walls: [
            [8, 9, textures.cantina, [0, 0], [0.3333, 1]],
            [9, 10, textures.wall],
            [10, 11, textures.elevator],
            [11, 12, textures.wall]
        ]
    },

    // Start of stairs
    i: {
        floor: [1, 1, 1],
        h: [STEP * 1, 120],
        walls: [
        ]
    },
    j: {
        floor: [1, 1, 1],
        h: [STEP * 2, 120],
        walls: [
        ]
    },
    k: {
        floor: [1, 1, 1],
        h: [STEP * 3, 120],
        walls: [
        ]
    },
    l: {
        floor: [1, 1, 1],
        h: [STEP * 4, 120],
        walls: [
        ]
    },
    m: {
        floor: [1, 1, 1],
        h: [STEP * 5, 120],
        walls: [
        ]
    },
    n: {
        floor: [1, 1, 1],
        h: [STEP * 6, 120],
        walls: [
        ]
    },
    o: {
        floor: [1, 1, 1],
        h: [STEP * 7, 120],
        walls: [
        ]
    },
    p: {
        floor: [1, 1, 1],
        h: [STEP * 8, 120],
        walls: [
        ]
    },
    q: {
        floor: [1, 1, 1],
        h: [STEP * 9, 120],
        walls: [
            [7, 8, textures.notfound],
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
    [23, 24, sectors.a, sectors.i, textures.stairs],
    [25, 26, sectors.i, sectors.j, textures.stairs],
    [27, 28, sectors.j, sectors.k, textures.stairs],
    [29, 30, sectors.k, sectors.l, textures.stairs],
    [31, 32, sectors.l, sectors.m, textures.stairs],
    [33, 34, sectors.m, sectors.n, textures.stairs],
    [35, 36, sectors.n, sectors.o, textures.stairs],
    [37, 38, sectors.o, sectors.p, textures.stairs],
    [39, 40, sectors.p, sectors.q, textures.stairs],

    // Left side of stair/lobby portals
    [23, 25, sectors.i, sectors.b, textures.stairs],
    [25, 27, sectors.j, sectors.b, textures.stairs],
    [27, 29, sectors.k, sectors.b, textures.stairs],
    [29, 31, sectors.l, sectors.b, textures.stairs],
    [31, 33, sectors.m, sectors.b, textures.stairs],
    [33, 35, sectors.n, sectors.c, textures.stairs],
    [35, 37, sectors.o, sectors.c, textures.stairs],
    [37, 39, sectors.p, sectors.c, textures.stairs],
    [39, 7, sectors.q, sectors.c, textures.stairs],

    // Right side of stair/lobby portals
    [26, 24, sectors.i, sectors.f, textures.stairs],
    [28, 26, sectors.j, sectors.f, textures.stairs],
    [30, 28, sectors.k, sectors.f, textures.stairs],
    [32, 30, sectors.l, sectors.f, textures.stairs],
    [34, 32, sectors.m, sectors.h, textures.stairs],
    [36, 34, sectors.n, sectors.h, textures.stairs],
    [38, 36, sectors.o, sectors.h, textures.stairs],
    [40, 38, sectors.p, sectors.h, textures.stairs],
    [8, 40, sectors.q, sectors.h, textures.stairs],
];

const player = {
    sector: sectors.m,
    startPos: [ 23, 10],
    angle: 0,
    height: 6 * STEP
};

//
// TOOL
//

// Setup texture indices
const textureIndices = {};
{
    let i = 0;
    for (let tex of Object.keys(textures)) {
        const name = (typeof textures[tex] !== "string") ? tex : textures[tex];
        textureIndices[name] = i++;
        console.log(`${i}: ${name}`);
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
