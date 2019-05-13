const fs = require("fs");

if (typeof process.argv[2] !== "string" || typeof process.argv[3] !== "string")
    throw new Error("Usage: node level_tool.js <input_filename> <output_filename>");
const root = JSON.parse(fs.readFileSync(process.argv[2], "utf8"));

//
// UTILITY FUNCTIONS
//
function createIncremental(arr) {
    for (var i = 1; i < arr.length; i++) {
        arr[i][0] += arr[i-1][0];
        arr[i][1] += arr[i-1][1];
    }
    return arr;
}

function checkVector(vector)
{
    if (!Array.isArray(vector) ||
        vector.length != 2 ||
        typeof vector[0] !== "number" ||
        typeof vector[1] !== "number")
        throw new Error("Invalid vector");
    return vector;
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

//
// PARSE LEVEL DATA
//

const XZ_FACTOR = root.xzFactor || 1.0;

let vertices = null;
if (("vertices" in root) && ("verticesIncremental") in root)
    throw new Error("Ambiguous vertex array found");
else if ("vertices" in root)
    vertices = root.vertices;
else if ("verticesIncremental" in root)
    vertices = createIncremental(root.verticesIncremental);
else
    throw new Error("No vertex array found");
if (!Array.isArray(vertices) || vertices.length <= 0)
    throw new Error("No vertices declared");

if (typeof root.textures !== "object" || Array.isArray(root.textures) || Object.keys(root.textures).length <= 0)
    throw new Error("No textures declared");
const textures = root.textures;

let defaultHeight = null;
let defaultFloorColor = null;
let defaultCeilColor = null;
if ("defaultHeight" in root)
    defaultHeight = checkVector(root.defaultHeight);
if ("defaultFloorColor" in root)
    defaultFloorColor = checkColor(root.defaultFloorColor);
if ("defaultCeilColor" in root)
    defaultCeilColor = checkColor(root.defaultCeilColor);

const sectors = root.sectors;
if (typeof root.sectors !== "object" || Array.isArray(root.sectors) || Object.keys(root.sectors).length <= 0)
    throw new Error("No sectors declared");

let portals = [];
if ("portals" in root) {
    if (!Array.isArray(portals))
        throw new Error("Invalid portal array");
    portals = root.portals;
}

const playerStart = root.playerStart;
if (typeof root.playerStart !== "object" || Array.isArray(root.playerStart))
    throw new Error("No player start information declared");

//
// CONVERSION
//

// Setup texture indices
const textureIndices = {};
{
    let i = 0;
    for (let tex of Object.keys(textures)) {
        textureIndices[tex] = i++;
        console.log(`${i}: ${tex}`);
    }
}

function getTextureIndex(name)
{
    if (!(name in textureIndices))
        throw new Error(`Invalid texture index ${name}`);
    return textureIndices[name];
}

// Check vertices
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

// Convert wall structures
let sectorObjects = [];
{
    for (let sectorName of Object.keys(sectors)) {
        const sector = sectors[sectorName];
        let sectorObject = {
            index: sector.index,
            name: sectorName,
            floorColor: checkColor(sector.floorColor || defaultFloorColor || (()=>{throw new Error(`No floor color for ${sectorName}`)})()),
            ceilColor: checkColor(sector.ceilColor || defaultCeilColor || (()=>{throw new Error(`No floor color for ${sectorName}`)})()),
            height: checkVector(sector.height || defaultHeight || (()=>{throw new Error(`No sector height for ${sectorName}`)})()),
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
        if (!(portal[2] in sectors))
            throw new Error(`Invalid portal source ${portal[2]}`);
        if (!(portal[3] in sectors))
            throw new Error(`Invalid portal target ${portal[3]}`);

        const fromI = sectors[portal[2]].index, toI = sectors[portal[3]].index;
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

// Check playerStart
checkVector(playerStart.position);
playerStart.position[0] *= XZ_FACTOR;
playerStart.position[1] *= XZ_FACTOR;
if (typeof playerStart.height !== "number")
    throw new Error("Invalid playerStart.height");
if (typeof playerStart.angle !== "number")
    throw new Error("Invalid playerStart.angle");
if ("sector" in playerStart)
{
    if (typeof playerStart.sector !== "string" || !(playerStart.sector in sectors))
        throw new Error(`Invalid playerStart sector ${playerStart.sector}`);
    playerStart.sector = sectors[playerStart.sector].index;
}

// Create output object
const OUTPUT = {
    playerStart: playerStart,
    sectors: sectorObjects.map(obj => ({
        comment: obj.name,
        heightOffset: obj.height[0],
        height: obj.height[1],
        floorColor: obj.floorColor,
        ceilColor: obj.ceilColor,
        walls: obj.wallChain.map(wall => ({
            startCorner: [
                vertices[wall.from][0] * XZ_FACTOR,
                vertices[wall.from][1] * XZ_FACTOR
            ],
            texture: wall.textureI,
            texCoord: {
                start: wall.texStart,
                end: wall.texEnd
            },
            portalTo: wall.portalTo
        }))
    }))
};
fs.writeFileSync(process.argv[3], JSON.stringify(OUTPUT, null, "    "));
