//
// LEVEL DATA
//

const XZ_FACTOR = 50;

const vertices = [
    [0, 0],
    [1, 0],
    [0, 1],
    [1, 1]
];

const textures = {
    wall: "wall"
};

const sectors = {
    left: {
        h: [0, 30],
        floor: [0, 0, 0],
        ceil: [2, 2, 2],
        walls: [
            [2, 0, textures.wall],
            [1, textures.wall]
        ]
    },
    right: {
        h: [15, 45],
        floor: [1, 1, 1],
        ceil: [2, 0, 0],
        walls: [
            [1, 3, textures.wall],
            [2, textures.wall]
        ]
    }
};

const portals = [
    [1, 2, sectors.left, sectors.right]
];

const player = {
    sector: 0,
    startPos: [ 0.1, 0.1 ],
    angle: 34,
    height: 10
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
            floor: checkColor(sector.floor),
            ceil: checkColor(sector.ceil),
            height: checkVector(sector.h),
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
                from: checkVertexIndex(arr[0]),
                to: checkVertexIndex(arr[1]),
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
            from: checkVertexIndex(portal[0]),
            to: checkVertexIndex(portal[1]),
            textureI: getTextureIndex(portal[4]),
            texStart: checkVector(portal[5]),
            texEnd: checkVector(portal[6]),
            portalTo: toI
        });
        sectorObjects[toI].walls.push({
            from: checkVertexIndex(portal[1]),
            to: checkVertexIndex(portal[0]),
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

let OUTPUT = "";

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
    `\t\t\t.sector = ${player.sector},\n` +
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

require("fs").writeFileSync("levelTempl.c", OUTPUT);
